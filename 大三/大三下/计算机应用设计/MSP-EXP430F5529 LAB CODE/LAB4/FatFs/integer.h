/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _INTEGER
#define _INTEGER

#ifdef _WIN32    /* FatFs development platform */

#include <windows.h>
#include <tchar.h>

#else            /* Embedded platform */

// Surpress warning for multiple defs of the same type.
// This is done so the FatFs can be a stanalone modular entity.
#ifdef __IAR_SYSTEMS_ICC__
#pragma diag_suppress=Pe301
#endif
#ifdef __TI_COMPILER_VERSION__
#pragma diag_suppress 303
#endif

/* These types must be 16-bit, 32-bit or larger integer */
typedef int             INT;
typedef unsigned int    UINT;

/* These types must be 8-bit integer */
typedef char             CHAR;
typedef unsigned char    UCHAR;
typedef unsigned char    BYTE;

/* These types must be 16-bit integer */
typedef short            SHORT;
typedef unsigned short   USHORT;
typedef unsigned int     WORD;
typedef unsigned short   WCHAR;

/* These types must be 32-bit integer */
typedef long             LONG;
typedef unsigned long    ULONG;
typedef unsigned long    DWORD;

#endif

#endif
