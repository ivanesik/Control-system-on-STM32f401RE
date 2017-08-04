#ifndef __CONFIG_H
#define __CONFIG_H

#if !defined(DEBUG)
#define DEBUG 0
#endif


#define UART_TYPE 0
#define SPI_TYPE 1


// Подключение заголовочных файлов
#include "global/g_hardware.h"	// Конфигурация оборудования
#include "global/g_Rom.h"  //совокупность параметров и данных в структурах
#include "comm/comm_ModbusRtu.h"
#include "stm32f4xx.h"
#include "global/std.h"

#endif