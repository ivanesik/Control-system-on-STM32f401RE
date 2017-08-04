#ifndef __MODBUS_RTU_H
#define __MODBUS_RTU_H

#include "config.h"
#include "comm_ModBusTimers.h"

#define FR_SUCCESS                       0x00
#define FR_ERR_BAD_PARAMS                0x0C
#define FR_ERR_NOT_RESPOND               0x0E
#define FR_ERR_FUNC_NOT_SUPPORTED        0x0F
#define FR_ERR_CRC_FAILED                0x11
#define FR_ERR_BAD_RESPONSE_LENGTH       0x12
#define FR_ERR_BAD_RESPONSE_ADDRESS      0x13
#define FR_ERR_BAD_RESPONSE_FUNC         0x14
#define FR_ERR_BAD_DATA_LENGTH           0x15
#define FR_ERR_BAD_DATA_ADDRESS          0x16
#define FR_ERR_BAD_DATA_NUM              0x17
#define FR_ERR_BAD_DATA_VALUE            0x18
#define FR_ERR_BAD_EXCERTION_RESPONSE    0x19

#define EX_ILLEGAL_FUNCTION              0x01
#define EX_ILLEGAL_DATA_ADDRESS          0x02
#define EX_ILLEGAL_DATA_VALUE            0x03
#define EX_SLAVE_DEVICE_FAILURE          0x04
#define EX_ACKNOWLEDGE                   0x05
#define EX_SLAVE_DEVICE_BUSY             0x06
#define EX_NEGATIVE_ACKNOWLEDGE          0x07
#define EX_MEMORY_PARITY_ERROR           0x08

#define IsMaster()	(hPort->Params.Mode == MB_MASTER)
#define IsSlave()	(hPort->Params.Mode == MB_SLAVE)

// Режимы работы
#define MB_SLAVE             0        // Режим ведомого устройства
#define MB_MASTER            1        // Режим ведущего устройства

// serial_communication
#define MB_SCALE			 (0.001 * Prd2kHZ)	// Масштабировка для связи по протоколу ModBus

// Поддерживаемые функции
#define MB_READ_REGS         0x03     // Чтение регистров хранения
#define MB_READ_INPUT_REGS   0x04     // Чтение регистров ввода
#define MB_WRITE_REG         0x06     // Запись регистра
#define MB_DIAGNOSTICS       0x08     // Диагностика
#define MB_WRITE_REGS        0x10     // Запись регистров
#define MB_REPORT_ID         0x11     // Чтение идентификаторов устройства

// Поддерживаемые подфункции для диагностики
#define MB_RET_QUERY_DATA    0x0000   // Вернуть запрашиваемые данные
#define MB_RESTART_COMM      0x0001   // Сброс коммуникации
#define MB_RET_DIAGN_REG     0x0002   // Чтение регистра диагностики
#define MB_FORCE_LISTEN      0x0004   // Переход в режим "Listen Mode Only"
#define MB_CLEAR_DIAGN_REG   0x000A   // Сброс счетчиков и регистра диагностики
#define MB_RET_BUS_MSG       0x000B   // Чтение счетчика сообщений
#define MB_RET_BUS_ERR       0x000C   // Чтение счетчика ошибок связи (CRC)
#define MB_RET_BUS_EXCEPT    0x000D   // Чтение счетчика исключений
#define MB_RET_SLAVE_MSG     0x000E   // Чтение счетчика обработанных сообщений
#define MB_RET_SLAVE_NO_RESP 0x000F   // Чтение счетчика неответов
#define MB_RET_SLAVE_NAK     0x0010   // Чтение счетчика отрицательных подтверждений
#define MB_RET_SLAVE_BUSY    0x0011   // Чтение счетчика занятости устройства
#define MB_RET_BUS_OVERRUN   0x0012   // Чтение счетчика наложения данных
#define MB_CLEAR_OVERRUN     0x0014   // Очистка счетчика наложения и флага ошибки

// Структура параметров драйвера
typedef void (*TMbTrFunc)(Byte);

typedef struct _TMbParams {
	Byte ChannelID;            // Идентификатор UART-порта
	Byte Mode;                 // Режим работы   (Master/Slave)
	Byte Slave;                // Адрес подчиненного устройства
	Uns  BaudRate;             // Скорость обмена
	Uns  UartBaud;             // Расчитанная скорость для UART
	Byte Parity;               // Режим паритета
	Byte RetryCount;           // Количество повторов передач (в режиме MASTER)
	Uns ModbusTimFriq;        // Частота вызова таймингов
	Uns  RxDelay;              // Задержка при приеме кадров (в тактах)
	Uns  TxDelay;              // Задержка при передаче кадров (в мсек)
	Uns  ConnTimeout;          // Тайм-аут определения связи (в мсек)
	Uns  AckTimeout;           // Тайм-аут подтверждения (в мсек)
	TMbTrFunc TrEnable;   	   // Функция разрешения передачи
	Uns  HardWareType;	   // тип аппаратного канала передачи данных
} TMbParams;

// Структра пакета данных
typedef struct _TMbPacket {
	Byte Request;              // Код функции запроса
	Byte Response;             // Код функции ответа
	Uns  SubRequest;           // Код подфункции запроса
	Uns  Addr;                 // Начальный адрес данных
	Uns  Data[100];            // Буфер данных
	Uns  Count;                // Количество данных
	Byte Exception;            // Код исключения (ошибки)
	Bool Acknoledge;           // Флаг ожидания подтверждения
	Byte ParamMode;	           // режим Modbus / Bluetooth
} TMbPacket;

// Структура кадра
typedef struct _TMbFrame {
	Bool       ListenMode;     // Признак нахождения в режиме "Listen Mode Only"
	Uns       RetryCounter;   // Количество повторов передач (в режиме MASTER)
	Bool       WaitResponse;   // Флаг ожидания ответа (в режиме MASTER)
	Bool       NewMessage;     // Флаг приема нового кадра
	TTimerList Timer1_5;       // Таймер для 1.5 символа
	TTimerList Timer3_5;       // Таймер для 3.5 символа
	TTimerList TimerPre;       // Таймер для преамбулы
	TTimerList TimerPost;      // Таймер для постамбулы
	TTimerList TimerConn;      // Таймер для определения наличия связи
	TTimerList TimerAsk;       // Таймер для для потверждения
	Uns        RxLength;       // Длина принятого кадра
	Uns        TxLength;       // Длина передаваемого кадра
	Uns 	   AddCount;
	Byte      *Data;           // Указатель в буфере данных кадара
	Byte       Buf[256];       // Буфер данных кадра
} TMbFrame;


typedef union {
  Uint16 all;
  struct {
    Uint16 Busy:1;		// 0 - занят идет выполнение последней соманды
    Uint16 Ready:1;		// 1 - готов команда выполнена
    Uint16 Wait:1;		// 2 - ожидание команды
    Uint16 NoConnect:1;         // 3 - нет связи
    Uint16 Error:1;		// 4 - ошибка при выполнении команды
    Uint16 Rsvd:11;		// 5-15
  } bit;
} TATS48_mbStatus;

// Режим связи
typedef enum {
  pmNone = 0,				// Нет проверки бита четности, 2 стоповых бита
  pmOdd  = 1,				// Проверка на нечетность, 1 стоповый бит
  pmEven = 2				// Проверка на четность, 1 стоповый бит
}TParityMode;

// Скорость связи ModBus
typedef enum {
  br2400   = 0,			// 2400 Бод
  br4800   = 1,			// 4800 Бод
  br9600   = 2,			// 9600 Бод
  br19200  = 3,			// 19200 Бод
  br38400  = 4,			// 38400 Бод
  br57600  = 5,			// 57600 Бод
  br115200 = 6			// 115200 Бод
}TBaudRate;


// Струкутра статистики работы
typedef struct _TMbStat {
	Uns DiagnReg;              // Регистр диагностики
	Uns BusMsgCount;           // Счетчик сообщений
	Uns BusErrCount;           // Счетчик ошибок связи
	Uns SyncTxErrCount;	   // Счетчик ошибок связи на приём
	Uns SyncRxErrCount;	   // Счетчик ошибок связи на передачу
	Uns BusExcCount;           // Счетчик исключений
	Uns SlaveMsgCount;         // Счетчик обработанных сообщений
	Uns SlaveNoRespCount;      // Счетчик неответов
	Uns SlaveNakCount;         // Счетчик отрицательных подтверждений
	Uns SlaveBusyCount;        // Счетчик занятости устройства
	Uns BusParityErrCount;	   // Счетчик ошибок бита паритета
	Uns BusOverrunErrCount;    // Счетчик наложения данных
	Uns BusFrameErrCount;	   // Счетчик ошибок отсутствия стопового бита
	Uns BusFrameLenErrCount;   // Счетчик ошибок приема слишком коротких кадров
	Uns BusFrameCrcErrCount;   // Счетчик ошибок CRC кадра
	Uns TxMsgCount;            // Счетчик переданных сообщений
	Uns RxBytesCount;          // Принятое количество байт
	Uns TxBytesCount;          // Отправление количество байт
	Uns MAMsgIn;
	Uns MAMsgOut;
	TATS48_mbStatus Status;	   // статус выполнения команды
} TMbStat;


typedef struct _TSerialInterface
{
	Uns            RsState;             // 0.Состояние связи
	TBaudRate      RsBaudRate;          // 1.Скорость связи
	Uns            RsStation;           // 2.Адрес станции
	TParityMode    RsMode;              // 3.Режим связи
	Uns            RsReset;             // 4.Сброс связи
	Uns            RsRxErrors;          // 6.Количество ошибок приема
	Uns            RsIndicTime;         // 7.Время определения связи
	Uns            RsWaitTime;          // 8.Время ожидания передачи
	Uns            Rsvd1;               // 9.Резерв
	Uns            PduRead;             // 10.Чтение данных в память ПДУ
	Uns            PduWrite;            // 11.Запись данных из памяти ПДУ
} TSerialInterface;


// Структура объекта драйвера
typedef struct _TMbPort {
  TMbParams Params;          // Параметры драйвера
  TMbPacket Packet;          // Пакет данных
  TMbFrame  Frame;           // Структура кадра
  TMbStat   Stat;            // Статистика работы
  TSerialInterface Serial;   // Послед. интерфейс
} TMbPort,*TMbHandle;



//Прототипы
void ModbusInit(TMbHandle);
void ModbusInvoke(TMbHandle);
void ModbusTimings(TMbHandle);

void ModbusRx(TMbHandle);
void ModbusTx(TMbHandle);



// Прототипы comm_ModbusCrc.h 
__inline void GenerateCrcTable(void);
__inline Uns CalcFrameCrc(Byte *Buf, Uns Count);


#endif