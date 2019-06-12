// (c)2010 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
  |                                                                             |
  |                              Texas Instruments                              |
  |                                                                             |
  |                          MSP430 USB-Example (MSC  Driver)                   |
  |                                                                             |
  +-----------------------------------------------------------------------------+
  |  Source: Usb_Msc_Req.h, File Version 1.0                                    |
  |  Description: This file contains function declarations of MSC class specific|
  |               Requests.                                                     |
  |  Author: MSP                                                                |
  |                                                                             |
  |  WHO          WHEN         WHAT                                             |
  |  ---          ----------   ------------------------------------------------ |
  |  MSP          01/02/2010   born                                             |
  +----------------------------------------------------------------------------*/
#ifndef _USB_MSC_REQ_H_
#define _USB_MSC_REQ_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* MSC Class defined Request.Reset State-Machine and makes endpoints ready again*/
VOID USBMSC_reset(VOID);

/* MSC Class defined Request.Tells the host the number of supported logical units*/
VOID Get_MaxLUN(VOID);

#ifdef __cplusplus
}
#endif
#endif //_USB_MSC_REQ_H_

