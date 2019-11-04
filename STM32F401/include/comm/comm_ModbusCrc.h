#ifndef __COMM_MODBUS_CRC_H
#define __COMM_MODBUS_CRC_H

#define INIT_CRC    0xFFFF
#define GOOD_CRC    0x0000
#define GENER_CRC   0xA001

Uns CrcTable[256];

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

#endif