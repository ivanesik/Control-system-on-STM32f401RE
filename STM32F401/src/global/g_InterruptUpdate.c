
#include "global/g_InterruptUpdate.h"
#include "peref/p_tempObserver.h"
#include "peref/p_voltControl.h"

#if defined(DEBUG)
void test_SPI(void);
void test_UART(void);

Byte test_data_Spi;
#endif

Uns count18kHz = 0, // Счетчик прерываний частотой 18 кГц
    count2kHz = 0,  // Счетчик прерываний частотой 2  кГц
    count200Hz = 0, // Счетчик прерываний частотой 200 Гц
    count50Hz = 0;  // Счетчик прерываний частотой 10  Гц

// Функция обработки прерываний
void InterruptUpdate(void)
{
    // Сброс бита переполнения
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    // Cчетчик прерывания разной частоты
    count18kHz++;
    if (count18kHz == 9)
    {
        count18kHz = 0;
        count2kHz++;

        SerialCommTimings(&g_Comm.mbDVibr);
        VoltObserverUpdate();

        if (count2kHz == 2)
        {
            count2kHz = 0;
            count200Hz++;

            if (count200Hz == 4)
            {
                count200Hz = 0;
                count50Hz++;

                if (count50Hz == 5)
                {
                    count50Hz = 0;
                    TempObserverUpdate();

#if defined(DEBUG)
                    test_SPI();
#endif
                }
            }
        }
    }
}

// Функции отладки
#if defined(DEBUG)

void test_SPI(void)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        NULL;
    SPI_I2S_SendData(SPI1, 0xAD);
}

void test_UART(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_1);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
        NULL; // ждет окончания передачи
    USART_SendData(USART2, (uint8_t)'q');
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}

#endif
