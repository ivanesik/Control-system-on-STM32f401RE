// -----------------------------------
// InitHardware
// Модуль конфигурации оборудования
// -----------------------------------
#include "global/g_hardware.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

void InitClock(void);
void InitTimer(void);

void InitGpio(void);
void InitSpi(void);
void InitUart(void);
void InitAdc(void);

// для передачи с ADC потому что данные пишутся
// в 1 регистр и перетирают друг друга
// (можно injected channel, но их всего 4, а каналов 6)
void InitDma(void); 

__IO uint16_t ADC1ConvertedValue = 0;

void InitHardware(void)
{
    InitClock();
    InitTimer();

    InitGpio();
    InitUart();
    InitDma();
    InitAdc();

    InitSpi();
}

//настройка шин тактирования (всё по схеме, шины на максимальной частоте)
void InitClock(void)
{
    RCC_PLLCmd(DISABLE); //выключает PLL, со включенный нельзя настраивать

    do
    {
        RCC_HSEConfig(RCC_HSE_ON);      //включаю внешний источник
    } while (!RCC_WaitForHSEStartUp()); //ожидание включения

    RCC_PLLConfig(RCC_PLLSource_HSE, //внешний генератор
                  8,                 //делитель M PLL (все эти контанты на схеме PLL)
                  84,                //множитель N PLL  = 16*84/(8*2)=84 МГц
                  2,                 //делитель P для главных SYSCLK (2,4,6,8)
                  4);                //(не используется) делитель Q для USB, OTG FS, SDIO, RNG clocks (4-15)

    do
    {
        RCC_PLLCmd(ENABLE);
    } while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET);

    //  RCC_HSICmd(DISABLE);  //выключаю внутренний генератор

    RCC_HCLKConfig(RCC_SYSCLK_Div1); //делитель перед шинами
    RCC_PCLK1Config(RCC_HCLK_Div2);  //делитель перед переферийной шиной 1 (медленная, чтобы 42МГц (максимальная))
    RCC_PCLK2Config(RCC_HCLK_Div1);  //делитель перед переферийной шиной 2 (быстрая 84 МГц максимально)

    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); // часы от осцилятора 32.768 КГц
    RCC_RTCCLKCmd(ENABLE);                  // включение часов RTC

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // SW - выбор тактирования в PLL (можно HSI, HSE)
    RCC_HSICmd(DISABLE);
    SystemCoreClockUpdate();
}

//настройка таймера 2 (к шине )
void InitTimer(void)
{
    // NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;

    //таймер к шине APB2 timer clock = 84 MHz
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_BaseInitStructure.TIM_Prescaler = (uint16_t)(SystemCoreClock / 1000000) - 1; //делитель на частоту 1МГц
    TIM_BaseInitStructure.TIM_Period = 55;                                           // =1МГц/18КГц = 55
    TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;                      // от нуля до TIM_Period
    TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure);

    //прерывание таймера 2
    /*  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  */

    //включение прерывания
    NVIC_EnableIRQ(TIM2_IRQn);
    //разрешение прерывания
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    //запуск таймера
    TIM_Cmd(TIM2, ENABLE);
}

void InitGpio(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_DeInit(GPIOA);
    GPIO_DeInit(GPIOB);
    GPIO_DeInit(GPIOC);
    GPIO_DeInit(GPIOD);

    // Enable the GPIOA peripheral
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    //Input_Output Ports
    //PA-----------------------------------------------------------------------------
    //PA0 - STOP_24 - из АСУ ТП активный уровень 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;        //какой пин в порте
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;     //режим - Вход
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // скорость
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   //двух тактный(1,0) (ещё есть с окрытым истоком)
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // указывается поддяжка вывода
    GPIO_Init(GPIOA, &GPIO_InitStructure);           //запись
                                                     //  GPIO_WriteBit(GPIOA,GPIO_Pin_0,0);
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);               //начальное значение

    //PA1 - UART RTS
    //PA2 - UART Tx
    //PA3 - UART Rx
    //PA4 - SPI NSS
    //PA5 - SPI SCK
    //PA6 - SPI MISO
    //PA7 - SPI MOSI

    //PA8 - резерв, аппаратно подтянут к 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //Поддяжка к 1
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //PA9 - резерв, аппаратно подтянут к 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //Поддяжка к 1
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //PA10 - ZV_OP - Звуковой опевещатель, инициализировать в 0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //нет поддяжки
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOA,GPIO_Pin_10,0);
    GPIO_ResetBits(GPIOA, GPIO_Pin_10);

    //PA11 - DPR - Сигнал датчика (сигнализатора) протечки, при протечке 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOA,GPIO_Pin_11,0);
    GPIO_ResetBits(GPIOA, GPIO_Pin_11);

    //PA12 - DVR - сигнал датчика вращения, миандр частотой 10-1000 Гц
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOA,GPIO_Pin_12,0);
    GPIO_ResetBits(GPIOA, GPIO_Pin_12);

    //PB--------------------------------------------------------------------------
    //PB0 - ADC - 420DVB - резервный сигнал датчика вибрации
    //PB1 - ADC - V_220  напряжение на БКС

    //PB2 - PUSK_KON - включение силового контактора
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOB,GPIO_Pin_2,0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_2);

    //PB5 - резерв, аппаратно подтянут к 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //Поддяжка к 1
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //PB6 - K1_BKS_RAB - Сигнал что включен первый контактор реверсивного пускателя БКС
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //нет поддяжки
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOB,GPIO_Pin_6,0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);

    //PB7 - K2_BKS_RAB - Сигнал что включен второй контактор реверсивного пускателя БКС
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOB,GPIO_Pin_7,0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_7);

    //PB8 - K_VIK1 - Сигнал с концевого выключателя 1 привода поворота
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOB,GPIO_Pin_8,0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_8);

    //PB9 - K_VIK2 - Сигнал с концевого выключателя 2 привода поворота
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOB,GPIO_Pin_9,0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_9);

    //PB10 - STOP_KON - Отключение силового контактора
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOB,GPIO_Pin_10,0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_10);

    //PB12 - AVAR_ASU - Сиганал об аварии, активный уровень 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOB,GPIO_Pin_12,0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);

    //PB13 - RAB_ASU - Сигнал о работе, активный уровень 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOB,GPIO_Pin_13,0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_13);

    //PB14 - резерв, аппаратно подтянут к 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //PB15 - резерв, аппаратно подтянут к 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //поддяжка к 1
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //PC---------------------------------------------------------------
    //PС0 - ADC - 420DVB - резервный сигнал датчика вибрации
    //PС1 - ADC - V_220  напряжение на БКС

    //PC2 - PUSK_220  - из АСУ ТП активный уровень 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //нет поддяжки
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOC,GPIO_Pin_2,0);
    GPIO_ResetBits(GPIOC, GPIO_Pin_2);

    //PC3 - PUSK_24 - из АСУ ТП активный уровень 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOC,GPIO_Pin_3,0);
    GPIO_ResetBits(GPIOC, GPIO_Pin_3);

    //PC4 - ADC - TMP_ED1
    //PC5 - ADC - TMP_P1

    //PC6 - резерв, аппаратно подтянут к 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //PC8 - CPU_LED - Зажигает диод на плате, Активный уровень 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOC,GPIO_Pin_8,0);
    GPIO_ResetBits(GPIOC, GPIO_Pin_8);

    //PC10 - SV_OP - Световая сигнализ. инициализировать в 0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOC,GPIO_Pin_10,0);
    GPIO_ResetBits(GPIOC, GPIO_Pin_10);

    //PC11 - K2_BKS_ON - Сигнал замыкания второго контактора
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOC,GPIO_Pin_11,0);
    GPIO_ResetBits(GPIOC, GPIO_Pin_11);

    //PC12 - K1_BKS_ON - Сигнал замыкания первого контактора
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOC,GPIO_Pin_12,0);
    GPIO_ResetBits(GPIOC, GPIO_Pin_12);

    //PD-------------------------------------------------------------------------
    //STOP_220 (PD2) - из АСУ ТП активный уровень 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    //GPIO_WriteBit(GPIOD,GPIO_Pin_2,0);
    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
}

void InitSpi(void)
{
    //SPI
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    // Тактирование модуля SPI1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; //CLK, MOSI,MISO
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Настраиваем ноги SPI1 для работы в режиме альтернативной функции
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1); // SCK
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1); // MISO
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1); // MOSI

    SPI_DeInit(SPI1);
    //Заполняем структуру с параметрами SPI модуля
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //передача в обе стороны
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                        // Режим - Ведомый
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                    // передаем по 8 бит
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                           // Полярность (уровень тишины = Low) и
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                         // фаза тактового сигнала (по первому фрону SCK)
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                            // Управлять состоянием сигнала NSS программно (бит SSI)
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // Предделитель SCK
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                   // Первым отправляется старший бит
    SPI_InitStructure.SPI_CRCPolynomial = 7;                             // Не юзаю CRC
    SPI_Init(SPI1, &SPI_InitStructure);                                  //Настраиваем SPI1

    NVIC_EnableIRQ(SPI1_IRQn);                   //Разрешаем прерывания от SPI1
    SPI_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE); // прерывание SPI по приему
                                                 //  SPI_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE); // прерывание SPI по передаче

    //  SPI_CalculateCRC(SPI1, DISABLE);

    // Включаем модуль SPI1....
    SPI_Cmd(SPI1, ENABLE);

    //Устанавливает/сбрасывает (мастер/ведомый) бит SSI (програмная имитация вывода NSS)
    SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
}

void InitUart(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    //Тактирование UART
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    //Init пинов
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //UP
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //RTS - вручную, управление режимом приема (0) и передачи (1)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);

    //Подключаю пины к UART (потому что для UART1 есть несколько
    //                        дублирующих выводов, надо указать)
    //  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1 ,GPIO_AF_USART2); //RTS
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //RX

    //Init для самого UART
    USART_InitStructure.USART_BaudRate = UART_SPEED; //Максимально 2625000 с 16 бит
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_2;                          // По протоколу 2 стоп бита
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // Нету четности
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // Нету управления потоком
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    //вкл прерывание UART
    NVIC_EnableIRQ(USART2_IRQn);
    //прерывание на прием и передачу
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //убрал потому что при запуске происходит прерывание по приему
    USART_ITConfig(USART2, USART_IT_TC, ENABLE);

    //Вкл UART
    USART_Cmd(USART2, ENABLE);
}

///ADC1 т.к. в STM32f401RE нету ADC2, ADC3
void InitAdc(void)
{
    ADC_InitTypeDef ADC_InitStricture;
    ADC_CommonInitTypeDef ADC_CommonInitStruct;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //включаем тактирование ацп1

    ADC_DeInit(); //сбрасываем настройки АЦП по умолчанию

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;           //режим - аналоговый вход (АЦП)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //ножка
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure); //записать настройки

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2; //предделитель частоты ADC
    ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStruct);

    //Сам АЦП
    ADC_InitStricture.ADC_Resolution = ADC_Resolution_12b;                  //12 бит
    ADC_InitStricture.ADC_ScanConvMode = ENABLE;                            //несколько каналов (DISABLE 1 канал)
    ADC_InitStricture.ADC_ContinuousConvMode = ENABLE;                      //непрерывное проеобразование
    ADC_InitStricture.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None; //не использовать триггер
    ADC_InitStricture.ADC_DataAlign = ADC_DataAlign_Right;                  //выравнивание результата по правому краю
    ADC_InitStricture.ADC_NbrOfConversion = 6;                              //использовать 6 каналов АЦП

    //запись настроек
    ADC_Init(ADC1, &ADC_InitStricture);

    //настройка каналов АЦП (ADC_SampleTime_3Cycles чем больше тем точнее, но медленее)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_3Cycles);  // TMP_P2
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_3Cycles);  // TMP_T
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 3, ADC_SampleTime_3Cycles); // TMP_ED1
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 4, ADC_SampleTime_3Cycles); // TMP_P1
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 5, ADC_SampleTime_3Cycles); // DVB
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 6, ADC_SampleTime_3Cycles); // V_220

    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

    ADC_DMACmd(ADC1, ENABLE);

    //ADC_ContinuousModeCmd(ADC1,ENABLE);

    ADC_Cmd(ADC1, ENABLE); //включить АЦП
    ADC_SoftwareStartConv(ADC1);
}

// Настройка DMA канала для передачи по //по мануалу ADC1 к первому каналу DMA
void InitDma(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    DMA_InitTypeDef DMA_InitStructure;

    DMA_InitStructure.DMA_Channel = DMA_Channel_0;                              //ADC1 висит на первом канале DMA
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;             //откуда данные
    DMA_InitStructure.DMA_Memory0BaseAddr = (Uns)&g_RomData.AdcDataPointStruct; //Куда пишем
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                     //передача с переферии в память (1 из 3 режимов DMA)
    DMA_InitStructure.DMA_BufferSize = 6;                                       //сколько данных
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            // Все время из АЦП1
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     // инкрементирование адреса памяти
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         // размер кадра буфера 16 бит
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             // Циклический режим
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         // Высокий приоритет
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                      // Fifo не используется
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;           // относится к Fifo
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                 // Отноосится к пакетной отсылке
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure); //по мануалу ADC1 к нулевому потоку каналу DMA

    // DMA2_Stream0 enable
    DMA_Cmd(DMA2_Stream0, ENABLE);
}