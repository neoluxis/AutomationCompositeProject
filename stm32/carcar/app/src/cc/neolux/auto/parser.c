//
// Created by neolux on 2025/12/26.
//

#include "cc/neolux/auto/parser.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

void parse_cmd(uint8_t *str, int *l, int *r) {
  char *saveptr; // 用于 strtok_r 或简单的指针操作

  // 获取命令 echo, set
  char *cmd = strtok((char *) str, " \r\n");
  if (cmd == NULL) return; // 空行直接返回

  if (strcmp(cmd, "echovel") == 0) {
    printf("L:%d, R:%d\r\n", *l, *r);
    return;
  }

  if (strcmp(cmd, "setvel") == 0) {
    char *val_l_str = strtok(NULL, " \r\n");
    char *val_r_str = strtok(NULL, " \r\n");

    if (val_l_str == NULL || val_r_str == NULL) {
      printf("Error: setvel requires 2 values (L and R)!\r\n");
    } else {
      *l = atoi(val_l_str);
      *r = atoi(val_r_str);
      printf("SetVel OK: L=%d, R=%d\r\n", *l, *r);
    }
    return;
  }

  // 获取变量 l, r
  char *sub_cmd = strtok(NULL, " \r\n");
  if (sub_cmd == NULL) {
    printf("Error: Missing sub-command (l/r)!\r\n");
    return;
  }

  // 逻辑解析
  if (strcmp(cmd, "echo") == 0) {
    if (strcmp(sub_cmd, "l") == 0) {
      printf("L Velocity: %d\r\n", *l);
    } else if (strcmp(sub_cmd, "r") == 0) {
      printf("R Velocity: %d\r\n", *r);
    } else {
      printf("Error: Unknown echo target '%s'. Use 'l' or 'r'.\r\n", sub_cmd);
    }
  } else if (strcmp(cmd, "set") == 0) {
    // 获取数值
    char *val_str = strtok(NULL, " \r\n");
    if (val_str == NULL) {
      printf("Error: 'set' requires a value!\r\n");
      return;
    }

    int temp_val = atoi(val_str); // 转换为整数

    if (strcmp(sub_cmd, "l") == 0) {
      *l = temp_val;
      printf("Set L OK: %d\r\n", *l);
    } else if (strcmp(sub_cmd, "r") == 0) {
      *r = temp_val;
      printf("Set R OK: %d\r\n", *r);
    } else {
      printf("Error: Unknown set target '%s'.\r\n", sub_cmd);
    }
  } else if (strcmp(cmd, "setvel")==0) {
    // 获取数值
    char *val_str = strtok(NULL, " \r\n");
    if (val_str == NULL) {
      printf("Error: 'set' requires a value!\r\n");
      return;
    }

    int temp_val = atoi(val_str); // 转换为整数

    if (strcmp(sub_cmd, "l") == 0) {
      *l = temp_val;
      printf("Set L OK: %d\r\n", *l);
    } else if (strcmp(sub_cmd, "r") == 0) {
      *r = temp_val;
      printf("Set R OK: %d\r\n", *r);
    } else {
      printf("Error: Unknown set target '%s'.\r\n", sub_cmd);
    }
  } else {
    printf("Error: Unknown command '%s'. Available: set, echo\r\n", cmd);
  }
}
