#include "peref/p_voltControl.h"


void VoltObserverUpdate(void)
{
    signed int j;
    unsigned int i, num;

    unsigned int sum = 0;

    MdBits *Data = g_RomData.AdcDataBufStruct.V_220;
    MdBits *Point = &g_RomData.AdcDataPointStruct.V_220;
    MdBits *Average = &g_RomData.AdcDataAverageStruct.V_220;

    for (i = 0; i < ADC_VOLT_CHANNEL_NUM; i++)
    {
        for (j = ADC_VOLT_DATA_ROM_SIZE - 2; j > -2; j--)
        {
            Data[j + 1] = Data[j];
        }
        Data[0] = *Point;

        num = 0;
        sum = 0;

        // Вычисление среднего после сдвига и записи нового
        for (j = 0; j < ADC_VOLT_DATA_ROM_SIZE; j++)
        {
            if (Data[j] != 0)
            {
                sum = sum + Data[j]; //sum += Data[j];
                num++;
            }
        }
        *Average = sum / num;

        Data = Data + ADC_VOLT_DATA_ROM_SIZE; //Data += ADC_DATA_ROM_SIZE;
        Point++;
        Average++;
    }
}