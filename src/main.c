#include <arch/nRF24LU1p.h>
#include <arch/delay.h>
#include <stdbool.h>
#include <arch/antares.h>
#include <mcs51reg.h>
#include <usb.h>

void led(int s)
{
	P0 &= ~(1<<3);
	P0 |= s << 3;	
}

/*
typedef struct usbRequest{
    uint8_t       bmRequestType;
    uint8_t       bRequest;
    uint16_t   wValue;
    uint16_t   wIndex;
    uint16_t   wLength;
} usbRequest_t;
*/

#define APP_FLASH_SIZE (CONFIG_FLASH_SIZE )

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

void run_main_app (void)
{
	__xdata uint8_t *fl = 0x0;
	uint16_t rstvec;

	USBCS |= 0x08;
	delay_ms(50);

	rstvec = fl[1] << 8 | fl[2]; 
	((void (__code *) (void)) rstvec) ();
}


void ep0_send_data(uint8_t __xdata *data, uint8_t len);

static uint16_t flash_addr;
void flash_erase_page(uint8_t pn);
void flash_write_data(uint16_t a, __xdata uint8_t *p, uint16_t n);

void usb_handle_data(void *buf, uint8_t len)
{
	flash_unprotect();
	if (!(flash_addr & 511))
		flash_erase_page(flash_addr >> 9);
	
	flash_write_data(flash_addr, buf, len);
	flash_protect();
	flash_addr+=len;
}

bool handle_class_request(usbRequest_t *rq)
{
	uint8_t repId = rq->wValue & 0xff;
	bool write = (rq->bRequest == USBRQ_HID_SET_REPORT);

	if (0 == repId)
		if (write) { 
			run_main_app();
			return 0; /* Never reached */
		}
		else {
			flash_addr = 0;
			ep0_send_data((__xdata uint8_t *) ldr_dev_info, 
				      sizeof(struct deviceInfo)); 
			return 0;
		}
	
	FSR&=~(1<<3);
	if (repId > 1)
		FSR|=(1<<3);		
	
	if (!write) { 
		ep0_send_data((__xdata uint8_t *) flash_addr, MAX_PACKET_SIZE_EP0); 
		flash_addr+=MAX_PACKET_SIZE_EP0;
		return 0;
	}
	return 1;
}


int main()
{
	P0DIR &= ~(1<<3);
	P0 |= 1<< 3;
	EA = 0;
	
	CKCON = 0x02;       // See nRF24LU1p AX PAN
	usb_init();
	for(;;)
	{
		if (USBF)
		{
			USBF = 0;
			usb_irq();
		}
	}
}





