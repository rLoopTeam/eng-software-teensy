#include <Wire.h>
#include "amc7812.h"

extern AMC7812Class AMC7812;

#define MAX4948_CB_PIN 14

void setup()
{
  pinMode(MAX4948_CB_PIN, OUTPUT);
  digitalWrite(MAX4948_CB_PIN, LOW);
  pinMode(AMC7812_DSCLR0_PIN, OUTPUT);
  digitalWrite(AMC7812_DSCLR0_PIN, HIGH);
  pinMode(AMC7812_DSCLR1_PIN, OUTPUT);
  digitalWrite(AMC7812_DSCLR1_PIN, HIGH);

  Wire.begin();
  AMC7812.begin();
  AMC7812.EnableDACs();
  AMC7812.SetContinuousDACMode();
}

void loop()
{
  int i, j;
  for (i = 0; i <= 0x1000; i += 0x0200)
  {
    if (i == 0x1000) {
      i--;
    }
    AMC7812.Write(AMC7812_DAC_BASE_ADDR, (uint16_t)i);
    AMC7812.Write(AMC7812_DAC_BASE_ADDR + 1, (uint16_t)i);
    for (j = 0; j < 6; j++) {
      delay(50);
      digitalWrite(MAX4948_CB_PIN, HIGH);
      delay(100);
      digitalWrite(MAX4948_CB_PIN, LOW);
      delay(50);
    }
    if (i == 0x0FFF) {
      i++;
    }
  }
}
