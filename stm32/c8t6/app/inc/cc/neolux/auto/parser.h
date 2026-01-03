//
// Created by neolux on 2025/12/26.
//

#ifndef CARCAR_PARSER_H
#define CARCAR_PARSER_H

#include "stdint.h"

/**
 * 解析命令字符串
 * @param str 命令字符串, set: 设置速度， echo: 回显速度
  *            格式: set l 50\r\n, set r 60\r\n, echo l\r\n, echo r\r\n
 * @param l 左电机速度指针
 * @param r 右电机速度指针
 */
extern void parse_cmd(uint8_t *str, int *l, int *r);

#endif //CARCAR_PARSER_H
