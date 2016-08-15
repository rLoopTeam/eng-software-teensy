#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEBUG

#define MODE_SERIAL
//#define MODE_I2C

#ifdef MODE_I2C
#include <rI2CTX.h>
#include <rI2CRX.h>
#endif

extern char optoncdt_flag;
extern uint32_t optoncdt_value;
extern char asi_dataFlag;
extern char asi_allDataFlag;
extern uint16_t asi_data[];
extern uint16_t asi_lastThrottle;
extern int8_t asi_lastReqIdx;
extern uint8_t asi_maxReqIdx;

char canStart = 0;
char bothReady = 0;
uint32_t startTime = 0;
char bacdoor = 0, ild1320 = 0;
char asiHasSomeConfig = 0;

#ifdef MODE_I2C
uint32_t prevSpeed = 0;

IntervalTimer controlTimer;
uint32_t readingPi;
uint32_t processingPi;
uint32_t transmitting;

void recvParam(rI2CRX_decParam decParam);
void gotAFrame() {}
void endFrame() {}
#endif

void setup()
{
  #ifdef MODE_SERIAL
  Serial.begin(115200); // baud doesn't matter, this is a virtual bus

  #ifdef DEBUG
  while(1)
  {
    // wait for user input
    if (Serial.available() > 0) {
      Serial.read();
      break;
    }
  }
  #endif
  #endif
  #ifdef MODE_I2C
  Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_INT, I2C_RATE_1000, I2C_OP_MODE_DMA);
  Wire.resetBus();
  rI2CRX_begin();
  rI2CRX_recvDecParamCB = &recvParam;
  rI2CRX_frameRXBeginCB = &gotAFrame;
  rI2CRX_frameRXEndCB = &endFrame;

  //The I2C Library needs to be able to fire some interrupts
  //during our control loop to manage the start & stop
  //transactions in DMA mode.
  controlTimer.priority(200);

    //Setup the 100Hz control loop timer
  controlTimer.begin(ControlLoop, 10000);
  
  #endif
  modbus_init();
  optoncdt_init();
  #ifndef MODE_SERIAL
  optoncdt_startReadings();
  asi_setupDac();
  //asi_setupRemote();
  canStart = 1;
  #endif
}

#define COMPUTERCMD_BUFF_MAX 256
uint8_t computer_cmd_buff[COMPUTERCMD_BUFF_MAX];
uint16_t computer_cmd_buff_idx = 0;

void loop()
{
  static uint64_t last_report_time = 0;
  uint32_t now;

  if (bacdoor != 0 || ild1320 != 0)
  {
    bridge_task();
    return;
  }

  if (canStart != 0) {
    optoncdt_task();
    asi_task();
  }

  if (optoncdt_flag != 0 && asi_dataFlag != 0 && asi_allDataFlag != 0) {
    bothReady = 1;
  }

  if (((optoncdt_flag != 0 && asi_dataFlag != 0 && asi_allDataFlag != 0 && bothReady != 0)
  || (bothReady == 0 && (optoncdt_flag != 0 || (asi_dataFlag != 0 && asi_allDataFlag != 0)))
  )
  && (millis() - last_report_time) >
    #ifndef DEBUG
    200
    #else
    2000
    #endif
  )
  {
    int di;

    optoncdt_flag = 0;
    asi_dataFlag = 0;

    now = millis();
    last_report_time = now;
    if (startTime == 0) {
      startTime = now;
    }
    
    #ifdef MODE_SERIAL
    Serial.print((now - startTime), DEC);
    Serial.print(",");
    Serial.print(asi_lastThrottle, DEC);
    Serial.print(",");
    Serial.print(optoncdt_value, DEC);
    Serial.print(",");

    for (di = 0; di <= asi_maxReqIdx; di++)
    {
      Serial.print(asi_data[di], DEC);
      Serial.print(",");
    }
    Serial.print("\r\n");
    Serial.send_now();
    #endif
    #ifdef MODE_I2C
    rI2CTX_beginFrame();
    rI2CTX_addParameter(2, (int32_t)(millis() - startTime));
    rI2CTX_addParameter(3, (int32_t)asi_lastThrottle);
    rI2CTX_addParameter(4, (int32_t)optoncdt_value);
    for (di = 0; di <= asi_maxReqIdx; di++)
    {
      uint32_t x32 = asi_data[di];
      rI2CTX_addParameter(5 + di, (int32_t)x32);
    }
    rI2CTX_endFrame();
    Wire.beginTransmission(51);
    Wire.write(rI2CTX_buffer, rI2CTX_bufferPos);
  
    transmitting = micros();
    Wire.endTransmission(I2C_STOP, 3000);
    transmitting = micros() - transmitting;
    #endif
    
  }

  #ifdef MODE_SERIAL
  while (Serial.available())
  {
    uint8_t c = Serial.read();
    if (c != '\r' && c != '\n') {
      char is_cancel = 0;
      if ((c == ' ' && computer_cmd_buff_idx == 0) || c < ' ')
      {
        is_cancel = 1;
      }
      if (is_cancel == 0)
      {
        computer_cmd_buff[computer_cmd_buff_idx] = c;
        computer_cmd_buff[computer_cmd_buff_idx + 1] = 0;
        if (computer_cmd_buff_idx < COMPUTERCMD_BUFF_MAX - 2) {
          computer_cmd_buff_idx++;
        }
      }
      else
      {
        Serial.println("\r\ncommand cancelled\r\n");
        computer_cmd_buff_idx = 0;
        asi_emergencyStop();
      }
    }
    else {
      // received a return
      computer_cmd_buff[computer_cmd_buff_idx] = 0;
      if (strncmp((const char*)computer_cmd_buff, "THROTTLE", 8) == 0) {
        char* read_end_ptr;
        char* write_end_ptr = (char*)&computer_cmd_buff[computer_cmd_buff_idx];
        int set_val = strtol((const char*)&computer_cmd_buff[8], &read_end_ptr, 10);
        if ((uint32_t)read_end_ptr != (uint32_t)write_end_ptr) {
          Serial.printf("failed to parse int %08X %08X\r\n", (uint32_t)read_end_ptr, (uint32_t)write_end_ptr);
          asi_emergencyStop();
        }
        else if (set_val < 0 || set_val > 10000) {
          Serial.println("value out of range");
          asi_emergencyStop();
        }
        else {
          #ifdef DEBUG
          Serial.println("CMD THROTTLE");
          #endif
          asi_setThrottle(set_val);
        }
      }
      else if (strncmp((const char*)computer_cmd_buff, "SETRPM", 6) == 0) {
        char* read_end_ptr;
        char* write_end_ptr = (char*)&computer_cmd_buff[computer_cmd_buff_idx];
        int set_val = strtol((const char*)&computer_cmd_buff[6], &read_end_ptr, 10);
        if ((uint32_t)read_end_ptr != (uint32_t)write_end_ptr) {
          Serial.printf("failed to parse int %08X %08X\r\n", (uint32_t)read_end_ptr, (uint32_t)write_end_ptr);
          asi_emergencyStop();
        }
        else if (set_val < 0 || set_val > 10000) {
          Serial.println("value out of range");
          asi_emergencyStop();
        }
        else {
          #ifdef DEBUG
          Serial.println("CMD SETRPM");
          #endif
          asi_setRpm(set_val);
        }
      }
      else if (strncmp((const char*)computer_cmd_buff, "SEND", 4) == 0) {
        int ci, cmdlen;
        HardwareSerial* tgt = computer_cmd_buff[4] == '2' ? &Serial2 : &Serial1;
        cmdlen = strlen((const char*)computer_cmd_buff);
        for (ci = 5; ci < cmdlen; ci += 2) {
          char hexPair[3];
          uint8_t hexRes;
          hexPair[0] = computer_cmd_buff[ci];
          hexPair[1] = computer_cmd_buff[ci + 1];
          hexPair[2] = 0;
          hexRes = (uint8_t)strtol(hexPair, NULL, 16);
          tgt->write(hexRes);
        }
        #ifdef DEBUG
        Serial.print("CMD SEND ");
        Serial.print((ci - 5) / 2, DEC);
        Serial.println();
        #endif
      }
      else if (strncmp((const char*)computer_cmd_buff, "MODBUSPARAMSET", 14) == 0)
      {
        int ci, bi, cmdlen;
        uint16_t v16s[2];
        cmdlen = strlen((const char*)computer_cmd_buff);
        for (ci = 14, bi = 0; ci < cmdlen && bi < 2; ci += 4, bi++) {
          char hexPair[5];
          hexPair[0] = computer_cmd_buff[ci];
          hexPair[1] = computer_cmd_buff[ci + 1];
          hexPair[2] = computer_cmd_buff[ci + 2];
          hexPair[3] = computer_cmd_buff[ci + 3];
          hexPair[4] = 0;
          v16s[bi] = (uint8_t)strtol(hexPair, NULL, 16);
        }
        if (bi == 2) {
          #ifdef DEBUG
          Serial.print("Sending ModRTF_PresetParam ");
          Serial.print(v16s[0], HEX);
          Serial.print(" ");
          Serial.print(v16s[1], HEX);
          Serial.println();
          #endif
          ModRTF_PresetParam(v16s[0], v16s[1]);
        }
      }
      else if (strcmp((const char*)computer_cmd_buff, "CONFIGLASER") == 0)
      {
        optoncdt_startReadings();
      }
      else if (strcmp((const char*)computer_cmd_buff, "CONFIGDAC") == 0)
      {
        optoncdt_startReadings();
        asi_setupDac();
        asiHasSomeConfig = 1;
      }
      else if (strcmp((const char*)computer_cmd_buff, "CONFIGREMOTEDAC") == 0)
      {
        optoncdt_startReadings();
        asi_setupRemote();
        asiHasSomeConfig = 1;
      }
      else if (strcmp((const char*)computer_cmd_buff, "CONFIGRPM") == 0)
      {
        optoncdt_startReadings();
        asi_setupRemoteRpm();
        asiHasSomeConfig = 1;
      }
      else if (strcmp((const char*)computer_cmd_buff, "FLUSH") == 0)
      {
        Serial1.flush();
        Serial2.flush();
      }
      else if (strcmp((const char*)computer_cmd_buff, "START") == 0)
      {
        if (asiHasSomeConfig == 0) {
          asi_setupDac();
          asiHasSomeConfig = 1;
        }
        pinMode(13, OUTPUT);
        digitalWrite(13, HIGH);
        Serial.println("Start");
        canStart = 1;
        startTime = 0;
      }
      else if (strcmp((const char*)computer_cmd_buff, "STOP") == 0)
      {
        asi_emergencyStop();
      }
      else if (strcmp((const char*)computer_cmd_buff, "BACDOOR") == 0)
      {
        bacdoor = 1;
        Serial.println("BacDoor Passthrough Enabled");
      }
      else if (strcmp((const char*)computer_cmd_buff, "ILD1320") == 0)
      {
        ild1320 = 1;
        Serial.println("ILD1320 Passthrough Enabled");
      }
      else if (strcmp((const char*)computer_cmd_buff, "RESET") == 0)
      {
        #define RESTART_ADDR       0xE000ED0C
        #define READ_RESTART()     (*(volatile uint32_t *)RESTART_ADDR)
        #define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))
        WRITE_RESTART(0x5FA0004);
      }
      else if (strlen((const char*)computer_cmd_buff) > 0){
        Serial.println("invalid command");
        asi_emergencyStop();
      }
      computer_cmd_buff_idx = 0;
    }
    Serial.send_now();
  }
  #endif
}

#ifdef MODE_I2C
void recvParam(rI2CRX_decParam decParam)
{
  if(decParam.index == 1 && decParam.type == rI2C_FLOAT)
  {
    float x = *((float*)decParam.val);
    uint32_t x32 = roundf(x);
    if (x32 >= 0 && x32 <= 10000)
    {
      if (x32 != prevSpeed) {
        asi_setSpeed((double)x32);
      }
      prevSpeed = x32;
    }
    else
    {
      asi_emergencyStop();
      prevSpeed = 0;
    }
  }
}

void ControlLoop()
{
  uint8_t recvByte;

  interrupts();

  readingPi = micros();

  Wire.requestFrom(51, 100, 500);

  readingPi = micros() - readingPi;

  processingPi = micros();
  
  while (Wire.available())
  {
    recvByte = Wire.readByte();
    rI2CRX_receiveBytes(&recvByte, 1);
  }

  processingPi = micros() - processingPi;
}
#endif

void bridge_task(void)
{
  HardwareSerial* tgt = NULL;
  if (bacdoor != 0) {
    tgt = &Serial1;
  }
  else if (ild1320 != 0) {
    tgt = &Serial2;
  }
  while (Serial.available() > 0)
  {
    uint8_t c;
    c = Serial.read();
    tgt->write(c);
  }
  while (tgt->available() > 0)
  {
    uint8_t c;
    c = tgt->read();
    Serial.write(c);
  }
  Serial.send_now();
}

