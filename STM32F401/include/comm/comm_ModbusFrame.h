#ifndef __MODBUS_FRAME_H
#define __MODBUS_FRAME_H

#include "comm/comm_ModbusRtu.h"
#include "peref/p_vibrIvd3.h"

__inline void WordPack(Byte *Buf, Uns Data);
__inline void CrcPack(TMbPort *);
static void DataUnPack(Byte *Buf, Uns *Data, Byte Count);


static void ReadRegsConfirmation(TMbPort *hPort)
{
  Byte *Buf = hPort->Frame.Buf;
  if (hPort->Frame.RxLength != (Buf[2] + 5))
  {
    hPort->Packet.Exception = FR_ERR_BAD_RESPONSE_LENGTH;
    return;
  }
  if (Buf[2] != (hPort->Packet.Count << 1))
  {
    hPort->Packet.Exception = FR_ERR_BAD_DATA_NUM;
    return;
  }
 // DataUnPack(&Buf[3], hPort->Packet.Data, hPort->Packet.Count);
  
  DVibrDataUnpack(Buf);
  
}



//-------------------------------------------------------------------------------
__inline void ReadRegsRequest(TMbPort *hPort)
{
  Byte *Buf = hPort->Frame.Buf;
  if (!hPort->Packet.Count || (hPort->Packet.Count > 210))
  {
    hPort->Packet.Exception = FR_ERR_BAD_DATA_NUM;
    return;
  }
  Buf[0] = hPort->Params.Slave;
  Buf[1] = hPort->Packet.Request;
  WordPack(&Buf[2], hPort->Packet.Addr);
  WordPack(&Buf[4], hPort->Packet.Count);
  hPort->Frame.TxLength = 6;
}


//-------------------------------------------------------------------------------
__inline void WriteRegRequest(TMbPort *hPort)
{
  Byte *Buf = hPort->Frame.Buf;
  Buf[0] = hPort->Params.Slave;
  Buf[1] = MB_WRITE_REG;
  WordPack(&Buf[2], hPort->Packet.Addr);
  WordPack(&Buf[4], hPort->Packet.Data[0]);
  hPort->Frame.TxLength = 6;
}



__inline void MasterPacketConstructor (TMbPort *hPort, Byte Slave, Byte Request, Uns  Addr, Uns  Count)
{
  TMbPacket *Packet = &hPort->Packet;
  
  hPort->Params.Slave = Slave;
  Packet->Request = Request;
  Packet->Addr = Addr;
  Packet->Count = Count;
}



__inline void WordPack(Byte *Buf, Uns Data)
{
  Buf[0] = (Byte)(Data >> 8);
  Buf[1] = (Byte)(Data & 0xFF);
}


static void DataUnPack(Byte *Buf, Uns *Data, Byte Count)
{
  do
  {
    *Data  = (Uns)(*Buf++) << 8;
    *Data |= (Uns)(*Buf++);
    Data++;
  } while (--Count);
}


__inline void CrcPack(TMbPort *hPort)
{
  Uns Crc = CalcFrameCrc(hPort->Frame.Buf , hPort->Frame.TxLength);

  hPort->Frame.Buf[hPort->Frame.TxLength++] = (Byte)(Crc & 0xFF);
  hPort->Frame.Buf[hPort->Frame.TxLength++] = (Byte)(Crc >> 8);
  if (hPort->Params.Mode==0) if (hPort->Frame.Buf[1]!=16)
    hPort->Frame.Buf[7]=0;
}



__inline void frame_error (TMbPort *hPort)
{
  TMbStat  *Stat  = &hPort->Stat;
  
  Stat->BusErrCount++;

  if (hPort->Frame.Buf[0]==0) hPort->Frame.Buf[0]=1;
  if (hPort->Frame.Buf[1]!=3 || hPort->Frame.Buf[1]!=16) hPort->Frame.Buf[1]=3;
  hPort->Packet.Exception = hPort->Frame.Buf[1];
  hPort->Packet.Response = EX_SLAVE_DEVICE_FAILURE;
  
}





#endif