#ifndef __STD_H
#define __STD_H

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Common types definition
 */

#if defined(__TMS320C28X__)      || \
    defined(_TMS320C2XX)         || \
    defined(__IAR_SYSTEMS_ICC__) || \
    defined(AVRGCC)
typedef int             Int;
typedef unsigned        Uns;
#else
typedef short           Int;
typedef unsigned short  Uns;
#endif

typedef char        Char;
typedef char        Bool;
typedef char       *String;
typedef void       *Ptr;
typedef const char *CString;

/*
 *  8, 16, 32-bit type definitions
 *
 *  Sm* - 8-bit type
 *  Md* - 16-bit type
 *  Lg* - 32-bit type
 *
 *  *Int - signed type
 *  *Uns - unsigned type
 *  *Bits - unsigned type (bit-maps)
 */

typedef signed char    SmInt;   /* SMSIZE-bit signed integer */
typedef signed short   MdInt;   /* MDSIZE-bit signed integer */
typedef signed long    LgInt;   /* LGSIZE-bit signed integer */

typedef unsigned char  SmUns;   /* SMSIZE-bit unsigned integer */
typedef unsigned short MdUns;   /* MDSIZE-bit unsigned integer */
typedef unsigned long  LgUns;   /* LGSIZE-bit unsigned integer */

typedef unsigned char  SmBits;  /* SMSIZE-bit bit string */
typedef unsigned short MdBits;  /* MDSIZE-bit bit string */
typedef unsigned long  LgBits;  /* LGSIZE-bit bit string */

typedef signed long    Long;
typedef signed short   Short;

#if defined(__TMS320C28X__)      || \
    defined(_TMS320C2XX)         || \
    defined(__IAR_SYSTEMS_ICC__) || \
    defined(AVRGCC)
typedef SmBits         Byte;    /* smallest unit of addressable store */
#elif !defined(__cplusplus)
typedef unsigned char  Byte;
#endif

#define Void void

typedef void *Arg;
typedef Int (*Fxn)(void);       /* generic function type */

typedef void (*TIntFunc)(Arg);

typedef float 		Float;
typedef double 		Double;
typedef long double	Extended;

typedef char ShortString[21];
typedef char LongString[257];

#if !defined(__TMS320C28X__) && \
	!defined(_TMS320C2XX)
typedef short           int16;
typedef long            int32;
typedef unsigned short  Uint16;
typedef unsigned long   Uint32;
typedef float           float32;
typedef long double     float64;
#endif

typedef unsigned char   U8;
typedef unsigned short  U16;
typedef unsigned long   U32;
typedef signed   char   S8;
typedef signed   short  S16;
typedef signed   long   S32;

#define False			((Bool)0)
#define True			((Bool)1)

#if !defined(FALSE) && !defined(TRUE)
#define FALSE   		False
#define TRUE    		True
#endif

#if !defined(NULL)
#define NULL    		((Ptr)0)
#endif

#define null			NULL
#define Null			NULL

#ifndef __cplusplus
typedef Bool  		bool;
#define false			False
#define true			True
#endif

#define ArgToInt(A)     ((Int)((Long)(A) & 0xFFFF))
#define ArgToPtr(A)     ((Ptr)(A))

#define ToPtr(p)			 (Ptr)(p)
#define ToIntPtr(p)		 (Int *)(p)
#define ToInt(p)			*(Int *)(p)
#define ToUnsPtr(p)		 (Uns *)(p)
#define ToUns(p)			*(Uns *)(p)
#define ToLongPtr(p)	 (LgInt *)(p)
#define ToLong(p)		*(LgInt *)(p)
#define ToULongPtr(p)	 (LgUns *)(p)
#define ToULong(p)		*(LgUns *)(p)

#define _IQtoIQ0(A)		((LgInt)(A) >> GLOBAL_Q)
#define _IQ0toIQ(A)		((LgInt)(A) << GLOBAL_Q)

#define _IQNtoIQ0(A, N)	((LgInt)(A) >> N)
#define _IQ0toIQN(A, N)	((LgInt)(A) << N)

#if defined(__TMS320C28X__)      || \
    defined(_TMS320C2XX)         || \
    defined(__IAR_SYSTEMS_ICC__) || \
    defined(AVRGCC)
#define __inline	inline
#endif

#define __INLINE		__inline

#if defined(__IAR_SYSTEMS_ICC__) || \
    defined(AVRGCC)
#define __CODE    __flash
#else
#define __CODE
#endif

#define M_E         2.71828182845904523536
#define M_LOG2E     1.44269504088896340736
#define M_LOG10E    0.434294481903251827651
#define M_LN2       0.693147180559945309417
#define M_LN10      2.30258509299404568402
#define M_PI        3.14159265358979323846
#define M_2PI			6.283185307179586476925
#define M_PI_2      1.57079632679489661923
#define M_PI_3		1.0471975511965977461542
#define M_PI_4      0.785398163397448309616
#define M_2PI_3		2.094395102393195492308
#define M_1_PI      0.318309886183790671538
#define M_2_PI      0.636619772367581343076
#define M_1_SQRTPI  0.564189583547756286948
#define M_2_SQRTPI  1.12837916709551257390
#define M_1DIV3		0.333333333333333333333
#define M_2DIV3		0.666666666666666666666
#define M_SQRT_2    1.41421356237309504880
#define M_SQRT_3		1.732050807568877293527
#define M_SQRT_6		2.449489742783178098197
#define M_SQRT_3_2	0.8660254037844386467637
#define M_SQRT_2_3	0.8164965809277260327324
#define M_1_SQRT2	0.707106781186547524401
#define M_1_SQRT3	0.577350269189625764509

#ifdef __cplusplus
}
#endif // extern "C"

#endif



