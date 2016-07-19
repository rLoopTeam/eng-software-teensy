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
  uint8_t commandString[] = { 0x01, 0x03, 0xFF, 0xFF, 0x00, 0x01, 0x00, 0x00 };
  commandString[2] = param & 0xFF;
  commandString[3] = param >>8;

  uint16_t CRC = ModRTU_CRC(commandString, 6);
  commandString[6] = CRC & 0xFF;
  commandString[7] = CRC >> 8;

  //TODO: Send the uart data
}
  
void setupHE(uint8_t transmitPin){
    uart1.begin(115200);
    uart1.transmitterEnable(transmitPin);
    uart1.clear();
}


void retParam()
{

}

