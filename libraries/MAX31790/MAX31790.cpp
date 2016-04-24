/*************************************************
 *	
 *	MAX31790 Device Class
 *	Based on Maxim MAX31790 datasheet rev. 2, 01/2015
 *
 *	Latest update on 04/04/2016 by Joao Paulo Porto <jpjporto(at)gmail.com>
 *
 *	Changelog:
 *		v0.0.1 - initial release
 *
 *************************************************/

#include <MAX31790.h>
#include "I2Cdev.h"


MAX31790::MAX31790(int address)
{
	devAddr = address;

}

// Get Functions
uint16_t getRPM(uint8_t fan_num)
{
	//Reads two 
	I2Cdev::readBytes(devAddr, TACH_COUNT(fan_num), 2, buffer);
	uint16_t tach_out = ((((int16_t)buffer[1]) << 8) | buffer[0]) >> 5;
	
	//convert to RPM
	return 60*SR*8192/(NP*tach_out);
}





// Set Functions
void setRPM(uint16_t rpm, uint8_t fan_num)
{
	uint16_t tach_count = 60*SR*8192/(NP*rpm);
	
	I2Cdev::writeBytes(devAddr, TACH_TARGET(fan_num), 2, &tach_count)
	
}

void setRPMMode(uint8_t fan_num)
{
	I2Cdev::writeBit(devAddr, FAN_CONFIG(fan_num), 7, 1);
}

void setPWMMode(uint8_t fan_num)
{
	I2Cdev::writeBit(devAddr, FAN_CONFIG(fan_num), 7, 0);
}