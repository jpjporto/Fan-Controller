/*************************************************
 *	
 *	MAX31790 Device Class
 *	Based on Maxim MAX31790 datasheet rev. 2, 01/2015
 *
 *	Latest update on 07/24/2016 by Joao Paulo Porto <jpjporto(at)gmail.com>
 *
 *	Changelog:
 *		v0.0.1 - initial release.
 *		v0.0.2 - Bug fixes.
 *		v0.1.0 - Code stable enough for me to trust it on my loop.
 *
 *************************************************/

#include <MAX31790.h>
#include "I2Cdev.h"


MAX31790::MAX31790(int address)
{
	devAddr = address;

}

void MAX31790::initialize()
{
	for(int i=1; i<=6; i++)
	{
		MAX31790::setRPM(2500, i);
		I2Cdev::writeBit(devAddr, FAN_CONFIG(i), 3, 1);
	}
	//MAX31790::setPWM(311, 1);
}

// Get Functions
uint16_t MAX31790::getRPM(uint8_t fan_num)
{
	buffer[0] = 0;
	buffer[1] = 0;
	//Reads two 
	I2Cdev::readBytes(devAddr, TACH_COUNT(fan_num), 2, buffer);
	uint16_t tach_out = ((((int16_t)buffer[0]) << 8) | buffer[1]) >> 5;
	
	//convert to RPM
	return 60*_SR*8192/(NP*tach_out);
}

uint16_t MAX31790::getRPMTarget(uint8_t fan_num)
{
	buffer[0] = 0;
	buffer[1] = 0;
	//Reads two 
	I2Cdev::readBytes(devAddr, TACH_TARGET(fan_num), 2, buffer);
	uint16_t tach_out = ((((int16_t)buffer[0]) << 8) | buffer[1]) >> 5;
	
	//convert to RPM
	return 60*_SR*8192/(NP*tach_out);
}





// Set Functions
void MAX31790::setRPM(uint16_t rpm, uint8_t fan_num)
{
	uint16_t tach_count = 60*_SR*8192/(NP*rpm)<<5;
	if(rpm == 0)
	{
		tach_count = 0b1111111111111111;
	}
	buffer[0] = tach_count >> 8;
	buffer[1] = tach_count;
	
	I2Cdev::writeBytes(devAddr, TACH_TARGET(fan_num), 2, buffer);
	
}

void MAX31790::setPWM(uint16_t pwm, uint8_t fan_num)
{
	uint16_t pwm_bit = pwm<<7;
	buffer[0] = pwm_bit >> 8;
	buffer[1] = pwm_bit;
	
	I2Cdev::writeBytes(devAddr, PWMOUT(fan_num), 2, buffer);
	
}

void MAX31790::setRPMMode(uint8_t fan_num)
{
	I2Cdev::writeBit(devAddr, FAN_CONFIG(fan_num), 7, 1);
}

void MAX31790::setPWMMode(uint8_t fan_num)
{
	I2Cdev::writeBit(devAddr, FAN_CONFIG(fan_num), 7, 0);
}
