// (c)2010 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
  |                                                                             |
  |                              Texas Instruments                              |
  |                                                                             |
  |                          MSP430 USB-Example (MSC Driver)                    |
  |                                                                             |
  +-----------------------------------------------------------------------------+
  |  Source: UsbMsc.h, File Version 1.01                                        |
  |  Description: This file contains API declarations for function to use by    |
  |               User Application.                                             |
  |  Author: RSTO                                                               |
  |                                                                             |
  |  WHO          WHEN         WHAT                                             |
  |  ---          ----------   ------------------------------------------------ |
  |  RSTO         2010/10/29   Created                                          |
  +----------------------------------------------------------------------------*/
#ifndef _USB_MSC_H_
#define _USB_MSC_H_

#include "UsbMscScsi.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*Return values of getState() and USBMSC_poll() API */
#define kUSBMSC_idle               0
#define kUSBMSC_readInProgress     1
#define kUSBMSC_writeInProgress    2
#define kUSBMSC_cmdBeingProcessed  3
#define kUSBMSC_okToSleep          4
#define kUSBMSC_processBuffer      5

/*----------------------------------------------------------------------------+
| Function Prototypes                                                         |
+----------------------------------------------------------------------------*/
/*Function to handle the MSC SCSI state machine */
BYTE USBMSC_poll(VOID);

/* MSC functions */
BOOL MSCToHostFromBuffer();
BOOL MSCFromHostToBuffer();
BYTE USBMSC_bufferProcessed(VOID);
BYTE USBMSC_getState();
BYTE USBMSC_updateMediaInfo(BYTE lun, struct USBMSC_mediaInfoStr *info);

BYTE USBMSC_handleBufferEvent(VOID);
BYTE USBMSC_registerBufInfo( BYTE* RWbuf_x, BYTE* RWbuf_y, WORD size);

#ifdef __cplusplus
}
#endif
#endif //_USB_MSC_H_
