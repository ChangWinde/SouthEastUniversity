// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC/HID/MSC Driver)            |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: Usb.h, File Version 1.04 2010/10/30                                |
|  Author: RSTO                                                               |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  RSTO         2008/09/03   born                                             |
|  RSTO         2008/12/23   enhancements of CDC API                          |
|  RSTO         2009/05/15   changed USB_connectionStatus()                   |
|                            to USB_connectionInfo()                          |
|  RSTO         2009/05/26   remove kUSB_failedEnumEvent                      |
|  RSTO         2009/07/17   added __data16 qualifier for USB buffers         |
|  MSP,Biju     2009/10/20   Composite support changes                        |
|  RSTO         2009/11/05   added event ST_NOENUM_SUSPENDED                  |
|  MSP,Biju     2009/12/28   macros DESC_TYPE_IAD added due to IAD            |
|                            support                                          |
|  RSTO         2010/10/30   added kUSB_allXXXEvents                          |
+----------------------------------------------------------------------------*/

#ifndef _USB_H_
#define _USB_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------------+
| Constant Definition                                                         |
+----------------------------------------------------------------------------*/
#define USB_RETURN_DATA_LENGTH  8
#define SIZEOF_DEVICE_REQUEST   0x08

//  Bit definitions for DEVICE_REQUEST.bmRequestType
//  Bit 7:   Data direction
#define USB_REQ_TYPE_OUTPUT     0x00    // 0 = Host sending data to device
#define USB_REQ_TYPE_INPUT      0x80    // 1 = Device sending data to host

//  Bit 6-5: Type
#define USB_REQ_TYPE_MASK       0x60    // Mask value for bits 6-5
#define USB_REQ_TYPE_STANDARD   0x00    // 00 = Standard USB request
#define USB_REQ_TYPE_CLASS      0x20    // 01 = Class specific
#define USB_REQ_TYPE_VENDOR     0x40    // 10 = Vendor specific

//  Bit 4-0: Recipient
#define USB_REQ_TYPE_RECIP_MASK 0x1F    // Mask value for bits 4-0
#define USB_REQ_TYPE_DEVICE     0x00    // 00000 = Device
#define USB_REQ_TYPE_INTERFACE  0x01    // 00001 = Interface
#define USB_REQ_TYPE_ENDPOINT   0x02    // 00010 = Endpoint
#define USB_REQ_TYPE_OTHER      0x03    // 00011 = Other

//  Values for DEVICE_REQUEST.bRequest
// Standard Device Requests
#define USB_REQ_GET_STATUS              0
#define USB_REQ_CLEAR_FEATURE           1
#define USB_REQ_SET_FEATURE             3
#define USB_REQ_SET_ADDRESS             5
#define USB_REQ_GET_DESCRIPTOR          6
#define USB_REQ_SET_DESCRIPTOR          7
#define USB_REQ_GET_CONFIGURATION       8
#define USB_REQ_SET_CONFIGURATION       9
#define USB_REQ_GET_INTERFACE           10
#define USB_REQ_SET_INTERFACE           11
#define USB_REQ_SYNCH_FRAME             12

// CDC CLASS Requests
#define USB_CDC_GET_LINE_CODING         0x21
#define USB_CDC_SET_LINE_CODING         0x20
#define USB_CDC_SET_CONTROL_LINE_STATE  0x22

// HID CLASS Requests
#define USB_HID_REQ                     0x81
#define USB_REQ_GET_REPORT		        0x01
#define USB_REQ_GET_IDLE		        0x02
#define USB_REQ_SET_REPORT		        0x09
#define USB_REQ_SET_IDLE		        0x0A
#define USB_REQ_SET_PROTOCOL            0x0B
#define USB_REQ_GET_PROTOCOL            0x03

// MSC CLASS Requests
#define USB_MSC_RESET_BULK              0xFF
#define USB_MSC_GET_MAX_LUN             0xFE

//HID Values for HID Report Types (tSetup.bValueH)
#define USB_REQ_HID_INPUT               0x01
#define USB_REQ_HID_OUTPUT              0x02
#define USB_REQ_HID_FEATURE             0x03


//  Descriptor Type Values
#define DESC_TYPE_DEVICE                1       // Device Descriptor (Type 1)
#define DESC_TYPE_CONFIG                2       // Configuration Descriptor (Type 2)
#define DESC_TYPE_STRING                3       // String Descriptor (Type 3)
#define DESC_TYPE_INTERFACE             4       // Interface Descriptor (Type 4)
#define DESC_TYPE_ENDPOINT              5       // Endpoint Descriptor (Type 5)
#define DESC_TYPE_DEVICE_QUALIFIER      6       // Endpoint Descriptor (Type 6)
#define DESC_TYPE_IAD                   0x0B
#define DESC_TYPE_HUB                   0x29    // Hub Descriptor (Type 6)
#define DESC_TYPE_HID			        0x21    // HID Descriptor
#define DESC_TYPE_REPORT		        0x22    // Report Descriptor
#define DESC_TYPE_PHYSICAL		        0x23	// Physical Descriptor

//  Feature Selector Values
#define FEATURE_REMOTE_WAKEUP           1       // Remote wakeup (Type 1)
#define FEATURE_ENDPOINT_STALL          0       // Endpoint stall (Type 0)

// Device Status Values
#define DEVICE_STATUS_REMOTE_WAKEUP     0x02
#define DEVICE_STATUS_SELF_POWER        0x01

//  Maximum descriptor size
#define MAX_DESC_SIZE                   256

//  DEVICE_DESCRIPTOR structure
#define SIZEOF_DEVICE_DESCRIPTOR        0x12
#define OFFSET_DEVICE_DESCRIPTOR_VID_L  0x08
#define OFFSET_DEVICE_DESCRIPTOR_VID_H  0x09
#define OFFSET_DEVICE_DESCRIPTOR_PID_L  0x0A
#define OFFSET_DEVICE_DESCRIPTOR_PID_H  0x0B
#define OFFSET_CONFIG_DESCRIPTOR_POWER  0x07
#define OFFSET_CONFIG_DESCRIPTOR_CURT   0x08

//  CONFIG_DESCRIPTOR structure
#define SIZEOF_CONFIG_DESCRIPTOR 0x09

//  HID DESCRIPTOR structure
//#define SIZEOF_HID_DESCRIPTOR 0x09

//  Bit definitions for CONFIG_DESCRIPTOR.bmAttributes
#define CFG_DESC_ATTR_SELF_POWERED  0x40    // Bit 6: If set, device is self powered
#define CFG_DESC_ATTR_BUS_POWERED   0x80    // Bit 7: If set, device is bus powered
#define CFG_DESC_ATTR_REMOTE_WAKE   0x20    // Bit 5: If set, device supports remote wakeup

//  INTERFACE_DESCRIPTOR structure
#define SIZEOF_INTERFACE_DESCRIPTOR 0x09

//  ENDPOINT_DESCRIPTOR structure
#define SIZEOF_ENDPOINT_DESCRIPTOR 0x07

//  Bit definitions for EndpointDescriptor.EndpointAddr
#define EP_DESC_ADDR_EP_NUM     0x0F    // Bit 3-0: Endpoint number
#define EP_DESC_ADDR_DIR_IN     0x80    // Bit 7: Direction of endpoint, 1/0 = In/Out

//  Bit definitions for EndpointDescriptor.EndpointFlags
#define EP_DESC_ATTR_TYPE_MASK  0x03    // Mask value for bits 1-0
#define EP_DESC_ATTR_TYPE_CONT  0x00    // Bit 1-0: 00 = Endpoint does control transfers
#define EP_DESC_ATTR_TYPE_ISOC  0x01    // Bit 1-0: 01 = Endpoint does isochronous transfers
#define EP_DESC_ATTR_TYPE_BULK  0x02    // Bit 1-0: 10 = Endpoint does bulk transfers
#define EP_DESC_ATTR_TYPE_INT   0x03    // Bit 1-0: 11 = Endpoint does interrupt transfers

// Definition to indicate valid/invalid data
#define DATA_VALID      1
#define DATA_INVALID    0

extern __no_init tDEVICE_REQUEST __data16 tSetupPacket;
extern __no_init BYTE __data16 abIEP0Buffer[];
extern __no_init BYTE __data16 abOEP0Buffer[];
extern __no_init BYTE __data16 pbXBufferAddressEp1[];
extern __no_init BYTE __data16 pbYBufferAddressEp1[];
extern __no_init BYTE __data16 pbXBufferAddressEp81[];
extern __no_init BYTE __data16 pbYBufferAddressEp81[];
extern __no_init BYTE __data16 pbXBufferAddressEp2[];
extern __no_init BYTE __data16 pbYBufferAddressEp2[];
extern __no_init BYTE __data16 pbXBufferAddressEp82[];
extern __no_init BYTE __data16 pbYBufferAddressEp82[];

extern __no_init BYTE __data16 pbXBufferAddressEp3[];
extern __no_init BYTE __data16 pbYBufferAddressEp3[];
extern __no_init BYTE __data16 pbXBufferAddressEp83[];
extern __no_init BYTE __data16 pbYBufferAddressEp83[];

extern __no_init BYTE __data16 pbXBufferAddressEp4[];
extern __no_init BYTE __data16 pbYBufferAddressEp4[];
extern __no_init BYTE __data16 pbXBufferAddressEp84[];
extern __no_init BYTE __data16 pbYBufferAddressEp84[];

extern __no_init BYTE __data16 pbXBufferAddressEp5[];
extern __no_init BYTE __data16 pbYBufferAddressEp5[];
extern __no_init BYTE __data16 pbXBufferAddressEp85[];
extern __no_init BYTE __data16 pbYBufferAddressEp85[];


extern __no_init BYTE __data16 pbXBufferAddressEp6[];
extern __no_init BYTE __data16 pbYBufferAddressEp6[];
extern __no_init BYTE __data16 pbXBufferAddressEp86[];
extern __no_init BYTE __data16 pbYBufferAddressEp86[];

extern __no_init BYTE __data16 pbXBufferAddressEp7[];
extern __no_init BYTE __data16 pbYBufferAddressEp7[];
extern __no_init BYTE __data16 pbXBufferAddressEp87[];
extern __no_init BYTE __data16 pbYBufferAddressEp87[];

extern WORD wBytesRemainingOnIEP0;
extern WORD wBytesRemainingOnOEP0;
extern BYTE abUsbRequestReturnData[];
extern BYTE abUsbRequestIncomingData[];
extern BYTE bEnumerationStatus;
extern BYTE bFunctionSuspended;

//Function return values
#define kUSB_succeed        0x00
#define kUSB_generalError   0x01
#define kUSB_notEnabled     0x02
//#define kUSB_VbusNotPresent 0x03

//return values USB_connectionInfo(), USB_connect()
#define kUSB_vbusPresent    0x01
#define kUSB_busActive      0x02    // frame sync packets are being received
#define kUSB_ConnectNoVBUS  0x04
#define kUSB_suspended      0x08
#define kUSB_NotSuspended   0x10
#define kUSB_Enumerated     0x20
#define kUSB_purHigh        0x40

// Parameters for function USB_setEnabledEvents()
#define kUSB_clockFaultEvent        0x0001
#define kUSB_VbusOnEvent            0x0002
#define kUSB_VbusOffEvent           0x0004
#define kUSB_UsbResetEvent          0x0008
#define kUSB_UsbSuspendEvent        0x0010
#define kUSB_UsbResumeEvent         0x0020
#define kUSB_dataReceivedEvent      0x0040
#define kUSB_sendCompletedEvent     0x0080
#define kUSB_receiveCompletedEvent  0x0100
#define kUSB_allUsbEvents           0x01FF

// USB connection states
#define ST_USB_DISCONNECTED         0x80
#define ST_USB_CONNECTED_NO_ENUM    0x81
#define ST_ENUM_IN_PROGRESS         0x82
#define ST_ENUM_ACTIVE              0x83
#define ST_ENUM_SUSPENDED           0x84
//#define ST_FAILED_ENUM              0x85
#define ST_ERROR                    0x86
#define ST_NOENUM_SUSPENDED         0x87

VOID usbStallInEndpoint(BYTE);
VOID usbStallOutEndpoint(BYTE);
VOID usbStallEndpoint(BYTE);
VOID usbClearOEPByteCount(BYTE);


/*----------------------------------------------------------------------------
These functions can be used in application
+----------------------------------------------------------------------------*/

/*
MSP430 USB Module Management functions
*/

/**
Init the USB HW interface.
*/
BYTE USB_init(VOID);

/**
Init and start the USB PLL.
*/
BYTE USB_enable();

/**
Disables the USB module and PLL.
*/
BYTE USB_disable(VOID);

/*
Enables/disables various USB events.
*/
BYTE USB_setEnabledEvents(WORD events);

/*
Returns which events are enabled and which are disabled.
*/
WORD USB_getEnabledEvents();

/*
Instruct USB module to make itself available to the PC for connection, by pulling PUR high.
*/
BYTE USB_connect();

/*
Force a disconnect from the PC by pulling PUR low.
*/
BYTE USB_disconnect();

/**
Reset USB-SIE and global variables.
*/
BYTE USB_reset();

/**
Suspend USB.
*/
BYTE USB_suspend(VOID);

/**
Resume USB.
*/
BYTE USB_resume(VOID);

/*
Force a remote wakeup of the USB host.
       This method can be generated only if device supports
       remote wake-up feature in some of its configurations.
       The method wakes-up the USB bus only if wake-up feature is enabled by the host.
*/
BYTE USB_forceRemoteWakeup();

/*
Returns the status of the USB connection.
*/
BYTE USB_connectionInfo();

/*
Returns the state of the USB connection.
*/
BYTE USB_connectionState();

/*
Event-Handling routines
*/

/*
If this function gets executed, it's a sign that the output of the USB PLL has failed.
returns TRUE to keep CPU awake
*/
BYTE USB_handleClockEvent();

/*
If this function gets executed, it indicates that a valid voltage has just been applied to the VBUS pin.
returns TRUE to keep CPU awake
*/
BYTE USB_handleVbusOnEvent();

/*
If this function gets executed, it indicates that a valid voltage has just been removed from the VBUS pin.
returns TRUE to keep CPU awake
*/
BYTE USB_handleVbusOffEvent();

/*
If this function gets executed, it indicates that the USB host has issued a USB reset event to the device.
returns TRUE to keep CPU awake
*/
BYTE USB_handleResetEvent();

/*
If this function gets executed, it indicates that the USB host has chosen to suspend this device after a period of active operation.
returns TRUE to keep CPU awake
*/
BYTE USB_handleSuspendEvent();

/*
If this function gets executed, it indicates that the USB host has chosen to resume this device after a period of suspended operation.
returns TRUE to keep CPU awake
*/
BYTE USB_handleResumeEvent();

/*
If this function gets executed, it indicates that the USB host has enumerated this device :
after host assigned the address to the device.
returns TRUE to keep CPU awake
*/
BYTE USB_handleEnumCompleteEvent();

/**
Send stall handshake for in- and out-endpoint0 (control pipe)
*/
VOID usbStallEndpoint0(VOID);

/**
Clear byte counter for endpoint0 (control pipe)
*/
VOID usbClearOEP0ByteCount(VOID);

/**
Send stall handshake for out-endpoint0 (control pipe)
*/
VOID usbStallOEP0(VOID);

/**
Send further data over control pipe if needed.
       Function is called from control-in IRQ. Do not call from user application
*/
VOID usbSendNextPacketOnIEP0(VOID);

/**
Send data over control pipe to host.
       Number of bytes to transmit should be set with
       global varible "wBytesRemainingOnIEP0" before function is called.
*/
VOID usbSendDataPacketOnEP0(PBYTE pbBuffer);

/**
Receive further data from control pipe if needed.
       Function is called from control-out IRQ. Do not call from user application
*/
VOID usbReceiveNextPacketOnOEP0(VOID);

/**
Receive data from control pipe.
       Number of bytes to receive should be set with
       global varible "wBytesRemainingOnOEP0" before function is called.
*/
VOID usbReceiveDataPacketOnEP0(PBYTE pbBuffer);

/**
Send zero length packet on control pipe.
*/
VOID usbSendZeroLengthPacketOnIEP0(VOID);

/*Send data to host.*/
BYTE MscSendData(const BYTE* data, WORD size);

/**
Decode incoming usb setup packet and call corresponding function
       usbDecodeAndProcessUsbRequest is called from IRQ. Do not call from user application
*/
BYTE usbDecodeAndProcessUsbRequest(VOID);
VOID usbClearEndpointFeature(VOID);
VOID usbGetConfiguration(VOID);
VOID usbGetDeviceDescriptor(VOID);
VOID usbGetConfigurationDescriptor(VOID);
VOID usbGetStringDescriptor(VOID);
VOID usbGetInterface(VOID);
VOID usbGetDeviceStatus(VOID);
VOID usbGetEndpointStatus(VOID);
VOID usbGetInterfaceStatus(VOID);
VOID usbSetAddress(VOID);
VOID usbSetConfiguration(VOID);
VOID usbClearDeviceFeature(VOID);
VOID usbSetDeviceFeature(VOID);
VOID usbSetEndpointFeature(VOID);
VOID usbSetInterface(VOID);
VOID usbInvalidRequest(VOID);


#define ENUMERATION_COMPLETE 0x01

/*----------------------------------------------------------------------------+
| End of header file                                                          |
+----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif /* _USB_H */
/*------------------------ Nothing Below This Line --------------------------*/
