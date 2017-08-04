
#include "global/g_InterruptUpdate.h"
#include "peref/p_tempObserver.h"
#include "peref/p_voltControl.h"


#if defined(DEBUG)
void test_SPI (void);
void test_UART (void);

Byte test_data_Spi;
#endif



Uns count18kHz = 0,             //—четчик прерываний частотой 18 к√ц
    count2kHz = 0,              //—четчик прерываний частотой 2  к√ц
    count200Hz = 0,             //—четчик прерываний частотой 200 √ц
    count50Hz = 0;              //—четчик прерываний частотой 10  √ц



void InterruptUpdate(void)  //‘ункци€ обработки прерываний
{
//—брос бита переполнени€
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
  
//счетчик прерывани€ разной частоты
  count18kHz++;
  if (count18kHz == 9)
  {
    count18kHz = 0;
    count2kHz++;
    
    //tskCnt2kHz = 0;
    SerialCommTimings(&g_Comm.mbDVibr);
    VoltObserverUpdate();

    
    if (count2kHz == 2) //10
    {
      count2kHz = 0;
      count200Hz++;
      //tskCnt200Hz = 0;
     
    
      if (count200Hz == 4)
      {
        count200Hz=0;
        count50Hz++;
        //tskCnt50Hz = 0;
        
        if (count50Hz == 5)
        {
          count50Hz = 0;
          //tskCnt10Hz = 0
         
          TempObserverUpdate ();
          
#if defined(DEBUG)
    test_SPI ();
#endif
        }
      }
    }
  }
}




// “естовые функции----------------------------------------------------
#if defined(DEBUG)

void test_SPI (void)
{
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, 0xAD);
}


void test_UART (void)
{
  GPIO_SetBits(GPIOA,GPIO_Pin_1);
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//ждет окончани€ передачи
  USART_SendData(USART2, (uint8_t) 'q');
  GPIO_ResetBits(GPIOA,GPIO_Pin_1);
}

#endif
//-----------------------------------------------------------------