#include "rI2CTx.h"
#include <string.h>

uint8_t rI2CTX_buffer[RPOD_I2C_BUFFER_SIZE];
uint16_t rI2CTX_bufferPos;
uint8_t checksum;
uint16_t rI2CTX_frameLength;

void rI2CTX_beginFrame()
{
	rI2CTX_buffer[0] = RPOD_I2C_CONTROL_CHAR;
	rI2CTX_buffer[1] = RPOD_I2C_FRAME_START;
	rI2CTX_bufferPos = 4;
	//2,3 will be filled in with the length in endFrame()
}

void rI2CTX_calculateChecksum(uint16_t lastByte)
{
	checksum = 0;
	for (uint16_t i = 0; i < lastByte;i++)
		checksum ^= rI2CTX_buffer[i];
}

uint16_t rI2CTX_endFrame()
{
	rI2CTX_frameLength = rI2CTX_bufferPos;

	uint8_t length1 = (rI2CTX_frameLength & 0xFF00) >> 8;
	uint8_t length2 = rI2CTX_frameLength & 0xFF;

	if (length1 != RPOD_I2C_CONTROL_CHAR && length2 != RPOD_I2C_CONTROL_CHAR)
	{
		rI2CTX_buffer[2] = length1;
		rI2CTX_buffer[3] = length2;
	}
	else if (length1 == RPOD_I2C_CONTROL_CHAR && length2 == RPOD_I2C_CONTROL_CHAR)
	{
		rI2CTX_buffer[2] = length1;
		rI2CTX_buffer[3] = length1;
		rI2CTX_buffer[4] = length2;
		rI2CTX_buffer[5] = length2;
		memcpy(rI2CTX_buffer + 4, rI2CTX_buffer + 6, rI2CTX_bufferPos - 4); //DOUBLE CHECK THIS
		rI2CTX_bufferPos += 2;
	}
	else{
		if (length1 == RPOD_I2C_CONTROL_CHAR){
			rI2CTX_buffer[2] = length1;
			rI2CTX_buffer[3] = length1;
			rI2CTX_buffer[4] = length2;
		}
		else{
			rI2CTX_buffer[2] = length1;
			rI2CTX_buffer[3] = length2;
			rI2CTX_buffer[4] = length2;
		}
		memcpy(rI2CTX_buffer + 4, rI2CTX_buffer + 5, rI2CTX_bufferPos - 4);  //DOUBLE CHECK THIS
		rI2CTX_bufferPos += 1;
	}
	
	

	rI2CTX_calculateChecksum(rI2CTX_bufferPos);

	rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
	rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_FRAME_END;
	rI2CTX_buffer[rI2CTX_bufferPos + 2] = checksum;
	rI2CTX_buffer[rI2CTX_bufferPos + 3] = 0x00;
	rI2CTX_bufferPos += 4;

	return rI2CTX_bufferPos;
}


void rI2CTX_addParameter(uint8_t index, int8_t data)
{
	rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
	rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	rI2CTX_buffer[rI2CTX_bufferPos + 2] = 0x11;
	rI2CTX_buffer[rI2CTX_bufferPos + 3] = index;

	if (data ==RPOD_I2C_CONTROL_CHAR) {
		rI2CTX_buffer[rI2CTX_bufferPos + 4] = RPOD_I2C_CONTROL_CHAR;
		rI2CTX_buffer[rI2CTX_bufferPos + 5] = RPOD_I2C_CONTROL_CHAR;
		rI2CTX_bufferPos += 6;
	}
	else
	{
		rI2CTX_buffer[rI2CTX_bufferPos + 4] = data;
		rI2CTX_bufferPos += 5;
	}
}

void rI2CTX_addParameter(uint8_t index, uint8_t data)
{
	rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
	rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	rI2CTX_buffer[rI2CTX_bufferPos + 2] = 0x12;
	rI2CTX_buffer[rI2CTX_bufferPos + 3] = index;

	if (data ==RPOD_I2C_CONTROL_CHAR) {
		rI2CTX_buffer[rI2CTX_bufferPos + 4] = RPOD_I2C_CONTROL_CHAR;
		rI2CTX_buffer[rI2CTX_bufferPos + 5] = RPOD_I2C_CONTROL_CHAR;
		rI2CTX_bufferPos += 6;
	}
	else
	{
		rI2CTX_buffer[rI2CTX_bufferPos + 4] = data;
		rI2CTX_bufferPos += 5;
	}
 }

void rI2CTX_addParameter(uint8_t index, int16_t data)
{
	rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
	rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	rI2CTX_buffer[rI2CTX_bufferPos + 2] = 0x21;
	rI2CTX_buffer[rI2CTX_bufferPos + 3] = index;
	rI2CTX_bufferPos += 4;

	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		uint8_t byte = data >> ((sizeof(data)-i-1)*8);
		if (byte ==RPOD_I2C_CONTROL_CHAR) {
			rI2CTX_buffer[rI2CTX_bufferPos] =RPOD_I2C_CONTROL_CHAR;
			rI2CTX_buffer[rI2CTX_bufferPos+1] =RPOD_I2C_CONTROL_CHAR;
			rI2CTX_bufferPos += 2;
		}
		else
		{
			rI2CTX_buffer[rI2CTX_bufferPos] = byte;
			rI2CTX_bufferPos += 1;
		}
	}
}

void rI2CTX_addParameter(uint8_t index, uint16_t data)
{
	rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
	rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	rI2CTX_buffer[rI2CTX_bufferPos + 2] = 0x22;
	rI2CTX_buffer[rI2CTX_bufferPos + 3] = index;
	rI2CTX_bufferPos += 4;

	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		uint8_t byte = data >> ((sizeof(data) - i - 1) * 8);
		if (byte == RPOD_I2C_CONTROL_CHAR) {
			rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
			rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_CONTROL_CHAR;
			rI2CTX_bufferPos += 2;
		}
		else
		{
			rI2CTX_buffer[rI2CTX_bufferPos] = byte;
			rI2CTX_bufferPos += 1;
		}
	}
}

void rI2CTX_addParameter(uint8_t index, int64_t data)
{
	rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
	rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	rI2CTX_buffer[rI2CTX_bufferPos + 2] = 0x81;
	rI2CTX_buffer[rI2CTX_bufferPos + 3] = index;
	rI2CTX_bufferPos += 4;

	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		uint8_t byte = data >> ((sizeof(data) - i - 1) * 8);
		if (byte == RPOD_I2C_CONTROL_CHAR) {
			rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
			rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_CONTROL_CHAR;
			rI2CTX_bufferPos += 2;
		}
		else
		{
			rI2CTX_buffer[rI2CTX_bufferPos] = byte;
			rI2CTX_bufferPos += 1;
		}
	}
}

void rI2CTX_addParameter(uint8_t index, uint64_t data)
{
	rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
	rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	rI2CTX_buffer[rI2CTX_bufferPos + 2] = 0x82;
	rI2CTX_buffer[rI2CTX_bufferPos + 3] = index;
	rI2CTX_bufferPos += 4;

	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		uint8_t byte = data >> ((sizeof(data) - i - 1) * 8);
		if (byte == RPOD_I2C_CONTROL_CHAR) {
			rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
			rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_CONTROL_CHAR;
			rI2CTX_bufferPos += 2;
		}
		else
		{
			rI2CTX_buffer[rI2CTX_bufferPos] = byte;
			rI2CTX_bufferPos += 1;
		}
	}
}

void rI2CTX_addParameter(uint8_t index, int32_t data)
{
	rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
	rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	rI2CTX_buffer[rI2CTX_bufferPos + 2] = 0x41;
	rI2CTX_buffer[rI2CTX_bufferPos + 3] = index;
	rI2CTX_bufferPos += 4;

	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		uint8_t byte = data >> ((sizeof(data) - i - 1) * 8);
		if (byte == RPOD_I2C_CONTROL_CHAR) {
			rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
			rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_CONTROL_CHAR;
			rI2CTX_bufferPos += 2;
		}
		else
		{
			rI2CTX_buffer[rI2CTX_bufferPos] = byte;
			rI2CTX_bufferPos += 1;
		}
	}
}

void rI2CTX_addParameter(uint8_t index, uint32_t data)
{
	rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
	rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	rI2CTX_buffer[rI2CTX_bufferPos + 2] = 0x42;
	rI2CTX_buffer[rI2CTX_bufferPos + 3] = index;
	rI2CTX_bufferPos += 4;

	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		uint8_t byte = data >> ((sizeof(data) - i - 1) * 8);
		if (byte == RPOD_I2C_CONTROL_CHAR) {
			rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
			rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_CONTROL_CHAR;
			rI2CTX_bufferPos += 2;
		}
		else
		{
			rI2CTX_buffer[rI2CTX_bufferPos] = byte;
			rI2CTX_bufferPos += 1;
		}
	}
}

void rI2CTX_addParameter(uint8_t index, float data)
{
	rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
	rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	rI2CTX_buffer[rI2CTX_bufferPos + 2] = 0x43;
	rI2CTX_buffer[rI2CTX_bufferPos + 3] = index;
	rI2CTX_bufferPos += 4;

	uint32_t rawData;
	memcpy(&rawData, &data, 4);

	for (uint16_t i = 0; i < sizeof(rawData); i++)
	{
		uint8_t byte = rawData >> ((sizeof(rawData) - i - 1) * 8);
		if (byte == RPOD_I2C_CONTROL_CHAR) {
			rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
			rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_CONTROL_CHAR;
			rI2CTX_bufferPos += 2;
		}
		else
		{
			rI2CTX_buffer[rI2CTX_bufferPos] = byte;
			rI2CTX_bufferPos += 1;
		}
	}
}

void rI2CTX_addParameter(uint8_t index, double data)
{
	rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
	rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	rI2CTX_buffer[rI2CTX_bufferPos + 2] = 0x83;
	rI2CTX_buffer[rI2CTX_bufferPos + 3] = index;
	rI2CTX_bufferPos += 4;

	uint64_t rawData;
	memcpy(&rawData, &data, 8);

	for (uint16_t i = 0; i < sizeof(rawData); i++)
	{
		uint8_t byte = rawData >> ((sizeof(rawData) - i - 1) * 8);
		if (byte == RPOD_I2C_CONTROL_CHAR) {
			rI2CTX_buffer[rI2CTX_bufferPos] = RPOD_I2C_CONTROL_CHAR;
			rI2CTX_buffer[rI2CTX_bufferPos + 1] = RPOD_I2C_CONTROL_CHAR;
			rI2CTX_bufferPos += 2;
		}
		else
		{
			rI2CTX_buffer[rI2CTX_bufferPos] = byte;
			rI2CTX_bufferPos += 1;
		}
	}
}