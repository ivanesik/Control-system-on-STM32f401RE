#ifndef __COMM_MODBUS_CRC_H
#define __COMM_MODBUS_CRC_H

#define INIT_CRC    0xFFFF
#define GOOD_CRC    0x0000
#define GENER_CRC   0xA001

//__inline Uns crc_chk(Byte* buf, Uns len);


Uns CrcTable[256];

//-------------------------------------------------------------------------------
__inline void GenerateCrcTable(void)
{
  Uns i, j, Crc;
  for (i=0; i < 256; i++)
  {
    Crc = i;
    for (j=0; j < 8; j++)
    {
      if (Crc & 1) Crc = (Crc >> 1) ^ GENER_CRC;
      else Crc = (Crc >> 1);
    }
    CrcTable[i] = Crc;
  }
}


//-------------------------------------------------------------------------------
__inline Uns CalcFrameCrc(Byte *Buf, Uns Count)
{
  Uns Crc = INIT_CRC;
  
  do 
  {
    Crc = (Crc >> 8) ^ CrcTable[(Crc ^ (Uns)*Buf++) & 0x00FF];
  }
  while (--Count);
  
  return Crc;
}

/*
__inline Uns crc_chk(Byte* data, Uns length)
{
  Uns j; 
  Uns reg_crc=INIT_CRC; 
  while(length--)
  { 
    reg_crc ^= *data++; 
    for (j=0; j<8; j++)
    {
      if(reg_crc & 0õ01) 
        reg_crc=(reg_crc>>1) ^ GENER_CRC; // LSB(b0)=1 
      else 
        reg_crc=reg_crc>>1; 
    }
  }
  return reg_crc; 
}

__inline Uns crc_chk(Byte* buf, Uns len)
{
  Uns crc = INIT_CRC;

  for (int pos = 0; pos < len; pos++) 
  {
    crc ^= *buf++;

    for (int i = 8; i != 0; i--) 
    {
      if ((crc & 0x0001) != 0) 
      {
        crc >>= 1;
        crc ^= GENER_CRC;
      }
      else
        crc >>= 1;
    }
  }
  return crc;
}*/

#endif