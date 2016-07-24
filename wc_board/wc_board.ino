/*************************************************
 *	
 *	Watercooling Board Control Software
 *	For use with watercooling board available at https://github.com/jpjporto/Fan-Controller
 *
 *	Latest update on 07/24/2016 by Joao Paulo Porto <jpjporto(at)gmail.com>
 *
 *	Important Notice: This code has been stably running on my loop for at least a month (up to 14hours).
 *		However, I make no guarantees about it.
 *
 *	If you think this code is cool, feel free to use or modify it. However, it would be nice of you to
 *	let me know and give proper attribution back.
 *
 *	Changelog:
 *		v0.1.0 - First release.
 *
 *************************************************/


#include <i2c_t3.h>
#include "MAX31790.h"
#include "I2Cdev.h"
#include <PID_v1.h>

//Pin definitions
#define TEMP1 A1
#define TEMP2 A2
#define TEMP3 A3
#define TEMP4 A6
#define TEMP5 A7
#define TEMP6 A8
#define TEMP_EXT A10 //external temp
#define FLOW  14
#define FULL_SPEED  2
#define UV_PIN 3
#define G_PIN 4
#define R_PIN 5
#define B_PIN 6
#define ESP_RST 23

//System defs
#define NUM_RADS 2
#define NUM_FANS 5
#define NUM_TEMP 5 //rads*2 + 1

//MAX31790 fan_board(B0100000);
MAX31790 fan_board(0x40 >> 1);
MAX31790 fan_board2(0x46 >> 1);

char incomingByte = 0;

const int ANALOG_READ_RESOLUTION = 10; // Bits of resolution for the ADC.
const int ANALOG_READ_AVERAGING = 4;  // Number of samples to average with each ADC reading.

//Temp sensor values
uint16_t _nominalResistance = 10000;
uint16_t _seriesResistance = 10000;
uint16_t _betaCoefficient = 3950;
uint8_t _nominalTemperature = 25;

elapsedMillis flow_dt, flow_sample, temp_sample, fan_sample;
unsigned long timenow, lastPump, lastTemp;

uint32_t flow_read = 0;
uint8_t flow_count = 0;
double FlowMeas, prevFlowMeas, favg;
double FlowFreq;

double Setpoint = 1; //Desired flow = 1 gpm
double Output = 480; //Pump starting rate;

PID pumpPID(&FlowMeas, &Output, &Setpoint, 2, 6, 0.5, DIRECT);

//Sample Times
uint32_t Ts_pump = 500; // Flow sensor sample time, 0.5 seconds
uint32_t Ts_temp = 10;  // Get new temp data every 10msec
uint32_t Ts_fan  = 1000; // Check temperature readings and update fan speeds every 1 second

float rad_input[NUM_RADS], rad_output[NUM_RADS], ext_temp, rad_avg;
uint16_t fanspeed[NUM_FANS];
uint16_t pumpspeed;

bool temp_flag = 0;
uint8_t tempCount = 0;
uint32_t tempSum[NUM_TEMP];
float tempAvg[NUM_TEMP];

int MSI_cpu_temp, MSI_gpu_temp;
bool MSI_data = false;

// Fan curve settings
// please modify this to desired numbers.
uint8_t fan_dT[]  = {5,  10, 15, 20, 25, 30};
uint8_t fan_PWM[] = {40, 60, 70, 80, 90, 100};
float dT;
uint8_t steps = 6;
uint16_t fanPWMTarget;

//Struct for rgb strings
struct COLOR {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

COLOR default_color = {0, 0, 50};

void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial2.setTimeout(100);

  pinMode(FLOW, INPUT);
  pinMode(TEMP1, INPUT);
  pinMode(TEMP2, INPUT);
  pinMode(TEMP3, INPUT);
  pinMode(TEMP4, INPUT);
  pinMode(TEMP5, INPUT);
  pinMode(TEMP6, INPUT);
  pinMode(TEMP_EXT, INPUT);

  pinMode(UV_PIN, OUTPUT);
  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
  pinMode(FULL_SPEED, OUTPUT);
  //pinMode(ESP_RST, OUTPUT);

  analogReadResolution(ANALOG_READ_RESOLUTION);
  analogReadAveraging(ANALOG_READ_AVERAGING);

  digitalWrite(FULL_SPEED, HIGH);
  //digitalWrite(ESP_RST, LOW);

  setUV(200);
  setRGB(default_color);


  fan_board.initialize();
  fan_board2.initialize();

  pumpPID.SetOutputLimits(0, 511);
  pumpPID.SetSampleTime(Ts_pump);
  //pumpPID.SetMode(AUTOMATIC); //Uncomment this if you want the pump rate to match the desired Setpoint above

  flow_dt = 0;
  attachInterrupt(FLOW, flowISR, RISING);
}

void loop() {
  if (flow_sample > Ts_pump)
  {
    flow_sample = 0;
    //Serial.println(flow_count);
    FlowFreq = 1000.*flow_count / flow_read;
    flow_read = 0;
    flow_count = 0;
    FlowMeas = FlowFreq * 0.307 * 0.264172; //flow in GPM, values from INS-FM17N datasheet
    //favg = 0.5 * FlowMeas + 0.5 * prevFlowMeas;
    //prevFlowMeas = FlowMeas;
    pumpPID.Compute();
    setPumpPWM(Output);
    //Serial.printf("flow: %f gpm\n", FlowMeas);
  }

  /* This may look confusing, but the idea is to get an average of the temperature readings
     right before we update the fan speeds.
  */
  if ((temp_flag == 0) && (fan_sample > 750) && (temp_sample > Ts_temp) )
  {
    readTemps();
    temp_sample = 0;
  }

  if (fan_sample > Ts_fan)
  {
    dT = rad_avg - ext_temp;
    float interp_result = interpolate(dT, fan_dT, fan_PWM, steps);
    fanPWMTarget = interp_result * 5.11; //equivalent to map(interp_result,0,100,0,511)
    setAllPWM(fanPWMTarget);
    fan_sample = 0;
    temp_flag = 0;
    getAllRPM();
    //Serial.printf("dT: %f; rad_avg: %f, ext: %f, PWM: %f\n", dT, rad_avg, ext_temp, interp_result);
    //Serial.printf("Rad in: %f, Rad out: %f\n", rad_input[0], rad_output[0]);
    //Serial.println(pumpspeed);
    //Serial.println(fanspeed[0]);
  }

  if (Serial2.available()) // Check if the ESP8266 board sent new information
  {
    incomingByte = Serial2.read();
    if (incomingByte == 'C')
    {
      MSI_cpu_temp = Serial2.parseInt();
      MSI_gpu_temp = Serial2.parseInt();
      MSI_data = true;
    }

  }


}

uint32_t flow_temp;
void flowISR()
{
  flow_temp = flow_dt;
  if (flow_temp > 5) // added a small window to reduce tachometer bounce
  {
    flow_read += flow_dt;
    flow_dt = 0;
    flow_count++;
  }
}

float getTemp(float pin_value)
{
  float pin_volt = _seriesResistance * ((pow(2.0, ANALOG_READ_RESOLUTION) - 1) / pin_value - 1);
  float steinhart = (log(pin_volt / _nominalResistance)) / _betaCoefficient;
  steinhart += 1.0 / (_nominalTemperature + 273.15);
  steinhart = 1.0 / steinhart; // invert
  steinhart -= 273.15; // convert to celsius
  return steinhart;
}

/* Read temperature from sensors. My system only uses 5 sensors. */
void readTemps()
{
  tempSum[0] += analogRead(TEMP1);
  tempSum[1] += analogRead(TEMP2);
  tempSum[2] += analogRead(TEMP3);
  tempSum[3] += analogRead(TEMP4);
  tempSum[4] += analogRead(TEMP_EXT);
  //tempSum[5] += analogRead(TEMP6);
  //tempSum[6] += analogRead(TEMP7);
  tempCount++;
  if (tempCount == 10)
  {
    for (int j = 0; j < NUM_TEMP; j++)
    {
      tempAvg[j] = tempSum[j] / 10.;
      tempSum[j] = 0;
    }
    for (int i = 0; i < NUM_RADS; i++)
    {
      rad_input[i] = getTemp(tempAvg[2 * i]);
      rad_output[i] = getTemp(tempAvg[2 * i + 1]);
    }
    ext_temp = getTemp(tempAvg[4]); //2*NUM_RADS+1
    rad_avg = (rad_input[0] + rad_input[1] + rad_output[0] + rad_output[1]) / 4;
    tempCount = 0;
    temp_flag = 1;
  }
}

/* Set the PWM for all fans */
void setAllPWM(uint16_t pwm)
{
  for (int i = 1; i <= 6; i++)
  {
    fan_board.setPWM(pwm, i);
  }

  //#if NUM_FANS == 7
  fan_board2.setPWM(pwm, 1);
  //#endif
}

void setPumpPWM(uint16_t pwm)
{
  fan_board2.setPWM(pwm, 6);
}

void getAllRPM()
{
  for (int i = 0; i < 6; i++)
  {
    fanspeed[i] = fan_board.getRPM(i + 1);
  }
  //#if NUM_FANS == 7
  fan_board2.getRPM(1);
  //#endif

  pumpspeed = fan_board2.getRPM(6);
}

void setRGB(struct COLOR CC) {
  analogWrite(R_PIN, CC.r);
  analogWrite(G_PIN, CC.g);
  analogWrite(B_PIN, CC.b);
}
void setUV(uint8_t bright) {
  analogWrite(UV_PIN, bright);
}

float interpolate(float dT, uint8_t Temps[], uint8_t PWMs[], uint8_t N) {
  int i = 0;
  if (dT < Temps[0]) {
    return PWMs[0];
  } else if (dT > Temps[N - 1]) {
    return PWMs[N - 1];
  } else {
    while (dT > Temps[i]) {
      i++;
    }
    float slope = ( PWMs[i] - PWMs[i - 1]) / (float)(Temps[i] - Temps[i - 1]);
    return (slope * (dT - Temps[i - 1]) + PWMs[i - 1]);
  }
}

