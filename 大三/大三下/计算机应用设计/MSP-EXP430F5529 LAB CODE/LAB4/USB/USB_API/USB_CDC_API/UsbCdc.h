// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC Driver)                    |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: UsbCdc.h, File Version 1.00 2009/12/03                             |
|  Author: RSTO                                                               |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  RSTO         2008/09/03   born                                             |
|  RSTO         2008/12/23   enhancements of CDC API                          |
|  RSTO         2009/05/15   added param to USBCDC_rejectData()               |
|  RSTO         2009/05/26   added USBCDC_bytesInUSBBuffer()                  |
|  MSP,Biju     2009/12/03   file versioning started                          |
|                                                                             |
+----------------------------------------------------------------------------*/
#ifndef _UsbCdc_H_
#define _UsbCdc_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define kUSBCDC_sendStarted         0x01
#define kUSBCDC_sendComplete        0x02
#define kUSBCDC_intfBusyError       0x03
#define kUSBCDC_receiveStarted      0x04
#define kUSBCDC_receiveCompleted    0x05
#define kUSBCDC_receiveInProgress   0x06
#define kUSBCDC_generalError        0x07
#define kUSBCDC_busNotAvailable     0x08


/*----------------------------------------------------------------------------
These functions can be used in application
+----------------------------------------------------------------------------*/

/*
Sends data over interface intfNum, of size size and starting at address data.
  Returns:  kUSBCDC_sendStarted
            kUSBCDC_sendComplete
            kUSBCDC_intfBusyError
*/
BYTE USBCDC_sendData(const BYTE* data, WORD size, BYTE intfNum);

/*
Receives data over interface intfNum, of size size, into memory starting at address data.
*/
BYTE USBCDC_receiveData(BYTE* data, WORD size, BYTE intfNum);

/*
Aborts an active receive operation on interface intfNum.
  size: the number of bytes that were received and transferred
  to the data location established for this receive operation.
*/
BYTE USBCDC_abortReceive(WORD* size, BYTE intfNum);


#define kUSBCDC_noDataWaiting 1 //returned by USBCDC_rejectData() if no data pending

/*
This function rejects payload data that has been received from the host.
*/
BYTE USBCDC_rejectData(BYTE intfNum);

/*
Aborts an active send operation on interface intfNum.  Returns the number of bytes that were sent prior to the abort, in size.
*/
BYTE USBCDC_abortSend(WORD* size, BYTE intfNum);


#define kUSBCDC_waitingForSend      0x01
#define kUSBCDC_waitingForReceive   0x02
#define kUSBCDC_dataWaiting         0x04
#define kUSBCDC_busNotAvailable     0x08
#define kUSB_allCdcEvents           0xFF

/*
This function indicates the status of the interface intfNum.
  If a send operation is active for this interface,
  the function also returns the number of bytes that have been transmitted to the host.
  If a receiver operation is active for this interface, the function also returns
  the number of bytes that have been received from the host and are waiting at the assigned address.

returns kUSBCDC_waitingForSend (indicates that a call to USBCDC_SendData()
  has been made, for which data transfer has not been completed)

returns kUSBCDC_waitingForReceive (indicates that a receive operation
  has been initiated, but not all data has yet been received)

returns kUSBCDC_dataWaiting (indicates that data has been received
  from the host, waiting in the USB receive buffers)
*/
BYTE USBCDC_intfStatus(BYTE intfNum, WORD* bytesSent, WORD* bytesReceived);

/*
Returns how many bytes are in the buffer are received and ready to be read.
*/
BYTE USBCDC_bytesInUSBBuffer(BYTE intfNum);


/*----------------------------------------------------------------------------
Event-Handling routines
+----------------------------------------------------------------------------*/

/*
This event indicates that data has been received for interface intfNum, but no data receive operation is underway.
returns TRUE to keep CPU awake
*/
BYTE USBCDC_handleDataReceived(BYTE intfNum);

/*
This event indicates that a send operation on interface intfNum has just been completed.
returns TRUE to keep CPU awake
*/
BYTE USBCDC_handleSendCompleted(BYTE intfNum);

/*
This event indicates that a receive operation on interface intfNum has just been completed.
returns TRUE to keep CPU awake
*/
BYTE USBCDC_handleReceiveCompleted(BYTE intfNum);


/*----------------------------------------------------------------------------
These functions is to be used ONLY by USB stack, and not by application
+----------------------------------------------------------------------------*/

/**
Send a packet with the settings of the second uart back to the usb host
*/
VOID usbGetLineCoding0(VOID);
VOID usbGetLineCoding1(VOID);
VOID usbGetLineCoding2(VOID);

/**
Prepare EP0 to receive a packet with the settings for the second uart
*/
VOID usbSetLineCoding0(VOID);
VOID usbSetLineCoding1(VOID);
VOID usbSetLineCoding2(VOID);

/**
Function set or reset RTS
*/
VOID usbSetControlLineState(VOID);

/**
Readout the settings (send from usb host) for the second uart
*/
VOID Handler_SetLineCoding0(VOID);
VOID Handler_SetLineCoding1(VOID);
VOID Handler_SetLineCoding2(VOID);

#ifdef __cplusplus
}
#endif
#endif //_UsbCdc_H_
