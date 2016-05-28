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

float blinkRate;
int blinkCount;

//Define's ADC range based off of ADC resolution
int ADCmaxValue;

//Measure the control loop timing utilization
uint32_t duration;
uint32_t sensors;
uint32_t readingPi;
uint32_t processingPi;
uint32_t transmitting;
uint32_t lastMicros;

int bnoCalibration;



void recvParam(rI2CRX_decParam decParam);

void setup(void)
{


  pinMode(15,OUTPUT);
  digitalWrite(15,HIGH);

  //Iniitialize the I2C bus for the BNO sensor
  Wire1.begin(I2C_MASTER, 0, I2C_PINS_29_30, I2C_PULLUP_INT, I2C_RATE_100);

  int tries = 0;

  do{
    BNO_Error = 0;
  
    // Initialise the sensor
    if(!bno.begin(&Wire1))
    {
   // There was a problem detecting the BNO055 ... check your connections 
       BNO_Error = 1;
    }
  
    if(BNO_Error == 1){
      digitalWrite(15,LOW);
      delay(50); //Data sheet says at least 20ns
      digitalWrite(15,HIGH);
      delay(750); //Data sheet says 650 ms
    }

    tries++;
  }while(BNO_Error == 1 && tries <= 3);
    
  //Let the BNO get settled
  delay(2000);
  bno.setExtCrystalUse(true);

  //LED is using PWM from some old code, could be changed to a simple digital output
  pinMode(13,OUTPUT); //LED Pin
  analogWriteFrequency(5,490); //PWM output at 350 Hz
  analogWriteResolution(16); //PWM output at 16 bit resolution
  ADCmaxValue = 65535;

  //Open the I2C interface to the Pi
  Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_INT, I2C_RATE_1000, I2C_OP_MODE_DMA);
  Wire.resetBus();

  //Initialize the Pi to Teensy I2C data procotol and connect the callbacks
  rI2CRX_begin();
  rI2CRX_recvDecParamCB = &recvParam;
  rI2CRX_frameRXBeginCB = &gotAFrame;
  rI2CRX_frameRXEndCB = &endFrame;

  //See if the BNO is calibrated
  if(bno.readSensorOffsetsFromEEPROM(0))
    bnoCalibration = 1;
  else
    bnoCalibration = 0;

  //***************************
  //Initialize the rangefinder here
  //Probably send <esc> then commands to put it in continous sample mode
  //Could store the samples in a buffer via DMA or just let the control loop block
  //while it waits for a complete sample.
  //***************************

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
  //Don't put anything here
}

//A valid frame from the Pi has arrived
void gotAFrame()
{}

//The frame from the Pi has reached the end
void endFrame()
{}

//Called whenever a new
//parameter value is received from the
//Pi via the I2C bus
void recvParam(rI2CRX_decParam decParam)
{

/* Simpel example parameter
  if(decParam.index == 1 && decParam.type == rI2C_FLOAT)
    engineSV[0] = *((float*)decParam.val);
*/

  //Write BNO calibration to flash
  if(decParam.index == 19 && decParam.type == rI2C_FLOAT){
    if(bno.writeSensorOffsetsToEEPROM(0))
      bnoCalibration = 16;
    else
      bnoCalibration = 0;
    uint8_t system1, gyro, accel, mag;
    bno.getCalibration(&system1, &gyro, &accel, &mag);
    bnoCalibration += system1/3 + gyro/3*2 + accel/3 * 4 + mag/3 * 8;
  }

  //A simple test parameter that changes how fast the LED on the Teensy blinks
  if(decParam.index == 100 && decParam.type == rI2C_FLOAT)
    blinkRate = *((float*)decParam.val);
}

void ControlLoop(void)
{

  //Needed for i2c library
  interrupts();
  
  uint32_t beginM = micros();

  rI2CTX_beginFrame();

  //Timing stats on the control loop
  /*
  rI2CTX_addParameter(26,(int32_t)sensors);
  rI2CTX_addParameter(27,(int32_t)readingPi);
  rI2CTX_addParameter(28,(int32_t)processingPi);
  rI2CTX_addParameter(29,(int32_t)transmitting);
  rI2CTX_addParameter(30,(int32_t)duration);*/

  //Get the latest data from the BNO 9DOF
  sensors_event_t event;
  bno.getEvent(&event);

  //Add BNO data to be sent to the Pi
  rI2CTX_addParameter(30,event.orientation.x);
  rI2CTX_addParameter(31,event.orientation.y);
  rI2CTX_addParameter(32,event.orientation.z);
  rI2CTX_addParameter(49,(int32_t)bnoCalibration);


  float rangefinderDist = 0;
  //***************************
  //Process data from the rangefinder here
  //***************************
  rI2CTX_addParameter(10, rangefinderDist);
  
  rI2CTX_addParameter(50,(int32_t)i);
  
  rI2CTX_endFrame();

  sensors = micros() - beginM;
  readingPi = micros();

  //Request control data fromt the pi
  Wire.requestFrom(51, 100, 500);
  uint8_t recvByte;

  readingPi = micros() - readingPi;
  processingPi = micros();

  //Process any data sent over from the Pi
  while (Wire.available())
  {
    recvByte = Wire.readByte();
    rI2CRX_receiveBytes(&recvByte, 1);
  }

  processingPi = micros() - processingPi;

  //Send the log data to the Pi over I2C
  Wire.beginTransmission(51);
  Wire.write(rI2CTX_buffer, rI2CTX_bufferPos);
  
  transmitting = micros();
  //still a blocking call atm, will eventually done with nonblocking DMA transaction
  Wire.endTransmission(I2C_STOP, 3000);

  transmitting = micros() - transmitting;
  
  i++;

  //Blink the LED so we know the control loop is still alive
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

