/***************
 *
 * LoRaWAN_TLM922S - Kiwi-tec.com TLM922S-P01A LoRaWAN Module connector for Arduino
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/LoRaWAN_TLM922S
 * maintainer: askn https://twitter.com/askn37
 *
 */

#ifndef __LORAWAN_TLM922S_H
#define __LORAWAN_TLM922S_H

#include <Arduino.h>
#include "LoRaWAN_TLM922S_Table.h"

//// 継承するシリアルクラスを選択
#define LORAWAN_TLM922S_USED_MULTIUART
// #define LORAWAN_TLM922S_USED_SOFTWARESERIAL

#if defined(LORAWAN_TLM922S_USED_MULTIUART)
    #include <MultiUART.h>
    #define LORAWAN_TLM922S_SERIAL MultiUART
#elif defined(LORAWAN_TLM922S_USED_SOFTWARESERIAL)
    #include <SoftwareSerial.h>
    #define LORAWAN_TLM922S_SERIAL SoftwareSerial
#else
    #error no defined base class LORAWAN_TLM922S_USED_XXXX
#endif
#ifndef LORAWAN_TLM922S_DEBUG
#define LORAWAN_TLM922S_DEBUG Serial
#endif

#define ECHO_OFF	false
#define ECHO_ON		true
#define JOIN_OTAA	false
#define JOIN_ABP	true
#define ADR_OFF		false
#define ADR_ON		true
#define TX_UCNF		false
#define TX_CNF		true

class LoRaWAN_TLM922S : public LORAWAN_TLM922S_SERIAL {
private:
    String _echoBack;
    String _rxData;
    int32_t _value;
    uint8_t _current;
    uint8_t _result;
    uint8_t _rxPort;
    int8_t _margin;
    int8_t _gateways;
    uint8_t _echo = 0;

    uint8_t parsePrompt (const uint8_t);
    void putCommand (const uint8_t);
    void clearEchoBack (void) { _echoBack = ""; }
    void pushEchoBack (const uint8_t c) { if (_echo) _echoBack += (char)c; }
    void putEchoBack (void) { if (_echo) LORAWAN_TLM922S_DEBUG.print(_echoBack); clearEchoBack(); }

    bool wait(uint16_t = 0);
    uint8_t nextPrompt (uint16_t = 100);
    uint8_t skipPrompt (uint8_t = PS_READY, uint8_t = PS_READY, uint16_t = 100);
    bool runCommand (uint8_t, uint16_t = 1000);
    bool getBoolCommand (uint8_t, uint16_t = 1000);
    bool getStringCommand (uint8_t, uint16_t = 1000);
    uint32_t getValueCommand (uint8_t, uint16_t = 1000);

    uint32_t parseDecimal (void);
    void parseHexData (void);
    uint32_t parseValue (bool = false, uint16_t timeout = 1000);

public:
    // using LORAWAN_TLM922S_SERIAL::LORAWAN_TLM922S_SERIAL;
    LoRaWAN_TLM922S (uint8_t, uint8_t);

    #ifndef LORAWAN_TLM922S_USED_MULTIUART
	using LORAWAN_TLM922S_SERIAL::write;
	size_t write (const uint8_t c);
    #endif

    bool getReady (void);
    void setEchoThrough (bool = false);

    bool reset (void);
    bool sleep (uint16_t = 0);
    bool wakeUp (void);
    inline bool modSave (void) { return runCommand(EX_MOD_SAVE); }
    inline bool loraSave (void) { return runCommand(EX_LORA_SAVE); }
    inline bool factoryReset (void) { return runCommand(EX_MOD_FACTRY); }
    inline bool setEcho (bool echo = ECHO_ON) { return runCommand(echo ? EX_MOD_SET_ECHO_ON : EX_MOD_SET_ECHO_OFF); }
    inline bool getVersion (void) { return getStringCommand(EX_MOD_GET_VER); }
    inline bool getDevEUI (void) { return getStringCommand(EX_MOD_GET_DEVEUI); }
    inline bool getDevAddr (void) { return getStringCommand(EX_LORA_GET_DEVADDR); }
    inline int32_t getUpCount (void) { return getValueCommand(EX_LORA_GET_UPCNT); }
    inline int32_t getDownCount (void) { return getValueCommand(EX_LORA_GET_DWCNT); }
    inline uint8_t getDataRate (void) { return getValueCommand(EX_LORA_GET_DR); }
    bool setDataRate (uint8_t);

    bool join (bool = JOIN_OTAA);
    bool joinResult (void);
    inline bool getAdr (void) { return getBoolCommand(EX_LORA_GET_ADR); }
    inline bool setAdr (bool adr = ADR_ON) { return runCommand(adr ? EX_LORA_SET_ADR_ON : EX_LORA_SET_ADR_OFF); }
    inline bool setLinkCheck (void) { return runCommand(EX_LORA_SET_LINK); }
    inline bool tx  (bool confirm, uint8_t fport, char* data) {
        tx(confirm, fport);
        txData(data);
        return txRequest();
    }
    bool tx (bool, uint8_t);
    inline void txData (char* str) { this->LORAWAN_TLM922S_SERIAL::write(str); }
    inline void txData (char* str, int len) { this->LORAWAN_TLM922S_SERIAL::write(str, len); }
    inline void txData (char c) { this->LORAWAN_TLM922S_SERIAL::write(c); };
    inline void txData (uint8_t v) { txData((uint32_t)(v), 2); }
    inline void txData (int v) { txData((uint32_t)(v), 4); }
    inline void txData (uint16_t v) { txData((uint32_t)(v), 4); }
    inline void txData (long v) { txData((uint32_t)(v), 8); }
    inline void txData (uint32_t v) { txData((uint32_t)(v), 8); }
    void txData (uint32_t, int);
    bool txRequest (void);
    inline bool txRequest(bool c, uint8_t f, char* s) { return tx(c, f, s); }
    bool txResult (void);

    inline uint8_t getResult (void) { return _result; }
    inline uint32_t getValue (void) { return _value; }
    inline uint8_t isData (void) { return _rxData.length(); }
    inline String getData (void) { return _rxData; }
    inline bool isLinkCheck (void) { return (_margin >= 0 && _gateways > 0); }
    inline int8_t getMargin (void) { return _margin; }
    inline int8_t getGateways (void) { return _gateways; }
    inline int8_t getRxPort (void) { return _rxPort; }

    static void echoback (LORAWAN_TLM922S_SERIAL*);
    static void echobackDrop (LORAWAN_TLM922S_SERIAL*);
};

#endif
