#include <i2c_t3.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//Actual values sent out the port
int engine[4];

//Control values with range 0 - 1
float engineSV [4];
//maximum values for the engines
float engine_max;
//Define's ADC range based off of ADC resolution
int ADCmaxValue;

void updateOuputs()
{
  for(int i = 0;i<4;i++){
    if(engineSV[i] > 1)engineSV[i]= 1;
    if(engineSV[i] < 0) engineSV[i] = 0;
    engine[i] = (engineSV[i] * .49 + .490) * ADCmaxValue;
  }
    
  analogWrite(23,engine[0]);
  analogWrite(22,engine[1]);
  analogWrite(21,engine[2]);
  analogWrite(20,engine[3]);
}

void setup(void)
{
  pinMode(23,OUTPUT);
  pinMode(22,OUTPUT);
  pinMode(21,OUTPUT);
  pinMode(20,OUTPUT);

  pinMode(13,OUTPUT);

  analogWriteFrequency(5,490); //PWM output at 350 Hz
  analogWriteResolution(16); //PWM output at 16 bit resolution
  ADCmaxValue = 65535;
  for(int i =0;i<4;i++){
    engineSV[i] = 0;
  }

  //Hold throttle down for 2 seconds then startup engines
  updateOuputs();

  engineSV[0] = .8;
  engineSV[1] = .8;
  engineSV[2] = .8;
  engineSV[3] = .8;

  updateOuputs();

}

void loop(void)
{

  }



