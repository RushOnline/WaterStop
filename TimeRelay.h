/*
 * TimeRelay.h
 *
 *  Created on: 28 сент. 2014 г.
 *      Author: rush
 */

#ifndef TIMERELAY_H_
#define TIMERELAY_H_

#include "Debug.h"
#include "Timer.h"
#include "Button.h"

class TimeRelay : public Button::Handler, public Timer::Handler {
	uint8_t 		_pin;
	uint8_t 		_on;
	Timer			_timer;
	unsigned long	_ms, _learn_ms;
public:
	TimeRelay(uint8_t pin, uint8_t active, unsigned long ms) : _timer(*this) {
		_pin = pin;
		_on = active;
		_ms = ms;
		_learn_ms = 0;
	}

	void setup() {
		pinMode(_pin, OUTPUT);
		off();
		_timer.setup();
		DBG("relay: setup completed");
	}

	void update() {
		_timer.update();
	}

	void on() {
		digitalWrite(_pin, _on );
		DBG("relay: on");
	}

	void on( long unsigned int ms ) {
		on();
		_timer.pulse( ms );
	}

	void off() {
		_timer.cancel();
		digitalWrite(_pin, (_on == HIGH) ? LOW : HIGH );
		DBG("relay: off");
	}

	void on_timeover() {
		off();
	}

	void on_press() {
		if (_learn_ms && _timer.active()) {
			save();
		} else {
			on(_ms);
		}
	}

	void learn() {
		learn( 60000 );
	}

	void learn( unsigned long ms_max ) {
		_learn_ms = millis();
		on( ms_max );
		DBG("relay: start learning (%lums max)", ms_max);
	}

	void save() {
		_ms = millis() - _learn_ms;
		_learn_ms = 0;
		off();
		DBG("relay: learning complete at %lums", _ms);
	}

	void on_longpress() {
		learn();
	}

};

#endif /* TIMERELAY_H_ */
