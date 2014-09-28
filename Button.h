/*
 * Button.h
 *
 *  Created on: 28 сент. 2014 г.
 *      Author: rush
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "Debug.h"

class Button {
	int _on;
	int _pin;
	bool _was_pressed;
	unsigned long _ms;
	unsigned long _longpress;

	int _read_state() {
		return (digitalRead(_pin) == _on);
	}

public:

	class Handler {
	public:
		virtual void on_press() = 0;
		virtual void on_longpress() = 0;
		virtual void setup() = 0;
		virtual void update() = 0;
	};
	typedef void (Handler::*Callback)();

	Button(int pin /* button pin number */, int on /* pin state when button is pressed */, Handler &handler ) : _handler(handler) {
		_pin = pin;
		_on = on;
		_was_pressed = false;
		_longpress = 500; // TODO: remove hardcode
	}

	void setup() {
		_handler.setup();

		pinMode(_pin, INPUT);

		DBG("button: setup completed");
	}

	void update() {
		_handler.update();

		bool pressed = _read_state();

		// button still released
		if (not pressed and not _was_pressed) return;

		unsigned long ms = millis();

		// press
		if (pressed and not _was_pressed) {
			DBG("button: pin active");
			_ms = ms;
		}

		// hold
		if (pressed and _was_pressed) {
			if ( _ms && ((ms - _ms) > _longpress)) {
				DBG("button: call on_longpress handler callback");
				_ms = 0;
				_handler.on_longpress();
			}
		}

		// release
		if (not pressed and _was_pressed) {
			DBG("button: pin inactive");
			if (_ms) {
				DBG("button: call on_press handler callback");
				_handler.on_press();
			}
		}

		_was_pressed = pressed;

	}
private:
	Handler &_handler;
};

#endif /* BUTTON_H_ */
