
#define SWAP(x) (x)

#include "usb.h"
#include "usb_desc_bootloader.h"
#define APP_FLASH_SIZE (16*1024)
__code usb_dev_desc_t g_usb_dev_desc = {
  sizeof(usb_dev_desc_t), 
  USB_DESC_DEVICE, 
  SWAP(0x0200),       // bcdUSB
  0xff,               // bDeviceClass
  0x0,               // bDeviceSubclass
  0x0,               // bDeviceProtocol
  MAX_PACKET_SIZE_EP0,// bMaxPAcketSize0
  SWAP(0x16c0),         // Fake uISP vid
  SWAP(0x05df),       // and PID
  SWAP(0x0001),       // bcdDevice - Device Release Number (BCD)
  0x01,               // iManufacturer
  0x02,               // iProduct
  0x03,               // iSerialNumber
  0x01                // bNumConfigurations
};


__code uint8_t g_usb_hid_report_desc[USB_DESC_HID_LEN] = {
	0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
	0x09, 0x01,                    // USAGE (Vendor Usage 1)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
	0x75, 0x08,                    //   REPORT_SIZE (8)

	0x85, 0x01,                    //   REPORT_ID (1)
	0x95, 0x06,                    //   REPORT_COUNT (6)
	0x09, 0x00,                    //   USAGE (Undefined)
	0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)

	0x85, 0x02,                    //   REPORT_ID (2)
	0x95, 0x83,                    //   REPORT_COUNT (131)
	0x09, 0x00,                    //   USAGE (Undefined)
	0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
    
	0x85, 0x03,                    //   REPORT_ID (3)
	0x95, 0x05,                    //   REPORT_COUNT (0x3)
	0x09, 0x00,                    //   USAGE (Undefined)
	0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
    
	0xc0                           // END_COLLECTION
};



__code usb_conf_desc_bootloader_t g_usb_conf_desc = 
{
    {
        sizeof(usb_conf_desc_t),
        USB_DESC_CONFIGURATION,
        SWAP(sizeof(usb_conf_desc_bootloader_t)),
        1,            // bNumInterfaces
        1,            // bConfigurationValue
        0,            // iConfiguration
        0x80,         // bmAttributes - D6: self-powered, bus powered: 0xA0
        50,           // bMaxPower
    },
    /* Interface Descriptor 0 */ 
    {
        sizeof(usb_if_desc_t),
        USB_DESC_INTERFACE,
        0,            // bInterfaceNumber
        0,            // bAlternateSetting
        1,            // bNumEndpoints
        0x3,         // bInterfaceClass
        0x00,         // bInterfaceSubClass  
        0x0,         // bInterfaceProtocol 
        0x00,         // iInterface
    },
{
	9,          /* sizeof(usbDescrHID): length of descriptor in bytes */
	USB_DESC_HID,   /* descriptor type: HID */
	0x01, 0x01, /* BCD representation of HID version */
	0x00,       /* target country code */
	0x01,       /* number of HID Report (or other HID class) Descriptor infos to follow */
	0x22,       /* descriptor type: report */
	USB_DESC_HID_LEN, 0,  /* total length of report descriptor */
},

     /* Endpoint Descriptor EP1IN */
     {
        sizeof(usb_ep_desc_t),
        USB_DESC_ENDPOINT,
        0x81,                   // bEndpointAddress
        USB_ENDPOINT_TYPE_INTERRUPT, // bmAttributes
        SWAP(USB_EP1_SIZE),     // wMaxPacketSize
        0x06                    // bInterval
     },
};

#define USB_STRING_IDX_1_DESC "www.ncrmnt.org"

__code uint8_t g_usb_string_desc_1[] = 
{
    sizeof(USB_STRING_IDX_1_DESC) * 2, 0x03,
    'w',0,
    'w',0, 
    'w',0,
    '.',0,
    'n',0,
    'c',0,
    'r',0,
    'm',0,
    'n',0,
    't',0,
    '.',0,
    'o',0,
    'r',0,
    'g',0
};

#define USB_STRING_IDX_2_DESC "uHID"

__code uint8_t g_usb_string_desc_2[] = 
{
    sizeof(USB_STRING_IDX_2_DESC) * 2, 0x03,
    'u',0,
    'H',0,
    'I',0,
    'D',0,
};

#define USB_STRING_IDX_3_DESC "nRF24LU1p-boot"

__code uint8_t g_usb_string_desc_3[] = 
{
    sizeof(USB_STRING_IDX_3_DESC) * 2, 0x03,
    'n',0,
    'R',0,
    'F',0,
    '2',0,
    '4',0,
    'L',0,
    'U',0,
    '1',0,
    'p',0,
    '-',0,
    'b',0,
    '0',0,
    '0',0,
    't',0,
};


// This is for setting language American English (String descriptor 0 is an array of supported languages):
__code uint8_t string_zero[] = {0x04, 0x03, 0x09, 0x04} ;

#define UISP_PART_NAME_LEN  8
#define UISP_CHIP_NAME_LEN 16

struct partInfo {
	uint8_t       id;
	uint16_t      pageSize;
	uint32_t      size;
	uint8_t       ioSize;
	uint8_t       name[UISP_PART_NAME_LEN];
};

struct deviceInfo {
	uint8_t       reportId;
	uint8_t       name[UISP_CHIP_NAME_LEN];
	uint8_t       numParts;
	uint8_t       cpuFreq;
	struct partInfo parts[2];
};


__code struct deviceInfo ldr_dev_info = {
	.name = "nRF24LU1p",
	.numParts=2,
	.cpuFreq = 160,
	.parts = { 
		{
			.id=1,
			.ioSize   = MAX_PACKET_SIZE_EP0,
			.pageSize = 512,
			.size = APP_FLASH_SIZE,
			.name = "flash",
		}, 
		{ 	.id=2,
			.pageSize = 512,
			.size = 512,
			.ioSize   = MAX_PACKET_SIZE_EP0,
			.name = "ipage",
		}
	},
};

int main()
{

}
