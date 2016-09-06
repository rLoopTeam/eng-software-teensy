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
*****************************************************************************/
// uC is critcal path in data stream, so speed > memory

#include <Wire.h>

#include "amc7812.h"
#include "amc7812conf.h"
#include "amc7812err.h"

#include <Arduino.h> // for deugging only (serial comms)

AMC7812Class AMC7812;

//=============================================================================
//=============================================================================
//  UTILITY FUNCTIONS
//=============================================================================
//=============================================================================

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
uint16_t AMC7812Class::transfer ( const uint8_t cmd, const uint16_t data ){
	// avr is little endian
	union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } in, out;
	in.val = data;

	if ((cmd & AMC7812_READ_MASK) != 0)
	{
		Wire.beginTransmission(AMC7812_I2C_ADDR);
		Wire.write(cmd & (0x7F));
		Wire.endTransmission(false);
		Wire.requestFrom(AMC7812_I2C_ADDR, 2, true);
		while (Wire.available() < 2) ;
		out.msb = Wire.read();
		out.lsb = Wire.read();
		return out.val;
	}
	else
	{
		Wire.beginTransmission(AMC7812_I2C_ADDR);
		Wire.write(cmd);
		Wire.write(in.msb);
		Wire.write(in.lsb);
		Wire.endTransmission();
		return in.val;
	}
}

//=============================================================================
//=============================================================================
//  PUBLIC FUNCTIONS
//=============================================================================
//=============================================================================

//! prepare device for SPI communication
/*!
	* Tasks to be performed:
	* - Reset device
	* - Setup uC SPI interface
	* - Verify device ID
	* - Configure DACs
	* - Configure ADCs
	* - Trigger ADC read cycle
	*/
uint8_t AMC7812Class::begin(){
#ifdef AMC7812_CS_PIN
	pinMode(AMC7812_CS_PIN, OUTPUT);
	digitalWrite(AMC7812_CS_PIN, HIGH);
#endif
 
#ifdef AMC7812_CNVT_PIN // setup !convert pin if attached
	pinMode(AMC7812_CNVT_PIN, OUTPUT);
	digitalWrite(AMC7812_CNVT_PIN, HIGH);
#else
	//#pragma message "External Triggering is disabled for AMC7812"
#endif

#ifdef AMC7812_DAV_PIN // setup data available pin if attached
	pinMode(AMC7812_DAV_PIN, INPUT);
#else
	//#pragma message "External Data Available pin is disabled for AMC7812"
#endif

	// reset device
	pinMode(AMC7812_RST_PIN, OUTPUT);
	digitalWrite(AMC7812_RST_PIN, LOW);
	delay(1);
	digitalWrite(AMC7812_RST_PIN, HIGH);
	delay(1);

	// read the device ID register and verify that the expected value matches
	Read( AMC7812_DEV_ID );
	// dummy read to get the answer
	uint16_t response = Read( 0x00 );
	if( (response != 0x1220) && (response != 0x1221) ){
	//if( (response != 0x1221) ){
	return AMC7812_DEV_ID_ERR;
	//return response + 2;
	}

	// set dac gain to low range by default
	uint16_t gain_sp = 0x0000;
	WriteDACGains( gain_sp );
	ReadDACGains();
	// check that dac gain has been set low
	if( dac_gain != gain_sp ){
	return AMC7812_WRITE_ERR;
	}

	// turn on DAC output on all channels
	pd_reg = 0x7FFE;
	dac_status = 0x0FFF; 
	Write( AMC7812_POWER_DOWN, pd_reg );
	Read( AMC7812_POWER_DOWN );
	response = Read( 0x00 );
	// check that dac gain has been set low
	if( response != pd_reg ){
	return AMC7812_WRITE_ERR;
	}

	// set the ADC ref as internal, and continuous read mode
	uint16_t amc_config = (1<<AMC7812_CMODE)|(1<<AMC7812_ADC_REF);
	WriteAMCConfig( 0, amc_config );
	// turn on the adcs
	EnableADCs();
	TriggerADCsInternal();

	return 0; // no errors in initalization
}

//==============================================================================
// ADC FUNCTIONS
//==============================================================================

//! Batch read operation, faster operatoin is possible with assumptions
/*! 
	* Function is provided to conveniently measure the enabled ADC channels.
	* If continuous mode, write conversion flag to status register and read ADCs.
	* If triggered mode, send conversion trigger and read activated ADCs.
	* Values are stored in `adc_vals` member, retrieve with `GetADCReadings()`. 
	*/
uint8_t AMC7812Class::ReadADCs(){
	// if convert pin is connected, then use it for triggerring, 
	// otherwise use the internal trigger
	if ( !(amc_conf[0] & (1<<AMC7812_CMODE)) ){
#ifdef AMC7812_CNVT_PIN
	TriggerADCsExternal();
#else
	TriggerADCsInternal();
#endif
#ifdef AMC7812_DAV_PIN
	// wait for data available, polling
	// TODO: timeout cycles should have prefactor based on conversion speed 
	// setting and clock frequency
	uint8_t error = 1;
	for(uint16_t i=0; i<(uint16_t)AMC7812_TIMEOUT_CONV_CYCLS; i++){
		if ( !(AMC7812_DAV_PORT & (1<<AMC7812_DAV_PIN)) ){ 
		error = 0;
		break; 
		}
	}
	if (error){
		return AMC7812_TIMEOUT_ERR;
	}
#endif
	}

	int8_t last_valid = -1;
	for(uint8_t i=0; i<AMC7812_ADC_CNT; i++){
	if ( adc_status & (1<<i) ){
		uint16_t reading = ReadADC(i);
		if ( last_valid >= 0 ){
		adc_vals[last_valid] = reading;
		}
		last_valid = i;
	} else {
		adc_vals[i] = 0;
	}
	}
	// dummy read, if statement not necessary if at least one ADC is enabled
	if ( last_valid >= 0 ){
	adc_vals[last_valid] = ReadADC(0);
	}
	
	return 0;
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
uint16_t AMC7812Class::EnableADC( const uint8_t n ){
	uint16_t setval = 0; // if n out-of-range setval=0 and nothing happens
	// stupid register struture makes this crappy, pg 71 
	if( n < 13 ){
	setval = (1<<(12-n));
	if ( n < 4 ){
		setval = setval << 1;
		if ( n < 2 ){
		setval = setval << 1;
		}
	}
	} else {
	if ( n < AMC7812_ADC_CNT ){
		setval = 1<<(27-n);
	}
	}
	adc_status |= setval;  // save new status

	// adc channels > 12 are stored in ADC_1_CONF, one address above
	return Write( AMC7812_ADC_0_CONF + n/13, adc_status ); 
}

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
uint16_t AMC7812Class::EnableADCs(){
	// first command
	uint16_t response = Write( AMC7812_ADC_0_CONF, 0x6DFF );
	// second command
	Write( AMC7812_ADC_1_CONF, 0x7000 );
	// save adc status to class field
	adc_status = 0xFFFF;
	return response;
}

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
uint16_t AMC7812Class::DisableADC( const uint8_t n ){
	uint16_t setval = 0; // if n out-of-range setval=0 and nothing happens
	// stupid register struture makes this crappy, pg 71 
	if( n < 13 ){
	setval = (1<<(12-n));
	if ( n < 4 ){
		setval = setval << 1;
		if ( n < 2 ){
		setval = setval << 1;
		}
	}
	} else {
	if ( n < AMC7812_ADC_CNT ){
		setval = 1<<(27-n);
	}
	}
	adc_status &= ~setval; // reference old status

	// adc channels > 12 are stored in ADC_1_CONF, one address above
	return Write( AMC7812_ADC_0_CONF + n/13, adc_status ); 
}


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
uint16_t AMC7812Class::DisableADCs(){
	// first command
	uint16_t response = Write( AMC7812_ADC_0_CONF, 0x0000 );
	// second command
	Write( AMC7812_ADC_1_CONF, 0x0000 );
	// save adc status to class field
	adc_status = 0x0000;
	for(uint8_t i=0; i<AMC7812_ADC_CNT; i++){
	adc_vals[i]=0;
	}
	return response;
}

//==============================================================================
// DAC FUNCTIONS
//==============================================================================

uint16_t AMC7812Class::EnableDAC ( const uint8_t n ){
	if( n < AMC7812_DAC_CNT ){
	pd_reg |= (1<<(12-n));
	dac_status |= (1<<n);
	}
	return Write( AMC7812_POWER_DOWN, pd_reg );
}

uint16_t AMC7812Class::EnableDACs (){
	pd_reg |= 0x7FFE;
	dac_status = 0x0FFF;
	return Write( AMC7812_POWER_DOWN, pd_reg );
}

uint16_t AMC7812Class::DisableDAC ( const uint8_t n ){
	if( n < AMC7812_DAC_CNT ){
	pd_reg &= ~(1<<(12-n));
	dac_status &= ~(1<<n);
	}
	return Write( AMC7812_POWER_DOWN, pd_reg );
}

uint16_t AMC7812Class::DisableDACs (){
	pd_reg &= ~0x1FFE;
	dac_status = 0x0000;
	return Write( AMC7812_POWER_DOWN, pd_reg );
}

//==============================================================================
// CONFIGURATION REGISTERS FUNCTIONS
//==============================================================================

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
uint8_t AMC7812Class::WriteAMCConfig( const uint8_t n, uint16_t config ){
	// only save (R/W) values not flags or trigger values (ICONV & ILDAC)
	switch(n){
	case 0:
		config = 0x2600 & config;
		break;
	case 1:
		config = 0x03F8 & config;
		break;
	default:
		return AMC7812_PARAM_OOR_ERR;
	}
	amc_conf[n] = config;
	Write( AMC7812_WRITE_MASK|(AMC7812_AMC_CONF_0 + n), config);
	return 0;
}
