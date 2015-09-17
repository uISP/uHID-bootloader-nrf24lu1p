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
 * Header file for the Bootloader USB descriptor
 *
 */
#ifndef USB_DESC_BOOTLOADER_H__
#define USB_DESC_BOOTLOADER_H__

#include "usb_desc.h"

#define USB_STRING_DESC_COUNT 3
#define USB_DESC_HID_LEN 42

typedef struct
{
	usb_conf_desc_t conf;
	usb_if_desc_t if0;
	usb_hid_desc_t hid;
	usb_ep_desc_t ep1in;
} __attribute_packed__ usb_conf_desc_bootloader_t;

extern __code usb_conf_desc_bootloader_t g_usb_conf_desc;
extern __code usb_dev_desc_t g_usb_dev_desc;
extern __code uint8_t g_usb_string_desc_1[];
extern __code uint8_t g_usb_string_desc_2[];
extern __code uint8_t g_usb_string_desc_3[];

extern __code usb_hid_desc_t g_usb_hid_desc;

extern __code uint8_t g_usb_hid_report_desc[USB_DESC_HID_LEN];
extern __code uint8_t string_zero[4];

#endif  // USB_DESC_TEMPL_H__
