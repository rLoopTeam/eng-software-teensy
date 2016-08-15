#include <stdlib.h>
#include <math.h>
#include "modbus_defs.h"
#include "arxpax_regaddrs.h"

#define MODBUS_BAUD 115200
#define MODBUS_DIR_PIN 2
#define MODBUS_TIMEOUT 5

#define ASI_MAX_RPM 5000

#define ASI_DATA_STORE_SIZE 16
#ifdef DEFAULT_MODBUS_ADDRESS
#define ASI_DEVICE_ADDR DEFAULT_MODBUS_ADDRESS
#else
#define ASI_DEVICE_ADDR 1
#endif

char asi_modeIsRemote = 0;
double asi_pendingThrottle = -1, asi_pendingRpm = -1;
char asi_pendingReply = 0;
int8_t asi_lastReqIdx = 0;
uint8_t asi_maxReqIdx = 0;
uint16_t asi_data[ASI_DATA_STORE_SIZE];
char asi_dataFlag = 0, asi_allDataFlag = 0;
uint16_t asi_lastThrottle = 0, asi_lastRpm = 0;

void modbus_init()
{
  int i;
  pinMode(0, INPUT);
  pinMode(1, OUTPUT);
  pinMode(MODBUS_DIR_PIN, OUTPUT);
  Serial1.begin(MODBUS_BAUD); // this must be 115200
  Serial1.transmitterEnable(MODBUS_DIR_PIN);
  asi_pendingThrottle = -1;
  for (i = 0; i < ASI_DATA_STORE_SIZE; i++) {
    asi_data[i] = 0;
  }
#ifdef DEBUG
  Serial.println("modbus_init");
#endif
}

void asi_setupDac()
{
#define DAC_BITS 12
#define DAC_PIN A14
  //pinMode(DAC_PIN, OUTPUT);
  analogWriteResolution(DAC_BITS);
  analogWrite(DAC_PIN, 0);
  ModRTF_PresetParam(ArxPaxRegAddr_Throttle_full_voltage, 3 * 4096);
  delay(100);
  ModRTF_PresetParam(ArxPaxRegAddr_Throttle_off_voltage, 4096 / 2);
  delay(1000); // long enough to receive all packets we don't care about
  Serial1.flush();
  asi_pendingReply = 0;
  asi_modeIsRemote = 0;
#ifdef DEBUG
  Serial.println("asi_setupDac");
#endif
}

void asi_setupRemote()
{
  ModRTF_PresetParam(ArxPaxRegAddr_Throttle_full_voltage, 4096);
  delay(100);
  ModRTF_PresetParam(ArxPaxRegAddr_Throttle_off_voltage, 0);
  delay(100);
  ModRTF_PresetParam(ArxPaxRegAddr_Control_command_source, 0);
  delay(100);
  ModRTF_PresetParam(ArxPaxRegAddr_State_command, 2);
  delay(100);
  ModRTF_PresetParam(ArxPaxRegAddr_Remote_Throttle_Voltage, 0);
  delay(1000); // long enough to receive all packets we don't care about
  Serial1.flush();
  asi_pendingReply = 0;
  asi_modeIsRemote = 1;
#ifdef DEBUG
  Serial.println("asi_setupRemote");
#endif
}

void asi_setupRemoteRpm()
{
  ModRTF_PresetParam(ArxPaxRegAddr_Throttle_full_voltage, 4096);
  delay(100);
  ModRTF_PresetParam(ArxPaxRegAddr_Throttle_off_voltage, 0);
  delay(100);
  ModRTF_PresetParam(ArxPaxRegAddr_Control_command_source, 0);
  delay(100);
  ModRTF_PresetParam(ArxPaxRegAddr_State_command, 2);
  delay(100);
  ModRTF_PresetParam(ArxPaxRegAddr_Remote_Throttle_Voltage, 0);
  delay(100);
  ModRTF_PresetParam(ArxPaxRegAddr_Rated_motor_speed, 5000);
  delay(1000); // long enough to receive all packets we don't care about
  Serial1.flush();
  asi_pendingReply = 0;
  asi_modeIsRemote = 1;
#ifdef DEBUG
  Serial.println("asi_setupRemoteRpm");
#endif
}

void asi_setSpeed_internal()
{
  double x;
  double rgmax, y;
  int32_t rounded;
  if (asi_pendingThrottle < 0 && asi_pendingRpm < 0) {
    return;
  }

  if (asi_pendingRpm >= 0)
  {
    x = asi_pendingRpm;
  }
  else {
    x = asi_pendingThrottle;
  }
  if (asi_modeIsRemote == 0)
  {
    x /= 10000.0;
    rgmax = (1 << DAC_BITS) - 1;
    y = x * rgmax;
    rounded = round(y);
    if (rounded < 0) {
      rounded = 0;
    }
    if (rounded >= (1 << DAC_BITS)) {
      rounded = (1 << DAC_BITS) - 1;
    }
    analogWrite(DAC_PIN, (int)rounded);
    asi_lastThrottle = rounded;
#ifdef DEBUG
    Serial.println("DAC throttle output set");
#endif
  }
  else
  {
    uint16_t addr;
    if (asi_pendingRpm >= 0) {
      y = x / 50.0d;
      y *= 40.96d;
      rounded = round(y);
      addr = ArxPaxRegAddr_Remote_speed_command;
      asi_lastThrottle = 0;
      asi_lastRpm = rounded;
#ifdef DEBUG
      Serial.print("remote speed set ");
      Serial.print(x, DEC);
      Serial.print(" ");
      Serial.println(rounded, DEC);
#endif
    }
    else {
      addr = ArxPaxRegAddr_Remote_Throttle_Voltage;
      rgmax = 4096 - 1;
      y = x * rgmax;
      y /=
        rounded = round(y);
      if (rounded < 0) {
        rounded = 0;
      }
      if (rounded >= 4096) {
        rounded = 4096 - 1;
      }
      asi_lastThrottle = rounded;
      asi_lastRpm = 0;
#ifdef DEBUG
      Serial.println("remote throttle set");
#endif
    }
    ModRTF_PresetParam(addr, rounded);
    asi_pendingReply = 1;
  }
  asi_pendingThrottle = -1;
  asi_pendingRpm = -1;
}

void asi_setThrottle(double x)
{
  asi_pendingThrottle = x;
}

void asi_setRpm(double x)
{
  asi_pendingRpm = x;
}

void asi_emergencyStop()
{
  asi_pendingThrottle = -1;
  analogWrite(DAC_PIN, 0);
  ModRTF_PresetParam(ArxPaxRegAddr_Remote_Throttle_Voltage, 0);
  ModRTF_PresetParam(ArxPaxRegAddr_Remote_speed_command, 0);
  asi_lastThrottle = 0;
  asi_lastRpm = 0;
  asi_pendingReply = 1;
#ifdef DEBUG
  Serial.println("E-STOP");
#endif
}

const uint16_t asi_wantedAddrs[] = {
  ArxPaxRegAddr_motor_rpm,
  ArxPaxRegAddr_battery_voltage,
  ArxPaxRegAddr_motor_current,
  ArxPaxRegAddr_motor_input_power,
  ArxPaxRegAddr_battery_current,
  ArxPaxRegAddr_battery_power,
  ArxPaxRegAddr_throttle_voltage,
  ArxPaxRegAddr_controller_temperature,
  0xFFFF, // terminate table
};

void asi_task()
{
#define MODBUS_BUFF_SIZE 64
  static uint64_t last_char_time = 0;
  static uint8_t buff[MODBUS_BUFF_SIZE];
  static uint8_t buff_idx = 0;
  uint64_t now;

  if (asi_pendingReply == 0)
  {
    if (asi_pendingThrottle >= 0 || asi_pendingRpm >= 0)
    {
      asi_setSpeed_internal();
      return;
    }
    else
    {
      int32_t addr = -1;
      while (addr < 0)
      {
        uint16_t x;
        asi_lastReqIdx++;
        if (asi_lastReqIdx >= ASI_DATA_STORE_SIZE) {
          asi_lastReqIdx = -1;
          continue;
        }
        x = asi_wantedAddrs[asi_lastReqIdx];
        if (x != 0xFFFF) // not end of table
        {
          if (asi_lastReqIdx > asi_maxReqIdx) {
            asi_maxReqIdx = asi_lastReqIdx;
          }
          addr = x;
          break;
        }
        else
        {
          asi_lastReqIdx = -1;
        }
      }
      ModRTU_RequestParam(addr, 1);
      asi_pendingReply = 1;
      buff_idx = 0;
      last_char_time = millis();
    }
  }
  else
  {
    if (Serial1.available() <= 0) // nothing to read
    {
      now = millis();
      if ((now - last_char_time) >= MODBUS_TIMEOUT) // check for timeout
      {
        if (buff_idx >= 7) {
          asi_process(buff, buff_idx);
        }
        buff_idx = 0;
        asi_pendingReply = 0;
      }
    }
    else
    {
      while (Serial1.available() > 0)
      {
        uint8_t c;
        now = millis();
        if ((now - last_char_time) >= MODBUS_TIMEOUT) // check for timeout
        {
          if (buff_idx >= 7) {
            asi_process(buff, buff_idx);
          }
          buff_idx = 0;
          asi_pendingReply = 0;
        }
        last_char_time = now;
        c = Serial1.read();
        buff[buff_idx] = c;
        buff_idx++;
        if (buff_idx >= MODBUS_BUFF_SIZE) {
          buff_idx = 0;
        }
      }
    }
  }
}

void asi_process(uint8_t* buff, uint8_t len)
{
  //uint16_t crc_calc;
  //uint16_t* crc_got;

  uint16_t data;

  if (len <= 4) {
    return;
  }

  if (buff[0] != ASI_DEVICE_ADDR) {
    return;
  }
  if (buff[1] != ModBusFunc_Read_Holding_Registers) {
    return;
  }

  /*
    crc_got = (uint16_t*)&(buff[len - 2]);
    crc_calc = ModRTU_CRC(buff, len - 2);
    if (crc_calc != (*crc_got)) {
    return;
    }
    //*/

  if (buff[2] != 2) { // not right length
    return;
  }

  if (asi_lastReqIdx >= ASI_DATA_STORE_SIZE) {
    return;
  }

  data = buff[3];
  data <<= 8;
  data += buff[4];

  asi_data[asi_lastReqIdx] = data;
  asi_dataFlag = 1;
  if (asi_lastReqIdx < asi_maxReqIdx) {
    asi_allDataFlag = 1;
  }
}
