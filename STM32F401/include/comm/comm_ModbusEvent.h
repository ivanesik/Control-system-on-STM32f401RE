
#ifndef __MODBUS_EVENT_H
#define __MODBUS_EVENT_H

#include "comm_ModBusRtu.h"
#include "comm_ModbusFrame.h"
#include "comm_ModbusTimers.h"


//--Timer1_5----при приеме что между принятыми байтами не более 1.5------------------------------------------------------
__inline void BreakFrameEvent(TMbPort *hPort) 
{
 // USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
 // USART_ITConfig(USART2, USART_IT_TC, DISABLE);
  
  StopMbTimer(&hPort->Frame.TimerAsk);
}


//-Timer3_5----при приеме если прошло 3.5 значит пришел полный пакет данных, начинается обработка--
__inline void NewFrameEvent(TMbPort *hPort)
{
  hPort->Frame.NewMessage = true;
  hPort->Frame.WaitResponse = false;
  hPort->Frame.RxLength   = hPort->Frame.Data - hPort->Frame.Buf;//длинна принятого сообщения
  hPort->Frame.Data       = hPort->Frame.Buf; // Сброс указателя буфера в начало
  
//  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
//  USART_ITConfig(USART2, USART_IT_TC, DISABLE);
  
  StartMbTimer(&hPort->Frame.TimerPre);
}


//--по окончанию преамбулы начинается передача запроса (далее по прерыванию)-------------------------------------------
__inline void PreambleEvent(TMbPort *hPort)
{
  hPort->Frame.Data = hPort->Frame.Buf;
  
 // USART_ITConfig(USART2, USART_IT_TC, ENABLE);
 // USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);

  GPIO_SetBits(GPIOA,GPIO_Pin_1);
  
  USART_SendData(USART2, (uint8_t) *hPort->Frame.Data++);
}


// После передачи последнего байта пакета------------------------------------------
__inline void PostambleEvent(TMbPort *hPort)
{
  hPort->Frame.Data = hPort->Frame.Buf;  //сброс указателя на начало отправляемого буфера

 // USART_ITConfig(USART2, USART_IT_TC, DISABLE); // начинаем слушать
 // USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // перестаем передавать
  GPIO_ResetBits(GPIOA,GPIO_Pin_1);             // сброс бита отправки (аппаратный)
  
  hPort->Frame.WaitResponse = true;
  
  StartMbTimer(&hPort->Frame.TimerAsk);
}



//-------------------------------------------------------------------------------
__inline void NoAskEvent(TMbPort *hPort)
{    
 // USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
 // USART_ITConfig(USART2, USART_IT_TC, DISABLE);
     
  if (hPort->Stat.SlaveNoRespCount<65500)
    hPort->Stat.SlaveNoRespCount++;// Счетчик неответов
     
  if (hPort->Frame.RetryCounter<65500) 
    hPort->Frame.RetryCounter++;
    
  hPort->Frame.WaitResponse = false; //по этому флагу в главном цикле начинает заново опрашивать
  hPort->Stat.Status.bit.Error = 1;
  
  StartMbTimer(&hPort->Frame.TimerPre);
}





//------------------------------------------------------------------------
//-------------------------------------------------------------------------------
static void SendFrame(TMbPort *hPort)
{
  CrcPack(hPort);

//  USART_ITConfig(USART2, USART_IT_TC, ENABLE);
//  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
//  GPIO_SetBits(GPIOA,GPIO_Pin_1);
  hPort->Frame.WaitResponse = true;

  StartMbTimer(&hPort->Frame.TimerPre);
  hPort->Stat.TxMsgCount++;
}



/*
//-------------------------------------------------------------------------------
static void SendMasterResponse(TMbPort *hPort)
{
	hPort->Packet.Response    = hPort->Packet.Request;
	hPort->Packet.Request     = 0;
	hPort->Frame.RetryCounter = 0;
	hPort->Frame.WaitResponse = false;
}

*/

#endif