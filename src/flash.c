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
 * Flash (self) programming functions
 *
 */
#include <arch/nRF24LU1p.h>
#include "flash.h"

void flash_unprotect() {
    CKCON = 0x01;   // See nRF24LU1p AX PAN
    // Enable flash write operation:
    FCR = 0xAA;
    FCR = 0x55;
    WEN = 1;	
}

void flash_protect() {
    WEN = 0;
    CKCON = 0x02;	
}

void flash_erase_page(uint8_t pn)
{  
    FCR = pn;
}

void flash_write_data(uint16_t a, __xdata uint8_t *p, uint16_t n)
{
     __xdata uint8_t *pb;
    // Write the bytes directly to the flash:
    pb = ( __xdata uint8_t *)a;
    while(n--)
    {
        *pb++ = *p++;
    }
}
