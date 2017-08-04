#ifndef __COMM_H
#define __COMM_H

#include "comm_SerialComm.h"
#include "comm_ModbusRtu.h"


typedef struct _TComm{

  TMbPort       mbDVibr;
  TPpData       spiTSM;
}TComm;
//--------------------------------------------------------

void Comm_Init(TComm *);
void Comm_Update(TComm *);



extern TComm    g_Comm;

#endif