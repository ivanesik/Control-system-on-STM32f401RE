#ifndef __INTERFACE_H
#define __INTERFACE_H

#include "std.h"


#define ADC_TEMP_DATA_ROM_SIZE 80  // размер буфера АЦП для термосопротивлений
#define ADC_VOLT_DATA_ROM_SIZE 1   // размер буфера АЦП для однофазного напряжения


//так как значение виброскорости передается по стандарту IEEE-754 (с плавабщей запятой), 
//для работы с ним необходимо конвертировать буфер в данное значение,
//а принятые данные сохраняются для их дальнейшей передачи
typedef struct _TVibrDataStructure
{
  Uns X_VibrData;   // данные виброскорости с датчика
  Uns Y_VibrData;   // данные виброскорости с датчика
  Uns Z_VibrData;   // данные виброскорости с датчика
  
  Float X_Vibr;   // виброскорость по оси Х
  Float Y_Vibr;   // виброскорость по оси Y
  Float Z_Vibr;   // виброскорость по оси Z
 
}TVibrDataStructure;



typedef struct _TTempStructure
{
  MdInt Temp_P2;  // Температура подшипника 2
  MdInt Temp_T;   // Температура уплотнителя
  MdInt Temp_ED1; // Температура электродвигателя
  MdInt Temp_P1;  // Температура подшипника 1
  
}TTempStructure;


//структура данных с аналоговых датчиков
typedef struct _AdcDataPointStructure
{
  MdBits   TMP_P2;         //температура подшипника 2
  MdBits   TMP_T;          //температура торцевого уплотнителя
  MdBits   TMP_ED1;        //температура двигателя
  MdBits   TMP_P1;         //температура подшипника 1
  MdBits   A_DVB;          //резервный сигнал с датчика вибрации
  MdBits   V_220;          //напряжение обмотки 220 В
  
}AdcDataPointStructure;


//средние значения буфера данных аналоговых каналов
typedef struct _AdcDataAverageStructure
{
  MdBits   TMP_P2;         //температура подшипника 2
  MdBits   TMP_T;          //температура торцевого уплотнителя
  MdBits   TMP_ED1;        //температура двигателя
  MdBits   TMP_P1;         //температура подшипника 1
  MdBits   A_DVB;          //резервный сигнал с датчика вибрации
  MdBits   V_220;          //напряжение обмотки 220 В
  
}AdcDataAverageStructure;



typedef struct _AdcDataBufStructure
{
  MdBits   TMP_P2[ADC_TEMP_DATA_ROM_SIZE];      //температура подшипника 2
  MdBits   TMP_T[ADC_TEMP_DATA_ROM_SIZE];       //температура торцевого уплотнителя
  MdBits   TMP_ED1[ADC_TEMP_DATA_ROM_SIZE];     //температура двигателя
  MdBits   TMP_P1[ADC_TEMP_DATA_ROM_SIZE];      //температура подшипника 1
  MdBits   A_DVB[ADC_TEMP_DATA_ROM_SIZE];       //резервный сигнал с датчика вибрации
  MdBits   V_220[ADC_VOLT_DATA_ROM_SIZE];       //напряжение обмотки 220 В
  
}AdcDataBufStructure;

//Состояние линий АЦП
typedef struct _AdcChannelStateStructure
{
  Byte   TMP_P2;         //состояние канала P2
  Byte   TMP_T;          //состояние канала T
  Byte   TMP_ED1;        //состояние канала ED1
  Byte   TMP_P1;         //состояние канала P1
  Byte   A_DVB;          //состояние канала DVB
  Byte   V_220;          //состояние канала V220
  
}AdcChannelStateStructure;




// Регистр состояния пинов 1
typedef union _GPIO_StateUnion
{
  Uns all;
  struct
  {
    Uns  K1_BKS_RAB:1;     // 0 Сигнал что влючен первый контактор реверсивного пускателя БКС
    Uns  K2_BKS_RAB:1;     // 1 Сигнал что влючен второй контактор реверсивного пускателя БКС
    Uns  K1_BKS_ON:1;      // 2 Сигнал замыкания первого контактора
    Uns  K2_BKS_ON:1;      // 3 Сигнал замыкания второго контактора
    Uns  K_VIK1:1;         // 4 Сигнал с концевого выключателя 1 привода поворота
    Uns  K_VIK2:1;         // 5 Сигнал с концевого выключателя 2 привода поворота
    Uns  PUSK_KON:1;       // 6 Сигнал на Включение силового контактора
    Uns  STOP_KON:1;       // 7 Сигнал на Отключение силового контактора
    Uns  ZV_OP:1;          // 8 Звуковой опевещатель, активный уровень 1
    Uns  SV_OP:1;          // 9 Световая сигнализ, активный уровень 1
    Uns  DPR:1;            // 10 Сигнал датчика (сигнализатора) протечки, при протечке 1
    Uns  CPU_LED:1;        // 11 Зажигает диод на плате, Активный уровень 1
    Uns  Resv:4;           // 12-15 Резерв
  }bit;
}GPIO_StateUnion;


// Регистр состояния пинов связи с АСУ 
typedef union _GPIO_AsuStateUnion
{
  Uns all;
  struct
  {
    Uns  PUSK_220:1;       // 0 Сигнал управления из АСУ ТП активный уровень 1
    Uns  PUSK_24:1;        // 1 Сигнал управления из АСУ ТП активный уровень 1
    Uns  STOP_220:1;       // 2 Сигнал управления из АСУ ТП активный уровень 1
    Uns  STOP_24:1;        // 3 Сигнал управления из АСУ ТП активный уровень 1
    Uns  AVAR_ASU:1;       // 4 Сиганал об аварии из АСУ, активный уровень 1
    Uns  RAB_ASU:1;        // 5 Сигнал о работе из АСУ, активный уровень 1
    Uns  Resv:10;          // 6-15 резерв
  }bit;
}GPIO_AsuStateUnion;



typedef struct _TGroupA
{
//данные аналоговых каналов
  AdcDataPointStructure      AdcDataPointStruct;        // Мгновенное значение данных с АЦП
  AdcDataBufStructure        AdcDataBufStruct;          // Кольцевой буфер данных АЦП
  AdcDataAverageStructure    AdcDataAverageStruct;      // Среднее значение буфера АЦП
  AdcChannelStateStructure   AdcChannelState;           // Состояние аналоговых линий
  
  TTempStructure             TempStruct;                // Данные о температуре
  
//данные цифровых каналов (датчики) 
  TVibrDataStructure   Vibr;          //значение вибрация с датчика вибрации (RS-485)  
  Uns                  Friq_ED;       //частота вращения вала ~ частоте миандра 
  
//Регистр состояния пинов
  GPIO_StateUnion GpioStateUnion;
  GPIO_AsuStateUnion GpioAsuStateUnion;
}TGroupA;


extern TGroupA g_RomData;


void InterfaceInit(void);

#endif