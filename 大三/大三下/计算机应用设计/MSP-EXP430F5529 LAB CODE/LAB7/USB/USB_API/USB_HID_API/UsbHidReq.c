// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (HID Driver)                    |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: UsbHidReq.c, File Version 1.00 2009/12/03                          |
|  Author: RSTO                                                               |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  RSTO         2009/03/03   born                                             |
|  MSP/Biju     2009/10/21   Changes for composite support                    |
+----------------------------------------------------------------------------*/

#include "../USB_Common/device.h"
#include "../USB_Common/types.h"              // Basic Type declarations
#include "../USB_Common/defMSP430USB.h"
#include "../USB_Common/usb.h"                // USB-specific Data Structures
#include "UsbHidReportHandler.h"
#include <descriptors.h>

#ifdef _HID_

VOID usbClearOEP0ByteCount(VOID);
VOID usbSendDataPacketOnEP0(PBYTE pbBuffer);
VOID usbReceiveDataPacketOnEP0(PBYTE pbBuffer);


VOID usbGetHidDescriptor(VOID)
{
    static BYTE intfNum;
    if(intfNum >= HID_NUM_INTERFACES)
    {
      intfNum = 0;
    }
    usbClearOEP0ByteCount();
    wBytesRemainingOnIEP0 = 9;
    usbSendDataPacketOnEP0((PBYTE)&abromConfigurationDescriptorGroup.stHid[intfNum].blength_hid_descriptor);
    intfNum++;
}

//----------------------------------------------------------------------------
VOID usbGetReportDescriptor(VOID)
{
    wBytesRemainingOnIEP0 = SIZEOF_REPORT_DESCRIPTOR;
    usbSendDataPacketOnEP0((PBYTE)&abromReportDescriptor);
}

//----------------------------------------------------------------------------

VOID usbSetReport(VOID)
{
    usbReceiveDataPacketOnEP0((PBYTE) &abUsbRequestIncomingData);     // receive data over EP0 from Host
}

//----------------------------------------------------------------------------

VOID usbGetReport(VOID)
{
    switch((BYTE)tSetupPacket.wValue)
    {
       case USB_REQ_HID_FEATURE:
            Handler_InFeature();
            break;
       case USB_REQ_HID_INPUT:
            Handler_InReport();
            break;
       default:;
    }
}

#endif //_HID_
/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
