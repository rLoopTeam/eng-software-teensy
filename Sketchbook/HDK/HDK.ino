#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <rPodI2C.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);

uint16_t i = 0;
int BNO_Error = 0;

IntervalTimer controlTimer;

uint32_t lastMicros;



void setup(void) 
{
  
  /* Initialise the sensor */
  //if(!bno.begin())
 // {
    /* There was a problem detecting the BNO055 ... check your connections */
 //   BNO_Error = 1;
//  }

  //Let the BNO get settled
  delay(1000);
    
//  bno.setExtCrystalUse(true);

  //Open the I2C interface to the Pi
  Wire.begin(I2C_MASTER,0,I2C_PINS_18_19,I2C_PULLUP_INT,I2C_RATE_1000);

  analogReadResolution(14);

  //Setup the 100Hz control loop timer
  controlTimer.begin(ControlLoop, 10000);
}

void loop(void) 
{
}

void ControlLoop(void)
{

  uint32_t beginM = micros();

  /* Get a new sensor event */ 
  //sensors_event_t event; 

 // bno.getEvent(&event);

  rPodI2CbeginFrame();
  
  //rPodI2CaddParameter(1,event.orientation.x);
  //rPodI2CaddParameter(2,event.orientation.y);
  //rPodI2CaddParameter(3,event.orientation.z);
  
  rPodI2CaddParameter(4,(uint32_t)lastMicros);
  rPodI2CaddParameter(6,(uint32_t)1000);
  rPodI2CaddParameter(7,(int64_t)-1000);
  rPodI2CaddParameter(8,(uint64_t)1000);
  rPodI2CaddParameter(9,(float)-250.25);
  rPodI2CaddParameter(10,(double)-250.25);

  rPodI2CaddParameter(11,i);
  rPodI2CendFrame();

  Wire.beginTransmission(51);
  Wire.write(buffer,bufferPos);
  Wire.endTransmission();

  lastMicros = micros() - beginM;
  i++;
}

