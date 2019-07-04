/***************
 *
 * LoRaWAN_TLM922S_MultiUART.cpp - Kiwi-tec.com TLM922S-P01A LoRaWAN Module connector for Arduino
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/LoRaWAN_TLM922S
 * maintainer: askn https://twitter.com/askn37
 *
 */

#include <Arduino.h>
#include "LoRaWAN_TLM922S_MultiUART.h"

// エコーバックのコールバック
// 受信バッファの先頭が '\r' なら以後は応答なので捨てない
void LoRaWAN_TLM922S_MultiUART_echoback (MultiUART* UART) {
	if (UART->available() && UART->peek() != '\r') {
		LORAWAN_TLM922S_DEBUG.write(UART->read());
	}
}
// エコーしない場合でも読み捨ては行う
void LoRaWAN_TLM922S_MultiUART_echobackDrop (MultiUART* UART) {
	if (UART->available() && UART->peek() != '\r') UART->read();
}

// write時のエコーバックを有効・無効にする
void LoRaWAN_TLM922S_MultiUART::setEchoThrough (bool through) {
	if (through) _UART->setWriteBack(&LoRaWAN_TLM922S_MultiUART_echoback);
	else _UART->setWriteBack(&LoRaWAN_TLM922S_MultiUART_echobackDrop);
	super::setEchoThrough(through);
}

// end of code
