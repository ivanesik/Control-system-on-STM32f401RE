#ifndef __HARDWARE_H
#define __HARDWARE_H

#include "stm32f4xx.h"
#include "config.h"

#define UART_BAUDRATE_9600 9600
#define UART_BAUDRATE_19200 19200
#define UART_BAUDRATE_38400 38400
#define UART_BAUDRATE_57600 57600
#define UART_BAUDRATE_115200 115200

#define UART_SPEED UART_BAUDRATE_9600

#define ADC_CHANNEL_NUM 6


void InitHardware(void);

#endif