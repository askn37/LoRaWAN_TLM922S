/***************
 *
 * CommandSample - TLM922S-P01A command sample for Arduino
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/LoRaWAN_TLM922S
 * maintainer: askn https://twitter.com/askn37
 *
 */

#include <Arduino.h>
#include "LoRaWAN_TLM922S.h"

#define TX_PIN		12			// D12 O to I TLM_MISO/RX(12)
#define RX_PIN		11			// D11 I to O TLM_MOSI/TX(11)
#define WAKE_PIN	7			// D7  O to I TLM_INT2/WakeUp/~Sleep(7)

#define LORAWAN_BAUD	9600
#define CONSOLE_BAUD	9600

LoRaWAN_TLM922S LoRaWAN(RX_PIN, TX_PIN);

void setup (void) {
    while (!Serial);
    Serial.begin(CONSOLE_BAUD);
    Serial.println(F("Startup"));

    LoRaWAN.begin(LORAWAN_BAUD);
    while (!LoRaWAN.getReady())     delay(1000);
    while (!LoRaWAN.factoryReset()) delay(1000);

    Serial.println(F("try join otaa"));
    while (!(LoRaWAN.join(JOIN_OTAA) &&
             LoRaWAN.joinResult())) delay(1000);
    Serial.println(F("accepted"));
}

void loop (void) {
    uint32_t ms = millis();
    if (LoRaWAN.tx(TX_CNF, 1)) {
        Serial.print(F("millis:"));
        Serial.println(ms, HEX);
        LoRaWAN.txData(ms);
        if (LoRaWAN.txRequest() &&
            LoRaWAN.txResult()) {
            Serial.println(F("tx_ok"));
        }
        else {
            Serial.print(F("error:"));
            Serial.println(LoRaWAN.getResult());
        }
    }
    delay(10000);
}

// end of code


