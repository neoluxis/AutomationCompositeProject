package com.example.controller

import android.Manifest
import android.bluetooth.*
import android.bluetooth.le.*
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.os.Build
import android.os.Handler
import android.os.Looper
import androidx.core.app.ActivityCompat

class BluetoothScanner(
    private val context: Context,
    private val onDeviceFound: (String) -> Unit,
    private val onScanFinished: (Int) -> Unit,
    private val scanPeriod: Long = 15000L
) {

    private val handler = Handler(Looper.getMainLooper())
    private val foundDevices = mutableSetOf<String>()
    private val bluetoothAdapter: BluetoothAdapter by lazy {
        val manager = context.getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        manager.adapter
    }
    private var bleScanner: BluetoothLeScanner? = null

    private var bluetoothGatt: BluetoothGatt? = null
    private var writeCharacteristic: BluetoothGattCharacteristic? = null
    private var notifyCharacteristic: BluetoothGattCharacteristic? = null


    private val classicReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context?, intent: Intent?) {
            val action = intent?.action ?: return
            if (action == BluetoothDevice.ACTION_FOUND) {
                val device = intent.getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
                if (ActivityCompat.checkSelfPermission(context!!, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) return
                val name = device?.name ?: "未知 Classic"
                val addr = device?.address ?: "未知 地址"
                val info = "Classic: $name ($addr)"
                if (foundDevices.add(info)) onDeviceFound(info)
            }
        }
    }

    private val bleCallback = object : ScanCallback() {
        override fun onScanResult(callbackType: Int, result: ScanResult) {
            val device = result.device
            if (ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) return
            val name = device.name ?: "未知 BLE"
            val addr = device.address ?: "未知 地址"
            val info = "BLE: $name ($addr)"
            if (foundDevices.add(info)) onDeviceFound(info)
        }

        override fun onScanFailed(errorCode: Int) {
            onDeviceFound("BLE 扫描失败 code=$errorCode")
        }
    }

    fun startScan() {
        onDeviceFound("=== 开始扫描 BLE + Classic ===")
        foundDevices.clear()

        // Classic 扫描
        val filter = IntentFilter(BluetoothDevice.ACTION_FOUND)
        context.registerReceiver(classicReceiver, filter)
        if (ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) return
        if (bluetoothAdapter.isDiscovering) bluetoothAdapter.cancelDiscovery()
        bluetoothAdapter.startDiscovery()
        onDeviceFound("Classic 扫描开始...")

        // BLE 扫描
        bleScanner = bluetoothAdapter.bluetoothLeScanner
        if (bleScanner == null) onDeviceFound("BLE Scanner 初始化失败")
        else {
            val settings = ScanSettings.Builder().setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY).build()
            bleScanner!!.startScan(null, settings, bleCallback)
            onDeviceFound("BLE 扫描开始...")
        }

        // 定时停止
        handler.postDelayed({ stopScan() }, scanPeriod)
    }

    fun stopScan() {
        try {
            if (ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_SCAN) == PackageManager.PERMISSION_GRANTED) {
                bleScanner?.stopScan(bleCallback)
            }
        } catch (_: Exception) {}

        try {
            bluetoothAdapter.cancelDiscovery()
            context.unregisterReceiver(classicReceiver)
        } catch (_: Exception) {}

        onScanFinished(foundDevices.size)
    }
    /**
     * 根据设备名称尝试连接 BLE 设备
     */
    fun connectToNamedBleDevice(targetName: String) {
        // 从已扫描到的设备信息中查找匹配名称
        val targetInfo = foundDevices.firstOrNull { it.contains(targetName) }
        if (targetInfo == null) {
            onDeviceFound("未找到名称为 \"$targetName\" 的设备")
            return
        }

        // 从 info 中提取 MAC 地址
        val address = targetInfo.substringAfter("(").substringBefore(")")
        val device = bluetoothAdapter.getRemoteDevice(address)

        if (ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            onDeviceFound("缺少连接权限，无法连接设备")
            return
        }

        onDeviceFound("尝试连接 BLE 设备 \"$targetName\" ($address)...")

        // 调用 connectGatt 建立 BLE GATT 连接
        device.connectGatt(context, false, object : BluetoothGattCallback() {
            override fun onCharacteristicWrite(
                gatt: BluetoothGatt,
                characteristic: BluetoothGattCharacteristic,
                status: Int
            ) {
                super.onCharacteristicWrite(gatt, characteristic, status)
                sendingFinished=true
            }
            override fun onConnectionStateChange(gatt: BluetoothGatt?, status: Int, newState: Int) {
                super.onConnectionStateChange(gatt, status, newState)

                if (newState == BluetoothProfile.STATE_CONNECTED) {
                    bluetoothGatt = gatt
                    onDeviceFound("BLE 设备 \"$targetName\" 已连接")
                    // 连接成功，可以开始发现服务等操作
                    if (ActivityCompat.checkSelfPermission(
                            context,
                            Manifest.permission.BLUETOOTH_CONNECT
                        ) != PackageManager.PERMISSION_GRANTED
                    ) {
                        return
                    }
                    gatt?.discoverServices()
                } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                    onDeviceFound("BLE 设备 \"$targetName\" 已断开")
                }
            }

            @Deprecated("Deprecated in Java")
            override fun onCharacteristicChanged(
                gatt: BluetoothGatt,
                characteristic: BluetoothGattCharacteristic
            ) {
                val data = characteristic.value
                val msg = data.toString(Charsets.UTF_8)
                onDeviceFound("收到设备回传: $msg")
            }
            override fun onServicesDiscovered(gatt: BluetoothGatt?, status: Int) {
                super.onServicesDiscovered(gatt, status)
                if (writeCharacteristic != null) {
                    onDeviceFound("BLE 已就绪（可通信）")
                }

                if (status == BluetoothGatt.GATT_SUCCESS) {
                    onDeviceFound("服务发现成功，准备与 \"$targetName\" 通信")


                    if (gatt != null) {
                        for (service in gatt.services) {
                            for (ch in service.characteristics) {
                                val props = ch.properties
                                if (props and BluetoothGattCharacteristic.PROPERTY_WRITE != 0 ||
                                    props and BluetoothGattCharacteristic.PROPERTY_WRITE_NO_RESPONSE != 0
                                ) {
                                    writeCharacteristic = ch

                                    // 设置写入方式（更安全）
//                                    ch.writeType =
//                                        if (props and BluetoothGattCharacteristic.PROPERTY_WRITE_NO_RESPONSE != 0)
//                                            BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE
//                                        else
//                                            BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
                                    ch.writeType = BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
                                    onDeviceFound("找到可写特征: ${ch.uuid}")
                                }

                                /* 🔔 如果这个特征支持 Notify，就开启回传 */
                                if (props and BluetoothGattCharacteristic.PROPERTY_NOTIFY != 0) {

                                    notifyCharacteristic = ch
                                    if (ActivityCompat.checkSelfPermission(
                                            context,
                                            Manifest.permission.BLUETOOTH_CONNECT
                                        ) != PackageManager.PERMISSION_GRANTED
                                    ) {
                                        // TODO: Consider calling
                                        //    ActivityCompat#requestPermissions
                                        // here to request the missing permissions, and then overriding
                                        //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                                        //                                          int[] grantResults)
                                        // to handle the case where the user grants the permission. See the documentation
                                        // for ActivityCompat#requestPermissions for more details.
                                        return
                                    }
                                    gatt.setCharacteristicNotification(ch, true)

                                    val descriptor = ch.getDescriptor(
                                        java.util.UUID.fromString("00002902-0000-1000-8000-00805f9b34fb")
                                    )
                                    descriptor?.value = BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
                                    gatt.writeDescriptor(descriptor)

                                    onDeviceFound("已开启 Notify: ${ch.uuid}")
                                }

                            }
                        }
                    }

                    onDeviceFound("未找到可写特征")





                } else {
                    onDeviceFound("服务发现失败，状态码: $status")
                }
            }
        })
    }
    var sendingFinished=true
    /**
     * 向已连接的 BLE 设备发送字符串
     */
    fun sendString(text: String) {

        val gatt = bluetoothGatt
        val ch = writeCharacteristic

        if(!sendingFinished)
            return

        sendingFinished=false

        if (gatt == null || ch == null) {
            onDeviceFound("BLE 未连接或不可写")
            return
        }


        if (ActivityCompat.checkSelfPermission(
                context,
                Manifest.permission.BLUETOOTH_CONNECT
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            onDeviceFound("缺少 BLUETOOTH_CONNECT 权限")
            return
        }

        val data = text.toByteArray(Charsets.UTF_8)
        ch.value = data

        val success = gatt.writeCharacteristic(ch)
        if (!success)
            onDeviceFound("发送失败")
//        onDeviceFound(
//            if (success) "发送成功: $text"
//            else "发送失败"
//        )
    }


}

