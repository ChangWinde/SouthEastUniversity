// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC/HID Driver)                |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: device.h, File Version 1.00 2009/12/03                             |
|  Author: RSTO                                                               |
|                                                                             |
|  Description:                                                               |
|  This file is included in other source code files                           |
|  and this only one place to change the included device header               |
+----------------------------------------------------------------------------*/


#include <msp430.h>

#if defined (__MSP430F6638__) || defined (__MSP430F6637__) || defined (__MSP430F6636__) || \
	defined (__MSP430F6635__) || defined (__MSP430F6634__) || defined (__MSP430F6633__) || \
	defined (__MSP430F6632__) || defined (__MSP430F6631__) || defined (__MSP430F6630__) || \
	defined (__MSP430F5638__) || defined (__MSP430F5637__) || defined (__MSP430F5636__) || \
	defined (__MSP430F5635__) || defined (__MSP430F5634__) || defined (__MSP430F5633__) || \
	defined (__MSP430F5632__) || defined (__MSP430F5631__) || defined (__MSP430F5630__)
	#define __MSP430F563x_F663x
#elif defined (__MSP430F5510__) || defined (__MSP430F5509__) || defined (__MSP430F5508__) || \
	defined (__MSP430F5507__) || defined (__MSP430F5506__) || defined (__MSP430F5505__) || \
	defined (__MSP430F5504__) || defined (__MSP430F5503__) || defined (__MSP430F5502__) || \
	defined (__MSP430F5501__) || defined (__MSP430F5500__)
    #define __MSP430F550x
#elif defined (__MSP430F5529) || defined (__MSP430F5528__) || defined (__MSP430F5527__) || \
	defined (__MSP430F5526__) || defined (__MSP430F5525__) || defined (__MSP430F5524__) || \
	defined (__MSP430F5522__) || defined (__MSP430F5521__) || defined (__MSP430F5519__) || \
	defined (__MSP430F5517__) || defined (__MSP430F5515__) || defined (__MSP430F5514__) || \
	defined (__MSP430F5513__)
    #define __MSP430F552x
#else
    #error Define a constant of format __MSP430Fxxxx__ within the projects preprocessor settings,
    according to the device being used.
#endif
/*------------------------ Nothing Below This Line --------------------------*/
