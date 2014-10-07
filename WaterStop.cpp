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

#include "pin_macros.h"

#define BUTTON	B,PINB4,H
#define RELAY	B,PB3,H

#define	TICK					1UL
#define MS(MS_TIME)				((MS_TIME)/TICK)
#define S(S_TIME)				(S_TIME * (MS(1000)))

#define SHORTPRESS				MS(50)
#define	LONGPRESS				MS(2000)
#define LONGLONGPRESS			MS(2000)
#define MAX_WATER_TIME			S(90)			// water flow time hard limit

unsigned long relay_time_left = 0;
unsigned long button_pressed_time = 0;
unsigned long relay_time = S(5);
unsigned char relay_learning = 0;

int main(void)
{
	DRIVER(BUTTON,IN);
	DRIVER(BUTTON,PULLUP); 	// not for sensor

	DRIVER(RELAY,OUT);

	while (1) {
		if (ACTIVE(BUTTON)) {
			// Handle long press
			if ((button_pressed_time >= LONGPRESS) && (!relay_learning)) {
				ON(RELAY);
				relay_time_left = MAX_WATER_TIME;
				relay_learning = 1;
			} else {
				// Prevent counter overflow
				if (button_pressed_time < LONGLONGPRESS) button_pressed_time++;
			}
		} else {
			if ((button_pressed_time >= SHORTPRESS) && (button_pressed_time < LONGPRESS)) {
				if (relay_learning) {
					OFF(RELAY);
					relay_learning = 0;
					relay_time = MAX_WATER_TIME - relay_time_left;
				} else {
					ON(RELAY);
					relay_time_left = relay_time;
				}
			}
			button_pressed_time = 0;
		}

		if (relay_time_left) {
			relay_time_left--;
			if (!relay_time_left) {
				OFF(RELAY);
			}
		}

		_delay_ms(TICK);
	}

	return 0;
}
