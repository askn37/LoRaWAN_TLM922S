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

// #include <MultiUART.h>
#if defined(__MULTIUART_H)
#ifndef __LORAWAN_TLM922S_MULTIUART_H
#define __LORAWAN_TLM922S_MULTIUART_H

//
// エコーバックスルー先コンソールの指定（HardwareSerial）
//
#ifndef LORAWAN_TLM922S_DEBUG
#define LORAWAN_TLM922S_DEBUG Serial
#endif

#include <Arduino.h>
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
    inline void setThrottle (int16_t throttle = 0) {
        _UART->setThrottle(throttle);
    }
    inline void setRxBuffer (volatile char* _buffAddr = NULL, int _buffMax = 0) {
        _UART->setRxBuffer(_buffAddr, _buffMax);
    }

    // エコーバックのコールバック
    // 受信バッファの先頭が '\r' なら以後は応答なので捨てない
    static void LoRaWAN_TLM922S_MultiUART_echoback (MultiUART* UART) {
        if (UART->available() && UART->peek() != '\r') {
            LORAWAN_TLM922S_DEBUG.write(UART->read());
        }
    }
    // エコーしない場合でも読み捨ては行う
    static void LoRaWAN_TLM922S_MultiUART_echobackDrop (MultiUART* UART) {
        if (UART->available() && UART->peek() != '\r') UART->read();
    }

    // write時のエコーバックを有効・無効にする
    void setEchoThrough (bool through) {
        if (through) _UART->setWriteBack(&LoRaWAN_TLM922S_MultiUART_echoback);
        else _UART->setWriteBack(&LoRaWAN_TLM922S_MultiUART_echobackDrop);
        super::setEchoThrough(through);
    }

};
#endif
#endif

// end of header
