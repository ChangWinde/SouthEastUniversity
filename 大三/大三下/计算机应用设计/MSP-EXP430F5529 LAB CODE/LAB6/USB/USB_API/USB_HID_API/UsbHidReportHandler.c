// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (HID Driver)                    |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: HidReportHandler.c, File Version 1.00 2009/12/03                   |
|  Author: RSTO                                                               |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  RSTO         2009/03/03   born                                             |
|  RSTO         2009/07/17   added __data16 qualifier for USB buffers         |
+----------------------------------------------------------------------------*/

#include "../USB_Common/device.h"
#include "../USB_Common/types.h"              // Basic Type declarations
#include "../USB_Common/defMSP430USB.h"
#include "../USB_Common/usb.h"                // USB-specific Data Structures
#include "UsbHidReportHandler.h"
#include <descriptors.h>

#ifdef _HID_

extern __no_init tEDB0 __data16 tEndPoint0DescriptorBlock;
extern __no_init tEDB __data16 tInputEndPointDescriptorBlock[];
extern __no_init tEDB __data16 tOutputEndPointDescriptorBlock[];


//----------------------------------------------------------------------------

VOID Handler_InReport(VOID)
{
}

//----------------------------------------------------------------------------

VOID Handler_InFeature(VOID)
{
    switch((BYTE)tSetupPacket.wValue)    // tSetupPacket.wValue is contains HID-Report-ID
    {
        case 1:
            // user's specified code...
            break;

         case 2:
            // user's specified code...
            break;

        default:;
    }
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData);
}

//----------------------------------------------------------------------------

VOID Handler_OutReport(VOID)
{
}

//----------------------------------------------------------------------------

VOID Handler_OutFeature(VOID)
{
    switch((BYTE)tSetupPacket.wValue)    // tSetupPacket.wValue is contains HID-Report-ID
    {
        case 1:
            // user's specified code...
            break;

         case 2:
            // user's specified code...
            break;

        default:;
    }
}

#endif //_HID_
/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
