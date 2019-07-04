/***************
 *
 * LoRaWAN_TLM922S_MultiUART.h - Kiwi-tec.com TLM922S-P01A LoRaWAN Module connector for Arduino
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/LoRaWAN_TLM922S
 * maintainer: askn https://twitter.com/askn37
 *
 */

#ifndef __LORAWAN_TLM922S_MULTIUART_H
#define __LORAWAN_TLM922S_MULTIUART_H

//
// エコーバックスルー先コンソールの指定（HardwareSerial）
//
#ifndef LORAWAN_TLM922S_DEBUG
#define LORAWAN_TLM922S_DEBUG Serial
#endif

#include <Arduino.h>
#include <MultiUART.h>
#include "LoRaWAN_TLM922S.h"

//
// クラス定義
//
class LoRaWAN_TLM922S_MultiUART : public LoRaWAN_TLM922S {
private:
	MultiUART* _UART;
public:
 	using super = LoRaWAN_TLM922S;
	using super::super;
	using super::write;

	inline int available (void) { return _UART->available(); }
	inline size_t writeRaw (const uint8_t c) { return _UART->write(c); }
	inline int peek (void) { return _UART->peek(); }
	inline int read (void) { return _UART->read(); }
	inline void flush (void) { _UART->flush(); }
	void begin (long baud) { _UART->begin(baud); }

	LoRaWAN_TLM922S_MultiUART (uint8_t receivePin, uint8_t transmitPin) {
		_UART = new MultiUART(receivePin, transmitPin);
	}

	// MultiUARTの場合
	inline size_t write (const uint8_t c) { return _UART->write(c); }
	void setEchoThrough (bool);
    inline void setThrottle (int16_t throttle = 0) {
		_UART->setThrottle(throttle);
	}
    inline void setRxBuffer (volatile char* _buffAddr = NULL, int _buffMax = 0) {
		_UART->setRxBuffer(_buffAddr, _buffMax);
	}

};
#endif

// end of header
