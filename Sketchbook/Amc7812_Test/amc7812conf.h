/*******************************************************************************
*
* Title  : Texas Instruments AMC7812 Integrated ADC & DAC Driver
* Author : Matthew Ebert (c)2015
* Copyright: GPL V3
*
* This header defines default settings for connections for communication with
* an amc7812 ADC/DAC
*
* NOTE if you put this in the same directory as the executable you are
* compiling it overrides the default in [PROEJCT_DIR]/include/
*
*******************************************************************************/

#ifndef AMC7812CONF_H
#define AMC7812CONF_H

// I want a configuration file in the working directory to override the default
//#pragma message "using configuration file found at path: " __FILE__

#include <Arduino.h>
//#include <SPI.h>

#define AMC7812_TIMEOUT_CONV_CYCLS 16000 // 1 ms at 16 MHz clock, ignoring overhead

// VOLTAGE REFERENCE
// store as string since we dont want to do fp math
#define AMC7812_INT_AVREF "2.5"
#define AMC7812_AVREF AMC7812_INT_AVREF

#endif
