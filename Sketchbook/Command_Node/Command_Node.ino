#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <FlexCAN.h>
#include <Metro.h>

IntervalTimer controlTimer;
elapsedMillis freeCPU;
elapsedMillis usedCPU;
int lastUsed = 0;

int fReflectHits;
int mReflectHits;
int rReflectHits;
int reflect2MissDetect;
int reflect3MissDetect;
double currentTubePos;
double currentSpeed;
double currentTubeNum;
double reflect1LastPos;
double reflect2LastPos;
double reflect3LastPos;
double reflect1LastTime;
double reflect2LastTime;
double reflect3LastTime;
double reflect12dist;
double reflect23dist;


int i;

void setup(void) 
{
  //Open the I2C interface to the Pi
  Wire.begin(I2C_MASTER,0,I2C_PINS_18_19,I2C_PULLUP_INT,I2C_RATE_1000);

  Wire.beginTransmission(51);
  Wire.print("Booting.");
  Wire.endTransmission();

  analogReadResolution(14);

  fReflectHits = 0;
  mReflectHits = 0;
  rReflectHits = 0;
  currentTubePos = 0;
  currentSpeed = 0;
  currentTubeNum = 0;

  reflect1LastPos = 0;
  reflect2LastPos = 0;
  reflect3LastPos = 0;

  reflect2MissDetect=0;
  reflect3MissDetect=0;

  reflect12dist = .914;
  reflect23dist = .914;

  i = 0;

  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);

  attachInterrupt(digitalPinToInterrupt(10), fReflectISR, RISING );
  attachInterrupt(digitalPinToInterrupt(11), mReflectISR, RISING );
  attachInterrupt(digitalPinToInterrupt(12), rReflectISR, RISING );

  //Setup the 25Hz control loop timer
  controlTimer.begin(ControlLoop, 40000);
}

void hitMarker()
{

}

void fReflectISR()
{
  fReflectHits++;
  double stripeTime = micros();

  
  //if((currentTubePos - reflect1LastPos) < 30)
 // {
    //We've hit one of the markers
//    hitMarker();
 //   return;
//  }

  currentTubeNum++;
  reflect1LastPos = currentTubePos;
  reflect1LastTime = stripeTime;
}

void mReflectISR()
{
  mReflectHits++;
  double stripeTime = micros();
  double speedEstiamte;

 // if((currentTubePos - reflect2LastPos) < 30)
//  {
    //We've hit one of the markers
 //   hitMarker();
 //   return;
 // }

  currentSpeed = reflect12dist*1000000/(stripeTime-reflect1LastTime);
 // if(speedEstimate/currentSpeed < .2)
 //   currentSpeed = speedEstimate;

  currentTubeNum++;
  reflect2LastPos = currentTubePos;
  reflect2LastTime = stripeTime;
}

void rReflectISR()
{
  rReflectHits++;
  double stripeTime = micros();
  double speedEstimate;

//  if((currentTubePos - reflect3LastPos) < 30)
//  {
    //We've hit one of the markers
 //   hitMarker();
  //  return;
//  }

  currentSpeed = reflect23dist*1000000/(stripeTime-reflect2LastTime);
 // if(speedEstimate/currentSpeed < .2)
  //  currentSpeed = speedEstimate;

  currentTubeNum++;
  reflect3LastPos = currentTubePos;
  reflect3LastTime = stripeTime;
}

void loop(void) 
{
}

//Exectuted every 40 ms
void ControlLoop(void)
{
  
  sei();
  
  //Start a buffer to send data to the pi
  Wire.beginTransmission(51);

  currentTubePos += currentSpeed * 1/25;

  Wire.print("CPU: ");
  Wire.print(lastUsed);
  Wire.print("/");
  Wire.print(lastUsed+freeCPU);
  
  usedCPU = 0;
  i++;

  Wire.print(" Seq: ");
  Wire.print(String(i));

/*
  int ana0 = analogRead(0);
  int ana1 = analogRead(1);
  int ana2 = analogRead(2);
  int ana3 = analogRead(3);
  int ana6 = analogRead(6);
  int ana7 = analogRead(7);
  int ana8 = analogRead(8);
  int ana9 = analogRead(9);
  int ana10 = analogRead(10);
  int ana11 = analogRead(11);
  int ana12 = analogRead(12);*/

  Wire.print(" F: ");
  Wire.print(String(fReflectHits));

  Wire.print(" M: ");
  Wire.print(String(mReflectHits));

  Wire.print(" R: ");
  Wire.print(String(rReflectHits));

  Wire.print(" speed: ");
  Wire.print(String(currentSpeed*2.2));
  Wire.print(" mph ");

   Wire.print(" position: ");
  Wire.print(String(currentTubePos));
/*
  Wire.print(" A0: ");
  Wire.print(String((double)ana0/4964.8484849));
  Wire.print("V");

  Wire.print(" A1: ");
  Wire.print(String((double)ana1/4964.8484849));

  Wire.print(" A2: ");
  Wire.print(String(ana2));

  Wire.print(" A3: ");
  Wire.print(String(ana3));

  Wire.print(" A6: ");
  Wire.print(String(ana6));

  Wire.print(" A7: ");
  Wire.print(String(ana7));

  Wire.print(" A8: ");
  Wire.print(String(ana8));

  Wire.print(" A9: ");
  Wire.print(String(ana9));

  Wire.print(" A10: ");
  Wire.print(String(ana10));

  Wire.print(" A11: ");
  Wire.print(String(ana11));

  Wire.print(" A12: ");
  Wire.print(String(ana12));*/

  Wire.print("\n");
 
  Wire.endTransmission();

  lastUsed = usedCPU;
  i++;
  freeCPU = 0;
}

