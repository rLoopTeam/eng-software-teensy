#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_MMA8451.h>
#include <utility/imumaths.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <rI2CTX.h>
#include <rI2CRX.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);
Adafruit_MMA8451 mma = Adafruit_MMA8451();

uint16_t i = 0;
int BNO_Error = 0;

IntervalTimer controlTimer;

uint32_t lastMicros;

//Actual values sent out the port
int engine[4];
int servo[4];

//Control values with range 0 - 1
float engineSV [4];
//Control values with range -1 - 1
float servoSV [4];
//calibration values for the servos
float servoOffset[4];
//maximum values for the engines
float engine_max;
//Define's ADC range based off of ADC resolution
int ADCmaxValue;

float blinkRate;
int blinkCount;

uint32_t duration;
uint32_t sensors;
uint32_t readingPi;
uint32_t processingPi;
uint32_t transmitting;

float startEngine;

void updateOuputs()
{
  int ADCmidpoint = ADCmaxValue / 2;

  for(int i = 0;i<4;i++){
    if (servoSV[i] > 1) servoSV[i] = 1;
    if (servoSV[i] < -1) servoSV[i] = -1;
    servo[i] = servoSV[i] * ADCmidpoint + ADCmidpoint;

    //if(engineSV[i] > 1)engineSV[i]= 1;
    //if(engineSV[i] < 0) engineSV[i] = 0;
    engine[i] = engineSV[i] * ADCmaxValue;
  }

  analogWrite(5,servo[0]);
  analogWrite(6,servo[1]);
  analogWrite(9,servo[2]);
  analogWrite(10,servo[3]);

  analogWrite(23,engine[0]);
  analogWrite(22,engine[1]);
  analogWrite(21,engine[2]);
  analogWrite(20,engine[3]);
}

void recvParam(rI2CRX_decParam decParam);

void setup(void)
{

/*
  Wire1.begin(I2C_MASTER, 0, I2C_PINS_29_30, I2C_PULLUP_INT, I2C_RATE_100);

  // Initialise the sensor
  if(!bno.begin(&Wire1))
  {
 // There was a problem detecting the BNO055 ... check your connections 
     BNO_Error = 1;
  }
    
  //Let the BNO get settled
  delay(2000);
  bno.setExtCrystalUse(true);

  mma.begin(&Wire1);
  mma.setRange(MMA8451_RANGE_2_G);
*/

  //Open the I2C interface to the Pi
  Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_INT, I2C_RATE_1000, I2C_OP_MODE_DMA);

  rI2CRX_begin();

  rI2CRX_recvDecParamCB = &recvParam;
  rI2CRX_frameRXBeginCB = &gotAFrame;
  rI2CRX_frameRXEndCB = &endFrame;

  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(23,OUTPUT);
  pinMode(22,OUTPUT);
  pinMode(21,OUTPUT);
  pinMode(20,OUTPUT);

  pinMode(13,OUTPUT);

  analogWriteFrequency(5,490); //PWM output at 350 Hz
  analogWriteResolution(16); //PWM output at 16 bit resolution
  ADCmaxValue = 65535;
  for(int i =0;i<4;i++){
    servoSV[i] = 0;
    engineSV[i] = .49; //1000 us pulse
  }

  updateOuputs();

  blinkRate = 50;
  blinkCount = 0;

  //The I2C Library needs to be able to fire some interrupts
  //during our control loop to manage the start & stop
  //transactions in DMA mode.
  controlTimer.priority(200);

    //Setup the 100Hz control loop timer
  controlTimer.begin(ControlLoop, 1000000);
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

  //Set all the servos in microseconds
  if(decParam.index == 0 && decParam.type == rI2C_FLOAT)
  {
    float engineMicros = *((float*)decParam.val) / 2040.81632;
    engineSV[0] = engineMicros;
    engineSV[1] = engineMicros;
    engineSV[2] = engineMicros;
    engineSV[3] = engineMicros;
  }

  if(decParam.index == 1 && decParam.type == rI2C_FLOAT)
    engineSV[0] = *((float*)decParam.val);
  if(decParam.index == 2 && decParam.type == rI2C_FLOAT)
    engineSV[1] = *((float*)decParam.val);
  if(decParam.index == 3 && decParam.type == rI2C_FLOAT)
    engineSV[2] = *((float*)decParam.val);
  if(decParam.index == 4 && decParam.type == rI2C_FLOAT)
    engineSV[3] = *((float*)decParam.val);

  if(decParam.index == 5 && decParam.type == rI2C_FLOAT)
    engineSV[0] = *((float*)decParam.val) / 2040.81632;
  if(decParam.index == 6 && decParam.type == rI2C_FLOAT)
    engineSV[1] = *((float*)decParam.val) / 2040.81632;
  if(decParam.index == 7 && decParam.type == rI2C_FLOAT)
    engineSV[2] = *((float*)decParam.val) / 2040.81632;
  if(decParam.index == 8 && decParam.type == rI2C_FLOAT)
    engineSV[3] = *((float*)decParam.val) / 2040.81632;

  //Set all the servos in microseconds
  if(decParam.index == 10 && decParam.type == rI2C_FLOAT)
  {
    float engineMicros = *((float*)decParam.val) / 2040.81632;
    servoSV[0] = engineMicros;
    servoSV[1] = engineMicros;
    servoSV[2] = engineMicros;
    servoSV[3] = engineMicros;
  }

  if(decParam.index == 11 && decParam.type == rI2C_FLOAT)
    servoSV[0] = *((float*)decParam.val);
  if(decParam.index == 12 && decParam.type == rI2C_FLOAT)
    servoSV[1] = *((float*)decParam.val);
  if(decParam.index == 13 && decParam.type == rI2C_FLOAT)
    servoSV[2] = *((float*)decParam.val);
  if(decParam.index == 14 && decParam.type == rI2C_FLOAT)
    servoSV[3] = *((float*)decParam.val);

  if(decParam.index == 15 && decParam.type == rI2C_FLOAT)
    servoSV[0] = *((float*)decParam.val) / 2040.81632;
  if(decParam.index == 16 && decParam.type == rI2C_FLOAT)
    servoSV[1] = *((float*)decParam.val) / 2040.81632;
  if(decParam.index == 17 && decParam.type == rI2C_FLOAT)
    servoSV[2] = *((float*)decParam.val) / 2040.81632;
  if(decParam.index == 18 && decParam.type == rI2C_FLOAT)
    servoSV[3] = *((float*)decParam.val) / 2040.81632;
    
  if(decParam.index == 100 && decParam.type == rI2C_FLOAT)
    blinkRate = *((float*)decParam.val);
}

void ControlLoop(void)
{

  //Needed for i2c library
  interrupts();
  
  uint32_t beginM = micros();

  rI2CTX_beginFrame();
  rI2CTX_addParameter(26,(int32_t)sensors);
  rI2CTX_addParameter(27,(int32_t)readingPi);
  rI2CTX_addParameter(28,(int32_t)processingPi);
  rI2CTX_addParameter(29,(int32_t)transmitting);
  rI2CTX_addParameter(30,(int32_t)duration);
/*
  // Get a new sensor event 
  sensors_event_t event;

  bno.getEvent(&event);
  
  rI2CTX_addParameter(30,event.orientation.x);
  rI2CTX_addParameter(31,event.orientation.y);
  rI2CTX_addParameter(32,event.orientation.z);

  mma.read();
  mma.getEvent(&event);
  rI2CTX_addParameter(33,event.acceleration.x);
  rI2CTX_addParameter(34,event.acceleration.y);
  rI2CTX_addParameter(35,event.acceleration.z);
*/

  rI2CTX_addParameter(10,(float)servoSV[0]);
  rI2CTX_addParameter(11,(float)servoSV[1]);
  rI2CTX_addParameter(12,(float)servoSV[2]);
  rI2CTX_addParameter(13,(float)servoSV[3]);
  rI2CTX_addParameter(14,(int32_t)servo[0]);
  rI2CTX_addParameter(15,(int32_t)servo[1]);
  rI2CTX_addParameter(16,(int32_t)servo[2]);
  rI2CTX_addParameter(17,(int32_t)servo[3]);

  rI2CTX_addParameter(18,(float)engineSV[0]);
  rI2CTX_addParameter(19,(float)engineSV[1]);
  rI2CTX_addParameter(20,(float)engineSV[2]);
  rI2CTX_addParameter(21,(float)engineSV[3]);
  rI2CTX_addParameter(22,(int32_t)engine[0]);
  rI2CTX_addParameter(23,(int32_t)engine[1]);
  rI2CTX_addParameter(24,(int32_t)engine[2]);
  rI2CTX_addParameter(25,(int32_t)engine[3]);
  
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

