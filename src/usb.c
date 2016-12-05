/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic 
 * Semiconductor ASA.Terms and conditions of usage are described in detail 
 * in NORDIC SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRENTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *              
 * $LastChangedRevision: 133 $
 */

/** @file
 * Minimalistic USB code for the bootloader.
 *
 */
#include <arch/nRF24LU1p.h>
#include <arch/antares.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <usb.h>

extern void led(int s);

#ifndef NULL
#define NULL 0
#endif

/** Leaves the minimum of the two arguments */
#define MIN(a, b) ((a) < (b) ? (a) : (b))

//XDATA (BIG) Part (Partial definition. Lack ISO registers)
#define IVEC    (*((__xdata unsigned char *)0xC7A8))
#define IN_IRQ  (*((__xdata unsigned char *)0xC7A9))
#define OUT_IRQ (*((__xdata unsigned char *)0xC7AA))
#define USBIRQ  (*((__xdata unsigned char *)0xC7AB))
#define IN_IEN  (*((__xdata unsigned char *)0xC7AC))
#define OUT_IEN (*((__xdata unsigned char *)0xC7AD))
#define USBIEN  (*((__xdata unsigned char *)0xC7AE))

#define EP0CS   (*((__xdata unsigned char *)0xC7B4))

#define IN0BC   (*((__xdata unsigned char *)0xC7B5))
#define IN1CS   (*((__xdata unsigned char *)0xC7B6))
#define IN1BC   (*((__xdata unsigned char *)0xC7B7))
#define IN2CS   (*((__xdata unsigned char *)0xC7B8))
#define IN2BC   (*((__xdata unsigned char *)0xC7B9))
#define IN3CS   (*((__xdata unsigned char *)0xC7BA))
#define IN3BC   (*((__xdata unsigned char *)0xC7BB))
#define IN4CS   (*((__xdata unsigned char *)0xC7BC))
#define IN4BC   (*((__xdata unsigned char *)0xC7BD))
#define IN5CS   (*((__xdata unsigned char *)0xC7BE))
#define IN5BC   (*((__xdata unsigned char *)0xC7BF))

#define OUT0BC   (*((__xdata unsigned char *)0xC7C5))
#define OUT1CS   (*((__xdata unsigned char *)0xC7C6))
#define OUT1BC   (*((__xdata unsigned char *)0xC7C7))
#define OUT2CS   (*((__xdata unsigned char *)0xC7C8))
#define OUT2BC   (*((__xdata unsigned char *)0xC7C9))
#define OUT3CS   (*((__xdata unsigned char *)0xC7CA))
#define OUT3BC   (*((__xdata unsigned char *)0xC7CB))
#define OUT4CS   (*((__xdata unsigned char *)0xC7CC))
#define OUT4BC   (*((__xdata unsigned char *)0xC7CD))
#define OUT5CS   (*((__xdata unsigned char *)0xC7CE))
#define OUT5BC   (*((__xdata unsigned char *)0xC7CF))


//EP Address pointers
#define BOUT1ADDR (*((__xdata unsigned char *)0xC781))
#define BOUT2ADDR (*((__xdata unsigned char *)0xC782))
#define BOUT3ADDR (*((__xdata unsigned char *)0xC783))
#define BOUT4ADDR (*((__xdata unsigned char *)0xC784))
#define BOUT5ADDR (*((__xdata unsigned char *)0xC785))
#define BINSTADDR (*((__xdata unsigned char *)0xC788))
#define BIN1ADDR  (*((__xdata unsigned char *)0xC789))
#define BIN2ADDR  (*((__xdata unsigned char *)0xC78A))
#define BIN3ADDR  (*((__xdata unsigned char *)0xC78B))
#define BIN4ADDR  (*((__xdata unsigned char *)0xC78C))
#define BIN5ADDR  (*((__xdata unsigned char *)0xC78D))

#define USBCS      (*((__xdata unsigned char *)0xC7D6))
#define TOGCTL     (*((__xdata unsigned char *)0xC7D7))
#define USBFRAME   (*((__xdata unsigned short*)0xC7D8))
#define FNADDR     (*((__xdata unsigned char *)0xC7DB))
#define USBPAIR    (*((__xdata unsigned char *)0xC7DD))
#define INBULKVAL  (*((__xdata unsigned char *)0xC7DE))
#define OUTBULKVAL (*((__xdata unsigned char *)0xC7DF))

#define INISOVAL (*((__xdata unsigned char *)0xC7E0))
#define OUTISOVAL (*((__xdata unsigned char *)0xC700))

static volatile uint8_t __at (0xC7E8) SETUPBUF[8];

//EP data buffers
static volatile unsigned char __at (0xC6C0) OUT0BUF[64];
static volatile unsigned char __at (0xC700) IN0BUF[64];
static volatile unsigned char __at (0xC640) OUT1BUF[64];
static volatile unsigned char __at (0xC680) IN1BUF[64];
static volatile unsigned char __at (0xC5C0) OUT2BUF[64];
static volatile unsigned char __at (0xC600) IN2BUF[64];
static volatile unsigned char __at (0xC540) OUT3BUF[64];
static volatile unsigned char __at (0xC580) IN3BUF[64];
static volatile unsigned char __at (0xC4C0) OUT4BUF[64];
static volatile unsigned char __at (0xC500) IN4BUF[64];
static volatile unsigned char __at (0xC440) OUT5BUF[64];
static volatile unsigned char __at (0xC480) IN5BUF[64];

static uint8_t usb_bm_state;
static uint8_t usb_current_config;
static uint8_t usb_current_alt_interface;
static usb_state_t usb_state;

static uint8_t __xdata *packetizer_data_ptr;
static uint8_t packetizer_data_size;
static uint8_t packetizer_pkt_size;

static uint8_t bmRequestType;



static void packetizer_isr_ep0_in();
static void usb_process_get_status();
static void usb_process_get_descriptor();

static void delay_ms(uint16_t ms)
{
	uint16_t i, j;
    
	for(i = 0; i < ms; i++ )
	{
		for( j = 0; j < 1403; j++)
		{
			__asm
				nop /* 1 cycle */
				__endasm;

		}
	}
}


void usb_init(void)
{
	// Setup state information
	usb_state = DEFAULT;
	usb_bm_state = 0;

	// Setconfig configuration information
	usb_current_config = 0;
	usb_current_alt_interface = 0;
    
	// Disconnect from USB-bus since we are in this routine from a power on and not a soft reset:

	USBCS |= 0x08;
	delay_ms(50);
	USBCS &= ~0x08;

	USBIEN = 0x1d;
	IN_IEN = 0x01;
	IN_IRQ = 0x1f;
	OUT_IEN = 0x01;
	OUT_IRQ = 0x1f;

	// Setup the USB RAM with some OK default values:
	BOUT1ADDR = MAX_PACKET_SIZE_EP0/2;
	BOUT2ADDR = MAX_PACKET_SIZE_EP0/2 + USB_EP1_SIZE/2;
	BOUT3ADDR = MAX_PACKET_SIZE_EP0/2 + 2*USB_EP1_SIZE/2;
	BOUT4ADDR = MAX_PACKET_SIZE_EP0/2 + 3*USB_EP1_SIZE/2;
	BOUT5ADDR = MAX_PACKET_SIZE_EP0/2 + 4*USB_EP1_SIZE/2;
	BINSTADDR = 0xc0;
	BIN1ADDR = MAX_PACKET_SIZE_EP0/2;
	BIN2ADDR = MAX_PACKET_SIZE_EP0/2 + USB_EP1_SIZE/2;
	BIN3ADDR = MAX_PACKET_SIZE_EP0/2 + 2*USB_EP1_SIZE/2;
	BIN4ADDR = MAX_PACKET_SIZE_EP0/2 + 3*USB_EP1_SIZE/2;
	BIN5ADDR = MAX_PACKET_SIZE_EP0/2 + 4*USB_EP1_SIZE/2;

	// Set all endpoints to not valid (except EP0I and EP0OUT)
	INBULKVAL = 0x01;
	OUTBULKVAL = 0x01;
	INISOVAL = 0x00;
	OUTISOVAL = 0x00;

	IN_IEN |= 0x02;; 
	INBULKVAL |= 0x02;
	OUT_IEN |= 0x02;
	OUTBULKVAL |= 0x02;
	OUT1BC = 0xff;
}


static void packetizer_isr_ep0_in()
{
	uint8_t size, i;
	// We are getting a ep0in interupt when the host send ACK and do not have any more data to send
	if(packetizer_data_size == 0)
	{
		IN0BC = 0;
		USB_EP0_HSNAK();
		return;
	}

	size = MIN(packetizer_data_size, packetizer_pkt_size);

	// Copy data to the USB-controller buffer
	memcpy(IN0BUF, packetizer_data_ptr, size);

	// Tell the USB-controller how many bytes to send
	// If a IN is received from host after this the USB-controller will send the data
	IN0BC = size;

	// Update the packetizer data
	packetizer_data_ptr += size;
	packetizer_data_size -= size;
}

void ep0_send_data(uint8_t __xdata *data, uint8_t len)
{
	packetizer_data_ptr = data;
	packetizer_data_size = MIN(SETUPBUF[6], len);
	packetizer_isr_ep0_in();  
}

static void usb_process_get_status()
{
	IN0BUF[0] = 0;
	IN0BUF[1] = 0x00;
	if((usb_state == ADDRESSED) && (SETUPBUF[4] == 0x00))
	{
		IN0BC = 0x02;
	}
	else if(usb_state == CONFIGURED)
	{
		switch(bmRequestType)
		{
		case 0x80: // Device
			if((usb_bm_state & USB_BM_STATE_ALLOW_REMOTE_WAKEUP ) == USB_BM_STATE_ALLOW_REMOTE_WAKEUP)
			{
				IN0BUF[0] = 0x02;
			}
			IN0BC = 0x02;
			break;

		case 0x81: // Interface
			IN0BC = 0x02;
			break;

		case 0x82: // Endpoint
			if((SETUPBUF[4] & 0x80) == 0x80) // IN endpoints
				IN0BUF[0] = IN1CS;
			else
				IN0BUF[0] = OUT1CS;
			IN0BC = 0x02;
			break;
		default:
			USB_EP0_STALL();
			break;
		}
	}
	else
	{
		// We should not be in this state
		USB_EP0_STALL();
	}
}

#include "ipage_descriptors.h"

void send_ipage_descriptor(uint16_t addr, uint8_t len)
{
	FSR|=(1<<3);
	ep0_send_data((__xdata uint8_t*) addr, len);
}

static void usb_process_get_descriptor()
{ 
	FSR&=~(1<<3);
	switch(SETUPBUF[3])
	{	    
	case USB_DESC_DEVICE:
		ep0_send_data((__xdata uint8_t *)&g_usb_dev_desc, sizeof(usb_dev_desc_t));
		//send_ipage_descriptor(IPAGE_OFF_g_usb_dev_desc, IPAGE_LEN_g_usb_dev_desc);
		break;
	    
	case USB_DESC_CONFIGURATION:
		ep0_send_data((__xdata uint8_t *)&g_usb_conf_desc, sizeof(usb_conf_desc_bootloader_t));
		//send_ipage_descriptor(IPAGE_OFF_g_usb_conf_desc, IPAGE_LEN_g_usb_conf_desc);
		break;

	case USB_DESC_HID:
		ep0_send_data((__xdata uint8_t *)&g_usb_conf_desc.hid, sizeof(usb_hid_desc_t));
		break;
	case USB_DESC_HID_REPORT:
		ep0_send_data((__xdata uint8_t *)&g_usb_hid_report_desc, USB_DESC_HID_LEN);
		break;
        case USB_DESC_STRING:
		if(SETUPBUF[2] == 0x00)
		{
			ep0_send_data((__xdata uint8_t *)string_zero, sizeof(string_zero));
		}
		else
		{
			if((SETUPBUF[2] - 1) < USB_STRING_DESC_COUNT)
			{
				uint8_t *ptr;
				switch (SETUPBUF[2]) {
				case 1:
					ptr = g_usb_string_desc_1;
					break;
				case 2:
					ptr = g_usb_string_desc_2;
					break;
				case 3:
					ptr = g_usb_string_desc_3;
					break;
				}
				ep0_send_data((__xdata uint8_t *) ptr, ptr[0]);
			}
			else
			{
				USB_EP0_STALL();
			}
		}
		break;
        case USB_DESC_INTERFACE:
        case USB_DESC_ENDPOINT:
        case USB_DESC_DEVICE_QUAL:
        case USB_DESC_OTHER_SPEED_CONF:
        case USB_DESC_INTERFACE_POWER:
		USB_EP0_STALL();
		break;
        default:
		USB_EP0_HSNAK();
		break;
	}
}
#ifndef CONFIG_FLASH_SIZE
#error
#endif


bool handle_class_request(usbRequest_t *rq);

static void isr_sudav()
{
	packetizer_pkt_size = MAX_PACKET_SIZE_EP0;
	bmRequestType = SETUPBUF[0];
	if((bmRequestType & 0x60 ) == 0x00)
	{
		switch(SETUPBUF[1])
		{
		case USB_REQ_GET_DESCRIPTOR:
			usb_process_get_descriptor();
			break;

		case USB_REQ_GET_STATUS:
			usb_process_get_status();
			break;

		case USB_REQ_SET_ADDRESS:
			usb_state = ADDRESSED;
			usb_current_config = 0x00;
			break;

		case USB_REQ_GET_CONFIGURATION:
			switch(usb_state)
			{
			case ADDRESSED:
				IN0BUF[0] = 0x00;
				IN0BC = 0x01;
				break;
			case CONFIGURED:
				IN0BUF[0] = usb_current_config;
				IN0BC = 0x01;
				break;
			case ATTACHED:
			case POWERED:
			case SUSPENDED:
			case DEFAULT:
			default:
				USB_EP0_STALL();
				break;
			}
			break;

		case USB_REQ_SET_CONFIGURATION:
			switch(SETUPBUF[2])
			{
			case 0x00:
				usb_state = ADDRESSED;
				usb_current_config = 0x00;
				USB_EP0_HSNAK();
				break;
			case 0x01:
				usb_state = CONFIGURED;
				usb_bm_state |= USB_BM_STATE_CONFIGURED;
				usb_current_config = 0x01;
				USB_EP0_HSNAK();
				break;
			default:
				USB_EP0_STALL();
				break;
			}
			break;

		case USB_REQ_GET_INTERFACE: // GET_INTERFACE
			IN0BUF[0] = usb_current_alt_interface;
			IN0BC = 0x01;
			break;

		case USB_REQ_SET_DESCRIPTOR:
		case USB_REQ_SET_INTERFACE: // SET_INTERFACE
		case USB_REQ_SYNCH_FRAME:   // SYNCH_FRAME
		default:
			USB_EP0_STALL();
			break;
		}
	} 
	// bmRequestType = 0 01 xxxxx : Data transfer direction: Host-to-device, Type: Class
	else if((bmRequestType & 0x60 ) == 0x20)  // Class request
	{
		bool cret = handle_class_request((usbRequest_t *) SETUPBUF);
		
		if (SETUPBUF[6] != 0 && ((bmRequestType & 0x80) == 0x00))
			OUT0BC = 0xff;
		else if (!cret)
			USB_EP0_HSNAK();

	} else  // Unknown request type
	{
		USB_EP0_STALL();
	}

}

void usb_handle_data(void *buf, uint8_t len);

void usb_irq(void)
{
        switch(IVEC)
        {
	case INT_USBRESET:
		USBIRQ = 0x10;
		usb_state = DEFAULT;
		usb_current_config = 0;
		usb_current_alt_interface = 0;
		usb_bm_state = 0;
		break;		
	case INT_SUDAV:
                USBIRQ = 0x01;
                isr_sudav();
                break;
	case INT_SOF:
                USBIRQ = 0x02;
                break;
	case INT_SUTOK:
                USBIRQ = 0x04;
                packetizer_data_ptr = NULL;
                packetizer_data_size = 0;
                packetizer_pkt_size = 0;
                break;
	case INT_SUSPEND:
                USBIRQ = 0x08;
                break;
	case INT_EP0IN:
                IN_IRQ = 0x01;
                packetizer_isr_ep0_in();
                break;
	case INT_EP0OUT:
		usb_handle_data(OUT0BUF, OUT0BC);
                OUT_IRQ = 0x01;
		OUT0BC=0xff;
		USB_EP0_HSNAK();
                break;
	case INT_EP1IN:
                // Clear interrupt 
                IN_IRQ = 0x02;
                IN1CS = 0x02;
                break;
	case INT_EP1OUT:
                // Clear interrupt
                OUT_IRQ = 0x02;     
                OUT1BC = 0xff;
                break;
	default:
                break;
        }
}
