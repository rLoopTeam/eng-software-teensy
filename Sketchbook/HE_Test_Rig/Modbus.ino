#define DEFAULT_MODBUS_ADDRESS 1

void ModRTU_Send(uint8_t addr, uint8_t func, uint8_t* buf, uint8_t n)
{
  static uint8_t modbus_buff[MODBUS_BUFF_MAX];
  uint16_t* crc_ptr = (uint16_t*)&modbus_buff[2 + n];
  uint16_t crc;

  if (n + 4 >= MODBUS_BUFF_MAX) {
    // TODO error message
    return;
  }

  modbus_buff[0] = addr;
  modbus_buff[1] = func;
  memcpy(&modbus_buff[2], buf, n);
  crc = ModRTU_CRC(modbus_buff, 2 + n);
  *crc_ptr = crc;

  Serial1.write(modbus_buff, 2 + n + 2);
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
