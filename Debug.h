/*
 * Debug.h
 *
 *  Created on: 28 сент. 2014 г.
 *      Author: rush
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef DEBUG

void DBG(const char *fmt, ...) {
	va_list ap;
	char buf[128];

	snprintf(buf, sizeof(buf), "[%10lu] ", millis());
	Serial.print(buf);

	va_start(ap, fmt);
	if ( vsnprintf(buf, sizeof(buf), fmt, ap) > 0 ) {
		Serial.println(buf);
	}
}
#else
	void DBG(const char *fmt, ...) {};
#endif

#endif /* DEBUG_H_ */
