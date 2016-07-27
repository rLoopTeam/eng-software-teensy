#ifndef _MODBUS_DEFS_H_
#define _MODBUS_DEFS_H_

#include <stdint.h>

#include "arxpax_regaddrs.h"

#define BIGENDIAN16(x, h, l) do { h = (x) >> 8; l = (x) & 0xFF; } while (0)
#define MODBUS_BUFF_MAX 256

enum {
    ModBusFunc_Read_Coil_Status = 1,
    ModBusFunc_Read_Input_Status = 2,
    ModBusFunc_Read_Holding_Registers = 3,
    ModBusFunc_Read_Input_Registers = 4,
    ModBusFunc_Force_Single_Coil = 5,
    ModBusFunc_Preset_Single_Register = 6,
    ModBusFunc_Read_Exception_Status = 7,
    ModBusFunc_Diagnostics = 8,
    ModBusFunc_Program_484 = 9,
    ModBusFunc_Poll = 10,
    ModBusFunc_Fetch_Comm_Event_Ctr = 11,
    ModBusFunc_Fetch_Comm_Event_Log = 12,
    ModBusFunc_Program_Controller = 13,
    ModBusFunc_Poll_Controller = 14,
    ModBusFunc_Force_Multiple_Coils = 15,
    ModBusFunc_Preset_Multiple_Registers = 16,
    ModBusFunc_Report_Slave_ID = 17,
    ModBusFunc_Program_884 = 18,
    ModBusFunc_Reset_Comm_Link = 19,
    ModBusFunc_Read_General_Reference = 20,
    ModBusFunc_Write_General_Reference = 21,
};

typedef struct {
    uint8_t param_h;
    uint8_t param_l;
    uint8_t regcnt_h;
    uint8_t regcnt_l;
} read_holding_reg_t;

typedef struct {
    uint8_t param_h;
    uint8_t param_l;
    uint8_t val_h;
    uint8_t val_l;
} preset_single_reg_t;

#endif