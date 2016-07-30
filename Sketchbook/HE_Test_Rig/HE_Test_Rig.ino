#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
extern uint16_t asi_lastSpeed;
extern int8_t asi_lastReqIdx;
extern uint8_t asi_maxReqIdx;

char canStart = 0;
char bothReady = 0;
uint32_t startTime = 0;

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
  if (canStart != 0) {
    optoncdt_task();
    asi_task();
  }

  if (optoncdt_flag != 0 && asi_dataFlag != 0 && asi_allDataFlag != 0) {
    bothReady = 1;
  }

  if ((optoncdt_flag != 0 && asi_dataFlag != 0 && asi_allDataFlag != 0 && bothReady != 0)
  || (bothReady == 0 && (optoncdt_flag != 0 || (asi_dataFlag != 0 && asi_allDataFlag != 0)))
  )
  {
    int di;

    optoncdt_flag = 0;
    asi_dataFlag = 0;

    #ifdef MODE_SERIAL
    Serial.print((millis() - startTime), DEC);
    Serial.print(",");
    Serial.print(asi_lastSpeed, DEC);
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
    rI2CTX_addParameter(3, (int32_t)asi_lastSpeed);
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
      if (strncmp((const char*)computer_cmd_buff, "SET", 3) == 0) {
        char* read_end_ptr;
        char* write_end_ptr = (char*)&computer_cmd_buff[computer_cmd_buff_idx];
        int set_val = strtol((const char*)&computer_cmd_buff[3], &read_end_ptr, 10);
        if ((uint32_t)read_end_ptr != (uint32_t)write_end_ptr) {
          //Serial.printf("failed to parse int %08X %08X\r\n", (uint32_t)read_end_ptr, (uint32_t)write_end_ptr);
          asi_emergencyStop();
        }
        else if (set_val < 0 || set_val > 10000) {
          Serial.println("value out of range");
          asi_emergencyStop();
        }
        else {
          //Serial.println("DAC written");
          asi_setSpeed(set_val);
        }
      }
      else if (strcmp((const char*)computer_cmd_buff, "START") == 0)
      {
        pinMode(13, OUTPUT);
        digitalWrite(13, HIGH);
        Serial.println("Start");
        optoncdt_startReadings();
        asi_setupDac();
        //asi_setupRemote();
        canStart = 1;
        startTime = millis();
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
