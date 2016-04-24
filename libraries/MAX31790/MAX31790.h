#ifndef _MAX31790_h
#define _MAX31790_h
#define LIBRARY_VERSION	0.0.1


//Register Map
#define GLOBAL_CONFIG 	0x00
#define PWM_FREQ 		0x01
#define FAN_1_CONFIG 	0x02
#define FAN_2_CONFIG 	0x03
#define FAN_3_CONFIG 	0x04
#define FAN_4_CONFIG 	0x05
#define FAN_5_CONFIG 	0x06
#define FAN_6_CONFIG 	0x07
#define FAN_1_DYNAMICS 	0x08
#define FAN_2_DYNAMICS 	0x09
#define FAN_3_DYNAMICS 	0x0A
#define FAN_4_DYNAMICS 	0x0B
#define FAN_5_DYNAMICS 	0x0C
#define FAN_6_DYNAMICS 	0x0D

#define TACH1_COUNT		0x18
#define TACH2_COUNT		0x1A
#define TACH3_COUNT		0x1C
#define TACH4_COUNT		0x1E
#define TACH5_COUNT		0x20
#define TACH6_COUNT		0x22

#define TACH1_TARGET	0x50
#define TACH2_TARGET	0x52
#define TACH3_TARGET	0x54
#define TACH4_TARGET	0x56
#define TACH5_TARGET	0x58
#define TACH6_TARGET	0x5A




#define PWM_RATE_25		0b0000
#define PWM_RATE_30		0b0001
#define PWM_RATE_35		0b0010
#define PWM_RATE_100	0b0011
#define PWM_RATE_125	0b0100
#define PWM_RATE_149	0b0101
#define PWM_RATE_1250	0b0110
#define PWM_RATE_1470	0b0111
#define PWM_RATE_3570	0b1000
#define PWM_RATE_5000	0b1001
#define PWM_RATE_12500	0b1010
#define PWM_RATE_25000	0b1011



//Default Register Values
#define DEFAULT_FAN_CONFIG 	B10001000
#define DEFAULT_FAN_DYN 	B01001100



#define SR 4 //speed range
#define NP 2 //number of TACH pulse per revolution

#define MAX(a, b) (((a) > (b)) ? (a) : (b)) 
#define MIN(a, b) (((a) > (b)) ? (b) : (a))




class MAX31790
{
public:
	
	// Contructor
	MAX31790(int);
	
	initialize();
	
	
	// Get functions
	uint16_t getRPM(uint8_t);
	
	
	// Set functions
	void setRPM(uint16_t, uint8_t);
	
	void setPWMMode(uint8_t);
	void setRPMMode(uint8_t);

	
private:

	//variables
	uint8_t devAddr;
	uint8_t buffer[6];
	
	
	
	
	

}

#endif