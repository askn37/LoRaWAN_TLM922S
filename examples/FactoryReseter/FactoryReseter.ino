/***************
 *
 * FactoryReseter - TLM922S-P01A factory resetting tool for Arduino
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/LoRaWAN_TLM922S
 * maintainer: askn https://twitter.com/askn37
 *
 */

#define MULTIUART_BASEFREQ 76800

#include <Arduino.h>

#define CONSOLE_BAUD	9600

#define TX_PIN		12			// D12 O to I TLM_MISO/RX(12)
#define RX_PIN		11			// D11 I to O TLM_MOSI/TX(11)
#define WAKE_PIN	7			// D7  O to I TLM_INT2/WakeUp/~Sleep(7)

// LoRaWANオブジェクト作成
// 使用するUARTライブラリとの組み合わせを選ぶ

// #include "LoRaWAN_TLM922S_SoftwareSerial.h"						// Used SoftwareSerial
// LoRaWAN_TLM922S_SoftwareSerial LoRaWAN(RX_PIN, TX_PIN);			// SoftwareSerial

#include "LoRaWAN_TLM922S_MultiUART.h"						// Used MultiUART
LoRaWAN_TLM922S_MultiUART LoRaWAN(RX_PIN, TX_PIN);			// MultiUART

// #include "LoRaWAN_TLM922S_Serial.h"							// Used HardwareSerial-0
// LoRaWAN_TLM922S_Serial LoRaWAN;								// Serial

// #include "LoRaWAN_TLM922S_Serial1.h"							// Used HardwareSerial-1
// LoRaWAN_TLM922S_Serial1 LoRaWAN;								// Serial1

// #include "LoRaWAN_TLM922S_Serial2.h"							// Used HardwareSerial-2
// LoRaWAN_TLM922S_Serial2 LoRaWAN;								// Serial2

// #include "LoRaWAN_TLM922S_Serial3.h"							// Used HardwareSerial-3
// LoRaWAN_TLM922S_Serial3 LoRaWAN;								// Serial3

uint32_t baudrateLists[] = {38400, 28800, 19200, 9600};

volatile char buffer[256];
void setup (void) {

    pinMode(WAKE_PIN, OUTPUT);
    digitalWrite(WAKE_PIN, HIGH);

    Serial.begin(CONSOLE_BAUD);
    Serial.println(F("Startup"));
    for (auto baudrate : baudrateLists) {
        delay(1000);
        Serial.println();
        Serial.print(F("Baudrate: "));
        Serial.println(baudrate);
        LoRaWAN.begin(baudrate);
	    // LoRaWAN.setEchoThrough(ECHO_ON);
        if (LoRaWAN.getReady() &&
            LoRaWAN.factoryReset() &&
            LoRaWAN.getReady()) {
            Serial.println(F("  =success"));
            continue;
        }
        else {
            Serial.println(F("  =failed, unmatch baudrate"));
        }
    }
    Serial.println();

    LoRaWAN.setEcho(ECHO_OFF);
    LoRaWAN.getVersion();
    Serial.print(F("Version: ")); Serial.println(LoRaWAN.getData());

#ifdef __MULTIUART_H
	LoRaWAN.setThrottle(8);
    LoRaWAN.setRxBuffer(buffer, 256);
    LoRaWAN.getAllKey();
    LoRaWAN.setRxBuffer();

    String text[7];
    int idx = 0;
    for (auto c : LoRaWAN.getData()) {
        if (c == ' ' || text[idx].length() == 32) {
            idx++;
            continue;
        }
        text[idx] += c;
    }
    Serial.print(F("DevAddr: ")); Serial.println(text[0]);
    Serial.print(F("   UUID: ")); Serial.println(text[1]);
    Serial.print(F(" DevEUI: ")); Serial.println(text[2]);
    Serial.print(F(" AppEUI: ")); Serial.println(text[3]);
    Serial.print(F(" AppKey: ")); Serial.println(text[4]);
    Serial.print(F("NwkSKey: ")); Serial.println(text[5]);
    Serial.print(F("AppSKey: ")); Serial.println(text[6]);
#endif
}

void loop (void) {}

// end of code
