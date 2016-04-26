#include <i2c_t3.h>
#include "MAX31790.h"
#include "I2Cdev.h"

MAX31790 fan_board(0x40);

uint16_t fanspeed;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  fan_board.initialize();

  fan_board.setRPM(800, 1);
  fan_board.setRPMMode(1);

}

void loop() {

  fanspeed = fan_board.getRPM(1);

  Serial.println(fanspeed);
  delay(1000);
}
