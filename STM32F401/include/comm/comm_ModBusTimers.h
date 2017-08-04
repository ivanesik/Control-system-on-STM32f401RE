/*======================================================================
Имя файла:          csl_timer.h
Автор:              
Версия файла:       01.02
Дата изменения:		16/02/10
Описание:
Заголовочный файл для работы с таймерами
======================================================================*/

#ifndef __CSL_TIMER_H
#define __CSL_TIMER_H

#include "global/std.h"


typedef struct _TTimerList
{
  Uns Counter, Timeout;
} TTimerList;


#define StopMbTimer(Timer)	(Timer)->Counter = 0
#define StartMbTimer(Timer)	(Timer)->Counter = (Timer)->Timeout


__inline void SetMbTimeout(TTimerList *Timer, Uns Timeout)
{
  if (!Timeout) Timeout = 1;
  Timer->Timeout = Timeout;
}


__inline void InitMbTimer(TTimerList *Timer, Uns Timeout)
{
  StopMbTimer(Timer);
  SetMbTimeout(Timer, Timeout);
}


__inline Bool TimerMbPending(TTimerList *Timer)
{
  if (Timer->Counter > 0)
  {
    Timer->Counter--;
    if (!Timer->Counter)
      return FALSE;
  }
return TRUE;
}


#endif