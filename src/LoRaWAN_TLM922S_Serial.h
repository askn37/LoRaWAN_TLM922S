/***************
 *
 * LoRaWAN_TLM922S_Serial.h - Kiwi-tec.com TLM922S-P01A LoRaWAN Module connector for Arduino
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/LoRaWAN_TLM922S
 * maintainer: askn https://twitter.com/askn37
 *
 */

#ifndef __LORAWAN_TLM922S_SERIAL_H
#define __LORAWAN_TLM922S_SERIAL_H

//
// エコーバックスルー先コンソールの指定（HardwareSerial）
//
#ifndef LORAWAN_TLM922S_DEBUG
#define LORAWAN_TLM922S_DEBUG Serial1
#endif

#include <Arduino.h>
#include "LoRaWAN_TLM922S.h"

//
// クラス定義
//
class LoRaWAN_TLM922S_SoftwareSerial : public LoRaWAN_TLM922S {
public:
    using super = LoRaWAN_TLM922S;
    using super::super;
    using super::write;
    inline int available (void) { return Serial.available(); }
    inline size_t writeRaw (const uint8_t c) { return Serial.write(c); }
    inline int peek (void) { return Serial.peek(); }
    inline int read (void) { return Serial.read(); }
    inline void flush (void) { Serial.flush(); }
    void begin (long baud) { Serial.begin(baud); }
};
#endif

// end of header
