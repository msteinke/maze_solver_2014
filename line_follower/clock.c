/*
 * clock.c
 *
 * Created: 6/07/2014 12:16:38 p.m.
 *  Author: martin
 */ 

#include "clock.h"
#include "system.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Global variable to hold the time
 max size is 2^32 - 1 = 4294967295 ms
                      = 4294967 s
					  = 71582 minutes
					  = 1193 hours
					  = 49 days.
 */
static volatile unsigned long g_time_ms;

/** Interrupt service routine that is called
	every time OCR1A matches the value of timer1.*/
ISR(TIMER1_COMPA_vect)
{
	g_time_ms++;
	//PORTB ^= (1<<7);
	//TIFR1 |= (0<<OCF1A);
}

/** Sets the time to a desired time.
	@param time
	@return none */
void clock_set_ms(unsigned long time)
{
	//cli();
	clock_disable_interrupt();
	g_time_ms = time;
	clock_enable_interrupt();
	//sei();
	
}

/** Initialize timer1 registers
	You must enable global interrupts
	to use this module.
	@param none
	@return none */
void clock_init(void)
{
	
	// Configure PWM timer1
	DDRC |= (1<<6) | (1<<5); 
	
	TCCR1A = (1<<COM1A1) |
			(0<<COM1A0) |
			(0<<COM1B1) |
			(0<<COM1B0) |
			(0<<COM1C1) |
			(0<<COM1C0) |
			(0<<WGM11) | // CTC mode 4
			(0<<WGM10);  // CTC mode 4
	
	TCCR1B = //(0<<FOC0A) | // must disable these for PWM
			//(0<<FOC0B) | // must disable these for PWM
			(0<<WGM13) | // CTC mode 4
			(1<<WGM12) | // CTC mode 4
			(0<<CS12) | // divide clock by 64
			(1<<CS11) | // divide clock by 64
			(1<<CS10); //divide clock by 64
	
	//TIMSK1 = 0x00; // disable interrupts
	TIMSK1 = (0<<ICIE1) | // Timer/Countern, Input Capture Interrupt Enable
			(0<<OCIE1C) | // Timer/Countern, Output Compare C Match Interrupt Enable
			(0<<OCIE1B) | // Timer/Countern, Output Compare B Match Interrupt Enable
			(1<<OCIE1A) | // Timer/Countern, Output Compare A Match Interrupt Enable
			(0<<TOIE1); // Timer/Countern, Overflow Interrupt Enable
	
	OCR1A = CLOCK_TIMER1_COMPARE_VALUE;
	//OCR1A = (unsigned short) (F_CPU / CLOCK_TIMER1_PRESCALER) / CLOCK_RATE_HZ;
	//OCR1A = 125;
	//OCR1AL = 125;
	
	// clear the time
	clock_set_ms(0);
	
}

/** Disable timer1 interrupts so that
	critical sections can be protected.
	@param none
	@return none*/
void clock_disable_interrupt(void)
{
	//TIMSK1 &= ~(1<<OCIE1A);
	TIMSK1 = 0x00; // this stop the time from changing completely!!?!
	//continue;
}

/** Enable timer1 interrupts so that
	critical sections can be protected.
	@param none
	@return none*/
void clock_enable_interrupt(void)
{
	TIMSK1 |= (1<<OCIE1A);
}

/** Returns the time, in milliseconds,
	that the system has been turned on.
	@param none
	@return time */
unsigned long clock_get_ms(void)
{
	//cli();
	clock_disable_interrupt();
	//return g_time_ms;
	unsigned long time = g_time_ms;
	clock_enable_interrupt();
	//sei();
	
	return time;
}


/** Simple test program to make sure that
	the clock module works. Place an LED
	on pin PB6. This should flash every second.
	@param none
	@return none */

void clock_test(void)
{
	volatile short i = 0;
	DDRB |= (1<<PB3) | (1<<PB4);
	clock_set_ms(0);
	
	while(1)
	{
		if( (clock_get_ms() % 500) == 0)
		{
			PORTB ^= (1<<PB4);
			
		}
		
		PORTB ^= (1<<PB3);
		// wait so that the LED doesn't toggle twice
		for(i = 0; i < 100;i++)
		{
			continue;
		}
	}
}
/*
from table 11-1, Reset and Interrupt Vectors:
16 $001E TIMER1 COMPA Timer/Counter1 Compare Match A


TIMSK1 - Timer/Counter1 Interrupt Mask Register
		ICIEn:  Timer/Countern, Input Capture Interrupt Enable
		OCIEnC: Timer/Countern, Output Compare C Match Interrupt Enable
		OCIEnB: Timer/Countern, Output Compare B Match Interrupt Enable
		OCIEnA: Timer/Countern, Output Compare A Match Interrupt Enable
		TOIEn:  Timer/Countern, Overflow Interrupt Enable
		
		
		
		
TIFR1 - Timer/Counter1 Interrupt Flag Register
		ICFn:  Timer/Countern, Input Capture Flag
		OCFnC: Timer/Countern, Output Compare C Match Flag
		OCFnB: Timer/Counter1, Output Compare B Match Flag
		OCF1A: Timer/Counter1, Output Compare A Match Flag
		TOVn:  Timer/Countern, Overflow Flag
		
*/