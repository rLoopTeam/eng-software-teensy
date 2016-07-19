#include <UartEvent.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <HardwareSerial.h>

IntervalTimer controlTimer;

Uart1Event uart1;
uint16_t distance;
uint16_t timer;

void setup(void)
{

  //Run at 4khz for now
  controlTimer.begin(ControlLoop, 1000000/4000);
  
  
  delay(3000);
  Serial.println("Hello");
  Serial.println(String(SERIAL_8E1));
  distance = 34856;
  timer = 0;
  uart1.begin(115200, 6);
}

void loop(void)
{

}


void ControlLoop(void)
{
  uint8_t data[3] = {distance & 0x003E, 0x40|((distance & 0xFC0)>>6),0x80|((distance & 0xF000)>>12)};
  
  uart1.write(data, sizeof(data));
  timer++;
  if(timer == 4000)
  {
    timer = 0;
    distance++;
  }
}

