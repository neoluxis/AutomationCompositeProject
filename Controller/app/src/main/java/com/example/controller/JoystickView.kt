package com.example.controller

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import kotlin.math.*

class JoystickView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null
) : View(context, attrs) {

    private val basePaint = Paint().apply {
        color = Color.LTGRAY
        style = Paint.Style.FILL
    }
    private val handlePaint = Paint().apply {
        color = Color.DKGRAY
        style = Paint.Style.FILL
    }

    private var handleX = 0f
    private var handleY = 0f
    private var baseRadius = 0f
    private var handleRadius = 0f
    private var centerX = 0f
    private var centerY = 0f

    var listener: ((x: Float, y: Float) -> Unit)? = null

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
        super.onSizeChanged(w, h, oldw, oldh)
        centerX = w / 2f
        centerY = h / 2f
        baseRadius = min(w, h) / 2f * 0.8f
        handleRadius = baseRadius / 3f
        handleX = centerX
        handleY = centerY
    }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)
        // 绘制底盘
        canvas.drawCircle(centerX, centerY, baseRadius, basePaint)
        // 绘制摇杆
        canvas.drawCircle(handleX, handleY, handleRadius, handlePaint)
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        val dx = event.x - centerX
        val dy = event.y - centerY
        val distance = sqrt(dx * dx + dy * dy)

        when (event.action) {
            MotionEvent.ACTION_DOWN, MotionEvent.ACTION_MOVE -> {
                if (distance > baseRadius) {
                    val ratio = baseRadius / distance
                    handleX = centerX + dx * ratio
                    handleY = centerY + dy * ratio
                } else {
                    handleX = event.x
                    handleY = event.y
                }
                listener?.invoke(
                    ((handleX - centerX) / baseRadius).coerceIn(-1f, 1f),
                    ((handleY - centerY) / baseRadius).coerceIn(-1f, 1f) * -1  // y 方向反向
                )
            }
            MotionEvent.ACTION_UP -> {
                handleX = centerX
                handleY = centerY
                listener?.invoke(0f, 0f)
            }
        }
        invalidate()
        return true
    }
}
