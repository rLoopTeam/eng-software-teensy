#include "rPodI2C.h"
#include <string.h>

uint8_t buffer[RPOD_I2C_BUFFER_SIZE];
uint16_t bufferPos;
uint8_t checksum;
uint16_t frameLength;

void rPodI2CbeginFrame()
{
	buffer[0] = RPOD_I2C_CONTROL_CHAR;
	buffer[1] = RPOD_I2C_FRAME_START;
	bufferPos = 4;
	//2,3 will be filled in with the length in endFrame()
}

void rPodI2CcalculateChecksum(uint16_t lastByte)
{
	checksum = 0;
	for (uint16_t i = 0; i < lastByte;i++)
		checksum ^= buffer[i];
}

uint16_t rPodI2CendFrame()
{
	frameLength = bufferPos;

	uint8_t length1 = (frameLength & 0xFF00) >> 8;
	uint8_t length2 = frameLength & 0xFF;

	if (length1 != RPOD_I2C_CONTROL_CHAR && length2 != RPOD_I2C_CONTROL_CHAR)
	{
		buffer[2] = length1;
		buffer[3] = length2;
	}
	else if (length1 == RPOD_I2C_CONTROL_CHAR && length2 == RPOD_I2C_CONTROL_CHAR)
	{
		buffer[2] = length1;
		buffer[3] = length1;
		buffer[4] = length2;
		buffer[5] = length2;
		memcpy(buffer + 4, buffer + 6, bufferPos - 4); //DOUBLE CHECK THIS
		bufferPos += 2;
	}
	else{
		if (length1 == RPOD_I2C_CONTROL_CHAR){
			buffer[2] = length1;
			buffer[3] = length1;
			buffer[4] = length2;
		}
		else{
			buffer[2] = length1;
			buffer[3] = length2;
			buffer[4] = length2;
		}
		memcpy(buffer + 4, buffer + 5, bufferPos - 4);  //DOUBLE CHECK THIS
		bufferPos += 1;
	}
	
	

	rPodI2CcalculateChecksum(bufferPos);

	buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
	buffer[bufferPos + 1] = RPOD_I2C_FRAME_END;
	buffer[bufferPos + 2] = checksum;
	buffer[bufferPos + 3] = 0x00;
	bufferPos += 4;

	return bufferPos;
}


void rPodI2CaddParameter(uint8_t index, int8_t data)
{
	buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
	buffer[bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	buffer[bufferPos + 2] = 0x11;
	buffer[bufferPos + 3] = index;

	if (data ==RPOD_I2C_CONTROL_CHAR) {
		buffer[bufferPos + 4] = RPOD_I2C_CONTROL_CHAR;
		buffer[bufferPos + 5] = RPOD_I2C_CONTROL_CHAR;
		bufferPos += 6;
	}
	else
	{
		buffer[bufferPos + 4] = data;
		bufferPos += 5;
	}
}

void rPodI2CaddParameter(uint8_t index, uint8_t data)
{
	buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
	buffer[bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	buffer[bufferPos + 2] = 0x12;
	buffer[bufferPos + 3] = index;

	if (data ==RPOD_I2C_CONTROL_CHAR) {
		buffer[bufferPos + 4] = RPOD_I2C_CONTROL_CHAR;
		buffer[bufferPos + 5] = RPOD_I2C_CONTROL_CHAR;
		bufferPos += 6;
	}
	else
	{
		buffer[bufferPos + 4] = data;
		bufferPos += 5;
	}
 }

void rPodI2CaddParameter(uint8_t index, int16_t data)
{
	buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
	buffer[bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	buffer[bufferPos + 2] = 0x21;
	buffer[bufferPos + 3] = index;
	bufferPos += 4;

	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		uint8_t byte = data >> ((sizeof(data)-i-1)*8);
		if (byte ==RPOD_I2C_CONTROL_CHAR) {
			buffer[bufferPos] =RPOD_I2C_CONTROL_CHAR;
			buffer[bufferPos+1] =RPOD_I2C_CONTROL_CHAR;
			bufferPos += 2;
		}
		else
		{
			buffer[bufferPos] = byte;
			bufferPos += 1;
		}
	}
}

void rPodI2CaddParameter(uint8_t index, uint16_t data)
{
	buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
	buffer[bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	buffer[bufferPos + 2] = 0x22;
	buffer[bufferPos + 3] = index;
	bufferPos += 4;

	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		uint8_t byte = data >> ((sizeof(data) - i - 1) * 8);
		if (byte == RPOD_I2C_CONTROL_CHAR) {
			buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
			buffer[bufferPos + 1] = RPOD_I2C_CONTROL_CHAR;
			bufferPos += 2;
		}
		else
		{
			buffer[bufferPos] = byte;
			bufferPos += 1;
		}
	}
}

void rPodI2CaddParameter(uint8_t index, int64_t data)
{
	buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
	buffer[bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	buffer[bufferPos + 2] = 0x81;
	buffer[bufferPos + 3] = index;
	bufferPos += 4;

	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		uint8_t byte = data >> ((sizeof(data) - i - 1) * 8);
		if (byte == RPOD_I2C_CONTROL_CHAR) {
			buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
			buffer[bufferPos + 1] = RPOD_I2C_CONTROL_CHAR;
			bufferPos += 2;
		}
		else
		{
			buffer[bufferPos] = byte;
			bufferPos += 1;
		}
	}
}

void rPodI2CaddParameter(uint8_t index, uint64_t data)
{
	buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
	buffer[bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	buffer[bufferPos + 2] = 0x82;
	buffer[bufferPos + 3] = index;
	bufferPos += 4;

	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		uint8_t byte = data >> ((sizeof(data) - i - 1) * 8);
		if (byte == RPOD_I2C_CONTROL_CHAR) {
			buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
			buffer[bufferPos + 1] = RPOD_I2C_CONTROL_CHAR;
			bufferPos += 2;
		}
		else
		{
			buffer[bufferPos] = byte;
			bufferPos += 1;
		}
	}
}

void rPodI2CaddParameter(uint8_t index, int32_t data)
{
	buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
	buffer[bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	buffer[bufferPos + 2] = 0x41;
	buffer[bufferPos + 3] = index;
	bufferPos += 4;

	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		uint8_t byte = data >> ((sizeof(data) - i - 1) * 8);
		if (byte == RPOD_I2C_CONTROL_CHAR) {
			buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
			buffer[bufferPos + 1] = RPOD_I2C_CONTROL_CHAR;
			bufferPos += 2;
		}
		else
		{
			buffer[bufferPos] = byte;
			bufferPos += 1;
		}
	}
}

void rPodI2CaddParameter(uint8_t index, uint32_t data)
{
	buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
	buffer[bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	buffer[bufferPos + 2] = 0x42;
	buffer[bufferPos + 3] = index;
	bufferPos += 4;

	for (uint16_t i = 0; i < sizeof(data); i++)
	{
		uint8_t byte = data >> ((sizeof(data) - i - 1) * 8);
		if (byte == RPOD_I2C_CONTROL_CHAR) {
			buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
			buffer[bufferPos + 1] = RPOD_I2C_CONTROL_CHAR;
			bufferPos += 2;
		}
		else
		{
			buffer[bufferPos] = byte;
			bufferPos += 1;
		}
	}
}

void rPodI2CaddParameter(uint8_t index, float data)
{
	buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
	buffer[bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	buffer[bufferPos + 2] = 0x43;
	buffer[bufferPos + 3] = index;
	bufferPos += 4;

	uint32_t rawData;
	memcpy(&rawData, &data, 4);

	for (uint16_t i = 0; i < sizeof(rawData); i++)
	{
		uint8_t byte = rawData >> ((sizeof(rawData) - i - 1) * 8);
		if (byte == RPOD_I2C_CONTROL_CHAR) {
			buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
			buffer[bufferPos + 1] = RPOD_I2C_CONTROL_CHAR;
			bufferPos += 2;
		}
		else
		{
			buffer[bufferPos] = byte;
			bufferPos += 1;
		}
	}
}

void rPodI2CaddParameter(uint8_t index, double data)
{
	buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
	buffer[bufferPos + 1] = RPOD_I2C_PARAMETER_START;
	buffer[bufferPos + 2] = 0x83;
	buffer[bufferPos + 3] = index;
	bufferPos += 4;

	uint64_t rawData;
	memcpy(&rawData, &data, 8);

	for (uint16_t i = 0; i < sizeof(rawData); i++)
	{
		uint8_t byte = rawData >> ((sizeof(rawData) - i - 1) * 8);
		if (byte == RPOD_I2C_CONTROL_CHAR) {
			buffer[bufferPos] = RPOD_I2C_CONTROL_CHAR;
			buffer[bufferPos + 1] = RPOD_I2C_CONTROL_CHAR;
			bufferPos += 2;
		}
		else
		{
			buffer[bufferPos] = byte;
			bufferPos += 1;
		}
	}
}