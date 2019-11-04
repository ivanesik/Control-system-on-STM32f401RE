#ifndef __MATHEMATH_H
#define __MATHEMATH_H

#include "global/std.h"

__inline Uns rounding (Uns numerator, Uns denumerator);

__inline Uns rounding (Uns numerator, Uns denumerator)
{
  Uns c = numerator / denumerator;
  
  if (numerator % denumerator > 5)
    c++;
  
  return c;
}

#endif