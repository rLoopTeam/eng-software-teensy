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


//Blinks the LED on the Teensy
//as a heartbeat from the control loop
float blinkRate;
int blinkCount;

//Measures the timing of
//the control loop
uint32_t duration;
uint32_t sensors;
uint32_t readingPi;
uint32_t processingPi;
uint32_t transmitting;

void recvParam(rI2CRX_decParam decParam);

void setup(void)
{

  pinMode(15,OUTPUT);
  digitalWrite(15,HIGH);
  
  Wire1.begin(I2C_MASTER, 0, I2C_PINS_29_30, I2C_PULLUP_INT, I2C_RATE_100);

  //Resets the BNO if it got interrupted
  //in an I2C transaction
  do{
    Wire1.resetBus();
  
    BNO_Error = 0;
  
    // Initialise the sensor
    if(!bno.begin(&Wire1))
    {
   // There was a problem detecting the BNO055 ... check your connections 
       BNO_Error = 1;
    }
  
    if(BNO_Error == 1){
  
      digitalWrite(15,LOW);
      delay(1000);
      digitalWrite(15,HIGH);
      delay(1000);
    }
  }while(BNO_Error == 1);
    
  //Let the BNO get settled
  delay(2000);
  bno.setExtCrystalUse(true);

  //Open the I2C interface to the Pi
  Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_INT, I2C_RATE_1000, I2C_OP_MODE_DMA);

  rI2CRX_begin();

  rI2CRX_recvDecParamCB = &recvParam;
  rI2CRX_frameRXBeginCB = &gotAFrame;
  rI2CRX_frameRXEndCB = &endFrame;


  blinkRate = 50;
  blinkCount = 0;

  //The I2C Library needs to be able to fire some interrupts
  //during our control loop to manage the start & stop
  //transactions in DMA mode.
  controlTimer.priority(200);

    //Setup the 100Hz control loop timer
  controlTimer.begin(ControlLoop, 10000);

  angleSetPoint = 0;
}

void loop(void)
{

}

void gotAFrame()
{}

void endFrame()
{
  updateOuputs();
}

//Called whenever a new
//parameter value is received from the
//Pi via the I2C bus
void recvParam(rI2CRX_decParam decParam)
{
  //Example Code
  /*
  if(decParam.index == 1 && decParam.type == rI2C_FLOAT)
    engineSV[0] = *((float*)decParam.val);
  if(decParam.index == 2 && decParam.type == rI2C_FLOAT)
    engineSV[1] = *((float*)decParam.val);
  if(decParam.index == 3 && decParam.type == rI2C_FLOAT)
    engineSV[2] = *((float*)decParam.val);
  if(decParam.index == 4 && decParam.type == rI2C_FLOAT)
    engineSV[3] = *((float*)decParam.val);*/

  if(decParam.index == 100 && decParam.type == rI2C_FLOAT)
    blinkRate = *((float*)decParam.val);
}

void ControlLoop(void)
{

  //Needed for i2c library
  interrupts();
  
  uint32_t beginM = micros();

  rI2CTX_beginFrame();/*
  rI2CTX_addParameter(26,(int32_t)sensors);
  rI2CTX_addParameter(27,(int32_t)readingPi);
  rI2CTX_addParameter(28,(int32_t)processingPi);
  rI2CTX_addParameter(29,(int32_t)transmitting);
  rI2CTX_addParameter(30,(int32_t)duration);*/

  // Get a new sensor event 
  sensors_event_t event;

  bno.getEvent(&event);
  
  rI2CTX_addParameter(30,event.orientation.x);  //This one
 // rI2CTX_addParameter(31,event.orientation.y);
  //rI2CTX_addParameter(32,event.orientation.z);

//  mma.read();
//  mma.getEvent(&event);
//  rI2CTX_addParameter(33,event.acceleration.x);
//  rI2CTX_addParameter(34,event.acceleration.y);
//  rI2CTX_addParameter(35,event.acceleration.z);

  rI2CTX_addParameter(75,(float)error);
  rI2CTX_addParameter(76,(float)errorI);

  rI2CTX_addParameter(10,(float)servoSV[0]);
  rI2CTX_addParameter(11,(float)servoSV[1]);
  rI2CTX_addParameter(12,(float)servoSV[2]);
  rI2CTX_addParameter(13,(float)servoSV[3]);
  
  rI2CTX_addParameter(50,(int32_t)i);
  
  rI2CTX_endFrame();

  sensors = micros() - beginM;


  readingPi = micros();

  int request = 100;
  Wire.requestFrom(51, 100, 500);
  uint8_t recvByte;

  readingPi = micros() - readingPi;

  processingPi = micros();
  
  while (Wire.available())
  {
    recvByte = Wire.readByte();
    rI2CRX_receiveBytes(&recvByte, 1);
  }

  processingPi = micros() - processingPi;
  
  Wire.beginTransmission(51);
  Wire.write(rI2CTX_buffer, rI2CTX_bufferPos);

    transmitting = micros();
  Wire.endTransmission(I2C_STOP, 3000);

  transmitting = micros() - transmitting;
  
  i++;
  blinkCount++;
  if(blinkCount == blinkRate)
    analogWrite(13, ADCmaxValue);
  if(blinkCount >= blinkRate*2)
  {
    analogWrite(13,0);
    blinkCount = 0;
  }

  duration = micros() - beginM;

}

