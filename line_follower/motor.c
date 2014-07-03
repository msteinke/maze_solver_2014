/*
 * motor.c
 *
 * Created: 1/07/2014 4:52:17 p.m.
 *  Author: martin
 */ 


#include "motor.h"
#include "system.h"
//#include "pio.h"
#include <avr/io.h>

#define MOTOR_INIT_DUTY_CYCLE 0

// figure out which PWM register goes
// with which assigned pin. 
#if MOTOR_PIN_L_FWD == PB7
    #define LEFT_FWD_DUTY OCR0A
#elif MOTOR_PIN_L_FWD == PD0
    #define LEFT_FWD_DUTY OCR0B
#elif MOTOR_PIN_L_FWD == PC6
    #define LEFT_FWD_DUTY OCR1A
#elif MOTOR_PIN_L_FWD == PC5
    #define LEFT_FWD_DUTY OCR1B
#else 
    #error "LEFT_FWD_DUTY is undefined"
#endif

#if MOTOR_PIN_L_RVSE == PB7
    #define LEFT_RVSE_DUTY OCR0A
#elif MOTOR_PIN_L_RVSE == PD0
    #define LEFT_RVSE_DUTY OCR0B
#elif MOTOR_PIN_L_RVSE == PC6
    #define LEFT_RVSE_DUTY OCR1A
#elif MOTOR_PIN_L_RVSE == PC5
    #define LEFT_RVSE_DUTY OCR1B
#else
    #error "LEFT_RVSE_DUTY is undefined"
#endif

#if MOTOR_PIN_R_FWD == PB7
    #define RIGHT_FWD_DUTY OCR0A
#elif MOTOR_PIN_R_FWD == PD0
    #define RIGHT_FWD_DUTY OCR0B
#elif MOTOR_PIN_R_FWD == PC6
    #define RIGHT_FWD_DUTY OCR1A
#elif MOTOR_PIN_R_FWD == PC5
    #define RIGHT_FWD_DUTY OCR1B
#else
    #error "RIGHT_FWD_DUTY is undefined"
#endif

#if MOTOR_PIN_R_RVSE == PB7
    #define RIGHT_RVSE_DUTY OCR0A
#elif MOTOR_PIN_R_RVSE == PD0
    #define RIGHT_RVSE_DUTY OCR0B
#elif MOTOR_PIN_R_RVSE == PC6
    #define RIGHT_RVSE_DUTY OCR1A
#elif MOTOR_PIN_R_RVSE == PC5
    #define RIGHT_RVSE_DUTY OCR1B
#else
    #error "RIGHT_RVSE_DUTY is undefined"
#endif




/** Initialize PWM channels on timer0;
	PB7 and PD0.
	@param none
	@return none */
void motor_init_timer0(void)
{
	// Configure PWM.    i/o pin:
	DDRB |= (1<<PB7); // OC.0A
	DDRD |= (1<<PD0); // OC.0B
	
	TCCR0A = (1<<COM0A1) |
			(0<<COM0A0) |
			(1<<COM0B1) |
			(0<<COM0B0) |
			(0<<WGM01) | // PWM phase correct mode 5
			(1<<WGM00);  // PWM phase correct mode 5
	
	TCCR0B = //(0<<FOC0A) | // must disable these for PWM
			//(0<<FOC0B) | // must disable these for PWM
			(0<<WGM02) | // PWM phase correct mode 5
			(0<<CS02) | // divide clock by 64
			(1<<CS01) | // divide clock by 64
			(1<<CS00); //divide clock by 64
	
	OCR0A = MOTOR_INIT_DUTY_CYCLE; // PWM duty cycle on PB7 %85
	OCR0B = MOTOR_INIT_DUTY_CYCLE; // PWM duty cycle on PD0 %170
	
	TIMSK0 = 0x00; // disable interrupts
	//TIMSK0 = 0x03; // enable interrupts
	
}
/** Initialize PWM channels on timer1;
	PC5 and PC6.
	@param none
	@return none */
void motor_init_timer1(void)
{
	// Configure PWM 
	// pins: OC.1A & OC.1B
	DDRC |= (1<<PC6) | (1<<PC5); 
	
	TCCR0A = (1<<COM0A1) |
			(0<<COM0A0) |
			(1<<COM0B1) |
			(0<<COM0B0) |
			//(1<<WGM02) |  // PWM phase correct mode 5
			(0<<WGM01) | // PWM phase correct mode 5
			(1<<WGM00);  // PWM phase correct mode 5
	
	TCCR0B = //(0<<FOC0A) | // must disable these for PWM
			//(0<<FOC0B) | // must disable these for PWM
			(0<<WGM02) | // PWM phase correct mode 5
			(0<<CS02) | // divide clock by 64
			(1<<CS01) | // divide clock by 64
			(1<<CS00); //divide clock by 64
	
	OCR0A = MOTOR_INIT_DUTY_CYCLE; // PWM duty cycle on PB7 %85
	OCR0B = MOTOR_INIT_DUTY_CYCLE; // PWM duty cycle on PD0 %170
	
	TIMSK0 = 0x00; // disable interrupts
	//TIMSK0 = 0x03; // enable interrupts
	
}

/** Initialize motor PWM timers
	@param none
	@return none */
void motor_init(void)
{
	motor_init_timer0();
	//motor_init_timer1();
}


/** Control each motor from -127 to +127.
	@param left_speed
	@param right_speed */
void motor_set(short left_speed, short right_speed)
{
	// PWM register goes from 0 to 255
	// but I want to use a 'short' to save memory
	// and for simplicity.
	
	
	// set left motor speed
	if (left_speed > 0)
	{
		LEFT_FWD_DUTY = left_speed*2;
		LEFT_RVSE_DUTY = 0;
	}
	else if (left_speed < 0)
	{
		LEFT_FWD_DUTY = 0;
		LEFT_RVSE_DUTY = -left_speed*2;
	}
	else
	{
		LEFT_FWD_DUTY = 0;
		LEFT_RVSE_DUTY = 0;
	}
	
	// set right motor speed
	if (right_speed > 0)
	{
		RIGHT_FWD_DUTY = right_speed*2;
		RIGHT_RVSE_DUTY = 0;
	}
	else if (right_speed < 0)
	{
		RIGHT_FWD_DUTY = 0;
		RIGHT_RVSE_DUTY = -right_speed*2;
	}
	else
	{
		RIGHT_FWD_DUTY = 0;
		RIGHT_RVSE_DUTY = 0;
	}	
}

/** calls motor_set and sets each motor speed
	to zero.
	@param none
	@return none */
void motor_stop(void)
{
	motor_set(0,0);
}

/*
The Timer/Counter (TCNT0) and Output Compare Registers (OCR0A and OCR0B) are 8-bit
registers.

TCCR0A - Timer/Counter Control Register A
TCCR0B - Timer/Counter Control Register B
TCNT0 - Timer/Counter Register
OCR0A - Output Compare Register A
OCR0B - Output Compare Register B
TIMSK0 - Timer/Counter Interrupt Mask Register
TIFR0 - Timer/Counter 0 Interrupt Flag Register

http://extremeelectronics.co.in/avr-tutorials/pwm-signal-generation-by-using-avr-timers-part-ii/


OC.0A - PB7*
OC.0B - PD0
OC.1A - PC6
OC.1B - PC5

OC.1C - PB7*

*/