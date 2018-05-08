/***************
 *
 * TerminalSample - TLM922S-P01A terminal console for Arduino
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/LoRaWAN_TLM922S
 * maintainer: askn https://twitter.com/askn37
 *
 */

#define BAUDRATE_THROTTLE 1009

#include <Arduino.h>
#include <LoRaWAN_TLM922S.h>

#define CONSOLE_BAUD	9600
#define LORAWAN_BAUD	9600

#define TX_PIN		12			// D12 O to I TLM_MISO/RX(12)
#define RX_PIN		11			// D11 I to O TLM_MOSI/TX(11)
#define WAKE_PIN	7			// D7  O to I TLM_INT2/WakeUp/~Sleep(7)

LoRaWAN_TLM922S LoRaWAN(RX_PIN, TX_PIN);

void setup (void) {
    #ifdef BAUDRATE_THROTTLE
    LoRaWAN_TLM922S::setThrottle(BAUDRATE_THROTTLE);
    #endif

    pinMode(WAKE_PIN, OUTPUT);
    digitalWrite(WAKE_PIN, HIGH);

    Serial.begin(LORAWAN_BAUD);
    Serial.println(F("Startup"));

    LoRaWAN.begin(LORAWAN_BAUD);
    LoRaWAN.setEchoThrough(ECHO_ON);
    while (!LoRaWAN.getReady());
    LoRaWAN.reset();
}

void loop (void) {
    if (Serial.available()) {
        LoRaWAN.write(Serial.read());
    }
    while (LoRaWAN.available()) {
        Serial.write(LoRaWAN.read());
    }
}

// end if code
