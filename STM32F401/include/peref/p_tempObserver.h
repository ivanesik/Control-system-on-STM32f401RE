#ifndef __TEMPERATURE_OBSERVER_H
#define __TEMPERATURE_OBSERVER_H

#include "config.h"

#define ADC_TEMP_CHANNEL_NUM 4

#define TEMP_ERROR_NONE                 0;
#define TEMP_ERROR_NONE_CONNECTION      1;
#define TEMP_ERROR_SHORT_CIRCUIT        2;


void TempObserverUpdate (void);




#endif