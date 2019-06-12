// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC/HID Driver)                |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: usb_eventHandling.c, File Version 1.00 2009/12/03                  |
|  Author: RSTO                                                               |
|                                                                             |
|  Description:                                                               |
|  Event-handling placeholder functions.                                      |
|  All functios are called in interrupt context.                              |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  RSTO         2008/09/03   born                                             |
|  RSTO         2008/12/23   enhancements of CDC API                          |
|  MSP,Biju     2009/10/20   composite support changes                        |
|  RSTO         2009/11/06   added USB_handleEnumCompleteEvent()              |
|  Biju,MSP     2010/06/09   Added handleBufferEvent()                        |
+----------------------------------------------------------------------------*/

#include "..\USB_Common\device.h"
#include "..\USB_Common\types.h"            // Basic Type declarations
#include "descriptors.h"
#include "usb.h"                        // USB-specific Data Structures

extern BYTE *RWbuf_X, *RWbuf_Y;


/*
If this function gets executed, it's a sign that the output of the USB PLL has failed.
returns TRUE to keep CPU awake
*/
BYTE USB_handleClockEvent()
{
    //TO DO: You can place your code here

    return TRUE;
}

/*
If this function gets executed, it indicates that a valid voltage has just been applied to the VBUS pin.
returns TRUE to keep CPU awake
*/
BYTE USB_handleVbusOnEvent()
{
    //TO DO: You can place your code here

    //We switch on USB and connect to the BUS
    if (USB_enable() == kUSB_succeed)
    {
        USB_reset();
        USB_connect();  // generate rising edge on DP -> the host enumerates our device as full speed device
    }
    return TRUE;
}

/*
If this function gets executed, it indicates that a valid voltage has just been removed from the VBUS pin.
returns TRUE to keep CPU awake
*/
BYTE USB_handleVbusOffEvent()
{
    //TO DO: You can place your code here

    return TRUE;
}

/*
If this function gets executed, it indicates that the USB host has issued a USB reset event to the device.
returns TRUE to keep CPU awake
*/
BYTE USB_handleResetEvent()
{
    //TO DO: You can place your code here

    return TRUE;
}

/*
If this function gets executed, it indicates that the USB host has chosen to suspend this device after a period of active operation.
returns TRUE to keep CPU awake
*/
BYTE USB_handleSuspendEvent()
{
    //TO DO: You can place your code here

    return TRUE;
}

/*
If this function gets executed, it indicates that the USB host has chosen to resume this device after a period of suspended operation.
returns TRUE to keep CPU awake
*/
BYTE USB_handleResumeEvent()
{
    //TO DO: You can place your code here

    return TRUE;   //return TRUE to wake up after interrupt
}

/*
If this function gets executed, it indicates that the USB host has enumerated this device :
after host assigned the address to the device.
returns TRUE to keep CPU awake
*/
BYTE USB_handleEnumCompleteEvent()
{
    //TO DO: You can place your code here

    return TRUE;   //return TRUE to wake up after interrupt
}


#ifdef _CDC_

/*
This event indicates that data has been received for interface intfNum, but no data receive operation is underway.
returns TRUE to keep CPU awake
*/
BYTE USBCDC_handleDataReceived(BYTE intfNum)
{
    //TO DO: You can place your code here

    return TRUE;   //return TRUE to wake up after data was received
}

/*
This event indicates that a send operation on interface intfNum has just been completed.
returns TRUE to keep CPU awake
*/
BYTE USBCDC_handleSendCompleted(BYTE intfNum)
{
    //TO DO: You can place your code here

    return FALSE;   //return FALSE to go asleep after interrupt (in the case the CPU slept before interrupt)
}

/*
This event indicates that a receive operation on interface intfNum has just been completed.
*/
BYTE USBCDC_handleReceiveCompleted(BYTE intfNum)
{
    //TO DO: You can place your code here

    return FALSE;   //return FALSE to go asleep after interrupt (in the case the CPU slept before interrupt)
}

#endif // _CDC_

#ifdef _HID_
/*
This event indicates that data has been received for interface intfNum, but no data receive operation is underway.
returns TRUE to keep CPU awake
*/
BYTE USBHID_handleDataReceived(BYTE intfNum)
{
    //TO DO: You can place your code here

    return TRUE;   //return TRUE to wake up after data was received
}

/*
This event indicates that a send operation on interface intfNum has just been completed.
returns TRUE to keep CPU awake
*/
BYTE USBHID_handleSendCompleted(BYTE intfNum)
{
    //TO DO: You can place your code here

    return FALSE;   //return FALSE to go asleep after interrupt (in the case the CPU slept before interrupt)
}

/*
This event indicates that a receive operation on interface intfNum has just been completed.
*/
BYTE USBHID_handleReceiveCompleted(BYTE intfNum)
{
    //TO DO: You can place your code here

    return FALSE;   //return FALSE to go asleep after interrupt (in the case the CPU slept before interrupt)
}

#endif // _HID_

#ifdef _MSC_
BYTE USBMSC_handleBufferEvent(VOID)
{

    return TRUE;

}
#endif // _MSC_


/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
