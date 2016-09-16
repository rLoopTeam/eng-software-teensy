#ifndef __AVR__
#define OPTONCDT_BAUD 921600
#else
#define OPTONCDT_BAUD 9600
#endif

char optoncdt_flag = 0;
uint32_t optoncdt_value = 0;

void optoncdt_init()
{
  #ifndef __AVR__
  pinMode(9, INPUT);
  pinMode(10, OUTPUT);
  #else
  pinMode(17, INPUT);
  pinMode(16, OUTPUT);
  #endif
  Serial2.begin(OPTONCDT_BAUD);
  #ifdef DEBUG
  Serial.println("optoncdt_init");
  #endif
}

void optoncdt_startReadings()
{
  Serial2.print("LASERPOW FULL\r\n");
  delay(100);
  Serial2.print("OUTPUT RS422\r\n");
  #ifdef DEBUG
  Serial.println("optoncdt_startReadings");
  #endif
}

uint32_t optoncdt_convertRaw(uint8_t* buff)
{
  static uint32_t good_result = 0;
  static uint8_t bad_cnt = 0;
  uint32_t result = buff[0] & 0x3F;
  uint32_t mid = buff[1] & 0x3F;
  uint32_t hi = buff[2] & 0x0F;
  result += mid << 6;
  result += hi << 12;

#define FILTER_BAD_LASER_DATA
#ifdef FILTER_BAD_LASER_DATA

  if (result == 65467)
  {
    if (bad_cnt > 100)
    {
      return result;
    }
    else
    {
      bad_cnt++;
      return good_result;
    }
  }
  else
  {
    bad_cnt = 0;
    good_result = result;
    return result;
  }
#else
  return result;
#endif
}

void optoncdt_task()
{
  static uint8_t buff[3];
  while (Serial2.available() > 0)
  {
    uint8_t c = Serial2.read();
    if ((c & (1 << 7)) != 0) // higher bits, sent last
    {
      buff[2] = c;
      optoncdt_flag = 1;
      optoncdt_value = optoncdt_convertRaw(buff);
    }
    else if ((c & 0xC0) != 0) // middle bits
    {
      buff[1] = c;
    }
    else if ((c & 0xC0) == 0) // lower bits
    {
      buff[0] = c;
    }
  }
  // new line sequences in the data stream should be automatically ignored due to this algorithm
  // some junk data might exist but only at the very start
}

