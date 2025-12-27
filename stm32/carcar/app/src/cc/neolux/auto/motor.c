//
// Created by neolux on 2025/12/25.
//

#include "cc/neolux/auto/motor.h"

void motor_on(const Motor_t *motor) {
  HAL_TIM_PWM_Start(motor->timer, motor->channel);
}

void motor_off(const Motor_t *motor) {
  HAL_TIM_PWM_Stop(motor->timer, motor->channel);
}

void motor_set_direction(Motor_t *motor, const MotorDir_e dir) {
  motor->dir = dir;
  HAL_GPIO_WritePin(motor->dir_port, motor->dir_pin, (GPIO_PinState) dir);
}

void motor_set_speed(Motor_t *motor, int speed) {
  if (speed < 0) {
    speed = -speed;
    motor_set_direction(motor, MOTOR_DIR_BACKWARD);
  } else if (speed > 0) {
    motor_set_direction(motor, MOTOR_DIR_FORWARD);
  }
  if (speed > 100) speed = 100;
  if (!motor->dir) {
    __HAL_TIM_SET_COMPARE(motor->timer, motor->channel, speed*10);
  } else {
    __HAL_TIM_SET_COMPARE(motor->timer, motor->channel, 1000-speed*10);
  }
  motor->vel = speed;
}

void motor_set_speed_dir(Motor_t *motor, const MotorDir_e dir, const int speed) {
  motor_set_direction(motor, dir);
  motor_set_speed(motor, speed);
}

void motor_toggle_direction(Motor_t *motor) {
  motor_set_direction(motor, !motor->dir);
}

void motor_vel_incre(Motor_t *motor, const int delta) {
  motor_set_speed(motor, motor->vel + delta);
}

void motor_vel_decre(Motor_t *motor, const int delta) {
  motor_set_speed(motor, motor->vel - delta);
}
