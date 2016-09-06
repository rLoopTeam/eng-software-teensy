/*****************************************************************************
*
* Title  : Texas Instruments AMC7812 Integrated ADC & DAC Driver
* Author : Matthew Ebert (c)2015
* Copyright: GPL V3
*
* This driver provides initialization and transmit/receive
* functions for the TI AMC7812 integrated ADC and DAC.
* Only SPI standalone mode is supported.
*
* This file contains 8-bit error/status code definitions for return values
*
*****************************************************************************/

#ifndef AMC7812ERR_H
#define AMC7812ERR_H

#define AMC7812_DEV_ID_ERR    0x01 // device id does not match (comms failed or wrong device)
#define AMC7812_PARAM_OOR_ERR 0x02 // parameter out of range
#define AMC7812_WRITE_ERR     0x03 // read confirmation did not match written value
#define AMC7812_TIMEOUT_ERR   0x04 // an operation timed out

#endif
