package com.example.controller

import android.Manifest
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.view.View
import android.widget.*
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat

class MainActivity : AppCompatActivity() {

    private lateinit var searchButton: Button
    private lateinit var connectButton: Button
    private lateinit var debugTextView: TextView
    private lateinit var debugScrollView: ScrollView

    private lateinit var topButton1: Button
    private lateinit var topButton2: Button
    private lateinit var buttonUp: Button
    private lateinit var buttonDown: Button
    private lateinit var buttonLeft: Button
    private lateinit var buttonRight: Button
    private lateinit var joystick: JoystickView
    private lateinit var connectionIndicator: View
    // 顶部输入框
    private lateinit var topEditText: EditText
    private lateinit var bluetoothScanner: BluetoothScanner
    private val REQUEST_PERMISSIONS = 1001



    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // 绑定控件
        searchButton = findViewById(R.id.searchButton)
        connectButton = findViewById(R.id.connectButton)
        debugTextView = findViewById(R.id.debugTextView)
        debugScrollView = findViewById(R.id.debugScrollView)
        topEditText = findViewById(R.id.topEditText)

        topButton1 = findViewById(R.id.topButton1)
        topButton2 = findViewById(R.id.topButton2)
        buttonUp = findViewById(R.id.buttonUp)
        buttonDown = findViewById(R.id.buttonDown)
        buttonLeft = findViewById(R.id.buttonLeft)
        buttonRight = findViewById(R.id.buttonRight)
        joystick = findViewById(R.id.joystick)
        connectionIndicator = findViewById(R.id.connectionIndicator)

        //创建蓝牙设备
        bluetoothScanner = BluetoothScanner(
            context = this,
            onDeviceFound = { appendDebug(it) },
            onScanFinished = { total -> appendDebug("扫描结束，共发现 $total 个设备") }
        )
        //增加按钮绑定
        setupButtonListeners()
    }

    var MAX_SPEED = 30
    //用于换灯
    private fun showConnected() {
        connectionIndicator.setBackgroundResource(R.drawable.circle_green)
    }

    private fun showDisconnected() {
        connectionIndicator.setBackgroundResource(R.drawable.circle_red)
    }

    private fun setupButtonListeners() {
        // 顶部和摇杆按钮
        topButton1.setOnClickListener {
            val text = topEditText.text.toString()
            if (text.isNotEmpty()) {
                //发送框内的数据
                bluetoothScanner.sendString(text)

                appendDebug("发送: $text")
                // 发送后清空输入框
                topEditText.text.clear()
            } else {
                appendDebug("输入框为空，未发送")
            }
        }


        topButton2.setOnClickListener {
            val text = topEditText.text.toString()

            val value = text.toIntOrNull()
            if (value == null) {
                appendDebug("请输入合法数字")
                return@setOnClickListener
            }

            MAX_SPEED = value.coerceIn(0, 60)
            appendDebug("MAX_SPEED 设置为 $MAX_SPEED")
            topEditText.text.clear()
        }


        buttonUp.setOnClickListener {
            appendDebug("前进")
            bluetoothScanner.sendString("setvel $MAX_SPEED $MAX_SPEED\n")
        }

        buttonDown.setOnClickListener {
            appendDebug("后退")
            bluetoothScanner.sendString("setvel ${-MAX_SPEED} ${-MAX_SPEED}\n")
        }

        buttonLeft.setOnClickListener {
            appendDebug("左转")
            bluetoothScanner.sendString("setvel 0 $MAX_SPEED\n")
        }

        buttonRight.setOnClickListener {
            appendDebug("右转")
            bluetoothScanner.sendString("setvel $MAX_SPEED 0\n")
        }

        joystick.listener = { x, y ->
            //appendDebug("Joystick x=${"%.2f".format(x)}, y=${"%.2f".format(y)}")

            //x和y范围都是(-1,1)



            // 计算左右轮速度
            var leftSpeed = ((y + x) * MAX_SPEED).toInt().coerceIn(-MAX_SPEED, MAX_SPEED)
            var rightSpeed = ((y - x) * MAX_SPEED).toInt().coerceIn(-MAX_SPEED, MAX_SPEED)


            // 如果 y < 0，就让左右速度都为负（交换次序）
            if (y < 0) {
                val temp = leftSpeed
                leftSpeed = rightSpeed
                rightSpeed = temp
            }

            val cmd = "setvel $leftSpeed $rightSpeed\n"
            //appendDebug(cmd)

            bluetoothScanner.sendString(cmd)
        }


        // 搜索按钮：只扫描
        searchButton.setOnClickListener { checkPermissionsAndScan() }


        connectButton.setOnClickListener {
            val deviceName = topEditText.text.toString().trim()

            if (deviceName.isNotEmpty()) {
                //连接输入框内的设备，前提要在搜索里头找到该设备
                bluetoothScanner.connectToNamedBleDevice(deviceName)

                // 连接后清空输入框
                topEditText.text.clear()
            } else {
                //默认查找HC-42设备
                bluetoothScanner.connectToNamedBleDevice("HC-42")
            }}
    }
    //扫描周围设备
    private fun checkPermissionsAndScan() {
        val perms = mutableListOf<String>()
        //确认权限
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            perms.add(Manifest.permission.BLUETOOTH_SCAN)
            perms.add(Manifest.permission.BLUETOOTH_CONNECT)
        }
        perms.add(Manifest.permission.ACCESS_FINE_LOCATION)

        val missing = perms.filter {
            ActivityCompat.checkSelfPermission(this, it) != PackageManager.PERMISSION_GRANTED
        }

        if (missing.isNotEmpty()) {
            ActivityCompat.requestPermissions(this, missing.toTypedArray(), REQUEST_PERMISSIONS)
        } else {
            //如果权限没有问题调用扫描函数
            bluetoothScanner.startScan()
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_PERMISSIONS) {
            if (grantResults.all { it == PackageManager.PERMISSION_GRANTED }) {
                appendDebug("权限已授予，开始扫描...")
                bluetoothScanner.startScan()
            } else {
                appendDebug("用户未授予权限，无法扫描")
            }
        }
    }

    private fun appendDebug(msg: String) {
        //确保被BluetoothScanner的appendDebug也能显示调试信息
        runOnUiThread {
            // 用于滚动显示调试信息
            debugTextView.append("$msg\n")
            debugScrollView.post { debugScrollView.fullScroll(ScrollView.FOCUS_DOWN) }

            val trimmedMsg = msg.trim() // 去掉首尾空格/换行

            if (trimmedMsg.contains("已连接")) {

                showConnected()
            } else if (trimmedMsg.contains("已断开")) {
                showDisconnected()
            }
        }

    }

    override fun onDestroy() {
        super.onDestroy()
        bluetoothScanner.stopScan()
    }
}
