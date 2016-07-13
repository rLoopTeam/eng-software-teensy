#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "modbus_defs.h"

#define DEFAULT_MODBUS_ADDRESS 1

//#define INITIAL_BAUD 115200
#define INITIAL_BAUD 57600

#define DAC_BITS 12

#define DIR_CTRL_PIN 2
#define MODE_CTRL_PIN 5
#define DAC_PIN A14

#define ESCTOCOMPUTER_BUFF_MAX 64 // must be smaller than USB endpoint size
uint8_t esc_to_computer_buff[ESCTOCOMPUTER_BUFF_MAX];
uint16_t esc_to_computer_buff_idx = 0;

#define COMPUTERCMD_BUFF_MAX 256
uint8_t computer_cmd_buff[COMPUTERCMD_BUFF_MAX];
uint16_t computer_cmd_buff_idx = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(INITIAL_BAUD);
  pinMode(DIR_CTRL_PIN, OUTPUT);
  pinMode(MODE_CTRL_PIN, INPUT_PULLUP);
  analogWriteResolution(DAC_BITS);
}

void loop() {
  // put your main code here, to run repeatedly:
  static uint32_t prev_baud = 0;
  uint32_t baud = Serial.baud();
  static int64_t last_char_time = -1;
  char must_send = 0;
  static char prev_mode = HIGH;
  char cur_mode;
  static char lock_in_bridge_mode = 0;

  if (lock_in_bridge_mode != 0) {
    cur_mode = HIGH;
  }
  else {
    cur_mode = digitalRead(MODE_CTRL_PIN);
  }

  if (cur_mode != prev_mode) { // detect mode transitions
    if (cur_mode == LOW) {
      // this is console command mode
      computer_cmd_buff_idx = 0;
      analogWrite(A14, 0);
      // TODO other tasks required
    }
    else {
      // this is bridge mode
      esc_to_computer_buff_idx = 0;
      analogWrite(A14, 0);
      // TODO other tasks required
    }
    prev_mode = cur_mode;
  }

  if (baud != prev_baud) // this must've happened
  {
    Serial1.begin(baud);
    Serial2.begin(baud); // Serial2 used as debug
    Serial1.transmitterEnable(DIR_CTRL_PIN);
    prev_baud = baud;
  }

  while (Serial1.available())
  {
    uint8_t c = Serial1.read();
    esc_to_computer_buff[esc_to_computer_buff_idx] = c;
    esc_to_computer_buff_idx++;
    last_char_time = millis();
    if (esc_to_computer_buff_idx >= ESCTOCOMPUTER_BUFF_MAX) {
      must_send = 1;
      break;
    }
  }

  if (((millis() - last_char_time) > 200 && last_char_time >= 0) || must_send != 0) {
    // send all if timeout or out of buffer
    Serial.write(esc_to_computer_buff, esc_to_computer_buff_idx);
    Serial.send_now();
    esc_to_computer_buff_idx = 0;
  }

  while (Serial.available())
  {
    uint8_t c = Serial.read();
    if (cur_mode != LOW) {
      Serial1.write(c);
      Serial2.write(c);
    }
    else {
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
          analogWrite(A14, 0);
        }
      }
      else {
        // received a return
        computer_cmd_buff[computer_cmd_buff_idx] = 0;
        if (strncmp((const char*)computer_cmd_buff, "DAC", 3) == 0) {
          char* end_ptr;
          int dac_val = strtol((const char*)computer_cmd_buff, &end_ptr, 10);
          if ((uint32_t)end_ptr != (uint32_t)&computer_cmd_buff[computer_cmd_buff_idx]) {
            Serial.println("failed to parse int");
            analogWrite(A14, 0);
          }
          else if (dac_val < 0 || dac_val >= (1 << DAC_BITS)) {
            Serial.println("value out of range");
            analogWrite(A14, 0);
          }
          else {
            Serial.println("DAC written");
            analogWrite(A14, (int)dac_val);
          }
        }
        else if (strcmp((const char*)computer_cmd_buff, "RUN_DESIRED_CONFIG") == 0) {
          Serial.println("executing RUN_DESIRED_CONFIG\r\n");
          // TODO
        }
        else if (strcmp((const char*)computer_cmd_buff, "LOCK_IN_BRIDGE_MODE") == 0) {
          Serial.println("executing LOCK_IN_BRIDGE_MODE\r\n");
          lock_in_bridge_mode = 1;
        }
        else if (strlen((const char*)computer_cmd_buff) > 0){
          Serial.println("invalid command");
          analogWrite(A14, 0);
        }
      }
      Serial.send_now();
    }
  }
}

void ModRTU_Send(uint8_t addr, uint8_t func, uint8_t* buf, uint8_t n)
{
  static uint8_t modbus_buff[MODBUS_BUFF_MAX];
  uint16_t* crc_ptr = (uint16_t*)&modbus_buff[2 + n];
  uint16_t crc;
  modbus_buff[0] = addr;
  modbus_buff[1] = func;
  memcpy(&modbus_buff[2], buf, n);
  crc = ModRTU_CRC(modbus_buff, 2 + n);
  *crc_ptr = crc;

  delay(200); // todo: calculate proper
  Serial1.write(modbus_buff, 2 + n + 2);
  delay(200); // todo: calculate proper
}

void ModRTU_RequestParam(uint16_t param, uint16_t len)
{
  read_holding_reg_t packet;
  BIGENDIAN16(param, packet.param_h, packet.param_l);
  BIGENDIAN16(len, packet.regcnt_h, packet.regcnt_l);
  ModRTU_Send(DEFAULT_MODBUS_ADDRESS, ModBusFunc_Read_Holding_Registers, (uint8_t*)&packet, sizeof(read_holding_reg_t));
}

void ModRTF_PresetParam(uint16_t param, uint16_t value)
{
  preset_single_reg_t packet;
  BIGENDIAN16(param, packet.param_h, packet.param_l);
  BIGENDIAN16(value, packet.val_h, packet.val_l);
  ModRTU_Send(DEFAULT_MODBUS_ADDRESS, ModBusFunc_Preset_Single_Register, (uint8_t*)&packet, sizeof(preset_single_reg_t));
}

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

