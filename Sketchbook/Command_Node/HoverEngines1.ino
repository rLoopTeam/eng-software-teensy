/*Relevent Parameters:
 * 4 - Baud Rate
 * 5 - Slave ID
 * 6 - HW slave ID Enable bit
 * 64 - Baud rate port2
 * 65 - Slave ID Port 2
 * 127 - Swap Motor Phases
 * 151 - Throttle deadband threshold
 * 152 - Throttle fault range
 * 208 - Control command source
 * 212 - Features (includeing reverse)
 * 213,214 - Throttle range stuff
 * 215 - Throttle sensor source
 * 258 - Faults
 * 259 - Controller Temp
 * 262 - Motor Current Amps Scale:32
 * 263 - Motor RPM
 * 265 - Masured battery voltage Scale: 32
 * 266 - Measured Battery Current Scale 32
 * 268 - Battery Power Watts
 * 269 - Last Fault
 * 270 - Filtered Throttle Voltage
 * 277 - Warnings
 * 281 - DSP Temperature in C
 * 291 - Unfiltered battery voltage measurement (pu V?)
 * 312 - Calculated Wheel speed bsed on motor pole pairs (RPM)
 * 325 - Throttle Set Point
 * 334 - Motor input power in Watts
 * 340 - Modbus Error Count Port 1
 * 341 - Modbus Eror Count Port 2
 * 450 - Current User Pramater access level
 * 476 - State Run/Idle
 * 477 - Torque Command % (or rpm based on mode?)
 * 490 - Remote speed command - % of rated motor speed
 * 493 - Remote State Command Run/Idle
 * 494 - Remote torque command
 * 495 - Remote Throttle voltage
 * 499 - Birectrional torque command
 * 509 - Parameter access code
 * 511 - Write parameters to flash
 */

#include <UartEvent.h>

enum HERequestState { HERequest_IDLE, HERequest_WAITING, HERequest_DONE};

unsigned int parameterRequested;
unsigned int lastParameter;
unsigned int lastParameterValue;
uint8_t HEAddr;

Uart1Event uart1;
// Compute the MODBUS RTU CRC
//http://www.ccontrolsys.com/w/How_to_Compute_the_Modbus_RTU_Message_CRC
uint16_t ModRTU_CRC(uint8_t *buf, int len)
{
  uint16_t crc = 0xFFFF;

  for (int pos = 0; pos < len; pos++) {
    crc ^= (uint8_t)buf[pos];          // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}

//MODBUS Function code 3
//Based off sniffing the ASI BACDoor software
void requestParam(uint16_t param)
{
  uint8_t commandString[] = { HEAddr, 0x03, 0xFF, 0xFF, 0x00, 0x01, 0x00, 0x00 };
  commandString[3] = param & 0xFF;
  commandString[2] = param >>8;

  uint16_t CRC = ModRTU_CRC(commandString, 6);
  commandString[6] = CRC & 0xFF;
  commandString[7] = CRC >> 8;

  parameterRequested = param;

  uart1.clear();
  uart1.flush();
  uart1.write(commandString, sizeof(commandString));
}

//Called to process a hover engine response
bool checkResponse(uint16_t timeout) //timeout in microseconds
{
  elapsedMicros timeoutCounter;
  uint8_t rxBuf[7];
  while(uart1.available() < 7 && timeoutCounter < timeout)
  {
  }
  
  if(uart1.available() == 7)
  {
    for(int i = 0;i<7;i++)
      rxBuf[i] = uart1.read();
      
    if(ModRTU_CRC(rxBuf,5) == (rxBuf[6]<<8|rxBuf[5])){
      //Checksum Valid
      if(rxBuf[0] == HEAddr && rxBuf[1] == 3){
        lastParameter = parameterRequested;
        lastParameterValue = (rxBuf[3]<<8|rxBuf[4]);
      }
    }
  }
  
  uart1.clear();
  uart1.flush();
  return false;
}

uint16_t HE1lastParameter()
{
  return lastParameter;
}

uint16_t HE1lastParameterValue()
{
  return lastParameterValue;
}
  
void setupHE(uint8_t transmitPin, uint8_t heAddr){
    uart1.begin(115200, SERIAL_8E1);
    uart1.transmitterEnable(transmitPin);
    uart1.clear();  //RX (flush is clear tx buffer)
    HEAddr = heAddr;
}

void retParam()
{

}

