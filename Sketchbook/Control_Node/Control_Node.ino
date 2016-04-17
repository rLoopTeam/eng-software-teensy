#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <rPodI2C.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);

int i = 0;
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

  Wire.beginTransmission(51);
  Wire.print("Booting.");
  Wire.endTransmission();

  analogReadResolution(14);

  //Setup the 100Hz control loop timer
  //controlTimer.begin(ControlLoop, 10000);

  rPodI2CbeginFrame();
  rPodI2CaddParameter(1,(int8_t)-20);
  rPodI2CaddParameter(2,(uint8_t)20);
  rPodI2CaddParameter(3,(int16_t)-1000);
  rPodI2CaddParameter(4,(uint16_t)1000);
  rPodI2CaddParameter(5,(int32_t)-1000);
  rPodI2CaddParameter(6,(uint32_t)1000);
  rPodI2CaddParameter(7,(int64_t)-1000);
  rPodI2CaddParameter(8,(uint64_t)1000);
  rPodI2CaddParameter(9,(float)-250.25);
  rPodI2CaddParameter(10,(double)-250.25);
  rPodI2CendFrame();
  
  Wire.beginTransmission(51);
  Wire.write(buffer,bufferPos);
  Wire.endTransmission();
}

void loop(void) 
{
}

void ControlLoop(void)
{

  rPodI2CbeginFrame();
  
  
  /* Get a new sensor event */ 
  sensors_event_t event; 
  bno.getEvent(&event);


  int ana0 = analogRead(0);

  rPodI2CaddParameter(1,(uint16_t)ana0);
  rPodI2CaddParameter(2,(uint16_t)i);
  rPodI2CendFrame();

  Wire.beginTransmission(51);
  Wire.write(buffer,bufferPos);
  Wire.endTransmission();

  lastUsed = usedCPU;
  i++;
  freeCPU = 0;
}

