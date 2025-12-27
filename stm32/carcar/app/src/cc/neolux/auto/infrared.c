//
// Created by neolux on 2025/12/25.
//

#include "cc/neolux/auto/infrared.h"

uint8_t infrared_init(Infrared_t *ir, GPIO_TypeDef *port, const uint16_t pin) {
    if (!IS_GPIO_ALL_INSTANCE(port) || !IS_GPIO_PIN(pin)) return 1;
    ir->port = port;
    ir->pin = pin;
    return 0;
}

uint8_t infrared_read(const Infrared_t *ir) {
    return HAL_GPIO_ReadPin(ir->port, ir->pin);
}

uint8_t infrared_array_init(InfraredArray_t *array, const uint8_t count, const Infrared_t *irs) {
    array->count = count;
    if (array->count < 1) return 1;
    // for (uint8_t i = 0; i < array->count; i++) {
    //     if (irs + i == NULL) return 2; // 检查是否有 NULL
    // }
    array->irs = irs;
    return 0;
}

uint8_t infrared_array_read(const InfraredArray_t *irs) {
    if (irs == NULL) return 0;
    uint8_t ir_value = 0x00;
    for (uint8_t i = 0; i < irs->count; i++) {
        ir_value = ir_value << 1;
        ir_value |= infrared_read(&irs->irs[i]);
    }
    return ~ir_value;
}

uint8_t infrared_array_readone(const InfraredArray_t *array, const uint8_t index) {
    if (array == NULL || index >= array->count) return 0;
    return infrared_read(&array->irs[index]);
}
