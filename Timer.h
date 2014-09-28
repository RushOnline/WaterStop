/*
 * Timer.h
 *
 *  Created on: 28 сент. 2014 г.
 *      Author: rush
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "Debug.h"

class Timer {
	unsigned long _stop;

public:
	class Handler {
	public:
		virtual void on_timeover() = 0;
	};

	Timer( Handler &handler ) :_handler(handler) {
		_stop = 0;
	}

	void setup() {
		DBG("timer: setup completed");
	}

	void update() {
		if (not _stop) return;
		if (millis() >= _stop) {
			DBG("timer: call on_timeover callback");
			_stop = 0;
			_handler.on_timeover();
		}
	}

	void pulse( unsigned long ms ) {
		DBG("timer: start pulse %lums", ms);
		_stop = millis() + ms;
	}

	bool active() {
		return (_stop);
	}

	void cancel() {
		_stop = 0;
	}
private:
	Handler &_handler;
};

#endif /* TIMER_H_ */
