#include <avr/io.h>
#include <avr/interrupt.h>
#include <rI2CTX.h>
#include <rI2CRX.h>
#include <i2c_t3.h>

uint16_t i = 0;

IntervalTimer controlTimer;

uint32_t lastMicros;

float blinkRate;
int blinkCount;


uint32_t duration;
uint32_t sensors;
uint32_t readingPi;
uint32_t processingPi;
uint32_t transmitting;

void recvParam(rI2CRX_decParam decParam);

void setup(void)
{

  //Open the I2C interface to the Pi
  Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_INT, I2C_RATE_1000, I2C_OP_MODE_DMA);
  Wire.resetBus();

  rI2CRX_begin();

  rI2CRX_recvDecParamCB = &recvParam;
  rI2CRX_frameRXBeginCB = &gotAFrame;
  rI2CRX_frameRXEndCB = &endFrame;
  
  pinMode(13,OUTPUT);
  blinkRate = 50;
  blinkCount = 0;

  //The I2C Library needs to be able to fire some interrupts
  //during our control loop to manage the start & stop
  //transactions in DMA mode.
  controlTimer.priority(200);

    //Setup the 100Hz control loop timer
  controlTimer.begin(ControlLoop, 10000);

}

void loop(void)
{

  }

void gotAFrame()
{}

void endFrame()
{
}

//Called whenever a new
//parameter value is received from the
//Pi via the I2C bus
void recvParam(rI2CRX_decParam decParam)
{
  if(decParam.index == 100 && decParam.type == rI2C_FLOAT)
    blinkRate = *((float*)decParam.val);
}

void ControlLoop(void)
{

  //Needed for i2c library
  interrupts();
  
  uint32_t beginM = micros();

  rI2CTX_beginFrame();
  
  /*

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
  rI2CTX_addParameter(25,(int32_t)engine[3]);*/

  
  rI2CTX_addParameter(50,(int32_t)i);
  
  rI2CTX_endFrame();

  sensors = micros() - beginM;


  readingPi = micros();

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
    digitalWrite(13, HIGH);
  if(blinkCount >= blinkRate*2)
  {
    digitalWrite(13,LOW);
    blinkCount = 0;
  }

  duration = micros() - beginM;

}

