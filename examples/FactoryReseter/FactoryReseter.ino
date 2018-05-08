/***************
 *
 * FactoryReseter - TLM922S-P01A factory resetting for Arduino
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/LoRaWAN_TLM922S
 * maintainer: askn https://twitter.com/askn37
 *
 */

#define MULTIUART_BASEFREQ 76800
#define BAUDRATE_THROTTLE 1009

#include <Arduino.h>
#include <LoRaWAN_TLM922S.h>

#define CONSOLE_BAUD	9600

#define TX_PIN		12			// D12 O to I TLM_MISO/RX(12)
#define RX_PIN		11			// D11 I to O TLM_MOSI/TX(11)
#define WAKE_PIN	7			// D7  O to I TLM_INT2/WakeUp/~Sleep(7)

LoRaWAN_TLM922S LoRaWAN(RX_PIN, TX_PIN);

uint32_t baudrateLists[] = {38400, 28800, 19200, 9600};

void setup (void) {
    Serial.begin(CONSOLE_BAUD);
    Serial.println(F("Startup"));

    LoRaWAN_TLM922S::setThrottle(BAUDRATE_THROTTLE);
    LoRaWAN.setEchoThrough(ECHO_ON);

    for (auto baudrate : baudrateLists) {
        Serial.println();
        Serial.print(F("Baudrate: "));
        Serial.println(baudrate);
        LoRaWAN.begin(baudrate);
        if (LoRaWAN.getReady() &&
            LoRaWAN.factoryReset() &&
            LoRaWAN.getReady()) {
            Serial.println(F("  =success"));
            continue;
        }
        else {
            Serial.println(F("  =failed, unmatch baudrate"));
        }
        delay(1000);
    }

    LoRaWAN.getVersion();
    LoRaWAN.getDevEUI();
}

void loop (void) {}