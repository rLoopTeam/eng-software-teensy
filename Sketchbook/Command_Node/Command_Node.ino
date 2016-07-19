
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

//Hover Engine Parameters
float engine1Temp, engine2Temp, engine3Temp, engine4Temp, engine5Temp, engine6Temp, engine7Temp, engine8Temp; //ADC or HE?
float engine1RPM, engine2RPM, engine3RPM, engine4RPM, engine5RPM, engine6RPM, engine7RPM, engine8RPM; //263 in RPM
float engine1Watts, engine2Watts, engine3Watts, engine4Watts, engine5Watts, engine6Watts, engine7Watts, engine8Watts; //268 in Watts
float engine1Current, engine2Current, engine3Current, engine4Current, engine5Current, engine6Current, engine8Current, engine9Current; //266 in Amps / 32
float engine1DSPTemp, engine2DSPTemp, engine3DSPTemp, engine4DSPTemp, engine5DSPTemp, engine6DSPTemp, engine7DSPTemp, engine8DSPTemp; //259 Temp in C
float engine1WaxTemp, engine2WaxTemp, engine3WaxTemp, engine4WaxTemp, engine5WaxTemp, engine6WaxTemp, engine7WaxTemp, engine8WaxTemp;
float engine1Volts, engine2Volts, engine3Volts, engine4Volts, engine5Volts, engine6Volts, engine7Volts, engine8Volts; //265 in volts / 32

//270 throttle voltage / 4096
//213 full throttle / 4096
//214 0 throttle voltage / 4096

float gimbal2Temp, gimbal3Temp, gimbal6Temp, gimbal7Temp;
float lEddyBrakeStepperTemp, rEddyBrakeStepperTemp;
float internalPressure1, internalPressure2;
uint8_t parkingBrake;
uint8_t chargerState;

uint8_t runMode;
//0 - Standby, manual control

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
  blinkRate = 15;
  blinkCount = 0;

  //The I2C Library needs to be able to fire some interrupts
  //during our control loop to manage the start & stop
  //transactions in DMA mode.
  controlTimer.priority(200);

    //Setup the 30Hz control loop timer
  controlTimer.begin(ControlLoop, 33333);

  //Start HE driver, TX control on pin 8, HE address 1
  setupHE(8, 1); 
  
  
  delay(2000);
  requestParam(263);
  checkResponse(10000);
  Serial.print(String(HE1lastParameterValue()));
  Serial.println(" RPM");
  
  requestParam(268);
  checkResponse(10000);
  Serial.print(String(HE1lastParameterValue()));
  Serial.println(" Watts");

  requestParam(266);
  checkResponse(10000);
  Serial.print(String(HE1lastParameterValue()/32.0));
  Serial.println(" Amps");

  requestParam(265);
  checkResponse(10000);
  Serial.print(String(HE1lastParameterValue()/32.0));
  Serial.println(" Volts");

  requestParam(259);
  checkResponse(10000);
  Serial.print(String(HE1lastParameterValue()));
  Serial.println(" C");

  requestParam(270);
  checkResponse(10000);
  Serial.print(String(HE1lastParameterValue()/4096.0));
  Serial.println(" Current Throttle");

  requestParam(213);
  checkResponse(10000);
  Serial.print(String(HE1lastParameterValue()/4096.0));
  Serial.println(" Max Throttle V");
  
  
  requestParam(214);
  checkResponse(10000);
  Serial.print(String(HE1lastParameterValue()/4096.0));
  Serial.println(" Min Throttle V");


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

  rI2CTX_addParameter(0,(int32_t)i);
  rI2CTX_addParameter(1,(double)((double)beginM/1000000));
  rI2CTX_addParameter(2,(uint8_t)runMode);

  rI2CTX_addParameter(3,(float)engine1Temp);
  rI2CTX_addParameter(4,(float)engine2Temp);
  rI2CTX_addParameter(5,(float)engine3Temp);
  rI2CTX_addParameter(6,(float)engine4Temp);
  rI2CTX_addParameter(7,(float)engine5Temp);
  rI2CTX_addParameter(8,(float)engine6Temp);
  rI2CTX_addParameter(9,(float)engine7Temp);
  rI2CTX_addParameter(10,(float)engine8Temp);
  
  rI2CTX_addParameter(11,(float)engine1DSPTemp);
  rI2CTX_addParameter(12,(float)engine2DSPTemp);
  rI2CTX_addParameter(13,(float)engine3DSPTemp);
  rI2CTX_addParameter(14,(float)engine4DSPTemp);
  rI2CTX_addParameter(15,(float)engine5DSPTemp);
  rI2CTX_addParameter(16,(float)engine6DSPTemp);
  rI2CTX_addParameter(17,(float)engine7DSPTemp);
  rI2CTX_addParameter(18,(float)engine8DSPTemp);
  
  rI2CTX_addParameter(19,(float)engine1WaxTemp);
  rI2CTX_addParameter(20,(float)engine2WaxTemp);
  rI2CTX_addParameter(21,(float)engine3WaxTemp);
  rI2CTX_addParameter(22,(float)engine4WaxTemp);
  rI2CTX_addParameter(23,(float)engine5WaxTemp);
  rI2CTX_addParameter(24,(float)engine6WaxTemp);
  rI2CTX_addParameter(25,(float)engine7WaxTemp);
  rI2CTX_addParameter(26,(float)engine8WaxTemp);
  
  rI2CTX_addParameter(27,(float)engine1Watts);
  rI2CTX_addParameter(28,(float)engine2Watts);
  rI2CTX_addParameter(29,(float)engine3Watts);
  rI2CTX_addParameter(30,(float)engine4Watts);
  rI2CTX_addParameter(31,(float)engine5Watts);
  rI2CTX_addParameter(32,(float)engine6Watts);
  rI2CTX_addParameter(33,(float)engine7Watts);
  rI2CTX_addParameter(34,(float)engine8Watts);
  
  rI2CTX_addParameter(35,(float)engine1Volts);
  rI2CTX_addParameter(36,(float)engine2Volts);
  rI2CTX_addParameter(37,(float)engine3Volts);
  rI2CTX_addParameter(38,(float)engine4Volts);
  rI2CTX_addParameter(39,(float)engine5Volts);
  rI2CTX_addParameter(40,(float)engine6Volts);
  rI2CTX_addParameter(41,(float)engine7Volts);
  rI2CTX_addParameter(42,(float)engine8Volts);

  rI2CTX_addParameter(43,(float)engine1RPM);
  rI2CTX_addParameter(44,(float)engine2RPM);
  rI2CTX_addParameter(45,(float)engine3RPM);
  rI2CTX_addParameter(46,(float)engine4RPM);
  rI2CTX_addParameter(47,(float)engine5RPM);
  rI2CTX_addParameter(48,(float)engine6RPM);
  rI2CTX_addParameter(49,(float)engine7RPM);
  rI2CTX_addParameter(50,(float)engine8RPM);
  
  rI2CTX_addParameter(51,(float)gimbal2Temp);  
  rI2CTX_addParameter(52,(float)gimbal3Temp);  
  rI2CTX_addParameter(53,(float)gimbal6Temp);  
  rI2CTX_addParameter(54,(float)gimbal7Temp);  

  rI2CTX_addParameter(55,(float)lEddyBrakeStepperTemp);  
  rI2CTX_addParameter(56,(float)rEddyBrakeStepperTemp);  

  rI2CTX_addParameter(57,(float)internalPressure1);  
  rI2CTX_addParameter(58,(float)internalPressure2);  

  rI2CTX_addParameter(59,(uint8_t)parkingBrake);  
  rI2CTX_addParameter(60,(uint8_t)chargerState);  
  
  rI2CTX_addParameter(100,(uint8_t)duration);  
  
  rI2CTX_endFrame();

  sensors = micros() - beginM;

  readingPi = micros();

  Wire.requestFrom(51, 200, 500);
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
  Wire.endTransmissionNB(I2C_STOP, 3000);

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

