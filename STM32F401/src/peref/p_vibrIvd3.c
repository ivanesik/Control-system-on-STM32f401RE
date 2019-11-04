#include "peref/p_vibrIvd3.h"

void DVibrDataUnpack(Byte *Buf)
{
    Uns Z = (Buf[5] << 24) | (Buf[6] << 16) | (Buf[3] << 8) | Buf[4];
    Uns X = (Buf[9] << 24) | (Buf[10] << 16) | (Buf[7] << 8) | Buf[8];
    Uns Y = (Buf[13] << 24) | (Buf[14] << 16) | (Buf[11] << 8) | Buf[12];

    g_RomData.Vibr.Z_VibrData = Z;
    g_RomData.Vibr.X_VibrData = X;
    g_RomData.Vibr.Y_VibrData = Y;

    Uns s = Buf[5] >> 7;
    Uns p = ((Buf[5] & 0x7F) << 1 | (Buf[6] >> 7)) - 127;
    Uns m = ((Buf[6] << 16) | (Buf[3] << 8) | Buf[4]) & 0x7FFFFF;
    g_RomData.Vibr.Z_Vibr = pow((-1), s) * pow(2, p) * (1 + m / pow(2, 23));

    s = Buf[9] >> 7;
    p = ((Buf[9] & 0x7F) << 1 | (Buf[10] >> 7)) - 127;
    m = ((Buf[10] << 16) | (Buf[7] << 8) | Buf[8]) & 0x7FFFFF;
    g_RomData.Vibr.X_Vibr = pow((-1), s) * pow(2, p) * (1 + m / pow(2, 23));

    s = Buf[13] >> 7;
    p = ((Buf[13] & 0x7F) << 1 | (Buf[14] >> 7)) - 127;
    m = ((Buf[14] << 16) | (Buf[11] << 8) | Buf[12]) & 0x7FFFFF;
    g_RomData.Vibr.Y_Vibr = pow((-1), s) * pow(2, p) * (1 + m / pow(2, 23));
}
