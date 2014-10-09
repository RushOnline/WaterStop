/*
 * attiny13a pinouts
 *                             /^\_/^\
 * (PCINT5/RESET/ADC0/dW) PB5 -|1   8|- VCC
 *     (PCINT3/CLKI/ADC3) PB3 -|2   7|- PB2 (SCK/ADC1/T0/PCINT2)
 *          (PCINT4/ADC2) PB4 -|3   6|- PB1 (MISO/AIN1/OC0B/INT0/PCINT1)
 *                        GND -|4   5|- PB0 (MOSI/AIN0/OC0A/PCINT0)
 *                             \_____/
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "pin_macros.h"

#define BUTTON	B,PINB4,H
#define RELAY	B,PB3,H
#define LED		B,PB0,H
#define BUZZER	B,PB2,H

#define	RELAY_DEFAULT_TIME	5000	// how long relay must be on

enum {
	TIMER_RELAY, TIMER_LED, TIMER_LONGPRESS, TIMER_BUTTON, // how often button pin change polled
	TIMER_MAX
};

//#define LONGPRESS	1500		// how many systicks must have passed to detect button long press
//#define T_SYSTICKS	uint32_t

// TODO: fix random bug caused systicks overflow
// 1024.0 / 1200000 = 853us @ 1.2MHz (OCR0A = 0)
// T_SYSTICKS __volatile__ systick = 0;

uint16_t timer_init[TIMER_MAX];
uint16_t timer_value[TIMER_MAX];
uint8_t __volatile__ timer_f_int = 0;
uint8_t __volatile__ timer_enable = 0;

ISR(TIM0_COMPA_vect) {

	// systick++;

	uint8_t mask = 1;
	for (uint8_t i = 0; i < TIMER_MAX; i++) {
		if (timer_enable & mask) {
			if (!--timer_value[i]) {
				timer_f_int |= mask;
				timer_enable &= ~mask;
			}
		}
		mask <<= 1;
	}
}

// setup/cancel timer
#define TIMER_SETUP(KEY, VALUE)	{ \
	timer_enable &= ~_BV( TIMER_##KEY ); \
	timer_f_int &= ~_BV( TIMER_##KEY ); \
	timer_init[ TIMER_##KEY ] = VALUE; \
	}

#define TIMER_START(KEY) { \
	timer_value[ TIMER_##KEY ] = timer_init[ TIMER_##KEY ]; \
	timer_f_int &= ~_BV( TIMER_##KEY ); \
	timer_enable |= _BV( TIMER_##KEY ); \
	}

#define TIMER_STOP(KEY) { \
	timer_f_int &= ~_BV( TIMER_##KEY ); \
	timer_enable &= ~_BV( TIMER_##KEY ); \
	}

#define	TIMER_HANDLE(KEY)	if (timer_f_int & _BV( TIMER_##KEY ))

// TODO: remake this with PC interrupt
void button_update() {

	static bool was_active = false;

	// detect rising and falling edge

	bool active = ACTIVE(BUTTON);

	// no pin change
	if (active == was_active)
		return;

	// record last state
	was_active = active;

	if (active) {
		// button pressed - rising edge
		// trigger relay and it's timer on
		ON(RELAY);
		ON(LED);
		TIMER_START(LED);
		TIMER_START(RELAY);
		TIMER_START(LONGPRESS); // start long press detection timer
	} else {
		// button released - falling edge

		// TODO: detect DREBEZG by checking current long press timer value

		// cancel long press detection timer
		TIMER_STOP(LONGPRESS);
	}
}

int main(void) {
	/*
	 * TCNT0: Timer/Counter Register (8-bit)
	 * OCR0A,OCR0B: Output Compare Registers (8-bit)
	 * TIFR0: Timer Interrupt Flag Register, to indicate interrupt request signals
	 * TIMSK0: Timer Interrupt Mask Register, to mask interrupts individually
	 *
	 * OCR0A: Output Compare 0 Register A, used to manipulate the counter resolution in Clear Timer on Compare (CTC) mode.
	 */

	TCCR0A = _BV(WGM01);	// CTC, normal port operation, OC0[AB] disconnected.
	TCCR0B = _BV(CS02) | _BV(CS00); // CLKio/1024 (From prescaler)
	OCR0A = 0;						// Generate interrupt on every clock pulse
	TIMSK0 = _BV(OCIE0A);	// Enable Timer/Counter0 Interrupt on Compare Match

	sei();
	// Enable interrupts

	DRIVER(RELAY, OUT);
	DRIVER(RELAY, PULLUP);
	OFF(RELAY);
	TIMER_SETUP(RELAY, RELAY_DEFAULT_TIME);

	DRIVER(LED, OUT);
	ON(LED);
	TIMER_SETUP(LED, 500);
	TIMER_START(LED);

	DRIVER(BUTTON, IN);
	TIMER_SETUP(BUTTON, 10);	// poll button state every 10st
	TIMER_START(BUTTON);

	/*
	 TODO: support buzzer
	 DRIVER(BUZZER, OUT);
	 OFF(BUZZER);
	 */

	do {
		TIMER_HANDLE(BUTTON) {
			button_update();
			TIMER_START(BUTTON);
		}
		TIMER_HANDLE(RELAY) {
			OFF(RELAY);
		}
		TIMER_HANDLE(LED) {
			TOGGLE(LED);
			TIMER_START(LED);
		}
		TIMER_HANDLE(LONGPRESS) {
			TIMER_STOP(LED);
		}
		_delay_ms(7);
	} while (1);

	return 0;
}
