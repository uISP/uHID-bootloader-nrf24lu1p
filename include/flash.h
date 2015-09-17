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
 * Header file for flash.c
 *
 */
#ifndef FLASH_H__
#define FLASH_H__

#include <stdint.h>
#include <stdbool.h>

/** Function to erase a page in the Flash memory
 *  @param pn Page number
 */
void flash_page_erase(uint8_t pn);

/** Function to write n bytes to the Flash memory
 *  @param a 16 bit address in Flash
 *  @param *p pointer to bytes to write
 *  @param n number of bytes to write
 */
void flash_bytes_write(uint16_t a, __xdata uint8_t *p, uint16_t n);

/** Function to write a byte to the Flash memory
 *  @param a 16 bit address in Flash
 *  @param b byte to write
  */
void flash_byte_write(uint16_t a, uint8_t b);

/** Function to read n bytes from the Flash memory
 *  @param a 16 bit address in Flash
 *  @param *p pointer to bytes to write
 *  @param n number of bytes to read
 */
void flash_bytes_read(uint16_t a, __xdata uint8_t  *p, uint16_t n);

#endif  // FLASH_H__
