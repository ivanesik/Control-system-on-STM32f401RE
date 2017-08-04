#include "comm/comm.h"
#include "comm/comm_SerialComm.h"
#include "comm/comm_ModbusFrame.h"


void InitDVibrChanelModbus(TMbHandle hPort)
{
  //hPort->Params.ChannelID       = DVB_UART_ID;//
  hPort->Params.Mode            = MB_MASTER;//
  hPort->Params.BaudRate        = UART_SPEED; //Скорость передачи по UART2
  hPort->Params.Parity          = 0;//

  hPort->Params.RetryCount      = 2;
  hPort->Params.ModbusTimFriq   = 2000;
  hPort->Params.ConnTimeout     = 40;
  hPort->Params.RxDelay         = 10;
  hPort->Params.TxDelay         = 10;
  hPort->Params.AckTimeout      = 1000;
  hPort->Frame.TimerPre.Timeout = 1;

  hPort->Params.HardWareType	= UART_TYPE;
  hPort->Packet.ParamMode	= UART_TYPE;
}

void SerialCommTimings(TMbHandle hPort)
{
  ModbusTimings(hPort);
}

void SerialCommInit(TMbHandle hPort)
{
  ModbusReset(hPort);
}

void ModbusReset(TMbHandle hPort)
{
  ModbusInit(hPort);
}

void ModbusUpdate(TMbHandle hPort)
{  
  ModbusInvoke(hPort);
}





void SpiRx (TPpData *hPpData)
{
  Byte *Data = hPpData.Data;
  Byte InData = SPI_I2S_ReceiveData(SPI1);
  
  if (InData == SPI_SLAVE_IN_CHECK)
  {
    Data = hPpData.RxFrame;
    Spi_Answer(hPpData);
  }
  
  *Data = InData;
  Data ++;
}


void SpiAnswer (TPpData *hPpData)
{
  for (int i = 0; i < hPpData.TxLength; i++)
  {
    
  }
}

