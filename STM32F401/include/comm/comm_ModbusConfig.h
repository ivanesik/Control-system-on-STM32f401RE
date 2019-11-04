#ifndef __MODBUS_CONFIG_H
#define __MODBUS_CONFIG_H

#include "comm\comm_ModbusRtu.h"
#include "math\mathemath.h"
#include "peref\p_vibrIvd3.h"

//-------------------------------------------------------------------------------
static void ResetCommumication(TMbPort *hPort, Bool ClearEventLog)
{
    TMbParams *Params = &hPort->Params;
    TMbFrame *Frame = &hPort->Frame;
    Uns Tout1_5, Tout3_5;
    float TimerFriq = (Uns)Params->ModbusTimFriq;

    StopMbTimer(&Frame->Timer1_5);
    StopMbTimer(&Frame->Timer3_5);
    StopMbTimer(&Frame->TimerPre);
    StopMbTimer(&Frame->TimerPost);

    hPort->Packet.Request = 0;
    hPort->Packet.Response = 0;

    Frame->ListenMode = false;
    Frame->RetryCounter = 0;
    Frame->WaitResponse = false;
    Frame->NewMessage = false;

    Tout1_5 = rounding((15 * TimerFriq), Params->BaudRate); // 1.5 * 10 = 16.5  (10 - число бит)
    Tout3_5 = rounding((35 * TimerFriq), Params->BaudRate); // 3.5 * 10 = 38.5

    SetMbTimeout(&Frame->Timer1_5, Tout1_5);
    SetMbTimeout(&Frame->Timer3_5, Tout3_5);

    SetMbTimeout(&Frame->TimerPre, Params->TxDelay);
    SetMbTimeout(&Frame->TimerPost, Tout3_5);
    SetMbTimeout(&Frame->TimerAsk, Params->AckTimeout); // Ожидание AskTimeout таймера 2кГц

    MasterPacketConstructor(hPort, DVIBR_SLAVE_ID, MB_READ_INPUT_REGS, DVIBR_DATA_ADDR, DVIBR_REGS_COUNT);
}

#endif
