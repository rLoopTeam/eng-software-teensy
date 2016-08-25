#include <i2c_t3.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <rI2CTX.h>
#include <rI2CRX.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);

uint16_t i = 0;
int BNO_Error = 0;

IntervalTimer controlTimer;
elapsedMillis freeCPU;
elapsedMillis usedCPU;
int lastUsed = 0;



void setup(void) 
{
  
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    BNO_Error = 1;
  }

  //Let the BNO get settled
  delay(1000);
    
  bno.setExtCrystalUse(true);

  //Open the I2C interface to the Pi
  Wire.begin(I2C_MASTER,0,I2C_PINS_18_19,I2C_PULLUP_INT,I2C_RATE_1000);

  analogReadResolution(14);

  rI2CRX_begin(20);

  //Setup the 100Hz control loop timer
  //25 Hz for now
  controlTimer.begin(ControlLoop, 10000*50);
  
}

void loop(void) 
{
}

void ControlLoop(void)
{

  /* Get a new sensor event */ 
  sensors_event_t event; 
  bno.getEvent(&event);

  rI2CTX_beginFrame();
  
  rI2CTX_addParameter(1,event.orientation.x);
  rI2CTX_addParameter(2,event.orientation.y);
  rI2CTX_addParameter(3,event.orientation.z);/*
  rPodI2CaddParameter(4,(uint16_t)1000);
  rPodI2CaddParameter(5,(int32_t)-1000);
  rPodI2CaddParameter(6,(uint32_t)1000);
  rPodI2CaddParameter(7,(int64_t)-1000);
  rPodI2CaddParameter(8,(uint64_t)1000);
  rPodI2CaddParameter(9,(float)-250.25);
  rPodI2CaddParameter(10,(double)-250.25);*/



  rI2CTX_addParameter(11,i);
  rI2CTX_endFrame();

  Wire.beginTransmission(51);
  Wire.write(rI2CTX_buffer,rI2CTX_bufferPos);
  Wire.endTransmission();

  lastUsed = usedCPU;
  i++;
  freeCPU = 0;
}

