#ifndef __VIBR_IVD3_H
#define __VIBR_IVD3_H

#include "global/std.h"
#include "global/g_rom.h"
#include <math.h>

//Modbus
#define DVIBR_SLAVE_ID 1
#define DVIBR_DATA_ADDR 0
#define DVIBR_REGS_COUNT 6


void DVibrDataUnpack(Byte *Buf);



#endif