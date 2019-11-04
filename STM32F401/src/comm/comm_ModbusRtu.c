#include "comm/comm_ModbusRtu.h"
#include "comm/comm_ModbusEvent.h"
#include "comm/comm_ModbusConfig.h"
#include "comm/comm_ModbusTimers.h"
#include "comm/comm_ModbusCrc.h"
#include "comm/comm_ModbusFrame.h"

static Bool CrcTableGenFlag = false;

__inline void UpdateNewFrame(TMbPort *hPort);
__inline void MasterRequest(TMbPort *hPort);
__inline void MasterConfirmation(TMbPort *hPort);

void ModbusInit(TMbPort *hPort)
{
    if (!CrcTableGenFlag)
    {
        GenerateCrcTable();
        CrcTableGenFlag = true;
    }

    ResetCommumication(hPort, TRUE);
}

void ModbusInvoke(TMbPort *hPort)
{
    if (hPort->Frame.NewMessage) // ЧТЕНИЕ (если приходило сообщение и прошло 3.5)
    {
        UpdateNewFrame(hPort);
        hPort->Frame.NewMessage = FALSE; // сообщение принято, сброс флага нового сообщения
    }
    else // ЗАПРОС (если нету нового сообщения)
    {
        if (!hPort->Frame.WaitResponse)
            MasterRequest(hPort);
    }
}

// Функция формирования и отправки запроса от мастера
__inline void MasterRequest(TMbPort *hPort)
{
    TMbPacket *Packet = &hPort->Packet;

    //  MasterPacketConstructor(hPort, DVIBR_SLAVE_ID, MB_READ_REGS, DVIBR_DATA_ADDR, DVIBR_REGS_COUNT);

    if (!Packet->Request)
        return;

    Packet->Exception = 0;

    switch (Packet->Request) // формирование
    {
        case MB_READ_REGS:
            ReadRegsRequest(hPort);
            break;
        case MB_READ_INPUT_REGS:
            ReadRegsRequest(hPort);
            break;
        case MB_WRITE_REG:
            WriteRegRequest(hPort);
            break;
            //   case MB_WRITE_REGS:  WriteRegsRequest(hPort);break;
        default:
            Packet->Exception = EX_ILLEGAL_FUNCTION;
    }
    SendFrame(hPort);
}

__inline void UpdateNewFrame(TMbPort *hPort)
{

    TMbFrame *Frame = &hPort->Frame;
    TMbStat *Stat = &hPort->Stat;

    // hPort->Frame.WaitResponse = false;

    if (Frame->RxLength < 3)
    {
        Stat->BusFrameLenErrCount++;
        frame_error(hPort);
        return;
    }

    if (CalcFrameCrc(Frame->Buf, Frame->RxLength) != GOOD_CRC)
    {
        Stat->BusFrameCrcErrCount++;
        frame_error(hPort);
        return;
    }

    Stat->BusMsgCount++;

    if (IsMaster())
        MasterConfirmation(hPort);
    //  if (IsSlave())
    //    SlaveIndication(hPort);
}

__inline void MasterConfirmation(TMbPort *hPort)
{
    TMbPacket *Packet = &hPort->Packet;
    TMbFrame *Frame = &hPort->Frame;

    if ((Frame->Buf[1] & 0x7F) != Packet->Request)
    {
        Packet->Exception = FR_ERR_BAD_RESPONSE_FUNC;
    }
    else if (Frame->Buf[1] & 0x80)
    {
        if (Frame->RxLength != 5)
            Packet->Exception = FR_ERR_BAD_RESPONSE_LENGTH;
        else
        {
            Packet->Exception = Frame->Buf[2];
            hPort->Stat.BusExcCount++;
            switch (Packet->Exception)
            {
            case EX_SLAVE_DEVICE_BUSY:
                hPort->Stat.SlaveBusyCount++;
                break;
            case EX_NEGATIVE_ACKNOWLEDGE:
                hPort->Stat.SlaveNakCount++;
                break;
            }
        }
    }
    else
        switch (Packet->Request)
        {
        case MB_READ_REGS:
            ReadRegsConfirmation(hPort);
            break;
        case MB_READ_INPUT_REGS:
            ReadRegsConfirmation(hPort);
            break;
            //   case MB_WRITE_REG:   WriteRegConfirmation(hPort); break;
            //  case MB_WRITE_REGS:  WriteRegsConfirmation(hPort);     break;
            //   case MB_REPORT_ID:   ReportSlaveIdConfirmation(hPort); break;
        }

    if (Packet->Exception)
    {
        if (hPort->Stat.BusErrCount < 65500)
            hPort->Stat.BusErrCount++;
        if (hPort->Frame.RetryCounter > hPort->Params.RetryCount)
        {
            hPort->Stat.Status.bit.NoConnect = 1;
        }
        if (hPort->Frame.RetryCounter < 65500)
            hPort->Frame.RetryCounter++;
        hPort->Stat.Status.bit.Error = 1;
    }
    else
    {
        hPort->Stat.Status.bit.Error = 0;
        hPort->Stat.Status.bit.Ready = 1;
        hPort->Stat.Status.bit.NoConnect = 0;
        hPort->Stat.SlaveMsgCount++;
    }
}

void ModbusTimings(TMbPort *hPort)
{
    if (!TimerMbPending(&hPort->Frame.Timer1_5))
    {
        BreakFrameEvent(hPort);
    }
    if (!TimerMbPending(&hPort->Frame.Timer3_5))
    {
        NewFrameEvent(hPort);
    }
    if (!TimerMbPending(&hPort->Frame.TimerPre))
    {
        PreambleEvent(hPort);
    }
    if (!TimerMbPending(&hPort->Frame.TimerPost))
    {
        PostambleEvent(hPort);
    }
    /*  if (!TimerPending(&hPort->Frame.TimerConn))
  {
    ConnTimeoutEvent(hPort);
  }*/
    if (!TimerMbPending(&hPort->Frame.TimerAsk))
    {
        NoAskEvent(hPort);
    }
}

//// Handlers
// Обработка прерывания передачи
void ModbusTx(TMbPort *hPort)
{
    TMbFrame *Frame = &hPort->Frame;

    if ((Frame->Data - Frame->Buf) < Frame->TxLength)    //�������� ���� ��������� ��� �� ������ ������� ���������
        USART_SendData(USART2, (uint8_t)*Frame->Data++); //�������� ����� ������ ������
    else                                                 //����� ��������� ������ ����������
        StartMbTimer(&Frame->TimerPost);                 // = Timer3_5, ������ ������ ������ ��� ������ � ��������

    hPort->Stat.TxBytesCount++;
}

// Обработка прерывания приема
void ModbusRx(TMbPort *hPort)
{
    StopMbTimer(&hPort->Frame.TimerAsk);

    TMbFrame *Frame = &hPort->Frame;
    Byte Data = 0;

    Data = USART_ReceiveData(USART2);

    if ((Frame->Data - Frame->Buf) < 256)
        *Frame->Data++ = Data;

    StartMbTimer(&hPort->Frame.Timer3_5);
    StartMbTimer(&hPort->Frame.Timer1_5);

    hPort->Stat.RxBytesCount++;
}