/*****************************************************************************
*
* Title  : Texas Instruments AMC7812 Integrated ADC & DAC Driver
* Author : Matthew Ebert (c)2015
* Copyright: GPL V3
*
*This driver provides initialization and transmit/receive
*functions for the TI AMC7812 integrated ADC and DAC.
*
*****************************************************************************/

#ifndef AMC7812_H
#define AMC7812_H

#define AMC7812_I2C_ADDR (0xC2 >> 1) 
#define AMC7812_RST_PIN 23
#define AMC7812_DSCLR0_PIN 15
#define AMC7812_DSCLR1_PIN 16

// AMC7812 Register Mappings
// from page 60 of the datasheet SBAS513E
// defualt values are for SPI are shown in []

// TEMPERATURE
#define AMC7812_TEMP_BASE_ADDR   0x00 // base address for temperature sensors
#define AMC7812_TEMP_MAX_ADDR    0x02 // max address for termparture sensors
#define AMC7812_TEMP_SENS_CNT    3    // number of teperature sensors

#define AMC7812_TEMP_LOCAL       0x00 // (R) address for local temperature sensor
#define AMC7812_TEMP_D1          0x01 // (R) address for D1 temperature sensor
#define AMC7812_TEMP_D2          0x02 // (R) address for D2 temperature sensor
#define AMC7812_TEMP_CONF        0x0a // (R/W) temperature configuration [0x003C]
#define AMC7812_TEMP_CONV        0x0b // (R/W) temperature conversion rate [0x0007]
#define AMC7812_TEMP_D1_ETA      0x21 // (R/W) eta-factor correction for D1
#define AMC7812_TEMP_D2_ETA      0x22 // (R/W) eta-factor correction for D2
// ADCs
#define AMC7812_ADC_BASE_ADDR    0x23 // (R) ADC 0 address
#define AMC7812_ADC_MAX_ADDR     0x32 // (R) ADC n address
#define AMC7812_ADC_CNT          16   // number of onboard ADCs
// DACs
#define AMC7812_DAC_BASE_ADDR    0x33 // (R/W) DAC 0 Data
#define AMC7812_DAC_MAX_ADDR     0x3e // (R/W) DAC n Data
#define AMC7812_DAC_CNT          12   // number of onboard DACs
// DAC-CLRs
#define AMC7812_DAC_CLR_BASE_ADDR 0x3f // (R/W) DAC n CLR Setting [0x0000]
#define AMC7812_DAC_CLR_MAX_ADDR  0x4a // (R/W) DAC 11 CLR Setting [0x0000]
// OTHER STUFF
#define AMC7812_GPIO        0x4b // (R/W) GPIO [0x00FF]
#define AMC7812_AMC_CONF_0  0x4c // (R/W) AMC configuration 0 [0x2000]
#define AMC7812_AMC_CONF_1  0x4d // (R/W) AMC configuration 1 [0x0070]
#define AMC7812_AMC_CONF_CNT   2 // number of conversion registers
#define AMC7812_ALARM_CTRL  0x4e // (R/W) Alarm control [0x0000]
#define AMC7812_STATUS      0x4f // (R) Status
#define AMC7812_ADC_0_CONF  0x50 // (R/W) ADC 0 configuration [0x0000]
#define AMC7812_ADC_1_CONF  0x51 // (R/W) ADC 1 configuration [0x0000]
#define AMC7812_ADC_GAIN    0x52 // (R/W) ADC Gain [0xffff]
#define AMC7812_AUTO_DAC_CLR_SRC  0x53 // (R/W) auto dac clear source [0x0004]
#define AMC7812_AUTO_DAC_CLR_EN   0x54 // (R/W) auto dac clear enable [0x0000]
#define AMC7812_SW_DAC_CLR  0x55 // (R/W) software dac clear [0x0000]
#define AMC7812_HW_DAC_CLR_EN_0   0x56 // (R/W) hardware dac clear enable 0 [0x0000]
#define AMC7812_HW_DAC_CLR_EN_1   0x57 // (R/W) hardware dac clear enable 1 [0x0000]
#define AMC7812_DAC_CONF    0x58 // (R/W) DAC configuration
#define AMC7812_DAC_GAIN    0x59 // (R/W) DAC gain
#define AMC7812_IN_0_HT     0x5a // (R/W) input 0 high threshold [0xffff]
#define AMC7812_IN_0_LT     0x5b // (R/W) input 0 low threshold [0x0000]
#define AMC7812_IN_1_HT     0x5c // (R/W) input 1 high threshold [0xffff]
#define AMC7812_IN_1_LT     0x5d // (R/W) input 1 low threshold [0x0000]
#define AMC7812_IN_2_HT     0x5e // (R/W) input 2 high threshold [0xffff]
#define AMC7812_IN_2_LT     0x5f // (R/W) input 2 low threshold [0x0000]
#define AMC7812_IN_3_HT     0x60 // (R/W) input 3 high threshold [0xffff]
#define AMC7812_IN_3_LT     0x61 // (R/W) input 3 low threshold [0x0000]
#define AMC7812_TEMP_CHIP_HT  0x62 // (R/W) chip temperature high threshold [0x07ff]
#define AMC7812_TEMP_CHIP_LT  0x63 // (R/W) chip temperature low threshold [0x0800]
#define AMC7812_TEMP_D1_HT  0x64 // (R/W) sensor 1 temperature high threshold [0x07ff]
#define AMC7812_TEMP_D1_LT  0x65 // (R/W) sensor 1 temperature low threshold [0x0800]
#define AMC7812_TEMP_D2_HT  0x66 // (R/W) sensor 2 temperature high threshold [0x07ff]
#define AMC7812_TEMP_D2_LT  0x67 // (R/W) sensor 2 temperature low threshold [0x0800]
#define AMC7812_HYST_0      0x68 // (R/W) hysteresis 0 [0x0810]
#define AMC7812_HYST_1      0x69 // (R/W) hysteresis 1 [0x0810]
#define AMC7812_HYST_2      0x6a // (R/W) hysteresis 2 [0x2108]
#define AMC7812_POWER_DOWN  0x6b // (R/W) power down [0x0000]
#define AMC7812_DEV_ID      0x6c // (R) device id [0x1220]
#define AMC7812_SW_RST      0x7c // (R/W) software reset

// CONFIGURATION REGISTER BIT MAPPINGS
//
//AMC_CONF_0 settings (1<<setting), see pg 66
#define AMC7812_CMODE       13   // Conversion mode - 0: direct, 1: auto
#define AMC7812_ICONV       12   // Internal conversion bit, starts ADC conversion
#define AMC7812_ILDAC       11   // Load DAC, 1: sync update DAC channels set for SLDAC
#define AMC7812_ADC_REF     10   // ADC Reference, 1: internal, 0: external
#define AMC7812_EN_ALR       9   // Alarm 0: disabled, 1: enabled
#define AMC7812_DAVF         7   // (Read-Only) Data available flag (direct CMODE)
#define AMC7812_GALR         6   // (Read-Only) Global alarm bit
//AMC_CONF_1 settings (1<<setting), see pg 67
#define AMC7812_CONV_RATE_1  9   // ADC conversion rate bit
#define AMC7812_CONV_RATE_0  8   // ADC conversion rate bit
#define AMC7812_CH_FALR_2    7   // False alarm protection bit CH0-3
#define AMC7812_CH_FALR_1    6   // False alarm protection bit CH0-3
#define AMC7812_CH_FALR_0    5   // False alarm protection bit CH0-3
#define AMC7812_TEMP_FALR_1  4   // False alarm protection bit temp monitor
#define AMC7812_TEMP_FALR_0  3   // False alarm protection bit temp monitor
//ALARM_CTRL
#define AMC7812_EALR_0      14   // CH 0 alarm bit, enable to trigger alarm on CH0
#define AMC7812_EALR_1      13   // CH 1 alarm bit, enable to trigger alarm on CH1
#define AMC7812_EALR_2      12   // CH 2 alarm bit, enable to trigger alarm on CH2
#define AMC7812_EALR_3      11   // CH 3 alarm bit, enable to trigger alarm on CH3
#define AMC7812_EALR_LT_L   10   // Chip low temp alarm bit, enable to trigger alarm
#define AMC7812_EALR_LT_H    9   // Chip high temp alarm bit, enable to trigger alarm
#define AMC7812_EALR_D1_L    8   // D1 low temp alarm bit, enable to trigger alarm
#define AMC7812_EALR_D1_H    7   // D1 high temp alarm bit, enable to trigger alarm
#define AMC7812_EALR_D2_L    6   // D2 low temp alarm bit, enable to trigger alarm
#define AMC7812_EALR_D2_H    5   // D2 high temp alarm bit, enable to trigger alarm
#define AMC7812_EALR_D1_F    4   // D1 fail temp alarm bit, enable to trigger alarm
#define AMC7812_EALR_D2_F    3   // D2 fail temp alarm bit, enable to trigger alarm
#define AMC7812_ALR_LATCH    2   // 0: latched alarm, 1: non-latched alarm
// ADC CHANNEL REGISTER 0
// specify which channels are read ch0-12
// and if ch0-3 are single-ended or differential
// TODO: complete, pg 71
// ADC CHANNEL REGISTER 1
// specify which channels are read ch13-15
// TODO: complete, pg 72
// ADC GAIN REGISTER
// TODO: complete, pg 72
// AUTO-DAC-CLR-SOURCE REGISTER
// which alarms force the dac outputs to clear
// TODO: complete, pg 73
// AUTO-DAC-CLR-EN REGISTER
// TODO: complete, pg 74
// SW-DAC-CLR REGISTER
// software force dac clears
// TODO: complete, pg 74
// HW-DAC-CLR-EN-0 REGISTER
// hardware force dac clears on dac-clr-0 pin
// TODO: complete, pg 74
// HW-DAC-CLR-EN1 REGISTER
// hardware force dac clears on dac-clr-1 pin
// TODO: complete, pg 75
// DAC CONF REGISTER
// TODO: complete, pg 75
// DAC GAIN REGISTER
// TODO: complete, pg 75

// start reading again at pg 78

// STATUS REGISTER BIT MAPPINGS
// reading the status register clears alarms
// read-only
#define AMC7812_ALR_0       14  // channel 0 out-of-range
#define AMC7812_ALR_1       13  // channel 1 out-of-range
#define AMC7812_ALR_2       12  // channel 2 out-of-range
#define AMC7812_ALR_3       11  // channel 3 out-of-range
#define AMC7812_ALR_LT_L    10  // chip temp under range
#define AMC7812_ALR_LT_H     9  // chip temp over range
#define AMC7812_ALR_D1_L     8  // D1 temp under range
#define AMC7812_ALR_D1_H     7  // D1 temp over range
#define AMC7812_ALR_D2_L     6  // D2 temp under range
#define AMC7812_ALR_D2_H     5  // D2 temp over range
#define AMC7812_ALR_D1_F     4  // D1 fail temp alarm bit, enable to trigger alarm
#define AMC7812_ALR_D2_F     3  // D2 fail temp alarm bit, enable to trigger alarm
#define AMC7812_ALR_THERM    2  // Chip thermal alarm, T(chip) > 150 C


#define AMC7812_READ_MASK  0x80 // mask for read operations on addr byte
#define AMC7812_WRITE_MASK 0x00 // mask for write operations on addr byte

#define AMC7812_ADC_GAIN_HIGH 2
#define AMC7812_ADC_GAIN_LOW  1
#define AMC7812_DAC_GAIN_HIGH 5
#define AMC7812_DAC_GAIN_LOW  2

#define AMC7812_TIMEOUT_CONV_CYCLS 16000 // 1 ms at 16 MHz clock, ignoring overhead

// functions
#include <avr/io.h>
#include <stdint.h>
#include "amc7812conf.h"

//! SPI Driver for TI AMC7812 ADC and DAC 
/*!
 * detailed description
 */
class AMC7812Class {
  private:
    uint16_t amc_conf[2]; //!< current amc configuration register values, no flags or triggers
    uint16_t adc_status;  //!< current Enabled ADCs, bitwise storage
    uint16_t adc_gain;    //!< current ADC gain settings, bitwise
    uint16_t adc_vals[AMC7812_ADC_CNT]; //!< stores result of `ReadADCs()`, disabled channel default to 0
    uint16_t dac_status;  //!< current Enabled DACs, bitwise storage
    uint16_t pd_reg;      //!< current functionality powered, see pg 79
    uint16_t dac_gain;    //!< current DAC gain settings, bitwise, Gain(0) = 2, Gain(1) = 5

    //! SPI control register value for communication
    /*!
     *  Enable bit, Master bit, 4X prescalar, clock/data phase
     *  If `amc_spsr` is set to double time then 2X prescalar
     */
    static const uint8_t amc_spcr = (1<<SPE)|(1<<MSTR)|(1<<CPHA);//|(1<<SPR1); 
    //! SPI status register value (holds 2X status bit)
    static const uint8_t amc_spsr = (1<<SPI2X);  

    //! Low-level frame transfer to chip
    /*!
     * \param addr is the register address to be read from or written to
     * \param val is the value to be written to the register, value is not read for a read operation
     * \return returns last 2-bytes as response from previous frame
     *
     * Low-level SPI frame transfer protocol.
     * A frame is 3-bytes in the format: [ register address, value[15:8], value[7:0] ].
     *
     * _Note_: AMC7812_CS_PIN amd AMC7812_CS_PORT must be set correctly in amc7812conf.h
     */
    static uint16_t transfer ( uint8_t addr, uint16_t val );

  public:
    //! prepare device for SPI communication
    /*!
     * \return error code, see amc7812err.h, 0 for no error
     * Tasks to be performed:
     * - Reset device
     * - Setup uC SPI interface
     * - Verify device ID
     * - Configure DACs
     * - Configure ADCs
     * - Trigger ADC read cycle
     */
    uint8_t begin();

    //! Get the correct SPI control register value for communication
    /*!
     * \return default SPCR setting for AMC7812 communication
     * 
     * Intended use case is switching between multiple devices on a bus wih 
     * different settings
     *  
     * uint8_t old_spcr = SPCR;
     * SPCR = myclass.GetSPCR();
     * `do stuff`
     * SPCR = old_spcr;
     */
    static const uint8_t GetSPCR(){ return amc_spcr; }

    //! Get the correct SPI status register value for communication
    /*!
     * \return default SPSR setting for AMC7812 communication
     * 
     * Intended use case is switching between multiple devices on a bus with
     * different settings
     *  
     * uint8_t old_spcr = SPSR;
     * SPSR = myclass.GetSPSR();
     * `do stuff`
     * SPSR = old_spcr;
     */
    static const uint8_t GetSPSR(){ return amc_spsr; }

    //! Read register value at address
    /*!
     * \param addr is the register address to be read
     * \return returned value is the response for the previous frame
     *
     * This is a low-level register command, it is suggested that the user use a
     * register specific command if provided.
     * Responses are pipelined, so need to send a dummy second command to clock
     * out response to this command.
     * Sequence define starting pg 56 datasheet.
     */
    inline static uint16_t Read ( uint8_t addr ){
      return transfer( ( AMC7812_READ_MASK | addr ), 0x0000 );
    }

    //! Read register value at address
    /*!
     * \param addr is the register address to be written
     * \param val is the 16 bit value to be written to the register
     * \return returned value is the response for the previous frame
     *
     * This is a low-level register command, it is suggested that the user use a
     * register specific command if provided.
     * Responses are pipelined.
     * Sequence define starting pg 56 datasheet.
     */
    inline static uint16_t Write( uint8_t addr, uint16_t val ){
      return transfer( (AMC7812_WRITE_MASK | addr), val );
    }

    //=============================================================================
    // temperature functions
    //=============================================================================
 
    //! Read temperature from sensor, 12-bit value
    /*!
     * \param sensor number (MAX 2).
     * \return returned value is the response for the previous frame
     * Readings are fixed 12-bit precision temperature in Celcius from addressed
     * sensor bottom 4 LSBs are padded zeros. 
     * Conversion 0.125 C/LSB, 0x0000 = 0 C, pg. 61
     * Temp (C) = (result >> 4)/8
     *
     * No checks are performed to verify sensor is within range to maximize 
     * performance, nor are the GPIO pins checked to verify the status.
     * It is assumed that the user will either perform check at a higher level or 
     * will check at/before compilation.
     *
     * Possible sensors (and GPIO collisions) are:
     * - Local (on chip)
     * - D1 (GPIO 4&5)
     * - D2 (GPIO 6&7)
     */
    inline static uint16_t ReadTemp( uint8_t sensor ){
      return transfer( AMC7812_READ_MASK | (AMC7812_TEMP_BASE_ADDR + sensor), 0x0000 );
    }

    //=============================================================================
    // adc functions
    //=============================================================================

    //! Read last recorded ADCn value, 12-bit value
    /*!
     * \param n is an integer between 0 and 15 (no check is performed) to read ADCn
     * \return returned value is the response for the previous frame
     *
     * Read the last recorded value for ADCn. If triggered mode is active you may
     * need to reassert the trigger and wait for the data available flag/pin to
     * signify new readings.
     * In continuous mode the ADC registers will be refreshing cyclically at the 
     * specfied rate
     */
    inline uint16_t ReadADC( uint8_t n ){
      return transfer( AMC7812_READ_MASK | (AMC7812_ADC_BASE_ADDR + n), 0x0000 );
    }

    //! Batch read operation, faster operatoin is possible with assumptions
    /*! 
     * \return error code, 0 is no error
     *
     * Function is provided to conveniently measure the enabled ADC channels.
     * If continuous mode, write conversion flag to status register and read ADCs.
     * If triggered mode, send conversion trigger and read activated ADCs.
     * Values are stored in `adc_vals` member, retrieve with `GetADCReadings()`. 
     */
    uint8_t ReadADCs();

    //! Retrieve results of `ReadADCs()`
    /*! 
     * Holds the result of the last batch read operation.
     * Disabled channels default to 0.
     * The index matches the channel number i.e. ADCn value is stored at `ADC[n]`.
     */
    const uint16_t* GetADCReadings() const { return adc_vals; };

    //! Read ADC gain setting
    /*!
     * \return returned value is the response for the previous frame
     *
     * The ADC gain is stored bitwise with maximum output:
     * - Gain(0) = 1*Vref
     * - Gain(1) = 2*Vref
     *
     * The actual input voltage is given by:
     * ADCn = Gain * Vref * (Vsetpoint / 2**12)
     */
    uint16_t GetADCGains(){ return adc_gain; }

    //! Read ADC gain setting from chip
    /*!
     * \return returned value is the response for the previous frame
     *
     * The returned value is stored in adc_gain.
     * To retrieve use `GetADCGain()`
     *
     * The ADC gain is stored bitwise with maximum input:
     * - Gain(0) = 1*Vref
     * - Gain(1) = 2*Vref
     *
     * The actual input voltage is given by:
     * ADCn = Gain * Vref * (Vsetpoint / 2**12)
     */
    uint16_t ReadADCGains(){
      uint16_t response = Read( AMC7812_ADC_GAIN );
      adc_gain = Read( AMC7812_ADC_GAIN );
      return response;
    }

    //! Write gain settings for all ADC channels
    /*!
     * \param value is the bitwise ADC gain setting for all channels
     * \return returned value is the response for the previous frame
     *
     * The ADC gain is stored bitwise with maximum input:
     * - Gain(0) = 1*Vref
     * - Gain(1) = 2*Vref
     *
     * The actual input voltage is given by:
     * ADCn = Gain * Vref * (Vsetpoint / 2**12)
     *
     * If only a single channel is changed:
     * `WriteADCGains( GetADCGain() | (1<<n) )`
     * `WriteADCGains( GetADCGain() & ~(1<<n) )`
     */
    uint16_t WriteADCGains ( uint16_t value ){
      adc_gain = value;
      return Write( AMC7812_ADC_GAIN, adc_gain );
    }

    //! Enable ADCn
    /*!
     * \param n is an integer between 0 and 15 (a check is performed) to enable ADCn
     * \return returned value is the response for the previous frame
     *
     * Enabled ADCs are recorded everytime a reading is triggered.
     * If faster reads are required, disable unused channels to decrease cycle time
     * and latency.
     * ADC enabled/disabled status is stored bitwise in `adc_status` a high bit
     * signifies an enabled channel.
     * Use `GetADCStatus()` to retrieve `adc_status` member value.
     */
    uint16_t EnableADC ( uint8_t n );

    //! Enable All ADCs
    /*!
     * \return returned value is the response for the previous frame
     *
     * _Note_ this operation requires two frames to set all values.
     * The returned value is from the command preceeding both.
     *
     * Enabled ADCs are recorded everytime a reading is triggered.
     * If faster reads are required, disable unused channels to decrease cycle time
     * and latency.
     * ADC enabled/disabled status is stored bitwise in `adc_status` a high bit
     * signifies an enabled channel.
     * Use `GetADCStatus()` to retrieve `adc_status` member value.
     */
    uint16_t EnableADCs ();

    //! Disable ADCn
    /*!
     * \param n is an integer between 0 and 15 (a check is performed) to disable ADCn
     * \return returned value is the response for the previous frame
     *
     * Enabled ADCs are recorded everytime a reading is triggered.
     * If faster reads are required, disable unused channels to decrease cycle time
     * and latency.
     * ADC enabled/disabled status is stored bitwise in `adc_status` a high bit
     * signifies an enabled channel.
     * Use `GetADCStatus()` to retrieve `adc_status` member value.
     */
    uint16_t DisableADC ( uint8_t n );

    //! Disable All ADCs
    /*!
     * \return returned value is the response for the previous frame
     *
     * _Note_ this operation requires two frames to set all values.
     * The returned value is from the command preceeding both.
     *
     * Enabled ADCs are recorded everytime a reading is triggered.
     * If faster reads are required, disable unused channels to decrease cycle time
     * and latency.
     * ADC enabled/disabled status is stored bitwise in `adc_status` a high bit
     * signifies an enabled channel.
     * Use `GetADCStatus()` to retrieve `adc_status` member value.
     */
    uint16_t DisableADCs ();

    //! Get bitwise list of enabled ADC channels
    /*!
     * \return returned value is the bitwise enabled adc channel member `adc_status`
     *
     * Enabled ADCs are recorded everytime a reading is triggered.
     * If faster reads are required, disable unused channels to decrease cycle time
     * and latency.
     * ADC enabled/disabled status is stored bitwise in `adc_status` a high bit
     * signifies an enabled channel.
     */
    uint16_t GetADCStatus (){ return adc_status; };

    //! Set update mode to continuous
    /*!
     * \return returned value is the response for the previous frame
     *
     * In continuous mode, the chip continuously cycles through the enabled ADC
     * channels always updating.
     * In triggered mode, the chip performs a single ADC conversion cycle of the
     * enabled ADCs, then waits for another trigger.
     *
     * _Note_: If conserversions are not taking place a trigger will need to be
     * asserted to begin the cycle even if in continuous mode.
     * See page 30 of datasheet.
     */
    inline uint16_t SetContinuousADCMode(){
      return WriteAMCConfig( 0, amc_conf[0] | (1<<AMC7812_CMODE) );
    }

    //! Set update mode to triggered
    /*!
     * \return returned value is the response for the previous frame
     *
     * In continuous mode, the chip continuously cycles through the enabled ADC
     * channels always updating.
     * In triggered mode, the chip performs a single ADC conversion cycle of the
     * enabled ADCs, then waits for another trigger.
     *
     * See page 30 of datasheet.
     */
    inline uint16_t SetTriggeredADCMode(){
      return WriteAMCConfig( 0, amc_conf[0] & ~(1<<AMC7812_CMODE) );
    }

    //=============================================================================
    // dac functions
    //=============================================================================


    //! Read DAC output value, 12-bit value
    /*!
     * \param n is an integer between 0 and 15 (no check is performed) to write 
     * to DACn
     * \return returned value is the response for the previous frame
     *
     * Read the current setpoint for DACn.
     * The actual output voltage is given by:
     * DACn = Gain * Vref * (Vsetpoint / 2**12)
     */
    inline uint16_t ReadDAC( uint8_t n ){
      return Read( AMC7812_DAC_BASE_ADDR + n );
    }

    //! Set DAC output value, 12-bit value
    /*!
     * \param n is an integer between 0 and 15 (no check is performed) to write 
     * to DACn
     * \param value is the DAC setpoint
     * \return returned value is the response for the previous frame
     *
     * Write a new setpoint for DACn.
     * The actual output voltage is given by:
     * DACn = Gain * Vref * (setpoint / 2**12)
     */
    inline uint16_t WriteDAC( uint8_t n, uint16_t value ){
      return Write( AMC7812_DAC_BASE_ADDR + n, value );
    }

    //! Read DAC gain setting
    /*!
     * \return returned value is the response for the previous frame
     *
     * The DAC gain is stored bitwise with maximum output:
     * - Gain(0) = 2*Vref
     * - Gain(1) = 5*Vref
     *
     * The actual output voltage is given by:
     * DACn = Gain * Vref * (Vsetpoint / 2**12)
     */
    uint16_t GetDACGains(){ return dac_gain; }

    //! Read DAC gain setting from chip
    /*!
     * \return returned value is the response for the previous frame
     *
     * The returned value is stored in dac_gain.
     * To retrieve use `GetDACGain()`
     *
     * The DAC gain is stored bitwise with maximum output:
     * - Gain(0) = 2*Vref
     * - Gain(1) = 5*Vref
     *
     * The actual output voltage is given by:
     * DACn = Gain * Vref * (Vsetpoint / 2**12)
     */
    uint16_t ReadDACGains(){
      uint16_t response = Read( AMC7812_DAC_GAIN );
      dac_gain = Read( AMC7812_DAC_GAIN );
      return response;
    }

    //! Write gain settings for all DAC channels
    /*!
     * \param value is the bitwise DAC gain setting for all channels
     * \return returned value is the response for the previous frame
     *
     * The DAC gain is stored bitwise with maximum output:
     * - Gain(0) = 2*Vref
     * - Gain(1) = 5*Vref
     *
     * The actual output voltage is given by:
     * DACn = Gain * Vref * (Vsetpoint / 2**12)
     *
     * If only a single channel is changed:
     * `WriteDACGains( GetDACGain() | (1<<n) )`
     * `WriteDACGains( GetDACGain() & ~(1<<n) )`
     */
    uint16_t WriteDACGains ( uint16_t value ){
      dac_gain = 0x0FFF & value; // 12 values
      return Write( AMC7812_DAC_GAIN, dac_gain );
    }

    //! Get bitwise list of enabled DAC channels
    /*!
     * \return returned value is the bitwise enabled dac channel member `adc_status`
     *
     * Enabled DACs are recorded everytime a reading is triggered.
     * If faster reads are required, disable unused channels to decrease cycle time
     * and latency.
     * DAC enabled/disabled status is stored bitwise in `dac_status` a high bit
     * signifies an enabled channel.
     */
    uint16_t GetDACStatus (){ return dac_status; };

    //! Enable DACn
    /*!
     * \param n is an integer between 0 and 15 (a check is performed) to enable DACn
     * \return returned value is the response for the previous frame
     *
     * Enabled DACs are recorded everytime a reading is triggered.
     * If faster reads are required, disable unused channels to decrease cycle time
     * and latency.
     * DAC enabled/disabled status is stored bitwise in `dac_status` a high bit
     * signifies an enabled channel.
     * Use `GetDACStatus()` to retrieve `dac_status` member value.
     */
    uint16_t EnableDAC ( uint8_t n );

    //! Enable All DACs
    /*!
     * \return returned value is the response for the previous frame
     *
     * _Note_ this operation requires two frames to set all values.
     * The returned value is from the command preceeding both.
     *
     * Enabled DACs are recorded everytime a reading is triggered.
     * If faster reads are required, disable unused channels to decrease cycle time
     * and latency.
     * DAC enabled/disabled status is stored bitwise in `dac_status` a high bit
     * signifies an enabled channel.
     * Use `GetDACStatus()` to retrieve `dac_status` member value.
     */
    uint16_t EnableDACs ();

    //! Disable DACn
    /*!
     * \param n is an integer between 0 and 15 (a check is performed) to disable DACn
     * \return returned value is the response for the previous frame
     *
     * Enabled DACs are recorded everytime a reading is triggered.
     * If faster reads are required, disable unused channels to decrease cycle time
     * and latency.
     * DAC enabled/disabled status is stored bitwise in `dac_status` a high bit
     * signifies an enabled channel.
     * Use `GetDACStatus()` to retrieve `dac_status` member value.
     */
    uint16_t DisableDAC ( uint8_t n );

    //! Disable All DACs
    /*!
     * \return returned value is the response for the previous frame
     *
     * _Note_ this operation requires two frames to set all values.
     * The returned value is from the command preceeding both.
     *
     * Enabled DACs are recorded everytime a reading is triggered.
     * If faster reads are required, disable unused channels to decrease cycle time
     * and latency.
     * DAC enabled/disabled status is stored bitwise in `dac_status` a high bit
     * signifies an enabled channel.
     * Use `GetDACStatus()` to retrieve `dac_status` member value.
     */
    uint16_t DisableDACs ();

    //! Set dac update mode to continuous
    /*!
     * \return returned value is the response for the previous frame
     *
     * In continuous mode, the chip asynchronously updates the relevant DAC
     * output whenever a DAC output register is wirtten to.
     *
     * See page 75 of datasheet.
     */
    inline uint16_t SetContinuousDACMode(){
      return Write( AMC7812_DAC_CONF, 0x0000 );
    }

    //! Set dac update mode to triggered
    /*!
     * \return returned value is the response for the previous frame
     *
     * In triggered mode, the chip synchronously updates the DAC
     * outputs whenever the ILDAC register or pin is activated.
     *
     * See page 75 of datasheet.
     */
    inline uint16_t SetTriggeredDACMode(){
      return Write( AMC7812_DAC_CONF, 0x0FFF );
    }

    //=============================================================================
    // configuration registers functions
    //=============================================================================

    //! Write to one of the chip main configuration registers
    /*!
     *  \param n is the AMC configuraion regster to be address, 0 or 1 (checked)
     *  \param config is the register configuration value (masked)
     *  \return 0 if no error, AMC_PARAM_OOR_ERR if n is out of range
     *  
     *  Flags and internal trigger bits (ICONV & ILDAC) cannot be set through this
     *  function.
     *
     *  See page 66.
     */
    uint8_t WriteAMCConfig ( uint8_t n, uint16_t config );

    //! Trigger an ADC conversion cycle internally
    /*!
     * \return returned value is the response for the previous frame
     *
     * Trigger the adc conversion, using the status register, this should be less
     * accurate than an external trigger.
     * If in auto mode (CMODE = 1) then subsequent conversions happen automatically,
     * if not then a trigger needs to be 
     * sent each time the values need to be refreshed.
     */
    inline uint16_t TriggerADCsInternal(){
      return Write( AMC7812_AMC_CONF_0, amc_conf[0]|(1<<AMC7812_ICONV) );
    }

    //! Trigger an ADC conversion cycle externally
    /*!
     * Trigger the adc conversion, using the physical !CNVT pin on the chip.
     * This should be more accurate timing than internal triggering if trigger mode is
     * being used.
     *
     * If in auto mode (CMODE = 1) then subsequent conversions happen automatically,
     * if not then a trigger needs to be sent each time the values need to be refreshed.
     *
     * _NOTE_: External triggering is only defined if the external !CNVT pin is attached,
     * _AND_ the AMC7812_CNVT_PIN macro is defiend in `amc7812conf.h`.
     */
#ifdef AMC7812_CNVT_PIN
    inline void TriggerADCsExternal(){
      digitalWrite(AMC7812_CNVT_PIN, LOW);
      digitalWrite(AMC7812_CNVT_PIN, HIGH);
      AMC7812_CNVT_PORT &= ~(1<<AMC7812_CNVT_PIN); // low
      AMC7812_CNVT_PORT |= (1<<AMC7812_CNVT_PIN);  // high
    }
#endif

    //! Simultaneously update DAC outputs
    /*!
     *  Sends ILDAC bit to AMC config register to udpate all the DAC output
     *  simultaneously
     */
    inline uint16_t UpdateDAC(){
      return Write( AMC7812_AMC_CONF_0, amc_conf[0]|(1<<AMC7812_ILDAC) );
    }

};

#endif
