#ifndef CMD_IFACE_H
#define CMD_IFACE_H

#include "stm32l4xx.h"

void Cmd_Iface_Init(void);
void Cmd_UART_Rx(UART_HandleTypeDef *huart);
#endif
