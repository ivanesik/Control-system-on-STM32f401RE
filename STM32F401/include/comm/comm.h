#ifndef __COMM_H
#define __COMM_H

#include "comm_SerialComm.h"
#include "comm_ModbusRtu.h"

/**
 * Структура содержащая
 */
typedef struct _TComm{
  TMbPort mbDVibr;
  TPpData spiTSM;
}TComm;

// Начальная инициализация интерфейсов и протоколов внешней коммуникации
void Comm_Init(TComm *);

// Обновление состояния коммуникации
void Comm_Update(TComm *);

extern TComm g_Comm;

#endif