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
#include <avr/eeprom.h>
//#include <util/delay.h>

#include "pin_macros.h"

#define BUTTON	B,PINB4,H
#define RELAY	B,PB3,H
#define LED		B,PB0,H
#define BUZZER	B,PB2,H

#define	RELAY_EEPROM_TIME_ADDR	4
#define	RELAY_MAX_TIME			120000
#define	RELAY_DEFAULT_TIME		5000	// how long relay must be on
#define	BUTTON_LONGPRESS_TIME	2000	// how long press button to start learning cycle

enum {
	TIMER_RELAY, TIMER_LED, TIMER_LONGPRESS, TIMER_BUTTON, // how often button pin change polled
	TIMER_MAX
};

enum sysstate_t {
	NORMAL, LEARNING
} sysstate = NORMAL;

// TODO: fix random bug caused systicks overflow
// 1024.0 / 1200000 = 853us @ 1.2MHz (OCR0A = 0)

typedef uint32_t systick_t;

systick_t systick;

systick_t timer_init[TIMER_MAX];
systick_t timer_value[TIMER_MAX];
uint8_t timer_f_int = 0;
uint8_t timer_f_en = 0;

systick_t relay_time = RELAY_DEFAULT_TIME;

ISR(TIM0_COMPA_vect) {

	systick++;

	uint8_t mask = 1;
	systick_t *tv = &timer_value[0];
	while(mask < _BV(TIMER_MAX)) {
		if (timer_f_en & mask) {
			if (!--(*tv)) {
				timer_f_int |= mask;
				timer_f_en &= ~mask;
			}
		}
		mask <<= 1;
	}
}
void _timer_stop(uint8_t timer) {
	timer_f_en &= ~_BV( timer );
	timer_f_int &= ~_BV( timer );
}

void _timer_setup(uint8_t timer, systick_t value) {
	_timer_stop(timer);
	timer_init[ timer ] = value;
}

void _timer_start(uint8_t timer) {
	timer_value[ timer ] = timer_init[ timer ];
	timer_f_int &= ~_BV( timer );
	timer_f_en |= _BV( timer );
}

#define TIMER_SETUP(KEY, VALUE)	_timer_setup(TIMER_##KEY, VALUE)
#define TIMER_START(KEY) _timer_start( TIMER_##KEY )
#define TIMER_STOP(KEY) _timer_stop( TIMER_##KEY )
#define	TIMER_HANDLE(KEY)	if (timer_f_int & _BV( TIMER_##KEY ))

void relay_on(systick_t timer = 0) {
	ON(RELAY);
	systick = 0; // if longpress will be detected, it will use systick
	TIMER_STOP(RELAY);
	TIMER_SETUP(RELAY, timer ? timer : relay_time);
	TIMER_START(RELAY);

	TIMER_START(LED);
	ON(LED);
}

void relay_off() {
	OFF(RELAY);

//	TIMER_STOP(LED);
//	OFF(LED);

	if (sysstate == LEARNING) {
		sysstate = NORMAL;
		if ((systick <= RELAY_MAX_TIME) && (systick > 2*BUTTON_LONGPRESS_TIME)) {
			relay_time = systick;
//			eeprom_write_byte( (uint8_t*) 0, 0x01);
//			eeprom_write_dword( (uint32_t*) RELAY_EEPROM_TIME_ADDR, relay_time);
		}
	}
}

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
		if (sysstate == NORMAL) {
			// trigger relay and it's timer on
			relay_on();
			TIMER_START(LONGPRESS); // start long press detection timer
		} else {
			relay_off();
		}
	} else {
		// button released - falling edge

		// TODO: detect DREBEZG by checking current long press timer value

		// cancel long press detection timer
		TIMER_STOP(LONGPRESS);
	}
}

int main(void) {

	// Init timer

	TCCR0A = _BV(WGM01);	// CTC, normal port operation, OC0[AB] disconnected.
	TCCR0B = _BV(CS02) | _BV(CS00); // CLKio/1024 (From prescaler)
	OCR0A = 0;						// Generate interrupt on every clock pulse
	TIMSK0 = _BV(OCIE0A);	// Enable Timer/Counter0 Interrupt on Compare Match
	sei();


//	if (eeprom_read_byte( (uint8_t*) 0 ) == 0x01) {
//		relay_time = eeprom_read_dword( (uint32_t*) RELAY_EEPROM_TIME_ADDR );
//	} else {
		relay_time = RELAY_DEFAULT_TIME;
//	}

	DRIVER(RELAY, OUT);
	DRIVER(RELAY, PULLUP);
	OFF(RELAY);

	DRIVER(LED, OUT);
	TIMER_SETUP(LED, 500);
	TIMER_START(LED);

	DRIVER(BUTTON, IN);
	TIMER_SETUP(LONGPRESS, BUTTON_LONGPRESS_TIME);	// long press detection time
	TIMER_SETUP(BUTTON, 10);					// poll button state every 10st
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
			relay_off();
		}
		TIMER_HANDLE(LED) {
			TOGGLE(LED);
			TIMER_START(LED);
		}
		TIMER_HANDLE(LONGPRESS) {
			TOGGLE(LED);
			relay_on(RELAY_MAX_TIME);
			sysstate = LEARNING;
		}
		//_delay_ms(7);
	} while (1);

	return 0;
}
