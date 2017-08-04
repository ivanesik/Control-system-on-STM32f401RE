
#ifndef MATHEMATH_H
#define MATHEMATH_H

#include "std.h"


__inline Uns rounding (Uns numerator, Uns denumerator)
{
  Uns c = numerator / denumerator;
  
  if (numerator % denumerator > 3)
    c++;
  
  return c;
}

#endif