//
// Created by neolux on 2025/12/25.
//

#ifndef CARCAR_MOTO_H
#define CARCAR_MOTO_H

#include "stdint.h"
#include "stm32f1xx_hal.h"

/**
 * 电机方向枚举
 * @note MOTOR_DIR_BACKWARD: 后退
 * @note MOTOR_DIR_FORWARD: 前进
 * @note MOTOR_DIR_COUNT: 方向数量
 */
typedef enum {
    MOTOR_DIR_BACKWARD = 0,
    MOTOR_DIR_FORWARD,
    MOTOR_DIR_COUNT,
} MotorDir_e;

/**
 * 电机结构体
 * @note timer: 定时器句柄
 * @note channel: 定时器通道
 * @note dir_port: 方向控制端口
 * @note dir_pin: 方向控制引脚
 * @note dir: 当前方向
 * @note vel: 当前速度
 */
typedef struct {
    TIM_HandleTypeDef *timer;
    uint32_t channel;
    GPIO_TypeDef *dir_port;
    uint16_t dir_pin;
    MotorDir_e dir;
    int vel;
} Motor_t;

/**
 * 启动 PWM Generation for 电机
 * @param motor 电机结构体指针
 */
extern void motor_on(const Motor_t *motor);

/**
 * 停止 PWM Generation for 电机
 * @param motor 电机结构体指针
 */
extern void motor_off(const Motor_t *motor);

/**
 * 设置电机方向
 * @param motor 电机结构体指针
 * @param dir 电机方向
 */
extern void motor_set_direction(Motor_t *motor, MotorDir_e dir);

/**
 * 设置电机速度
 * @param motor 电机结构体指针
 * @param speed 速度设置
 */
extern void motor_set_speed(Motor_t *motor, int speed); // percentage

/**
 * 设置电机速度和方向
 * @param motor 电机结构体指针
 * @param dir 电机方向
 * @param speed 速度设置
 */
extern void motor_set_speed_dir(Motor_t *motor, MotorDir_e dir, int speed);

/**
 * 切换电机方向
 * @param motor 电机结构体指针
 */
extern void motor_toggle_direction(Motor_t *motor);

/**
 * 增加电机速度
 * @param motor 电机结构体指针
 * @param delta 速度增量
 */
extern void motor_vel_incre(Motor_t *motor, int delta);

/**
 * 减少电机速度
 * @param motor 电机结构体指针
 * @param delta 速度减量
 */
extern void motor_vel_decre(Motor_t *motor, int delta);

#endif //CARCAR_MOTO_H
