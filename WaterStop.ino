/*
 * main.c
 *
 *  Created on: 28 сент. 2014 г.
 *      Author: rush
 */

#define DEBUG
#define UART_SPEED		57600

#include "Debug.h"
#include "Timer.h"
#include "Button.h"
#include "TimeRelay.h"

#define PIN_BUTTON		10
#define PIN_RELAY   	13
#define DEFAULT_TIME	(1 * 1000)

TimeRelay relay( PIN_RELAY, HIGH, DEFAULT_TIME );
Button button(PIN_BUTTON, HIGH, relay);

void setup()
{
	Serial.begin(UART_SPEED);
		
	button.setup();
	
	DBG("main: setup complete");
}

void loop(void)
{
	button.update();
}
