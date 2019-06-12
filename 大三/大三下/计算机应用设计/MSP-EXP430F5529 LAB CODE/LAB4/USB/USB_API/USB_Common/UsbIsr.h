// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC/HID Driver)                |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: UsbIsr.h, File Version 1.00 2009/12/03                             |
|  Author: RSTO                                                               |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  RSTO         2008/09/03   born                                             |
|  RSTO         2008/12/23   enhancements of CDC API                          |
+----------------------------------------------------------------------------*/

#ifndef _ISR_H_
#define _ISR_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
Handle incoming setup packet.
returns TRUE to keep CPU awake
*/
BYTE SetupPacketInterruptHandler(VOID);

/**
Handle VBuss on signal.
*/
VOID PWRVBUSonHandler(VOID);

/**
Handle VBuss off signal.
*/
VOID PWRVBUSoffHandler(VOID);

/**
Handle In-requests from control pipe.
*/
VOID IEP0InterruptHandler(VOID);

/**
Handle Out-requests from control pipe.
*/
VOID OEP0InterruptHandler(VOID);

/*----------------------------------------------------------------------------+
| End of header file                                                          |
+----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif /* _ISR_H_ */

/*------------------------ Nothing Below This Line --------------------------*/
