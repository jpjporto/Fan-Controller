#ifndef _MAX31790_h
#define _MAX31790_h
#define LIBRARY_VERSION	0.0.1


//Register Map
#define GLOBAL_CONFIG 		0x00
#define PWM_FREQ 			0x01
#define FAN_CONFIG(ch)		(0x02+(ch-1))
#define FAN_DYNAMICS(ch) 	(0x08+(ch-1))

#define TACH_COUNT(ch)		(0x18+(ch-1)*2)
#define PWM_DUTY_CYCLE(ch)		(0x30+(ch-1)*2)
#define PWMOUT(ch)			(0x40+(ch-1)*2)
#define TACH_TARGET(ch)		(0x50+(ch-1)*2)




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
	
	void initialize();
	
	
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