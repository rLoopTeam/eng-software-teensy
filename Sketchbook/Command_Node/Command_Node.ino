
#include <i2c_t3.h>
#include "kinetis.h"
#include ContrastSensors.c"

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

double wheel1;
double wheel2;
double wheel3;
double wheel4;

//Using DMAs for ADC reading:https://community.freescale.com/docs/DOC-96507
//Use hardware averaging for accuracy

/*
 * DMA Map:
 * 0 - ADC 
 * 1 - ADC
 * 2 - ADC
 * 3 - Pulse Counter Port A
 * 4 - Pulse Counter Port B
 * 5 - I2C Read A
 * 6 - I2C Pushing A
 * 7 - CAN bus pushing
 */


//

double lastReflectDist = 0;

int i;

void setup(void) 
{
  //Open the I2C interface to the Pi
  Wire.begin(I2C_MASTER,0,I2C_PINS_18_19,I2C_PULLUP_INT,I2C_RATE_1000);
  //Wire1.begin(I2C_MASTER,0,I2C_PINS_26_31,I2C_PULLUP_INT,I2C_RATE_1000);

  Wire.beginTransmission(51);
  Wire.print("Booting.");
  Wire.endTransmission();

  analogReadResolution(14);

   SetupQuad();
  resetTrajectory();

  reflect12dist = .9144;
  reflect23dist = .9144;

  i = 0;

  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  

  attachInterrupt(digitalPinToInterrupt(10), fReflectISR, RISING );
  attachInterrupt(digitalPinToInterrupt(11), mReflectISR, RISING );
  attachInterrupt(digitalPinToInterrupt(12), rReflectISR, RISING );

  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  attachInterrupt(digitalPinToInterrupt(6), wheel1Pulse, RISING );
  attachInterrupt(digitalPinToInterrupt(7), wheel2Pulse, RISING );
  attachInterrupt(digitalPinToInterrupt(8), wheel3Pulse, RISING );
  attachInterrupt(digitalPinToInterrupt(9), wheel4Pulse, RISING );


  pinMode(14, INPUT);
  attachInterrupt(digitalPinToInterrupt(14), resetTrajectory, RISING );

  //Setup the 25Hz control loop timer
  controlTimer.begin(ControlLoop, 40000);
  
}

void resetTrajectory()
{
  fReflectHits = 0;
  mReflectHits = 0;
  rReflectHits = 0;
  currentTubePos = 15.24;
  currentSpeed = 0;
  currentTubeNum = 0;

  reflect1LastPos = 0;
  reflect2LastPos = 0;
  reflect3LastPos = 0;

  reflect2MissDetect=0;
  reflect3MissDetect=0;

  LPTMR0_CSR = 0b00100110; // disable
  LPTMR0_CSR = 0b00100111; // enable 

  wheel1 = 0;
  wheel2 = 0;
  wheel3 = 0;
  wheel4 = 0;

  FTM1_CNT = 0;
  FTM2_CNT = 0;
}

void wheel1Pulse()
{
  wheel1 += 1;
}

void wheel2Pulse()
{
  wheel2 += 1;
}

void wheel3Pulse()
{
  wheel3 += 1;
}

void wheel4Pulse()
{
  wheel4 += 1;
}

void nextTubeSection()
{
  lastReflectDist+=30.48;
  currentTubePos = lastReflectDist;
  currentTubeNum++;
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

  nextTubeSection();
  reflect1LastPos = currentTubePos;
  reflect1LastTime = stripeTime;
}

void mReflectISR()
{
  double stripeTime = micros();
  mReflectHits++;

//  double speedEstiamte;

 // if((currentTubePos - reflect2LastPos) < 30)
//  {
    //We've hit one of the markers
 //   hitMarker();
 //   return;
 // }

  currentSpeed = reflect12dist*1000000/(stripeTime-reflect1LastTime);
 // if(speedEstimate/currentSpeed < .2)
 //   currentSpeed = speedEstimate;

  reflect2LastPos = currentTubePos;
  reflect2LastTime = stripeTime;
}

void rReflectISR()
{
  double stripeTime = micros();
  rReflectHits++;

 // double speedEstimate;

  /*if((currentTubePos - reflect3LastPos) < 2)
  {
    //We've hit one of the markers
    hitMarker();
    return;
  }*/

  currentSpeed = reflect23dist*1000000/(stripeTime-reflect2LastTime);
 // if(speedEstimate/currentSpeed < .2)
  //  currentSpeed = speedEstimate;


  reflect3LastPos = currentTubePos;
  reflect3LastTime = stripeTime;
}

void loop(void) 
{
}

void SetupQuad()
{
  PORTB_PCR0 = PORT_PCR_MUX(6);
  PORTB_PCR1 =  PORT_PCR_MUX(6);
  FTM1_SC = FTM_SC_CLKS(1) || FTM_SC_PS(0);
  FTM1_CNTIN = 0;
  FTM1_CNT = 0;
  FTM1_MODE = FTM_MODE_WPDIS;
  FTM1_MODE = FTM_MODE_WPDIS || FTM_MODE_FTMEN;
  FTM1_C0SC = FTM_CSC_ELSA;
  FTM1_C1SC = FTM_CSC_ELSA;
  FTM1_MOD = 0xFFFF;
  FTM1_QDCTRL = FTM_QDCTRL_QUADMODE || FTM_QDCTRL_QUADEN;

  /*
  PORTB_PCR0 = PORT_PCR_MUX(6);
  PORTB_PCR1 =  PORT_PCR_MUX(6);
  FTM2_SC = FTM_SC_CLKS(1) || FTM_SC_PS(0);
  FTM2_CNTIN = 0;
  FTM2_CNT = 0;
  FTM2_MODE = FTM_MODE_WPDIS;
  FTM2_MODE = FTM_MODE_WPDIS || FTM_MODE_FTMEN;
  FTM2_C0SC = FTM_CSC_ELSA;
  FTM2_C1SC = FTM_CSC_ELSA;
  FTM2_MOD = 0xFFFF;
  FTM2_QDCTRL = FTM_QDCTRL_QUADMODE || FTM_QDCTRL_QUADEN;*/



  //Setup the low power timer
  SIM_SCGC5 |= SIM_SCGC5_LPTIMER;
  LPTMR0_CSR = 0;
  LPTMR0_PSR = 0b00000100; // bypass prescaler/filter
  LPTMR0_CMR = 0xFFFF;
  LPTMR0_CSR = 0b00100110; // counter, input=alt2, free running mode
  CORE_PIN13_CONFIG = PORT_PCR_MUX(3);
  LPTMR0_CSR = 0b00100111; // enable 


  //Pin 60 - Port D3
  PORTD_PCR3 = PORT_PCR_MUX(1) | PORT_PCR_IRQC(1);

  //DMA Timer Setup
  
}

//Exectuted every 40 ms
void ControlLoop(void)
{
  
  sei();

  while(Wire.read() != -1)
  {    }
  
  Wire.requestFrom(0x27,16,I2C_STOP);

  int32_t accel = Wire.read();
  accel *=256;
  accel += Wire.read();
  accel *=256;
  accel += Wire.read();
  accel *=256;
  accel += Wire.read();
  

  uint32_t vel = Wire.read();
  vel *=256;
  vel += Wire.read();
  vel *=256;
  vel += Wire.read();
  vel *=256;
  vel += Wire.read();


  double pos = Wire.read();
  pos *=256;
  pos += Wire.read();
  pos *=256;
  pos += Wire.read();
  pos *=256;
  pos += Wire.read();
  pos *=256;
  pos += Wire.read();
  pos *=256;
  pos += Wire.read();
  pos *=256;
  pos += Wire.read();
  pos *=256;
  pos += Wire.read();
  pos /= 1000000000;
  
    
  //Start a buffer to send data to the pi
  Wire.beginTransmission(51);

  currentTubePos += currentSpeed * 1/25;

  //Wire.print("CPU: ");
  //Wire.print(lastUsed);
  //Wire.print("/");
  //Wire.print(lastUsed+freeCPU);
  
  usedCPU = 0;
  i++;

  //Wire.print(" Seq: ");
  Wire.print(String(i));
  Wire.print(" ");  

  //Wire.print(" Time: ");
  Wire.print(String(micros()));
  Wire.print(" ");

  //Wire.print(" F: ");
  Wire.print(String(fReflectHits));
  Wire.print(" ");

  //Wire.print(" M: ");
  Wire.print(String(mReflectHits));
  Wire.print(" ");

  //Wire.print(" R: ");
  Wire.print(String(rReflectHits));
  Wire.print(" ");

  //Wire.print(" speed: ");
  Wire.print(String(currentSpeed));
  //Wire.print(" mph ");
  Wire.print(" ");

  //Wire.print(" position: ");
  Wire.print(String(currentTubePos));
  Wire.print(" ");

  Wire.print(" W1: ");
  Wire.print(String(wheel1));
  Wire.print(" ");

  Wire.print(" W2: ");
  Wire.print(String(wheel2));
  Wire.print(" ");

  Wire.print(" W3: ");
  Wire.print(String(wheel3));
  Wire.print(" ");

  Wire.print(" W4: ");
  Wire.print(String(wheel4));
  Wire.print(" ");

  Wire.print(" FPGA_ACCEL: ");
  Wire.print(String(((double)accel)/9800));
  Wire.print(" ");

  Wire.print(" FPGA_VEL: ");
  Wire.print(String(((double)vel)/1000000.0));
  Wire.print(" ");

  Wire.print(" FPGA_VEL_MPH: ");
  Wire.print(String(vel/447039.0));
  Wire.print(" ");


  Wire.print(" FPGA_POS: ");
  Wire.print(String(pos));

  Wire.print(" FTM1: ");
  Wire.print(String(FTM1_CNT));

  Wire.print(" LPTMR: ");
  LPTMR0_CNR=0;
  Wire.print(String(LPTMR0_CNR));

  Wire.print("\n");
 
  Wire.endTransmission();

  lastUsed = usedCPU;
  i++;
  freeCPU = 0;
}

