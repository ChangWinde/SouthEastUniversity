// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (HID Driver)                    |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: UsbHid.h, File Version 1.00 2009/12/03                             |
|  Author: RSTO                                                               |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  RSTO         2009/02/20   portet from UsbCdc.h                             |
|  RSTO         2009/05/15   added param to USBHID_rejectData()               |
|  RSTO         2009/05/26   added USBHID_bytesInUSBBuffer()                  |
+----------------------------------------------------------------------------*/
#ifndef _UsbHid_H_
#define _UsbHid_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define kUSBHID_sendStarted         0x01
#define kUSBHID_sendComplete        0x02
#define kUSBHID_intfBusyError       0x03
#define kUSBHID_receiveStarted      0x04
#define kUSBHID_receiveCompleted    0x05
#define kUSBHID_receiveInProgress   0x06
#define kUSBHID_generalError        0x07
#define kUSBHID_busNotAvailable     0x08

/*----------------------------------------------------------------------------
These functions can be used in application
+----------------------------------------------------------------------------*/

/*
Sends a pre-built report reportData to the host.
  Returns:  kUSBHID_sendComplete
            kUSBHID_intfBusyError
            kUSBHID_busSuspended
*/
BYTE USBHID_sendReport(const BYTE * reportData, BYTE intfNum);

/*
Receives report reportData from the host.
Return:     kUSBHID_receiveCompleted
            kUSBHID_generalError
            kUSBHID_busSuspended
*/
BYTE USBHID_receiveReport(BYTE * reportData, BYTE intfNum);

/*
Sends data over interface intfNum, of size size and starting at address data.
  Returns:  kUSBHID_sendStarted
            kUSBHID_sendComplete
            kUSBHID_intfBusyError
*/
BYTE USBHID_sendData(const BYTE* data, WORD size, BYTE intfNum);

/*
Receives data over interface intfNum, of size size, into memory starting at address data.
*/
BYTE USBHID_receiveData(BYTE* data, WORD size, BYTE intfNum);

/*
Aborts an active receive operation on interface intfNum.
  size: the number of bytes that were received and transferred
  to the data location established for this receive operation.
*/
BYTE USBHID_abortReceive(WORD* size, BYTE intfNum);


#define kUSBHID_noDataWaiting 1 //returned by USBHID_rejectData() if no data pending

/*
This function rejects payload data that has been received from the host.
*/
BYTE USBHID_rejectData(BYTE intfNum);

/*
Aborts an active send operation on interface intfNum.  Returns the number of bytes that were sent prior to the abort, in size.
*/
BYTE USBHID_abortSend(WORD* size, BYTE intfNum);


#define kUSBHID_waitingForSend      0x01
#define kUSBHID_waitingForReceive   0x02
#define kUSBHID_dataWaiting         0x04
#define kUSBHID_busNotAvailable     0x08
/*
This function indicates the status of the interface intfNum.
  If a send operation is active for this interface,
  the function also returns the number of bytes that have been transmitted to the host.
  If a receiver operation is active for this interface, the function also returns
  the number of bytes that have been received from the host and are waiting at the assigned address.

returns kUSBHID_waitingForSend (indicates that a call to USBHID_SendData()
  has been made, for which data transfer has not been completed)

returns kUSBHID_waitingForReceive (indicates that a receive operation
  has been initiated, but not all data has yet been received)

returns kUSBHID_dataWaiting (indicates that data has been received
  from the host, waiting in the USB receive buffers)
*/
BYTE USBHID_intfStatus(BYTE intfNum, WORD* bytesSent, WORD* bytesReceived);

/*
Returns how many bytes are in the buffer are received and ready to be read.
*/
BYTE USBHID_bytesInUSBBuffer(BYTE intfNum);

/*----------------------------------------------------------------------------
Event-Handling routines
+----------------------------------------------------------------------------*/

/*
This event indicates that data has been received for port port, but no data receive operation is underway.
returns TRUE to keep CPU awake
*/
BYTE USBHID_handleDataReceived(BYTE intfNum);

/*
This event indicates that a send operation on port port has just been completed.
returns TRUE to keep CPU awake
*/
BYTE USBHID_handleSendCompleted(BYTE intfNum);

/*
This event indicates that a receive operation on port port has just been completed.
returns TRUE to keep CPU awake
*/
BYTE USBHID_handleReceiveCompleted(BYTE intfNum);


#ifdef __cplusplus
}
#endif
#endif //_UsbHid_H_
