#ifndef __SERIAL_COMM_H
#define __SERIAL_COMM_H

#include "comm_ModbusRtu.h"


#define SPI_FRAME_BUF_SIZE	12	 // Размер массива буфера посылки

#define SPI_SLAVE_IN_CHECK      0x55     // Проверочный символ, который приходит с DSP
#define SPI_SLAVE_OUT_CHECK     0xAA	 // Проверочный символ, который отправляет на DSP





typedef struct _TPpData {
  
  Uns X_VibrData;   // данные виброскорости с датчика
  Uns Y_VibrData;   // данные виброскорости с датчика
  Uns Z_VibrData;   // данные виброскорости с датчика
  Uns Temp_P2;  // Температура подшипника 2
  Uns Temp_T;   // Температура уплотнителя
  Uns Temp_ED1; // Температура электродвигателя
  Uns Temp_P1;  // Температура подшипника 1
  
  GPIO_StateUnion Discr_State_In;  // Сигналы из АСУ на дискретных входах
  GPIO_AsuStateUnion Discr_ASU_In; // Остальные состояния дискретных входов (выходов)
  GPIO_StateUnion Discr_State_Out;  // Сигналы из АСУ на дискретных входах
  GPIO_AsuStateUnion Discr_ASU_Out; // Остальные состояния дискретных входов (выходов)
  
  AdcChannelStateStructure AdcState; // Состояние каналов АЦП
  Byte DvibrErr; // Ошибка датчика вибрации (ошибки ModBus)
  
  Byte *Data;                           // Указатель на элемент буфера
  Uns RxLength;       // Длина принятого кадра
  Uns TxLength;       // Длина передаваемого кадра
  Byte RxFrame[SPI_FRAME_BUF_SIZE];		// Буфер
  Byte TxFrame[SPI_FRAME_BUF_SIZE];		// Буфер
  
} TPpData, *TPpHandle;



void SerialCommInit(TMbHandle);
void ModbusReset(TMbHandle);
void InitDVibrChanelModbus(TMbHandle);

void SerialCommTimings(TMbHandle);

void ModbusUpdate(TMbHandle);

#endif