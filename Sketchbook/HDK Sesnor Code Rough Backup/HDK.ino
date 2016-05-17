#include <PulsePosition.h>

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

float lastLoopDuration;

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

float servoMax[4];
float servoMin[4];

float blinkRate;
int blinkCount;

void updateOuputs()
{
  int ADCmidpoint = ADCmaxValue / 2;
  float servoRange;


  for(int i = 0;i<4;i++){
    if (servoSV[i] > servoMax[i]) servoSV[i] = servoMax[i];
    if (servoSV[i] < servoMin[i]) servoSV[i] = servoMin[i];
    servoRange = servoMax[i] - servoMin[i];
    servo[i]=(servoRange*((servoSV[1]+1)/2) + servoMin[i]) * ADCmidpoint + ADCmidpoint;

    if(engineSV[i] > 1)engineSV[i]= 1;
    if(engineSV[i] < 0) engineSV[i] = 0;
    engine[i] = engineSV[i]*ADCmaxValue;
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

  pinMode(15,OUTPUT);
  digitalWrite(15,LOW);
  delay(200);
  digitalWrite(15,HIGH);
  delay(2000);

  Serial.begin(9600); // USB is always 12 Mbit/sec

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

  servoMax[0] = -.5;
  servoMax[1] = .2;
  servoMax[2] = -.4;
  servoMax[3] = .35;

  servoMin[0] = -.1;
  servoMin[1] = -.2;
  servoMin[2] = 0;
  servoMin[3] = -.1;

  //Open the I2C interface to the Pi
  Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_INT, I2C_RATE_1000);

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

  analogWriteResolution(16); //PWM output at 16 bit resolution
  analogWriteFrequency(5,300); //PWM output at 350 Hz

  ADCmaxValue = 65535;
  for(int i =0;i<4;i++){
    servoSV[i] = 0;
    engineSV[i] = .1;
  }

  updateOuputs();

  blinkRate = 1;
  blinkCount = 0;

    //Setup the 100Hz control loop timer
  controlTimer.begin(ControlLoop, 1000000);
}

void loop(void)
{}

void gotAFrame()
{Serial.println("Hello");}

void endFrame()
{
  updateOuputs();
}

//Called whenever a new
//parameter value is received from the
//Pi via the I2C bus
void recvParam(rI2CRX_decParam decParam)
{
  /*
  if (decParam.index == 0 && decParam.type == rI2C_INT8)
    test_int8_t = *((int8_t*)decParam.val);
  if (decParam.index == 1 && decParam.type == rI2C_UINT8)
    test_uint8_t = *((uint8_t*)decParam.val);
  if (decParam.index == 2 && decParam.type == rI2C_INT16)
    test_int16_t = *((int16_t*)decParam.val);
  if (decParam.index == 3 && decParam.type == rI2C_UINT16)
    test_uint16_t = *((uint16_t*)decParam.val);
  if (decParam.index == 4 && decParam.type == rI2C_INT64)
    test_int64_t = *((int64_t*)decParam.val);
  if (decParam.index == 5 && decParam.type == rI2C_UINT64)
    test_uint64_t = *((uint64_t*)decParam.val);
  if (decParam.index == 6 && decParam.type == rI2C_FLOAT)
    test_float = *((float*)decParam.val);
  if (decParam.index == 7 && decParam.type == rI2C_DOUBLE)
    test_double = *((double*)decParam.val);*/

  if(decParam.index == 0 && decParam.type == rI2C_FLOAT)
    engineSV[0] = *((float*)decParam.val);
  if(decParam.index == 1 && decParam.type == rI2C_FLOAT)
    engineSV[1] = *((float*)decParam.val);
  if(decParam.index == 2 && decParam.type == rI2C_FLOAT)
    engineSV[2] = *((float*)decParam.val);
  if(decParam.index == 3 && decParam.type == rI2C_FLOAT)
    engineSV[3] = *((float*)decParam.val);
    
  if(decParam.index == 4 && decParam.type == rI2C_FLOAT)
    servoSV[0] = *((float*)decParam.val);
  if(decParam.index == 5 && decParam.type == rI2C_FLOAT)
    servoSV[1] = *((float*)decParam.val);
  if(decParam.index == 6 && decParam.type == rI2C_FLOAT)
    servoSV[2] = *((float*)decParam.val);
  if(decParam.index == 7 && decParam.type == rI2C_FLOAT)
    servoSV[3] = *((float*)decParam.val);
  if(decParam.index == 8 && decParam.type == rI2C_FLOAT)
    blinkRate = *((float*)decParam.val);
}

void ControlLoop(void)
{

  uint32_t beginM = micros();

  interrupts();

  /* Get a new sensor event */
  sensors_event_t event;

  bno.getEvent(&event);

  rI2CTX_beginFrame();

  rI2CTX_addParameter(1,event.orientation.x);
  rI2CTX_addParameter(2,event.orientation.y);
  rI2CTX_addParameter(3,event.orientation.z);

 // mma.read();
 // mma.getEvent(&event);
 // rI2CTX_addParameter(4,event.acceleration.x);
 // rI2CTX_addParameter(5,event.acceleration.y);
 // rI2CTX_addParameter(6,event.acceleration.z);


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

  rI2CTX_addParameter(49,lastLoopDuration);
  rI2CTX_addParameter(50,(int32_t)i);
  
  rI2CTX_endFrame();

  Wire.requestFrom(51, 100, I2C_STOP, 1000);
  uint8_t recvByte;
  
  while (Wire.available())
  {
    recvByte = Wire.readByte();
    //Serial.println(String(recvByte));
    rI2CRX_receiveBytes(&recvByte, 1);
  }

  Wire.beginTransmission(51);
  Wire.write(rI2CTX_buffer, rI2CTX_bufferPos);
  Wire.endTransmission(I2C_STOP,3000); //3 ms timeout, increase as needed for more parameters, need to move to DMA
  
  i++;
  blinkCount++;
  if(blinkCount == blinkRate)
    analogWrite(13, ADCmaxValue);
  if(blinkCount >= blinkRate*2)
  {
    analogWrite(13,0);
    blinkCount = 0;
  }

  lastLoopDuration = (micros() - beginM)/1000;
}

