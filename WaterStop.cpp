/*
 * This file is part of the avr-gcc-examples project.
 *
 * Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <avr/io.h>
#define F_CPU 1200000UL
#include <util/delay.h>

#define PO_RELAY	(_BV(PB3))		// pin #2
#define PI_BUTTON	(_BV(PINB4))	// pin #3
#define PO_LED		(_BV(PB0))		// pin #5

#define button_pressed 			(!(PINB & PI_BUTTON))
#define relay_on				(PORTB |= PO_RELAY)
#define relay_off				(PORTB &= ~PO_RELAY)

#define led_on				(PORTB |= PO_LED)
#define led_off				(PORTB &= ~PO_LED)

#define	TICK					(10)				// 10ms loop
#define MS(MS_TIME)				((MS_TIME)/TICK)
#define S(S_TIME)				(S_TIME * (MS(1000)))

#define SHORTPRESS				MS(200)
#define	LONGPRESS				MS(1000)
#define MAXPRESS				MS(2000)
#define MAXWATER				S(90)			// water flow time hard limit

unsigned int relay_ticks_left = 0;
unsigned int button_ticks_pressed = 0;
unsigned int relay_ticks = S(5);
unsigned char relay_learning = 0;

int main(void)
{
	DDRB |= (PO_RELAY | PO_LED);
	PORTB |= (PI_BUTTON); // enable button pullup

	while (1) {
		if (button_pressed) {
			// Handle long press
			if ((button_ticks_pressed >= LONGPRESS) && (!relay_learning)) {
				relay_on;
				relay_ticks_left = MAXWATER;
				relay_learning = 1;
				led_on;
			} else {
				// Prevent counter overflow
				if (button_ticks_pressed < MAXPRESS) button_ticks_pressed++;
			}
		} else {
			if ((button_ticks_pressed >= SHORTPRESS) && (button_ticks_pressed < LONGPRESS)) {
				if (relay_learning) {
					relay_off;
					relay_learning = 0;
					relay_ticks = MAXWATER - relay_ticks_left;
					led_off;
				} else {
					relay_on;
					relay_ticks_left = relay_ticks;
				}
			}
			button_ticks_pressed = 0;
		}

		if (relay_ticks_left) {
			relay_ticks_left--;
			if (!relay_ticks_left) {
				relay_off;
			}
		}

		_delay_ms(TICK);
	}

	return 0;
}
