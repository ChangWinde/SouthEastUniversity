// (c)2010 by Texas Instruments Incorporated, All Rights Reserved.
/*-----------------------------------------------------------------------------+
|                                                                              |
|                              Texas Instruments                               |
|                                                                              |
|    This is an automatically generated script by MSP430 USB Descriptor Tool   |
|                                                                              |
|    Descriptor Tool Version: 3.0.10                                           |
|    Date: 2011/03/14 11:00:02                                                 |
|                                                                              |
|                                Descriptor.h                                  |
|-----------------------------------------------------------------------------*/

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*-----------------------------------------------------------------------------+
| Include files                                                                |
|-----------------------------------------------------------------------------*/
#include <USB_API/USB_Common/types.h>

//***********************************************************************************************
// CDC or HID - Define both for composite support
//***********************************************************************************************
#define _CDC_          // Needed for CDC inteface
#define _HID_          // Needed for HID interface
#define _MSC_          // Needed for MSCinterface
//***********************************************************************************************
// CONFIGURATION CONSTANTS
//***********************************************************************************************
// These constants configure the API stack and help define the USB descriptors.
// Refer to Sec. 6 of the MSP430 USB CDC API Programmer's Guide for descriptions of these constants.

// Configuration Constants that can change
// #define that relates to Device Descriptor
#define USB_VID               0x2047        // Vendor ID (VID)
#define USB_PID               0x0400        // Product ID (PID)
/*----------------------------------------------------------------------------+
| Firmware Version                                                            |
| How to detect version number of the FW running on MSP430?                   |
| on Windows Open ControlPanel->Systems->Hardware->DeviceManager->Ports->     |
|         Msp430->ApplicationUART->Details                                    |
+----------------------------------------------------------------------------*/
#define VER_FW_H              0x02          // Device release number, in binary-coded decimal
#define VER_FW_L              0x00          // Device release number, in binary-coded decimal
// If a serial number is to be reported, set this to the index within the string descriptor
//of the dummy serial number string.  It will then be automatically handled by the API.
// If no serial number is to be reported, set this to 0.
#define USB_STR_INDEX_SERNUM  3


#define DESCRIPTOR_TOTAL_LENGTH            130            // wTotalLength, This is the sum of configuration descriptor length  + CDC descriptor length  + HID descriptor length
#define USB_NUM_INTERFACES                 4              // Number of implemented interfaces.

#define MSC0_DATA_INTERFACE                0              // Data interface number of MSC0
#define MSC0_OUTEP_ADDR                    0x01           // Output Endpoint number of MSC0
#define MSC0_INEP_ADDR                     0x81           // Input Endpoint number of MSC0

#define CDC0_COMM_INTERFACE                1              // Comm interface number of CDC0
#define CDC0_DATA_INTERFACE                2              // Data interface number of CDC0
#define CDC0_INTEP_ADDR                    0x82           // Interrupt Endpoint Address of CDC0
#define CDC0_OUTEP_ADDR                    0x03           // Output Endpoint Address of CDC0
#define CDC0_INEP_ADDR                     0x83           // Input Endpoint Address of CDC0

#define HID0_REPORT_INTERFACE              3              // Report interface number of HID0
#define HID0_OUTEP_ADDR                    0x04           // Output Endpoint number of HID0
#define HID0_INEP_ADDR                     0x84           // Input Endpoint number of HID0

#define CDC_NUM_INTERFACES                   1           //  Total Number of CDCs implemented. should set to 0 if there are no CDCs implemented.
#define HID_NUM_INTERFACES                   1           //  Total Number of HIDs implemented. should set to 0 if there are no HIDs implemented.
#define MSC_NUM_INTERFACES                   1           //  Total Number of MSCs implemented. should set to 0 if there are no MSCs implemented.
// Interface numbers for the implemented CDSs and HIDs, This is to use in the Application(main.c) and in the interupt file(UsbIsr.c).
#define MSC0_INTFNUM                0
#define CDC0_INTFNUM                1
#define HID0_INTFNUM                2
#define MSC_MAX_LUN_NUMBER                   1           // Maximum number of LUNs supported

#define USB_OUTEP_INT_EN BIT0 | BIT1 | BIT3 | BIT4
#define USB_INEP_INT_EN BIT0 | BIT1 | BIT2 | BIT3 | BIT4
// MCLK frequency of MCU, in Hz
// For running higher frequencies the Vcore voltage adjustment may required.
// Please refer to Data Sheet of the MSP430 device you use
#define USB_MCLK_FREQ 25000000                // MCLK frequency of MCU, in Hz
#define USB_PLL_XT        2                  // Defines which XT is used by the PLL (1=XT1, 2=XT2)
#define USB_XT_FREQ       USBPLL_SETCLK_4_0  // Indicates the freq of the crystal on the oscillator indicated by USB_PLL_XT
#define USB_DISABLE_XT_SUSPEND 1             // If non-zero, then USB_suspend() will disable the oscillator
                                             // that is designated by USB_PLL_XT; if zero, USB_suspend won't
                                             // affect the oscillator
#define USB_DMA_CHAN             0x00        // Set to 0xFF if no DMA channel will be used 0..7 for selected DMA channel



// Controls whether the remote wakeup feature is supported by this device.
// A value of 0x20 indicates that is it supported (this value is the mask for
// the bmAttributes field in the configuration descriptor).
// A value of zero indicates remote wakeup is not supported.
// Other values are undefined, as they will interfere with bmAttributes.
#define USB_SUPPORT_REM_WAKE 0x00

// Controls whether the application is self-powered to any degree.  Should be
// set to 0x40, unless the USB device is fully supplied by the bus.
#define USB_SUPPORT_SELF_POWERED 0xc0

// Controls what the device reports to the host regarding how much power it will
// consume from VBUS.  Expressed in 2mA units; that is, the number of mA
// communicated is twice the value of this field.
#define USB_MAX_POWER 0x32
//Configuration constants that can not change ( Fixed Values)
#define CDC_CLASS 2
#define HID_CLASS 3
#define MSC_CLASS 4

#define MAX_PACKET_SIZE   0x40              // Max size of the USB packets.

//***********************************************************************************************
// DESCRIPTOR CONSTANTS
//***********************************************************************************************
#define SIZEOF_DEVICE_DESCRIPTOR  0x12
#define SIZEOF_REPORT_DESCRIPTOR  52
#define USBHID_REPORT_LENGTH      4  // length of whole HID report (including Report ID)
#define CONFIG_STRING_INDEX       4
#define INTF_STRING_INDEX         5
#define USB_CONFIG_VALUE          0x01
//***********************************************************************************************
// OUTWARD DECLARATIONS
//***********************************************************************************************

//Calculates the endpoint descriptor block number from given address
#define EDB(addr) ((addr&0x07)-1)

/* Structure for generic part of configuration descriptor */
struct abromConfigurationDescriptorGenric
{
	BYTE sizeof_config_descriptor;            // bLength
 	BYTE desc_type_config;                    // bDescriptorType: 2
	BYTE sizeof_configuration_descriptor1;    // wTotalLength
	BYTE sizeof_configuration_descriptor2;
	BYTE usb_num_configurations;              // bNumInterfaces
	BYTE bconfigurationvalue;                 // bConfigurationValue
	BYTE  config_string_index;                // iConfiguration Description offset
 	BYTE mattributes;                         // bmAttributes, bus power, remote wakeup
	BYTE usb_max_power;                       // Max. Power Consumption at 2mA unit
};

/************************************************CDC Descriptor**************************/
struct abromConfigurationDescriptorCdc
{
//Interface Association Descriptor
    BYTE bLength;					          // Size of this Descriptor in Bytes
    BYTE bDescriptorType;				      // Descriptor Type (=11)
    BYTE bFirstInterface;				      // Interface number of the first one associated with this function
    BYTE bInterfaceCount;				      // Numver of contiguous interface associated with this function
    BYTE bFunctionClass;				      // The class triad of this interface,
    BYTE bFunctionSubClass;		              // usually same as the triad of the first interface
    BYTE bFunctionProcotol;
    BYTE iInterface;				          // Index of String Desc for this function
// interface descriptor (9 bytes)
    BYTE blength_intf;	                      // blength: interface descriptor size
    BYTE desc_type_interface;	              // bdescriptortype: interface
    BYTE interface_number_cdc;                // binterfacenumber
    BYTE balternatesetting;                   // balternatesetting: alternate setting
    BYTE bnumendpoints;                       // bnumendpoints: three endpoints used
    BYTE binterfaceclass;                     // binterfaceclass: communication interface class
    BYTE binterfacesubclass;                  // binterfacesubclass: abstract control model
    BYTE binterfaceprotocol;                  // binterfaceprotocol: common at commands
    BYTE intf_string_index;	                  // interface:
//header functional descriptor
    BYTE blength_header;	                  // blength: endpoint descriptor size
    BYTE bdescriptortype_header;	          // bdescriptortype: cs_interface
    BYTE bdescriptorsubtype_header;	          // bdescriptorsubtype: header func desc
    BYTE bcdcdc1;
    BYTE bcdcdc2;	                          // bcdcdc: spec release number

//call managment functional descriptor
    BYTE bfunctionlength;	                  // bfunctionlength
    BYTE bdescriptortype_c;	                  // bdescriptortype: cs_interface
    BYTE bdescriptorsubtype_c;	              // bdescriptorsubtype: call management func desc
    BYTE bmcapabilities;	                  // bmcapabilities: d0+d1
    BYTE intf_number_cdc;                     // bdatainterface: 0

//acm functional descriptor
    BYTE bfunctionlength_acm;	              // bfunctionlength
    BYTE bdescriptortype_acm;	              // bdescriptortype: cs_interface
    BYTE bdescriptorsubtype_acm;	          // bdescriptorsubtype: abstract control management desc
    BYTE bmcapabilities_acm;	              // bmcapabilities

// Union Functional Descriptor
    BYTE bLength_ufd;                         // Size, in bytes
    BYTE bdescriptortype_ufd;                 // bDescriptorType: CS_INTERFACE
    BYTE bdescriptorsubtype_ufd;              // bDescriptorSubtype: Union Functional Desc
    BYTE bmasterinterface_ufd;                // bMasterInterface -- the controlling intf for the union
    BYTE bslaveinterface_ufd;                 // bSlaveInterface -- the controlled intf for the union

//Interrupt end point related fields
    BYTE sizeof_epintep_descriptor;           // blength: endpoint descriptor size
    BYTE desc_type_epintep;	                  // bdescriptortype: endpoint
    BYTE cdc_intep_addr;	                  // bendpointaddress: (in2)
    BYTE epintep_desc_attr_type_int;	      // bmattributes: interrupt
    BYTE epintep_wmaxpacketsize1;
    BYTE epintep_wmaxpacketsize;   		      // wmaxpacketsize, 64 bytes
    BYTE epintep_binterval;                   // binterval

// Data interface descriptor (9 bytes)
    BYTE blength_slaveintf;	                  // blength: interface descriptor size
    BYTE desc_type_slaveinterface;	          // bdescriptortype: interface
    BYTE interface_number_slavecdc;           // binterfacenumber
    BYTE balternatesetting_slave;             // balternatesetting: alternate setting
    BYTE bnumendpoints_slave;                 // bnumendpoints: three endpoints used
    BYTE binterfaceclass_slave;               // binterfaceclass: data interface class
    BYTE binterfacesubclass_slave;            // binterfacesubclass: abstract control model
    BYTE binterfaceprotocol_slave;            // binterfaceprotocol: common at commands
    BYTE intf_string_index_slave;	          // interface:

// Bulk out end point related fields
    BYTE sizeof_outep_descriptor;             // blength: endpoint descriptor size
    BYTE desc_type_outep;	                  // bdescriptortype: endpoint
    BYTE cdc_outep_addr;	                  // bendpointaddress: (out3)
    BYTE outep_desc_attr_type_bulk;	          // bmattributes: bulk
    BYTE outep_wmaxpacketsize1;
    BYTE outep_wmaxpacketsize2;               // wmaxpacketsize, 64 bytes
    BYTE outep_binterval; 	                  // binterval: ignored for bulk transfer

// Bulk in related fields
    BYTE sizeof_inep_descriptor;              // blength: endpoint descriptor size
    BYTE desc_type_inep;	                  // bdescriptortype: endpoint
    BYTE cdc_inep_addr;	                      // bendpointaddress: (in3)
    BYTE inep_desc_attr_type_bulk;	          // bmattributes: bulk
    BYTE inep_wmaxpacketsize1;
    BYTE inep_wmaxpacketsize2;  		      // wmaxpacketsize, 64 bytes
    BYTE inep_binterval;	                  // binterval: ignored for bulk transfer
}	;

/**************************************HID descriptor structure *************************/
struct abromConfigurationDescriptorHid
{
//INTERFACE DESCRIPTOR (9 bytes)
    BYTE sizeof_interface_descriptor;        // Desc Length
    BYTE desc_type_interface;                // DescriptorType
    BYTE interface_number_hid;               // Interface number
    BYTE balternatesetting;                  // Any alternate settings if supported
    BYTE bnumendpoints;                      // Number of end points required
    BYTE binterfaceclass;                    // Class ID
    BYTE binterfacesubclass;                 // Sub class ID
    BYTE binterfaceprotocol;                 // Protocol
    BYTE intf_string_index;                  // String Index

//hid descriptor (9 bytes)
    BYTE blength_hid_descriptor;             // HID Desc length
    BYTE hid_descriptor_type;                // HID Desc Type
    BYTE hidrevno1;                          // Rev no
    BYTE hidrevno2;                          // Rev no - 2nd part
    BYTE tcountry;	    	                  // Country code
    BYTE numhidclasses;                      // Number of HID classes to follow	
    BYTE report_descriptor_type;             // Report desc type
    BYTE tlength;                            // Total length of report descriptor
    BYTE size_rep_desc;

//input end point descriptor (7 bytes)
    BYTE size_inp_endpoint_descriptor;       // End point desc size
    BYTE desc_type_inp_endpoint;             // Desc type
    BYTE hid_inep_addr;                      // Input end point address
    BYTE ep_desc_attr_type_inp_int;          // Type of end point
    BYTE  inp_wmaxpacketsize1;               // Max packet size
    BYTE  inp_wmaxpacketsize2;
    BYTE inp_binterval;                      // bInterval in ms

 // Output end point descriptor; (7 bytes)
    BYTE size_out_endpoint_descriptor;       // Output endpoint desc size
    BYTE desc_type_out_endpoint;             // Desc type
    BYTE hid_outep_addr;                     // Output end point address
    BYTE ep_desc_attr_type_out_int;          // End point type
    BYTE out_wmaxpacketsize1;                // Max packet size
    BYTE out_wmaxpacketsize2;
    BYTE out_binterval;                      // bInterval in ms
};

/**************************************MSC descriptor structure *************************/
struct abromConfigurationDescriptorMsc
{
// INTERFACE DESCRIPTOR (9 bytes)
    BYTE sizeof_interface_descriptor;         // Desc Length
    BYTE desc_type_interface;                 // DescriptorType
    BYTE interface_number_hid;                // Interface number
    BYTE balternatesetting;                   // Any alternate settings if supported
    BYTE bnumendpoints;                       // Number of end points required
    BYTE binterfaceclass;                     // Class ID
    BYTE binterfacesubclass;                  // Sub class ID
    BYTE binterfaceprotocol;                  // Protocol
    BYTE intf_string_index;                   // String Index

// input end point descriptor (7 bytes)
    BYTE size_inp_endpoint_descriptor;        // End point desc size
    BYTE desc_type_inp_endpoint;              // Desc type
    BYTE hid_inep_addr;                       // Input end point address
    BYTE ep_desc_attr_type_inp_int;           // Type of end point
    BYTE  inp_wmaxpacketsize1;                // Max packet size
    BYTE  inp_wmaxpacketsize2;
    BYTE inp_binterval;                       // bInterval in ms

// Output end point descriptor; (7 bytes)
    BYTE size_out_endpoint_descriptor;        // Output endpoint desc size
    BYTE desc_type_out_endpoint;              // Desc type
    BYTE hid_outep_addr;                      // Output end point address
    BYTE ep_desc_attr_type_out_int;           // End point type
    BYTE out_wmaxpacketsize1;                 // Max packet size
    BYTE out_wmaxpacketsize2;
    BYTE out_binterval;                       // bInterval in ms
};

/* Global structure having Generic,CDC,HID, MSC structures */
struct  abromConfigurationDescriptorGroup
{
    /* Generic part of config descriptor */
    const struct abromConfigurationDescriptorGenric abromConfigurationDescriptorGenric;
#ifdef _MSC_
    /* MSC descriptor structure */
    const struct abromConfigurationDescriptorMsc stMsc[MSC_NUM_INTERFACES];
#endif
#ifdef _CDC_
    /* CDC descriptor structure */
    const struct abromConfigurationDescriptorCdc stCdc[CDC_NUM_INTERFACES];
#endif
#ifdef _HID_
    /* HID descriptor structure */
    const struct abromConfigurationDescriptorHid stHid[HID_NUM_INTERFACES];
#endif
};

extern const struct  abromConfigurationDescriptorGroup abromConfigurationDescriptorGroup;
extern BYTE const abromDeviceDescriptor[SIZEOF_DEVICE_DESCRIPTOR];
extern BYTE const abromStringDescriptor[];
extern BYTE const abromReportDescriptor[SIZEOF_REPORT_DESCRIPTOR];

/* Handle Structure - Will be populated in descriptors.c based on number of CDC,HID interfaces */
struct tUsbHandle
{
    BYTE ep_In_Addr;               // Input EP Addr
    BYTE ep_Out_Addr;              // Output EP Addr
    BYTE edb_Index;                // The EDB index
    BYTE dev_Class;                // Device Class- 2 for CDC, 3 for HID
    WORD intepEP_X_Buffer;         // Interupt X Buffer Addr
    WORD intepEP_Y_Buffer;         // Interupt Y Buffer Addr
    WORD oep_X_Buffer;             // Output X buffer Addr
    WORD oep_Y_Buffer;             // Output Y buffer Addr
    WORD iep_X_Buffer;             // Input X Buffer Addr
    WORD iep_Y_Buffer;             // Input  Y Buffer Addr
};

extern const struct tUsbHandle stUsbHandle[CDC_NUM_INTERFACES + HID_NUM_INTERFACES + MSC_NUM_INTERFACES];
extern const tDEVICE_REQUEST_COMPARE tUsbRequestList[];

#ifdef __cplusplus
}
#endif

#endif

/*------------------------ Nothing Below This Line --------------------------*/

