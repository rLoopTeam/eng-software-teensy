#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "stepper_pins.h"

Adafruit_BNO055 bno = Adafruit_BNO055(55);

int i = 0;
int BNO_Error = 0;

IntervalTimer brakeTimer;
IntervalTimer gimbalTimer;
IntervalTimer datalogTimer;
elapsedMillis freeCPU;
elapsedMillis usedCPU;
int lastUsed = 0;

#define WDT_OUT 0

double Gimbal1_sp = 0;
double Gimbal2_sp = 0;
double Gimbal3_sp = 0;
double Gimbal4_sp = 0;
double LBrake_sp = 0;
double RBrake_sp = 0;

void setup(void) 
{

  int brakeStepperSpeed = 200; //We need to store & read this from flash
  int gimbalStepperSpeed = 200; //We need to store & read this from flash
  int datalogRate = 100;

  //Open the I2C interface to the Pi
  Wire.begin(I2C_MASTER,0,I2C_PINS_18_19,I2C_PULLUP_INT,I2C_RATE_1000);

  Wire.beginTransmission(51);
  Wire.print("Booting.");
  Wire.endTransmission();

  //Setup the timer for the steppers
  brakeTimer.begin(BrakeStepperLoop, 1000000/brakeStepperSpeed);
  gimbalTimer.begin(GimbalStepperLoop, 1000000/gimbalStepperSpeed);

  //Setup for the timer for the Pi data dump
  datalogTimer.begin(I2CLoop,1000000/datalogRate);
}

void loop(void) 
{
}

void BrakeStepperLoop()
{
}

void GimbalStepperLoop()
{
  
}

void I2CLoop(void)
{
  Wire.beginTransmission(51);

  Wire.print("Hello Pi. I'm stepping away here.\n");
  //Send out a pulse to the watchdog teensy
  
  
  Wire.endTransmission();
}

