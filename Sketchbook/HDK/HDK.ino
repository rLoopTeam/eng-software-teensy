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

uint32_t lastMicros;

int8_t test_int8_t;
uint8_t test_uint8_t;
int16_t test_int16_t;
uint16_t test_uint16_t;

void recvParam(rI2CRX_decParam decParam);

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


  test_int8_t=0;
  test_uint8_t=0;
  test_int16_t=0;
  test_uint16_t=0;

  rI2CRX_recvDecParamCB = recvParam;

  rI2CRX_begin();

  //Setup the 100Hz control loop timer
  controlTimer.begin(ControlLoop, 10000);
}

void loop(void) 
{
}

//Called whenever a new
//parameter value is received from the
//Pi via the I2C bus
void recvParam(rI2CRX_decParam decParam)
{
    if(decParam.index == 0 && decParam.type == rI2C_INT8)
      test_int8_t = *((int8_t*)decParam.val);
    if(decParam.index == 1 && decParam.type == rI2C_UINT8)
      test_uint8_t = *((uint8_t*)decParam.val);
    if(decParam.index == 1 && decParam.type == rI2C_INT16)
      test_int16_t = *((int16_t*)decParam.val);
    if(decParam.index == 2 && decParam.type == rI2C_UINT16)
      test_uint16_t = *((uint16_t*)decParam.val);
}

void ControlLoop(void)
{

  uint32_t beginM = micros();

  /* Get a new sensor event */ 
  //sensors_event_t event; 

 // bno.getEvent(&event);

  rI2CTX_beginFrame();
  
  //rPodI2CaddParameter(1,event.orientation.x);
  //rPodI2CaddParameter(2,event.orientation.y);
  //rPodI2CaddParameter(3,event.orientation.z);
  
  rI2CTX_addParameter(4,(uint32_t)lastMicros);
  rI2CTX_addParameter(6,(uint32_t)1000);
  rI2CTX_addParameter(7,(int64_t)-1000);
  rI2CTX_addParameter(8,(uint64_t)1000);
  rI2CTX_addParameter(9,(float)-250.25);
  rI2CTX_addParameter(10,(double)-250.25);

  rI2CTX_addParameter(11,(double)test_int8_t);
  rI2CTX_addParameter(12,(double)test_uint8_t);
  rI2CTX_addParameter(13,(double)test_int16_t);
  rI2CTX_addParameter(14,(double)test_uint16_t);

  rI2CTX_addParameter(11,i);
  rI2CTX_endFrame();

  Wire.beginTransmission(51);
  Wire.write(rI2CTX_buffer,rI2CTX_bufferPos);
  Wire.endTransmission();

  Wire.requestFrom(51,30);
  uint8_t recvByte;
  while(Wire.available())
  {
    recvByte = Wire.read();
    rI2CRX_receiveBytes(&recvByte,1);
  }

  lastMicros = micros() - beginM;
  i++;
}

