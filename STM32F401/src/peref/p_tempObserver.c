
#include "peref/p_tempObserver.h"



MdInt Temp_from_ADC_Conversion (MdBits ADC_Value);
Float Temp_from_R_Conversion (Float R_Value);
Float R_from_ADC_Conversion (MdBits ADC_Value);

//обновление сдвигового буфера аналоговых каналов
void TempObserverUpdate (void)
{
  signed int j;
  unsigned int i, num;
  unsigned int sum = 0;
 // Float Temp_Resist;
   
  MdBits *Data = g_RomData.AdcDataBufStruct.TMP_P2;
  MdBits *Point = &g_RomData.AdcDataPointStruct.TMP_P2;
  MdBits *Average = &g_RomData.AdcDataAverageStruct.TMP_P2;
  Byte *State = &g_RomData.AdcChannelState.TMP_P2;
  MdInt *Temp = &g_RomData.TempStruct.Temp_P2;
  
  for (i =0; i < ADC_TEMP_CHANNEL_NUM; i++)
  {
    for (j = ADC_TEMP_DATA_ROM_SIZE-2; j > -2; j--)
    {
      Data[j+1] = Data[j];
    } 
    Data[0] = *Point;
  
    num = 0;
    sum=0;
    
 //вычисление среднего после сдвига и записи нового
    for(j = 0; j < ADC_TEMP_DATA_ROM_SIZE; j++)
    {
      if (Data[j] != 0)
      {
        sum = sum + Data[j]; //sum += Data[j];
        num++;
      }
    }    
    *Average = sum / num;
    
    
    
    if (*Average > 2700)
      {*State = TEMP_ERROR_NONE_CONNECTION;}
    else if (*Average < 100)
      {*State = TEMP_ERROR_SHORT_CIRCUIT;}
    else
      {*State = TEMP_ERROR_NONE;}
    
//    Temp_Resist = R_from_ADC_Conversion(*Average);
//    *Temp = Temp_from_R_Conversion(Temp_Resist);
    *Temp = Temp_from_ADC_Conversion(*Average);
    
    Data = Data + ADC_TEMP_DATA_ROM_SIZE; //Data += ADC_DATA_ROM_SIZE;
    Point++;
    Average++;
    Temp++;
    State++;
  }
}


MdInt Temp_from_ADC_Conversion (MdBits ADC_Value)
{
  MdInt Temp;
  
  if (ADC_Value < 1733)
    Temp = 0.1863 * ADC_Value - 286.49;
  else
    Temp = 0.3281 * ADC_Value - 534.47;
  
  return Temp;
}