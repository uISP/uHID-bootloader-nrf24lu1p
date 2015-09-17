#include <arch/nRF24LU1p.h>
#include <arch/delay.h>
#include <stdbool.h>
#include <arch/antares.h>
#include <mcs51reg.h>
#include <usb.h>


#define OFFSET_string_zero 256
#define OFFSET_g_usb_string_desc_1 260
#define OFFSET_g_usb_string_desc_2 290
#define OFFSET_g_usb_string_desc_3 300
#define OFFSET_g_usb_conf_desc 330
#define OFFSET_g_usb_hid_report_desc 364
#define OFFSET_g_usb_dev_desc 406
#define OFFSET_ldr_dev_info 424



