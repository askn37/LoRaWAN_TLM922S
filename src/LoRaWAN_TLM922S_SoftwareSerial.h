/***************
 *
 * LoRaWAN_TLM922S_SoftwareSerial.h - Kiwi-tec.com TLM922S-P01A LoRaWAN Module connector for Arduino
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/LoRaWAN_TLM922S
 * maintainer: askn https://twitter.com/askn37
 *
 */

#ifndef __LORAWAN_TLM922S_SOFTWARESERIAL_H
#define __LORAWAN_TLM922S_SOFTWARESERIAL_H

//
// エコーバックスルー先コンソールの指定（HardwareSerial）
//
#ifndef LORAWAN_TLM922S_DEBUG
#define LORAWAN_TLM922S_DEBUG Serial
#endif

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "LoRaWAN_TLM922S.h"

//
// クラス定義
//
class LoRaWAN_TLM922S_SoftwareSerial : public LoRaWAN_TLM922S {
private:
	SoftwareSerial* _Serial;
public:
 	using super = LoRaWAN_TLM922S;
	using super::super;
	using super::write;
	inline int available (void) { return _Serial->available(); }
	inline size_t writeRaw (const uint8_t c) { return _Serial->write(c); }
	inline int peek (void) { return _Serial->peek(); }
	inline int read (void) { return _Serial->read(); }
	inline void flush (void) { _Serial->flush(); }
	void begin (long baud) { _Serial->begin(baud); _Serial->flush(); }

	LoRaWAN_TLM922S_SoftwareSerial (
		uint8_t receivePin,
		uint8_t transmitPin,
		bool inverse_logic = false) {
		_Serial = new SoftwareSerial(receivePin, transmitPin, inverse_logic);
	}
};
#endif
