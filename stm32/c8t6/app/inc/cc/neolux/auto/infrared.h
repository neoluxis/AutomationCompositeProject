//
// Created by neolux on 2025/12/25.
//

#ifndef CARCAR_INFRARED_H
#define CARCAR_INFRARED_H

#include "stdint.h"
#include "stm32f1xx_hal.h"

/**
 * 红外传感器结构体
 * @note port: GPIO 端口
 * @note pin: GPIO 引脚
 */
typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} Infrared_t;

/**
 * 红外传感器数组结构体
 * @note count: 红外传感器数量
 * @note irs: 红外传感器数组指针
 */
typedef struct {
    uint8_t count;
    const Infrared_t *irs;
} InfraredArray_t;

/**
 * 初始化红外传感器
 * @param ir 红外传感器结构体指针
 * @param port GPIO 端口
 * @param pin GPIO 引脚
 * @return 0: 成功, 1: 失败
 */
extern uint8_t infrared_init(Infrared_t *ir, GPIO_TypeDef *port, uint16_t pin);

/**
 * 读取红外传感器状态
 * @param ir 红外传感器结构体指针
 * @return 0: 低于阈值； 1: 高于阈值
 */
extern uint8_t infrared_read(const Infrared_t *ir);

/**
 * 初始化红外传感器数组
 * @param array 红外传感器数组结构体指针
 * @param count 红外传感器数量
 * @param irs 红外传感器数组指针
 * @return 0: 成功, 1: 失败
 */
extern uint8_t infrared_array_init(InfraredArray_t *array, uint8_t count, const Infrared_t *irs);

/**
 * 读取红外传感器数组状态
 * @param irs 红外传感器数组结构体指针
 * @return 红外传感器状态位图
 */
extern uint8_t infrared_array_read(const InfraredArray_t *irs);

/**
 * 读取红外传感器数组中单个传感器状态
 * @param array 红外传感器数组结构体指针
 * @param index 传感器索引
 * @return 0: 低于阈值； 1: 高于阈值
 */
extern uint8_t infrared_array_readone(const InfraredArray_t *array, uint8_t index);

#endif //CARCAR_INFRARED_H
