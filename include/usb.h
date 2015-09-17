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
 *
 * This file contain structures and constants defined in Chapter 9 of the USB 2.0 standard
 *
 */
#ifndef USB_H__
#define USB_H__

#include "usb_desc_bootloader.h"

#define MAX_PACKET_SIZE_EP0 64
#define USB_EP1_SIZE        64


#define USB_EP0_HSNAK() do {EP0CS = 0x02; } while(0)
#define USB_EP0_STALL() do {EP0CS = 0x11; } while(0) // Set both DSTALL and STALL when we want to stall a request during a SETUP transaction

#define INT_SUDAV    0x00
#define INT_SOF      0x04
#define INT_SUTOK    0x08
#define INT_SUSPEND  0x0C
#define INT_USBRESET 0x10
#define INT_EP0IN    0x18
#define INT_EP0OUT   0x1C
#define INT_EP1IN    0x20
#define INT_EP1OUT   0x24

#define USB_BM_STATE_CONFIGURED           0x01
#define USB_BM_STATE_ALLOW_REMOTE_WAKEUP  0x02

/** An enum describing the USB state
 * 
 *  The states described in this enum are found in Chapter 9 of the USB 2.0 specification
 */

typedef enum 
{
    ATTACHED,
    POWERED,
    DEFAULT,
    ADDRESSED,
    CONFIGURED,
    SUSPENDED
} usb_state_t;

void usb_init(void);
void usb_irq(void);

typedef struct usbRequest{
    uint8_t       bmRequestType;
    uint8_t       bRequest;
    uint16_t   wValue;
    uint16_t   wIndex;
    uint16_t   wLength;
} usbRequest_t;


#define USBRQ_HID_SET_REPORT 0x09
#define USBRQ_HID_GET_REPORT 0x01
#endif  // USB_H__
