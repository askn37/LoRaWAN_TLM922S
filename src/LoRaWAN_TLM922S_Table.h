/***************
 *
 * LoRaWAN_TLM922S_Table.h
 *
 * release site: https://github.com/askn37/LoRaWAN_TLM922S
 * maintainer: askn https://twitter.com/askn37
 *
 */

#ifndef __LoRaWAN_TLM922S_T_H
#define __LoRaWAN_TLM922S_T_H

#include <Arduino.h>
#include <avr/pgmspace.h>

#define PS_DICT_MAX (167)
#define CMD_DIC_LEN (178)
#define CMD_TABLE_LEN (25)

extern const PROGMEM uint32_t PS_DICT[];    // 668 bytes
extern const PROGMEM uint8_t CMD_DICT[];    // 534 bytes
extern const PROGMEM uint16_t CMD_TABLE[];  // 50 bytes

// total 1252 bytes

typedef enum tlmps {
	  PS_NOOP        	= 0
    , PS_READY          = 1     // >
    , PS_PREFIX         = 2     // >>
    , PS_ENDRESET       = 3     // <
    , PS_MODRESET       = 4     // > TLM922S <
    , PS_DEMODMARG      = 5     // >> DemodMargin =
    , PS_NBGATEWAYS     = 6     // >> NbGateways =
    , PS_RX             = 7     // >> rx
    , PS_OK             = 8     // >> Ok
    , PS_TXOK           = 10    // >> tx_ok
    , PS_ACCEPTED       = 11    // >> accepted
    , PS_JOINED         = 12    // >> joined
    , PS_ON             = 14    // >> on
    , PS_OFF            = 15    // >> off
    , PS_INVALID        = 16    // >> Invalid
    , PS_UNKOWN         = 17    // >> Unknown command!
    , PS_ERR            = 18    // >> err
    , PS_UNSUCCESS      = 19    // >> unsuccess
    , PS_UNJOINED       = 20    // >> unjoined
    , PS_INVDLEN        = 21    // >> invalid_data_length
    , PS_KEYNOTINIT     = 22    // >> keys_not_init
    , PS_NOFREECH       = 23    // >> no_free_ch
    , PS_BUSY           = 24    // >> busy
    , PS_NOTJOINED      = 25    // >> not_joined
} tlmps_t;

typedef enum tlmex {
      EX_NOOP               = 0
    , EX_MOD_SAVE           = 1     // mod save\r []
    , EX_MOD_RESET          = 2     // mod reset\r [ex_resetAfter]
    , EX_MOD_FACTRY         = 3     // mod factory_reset\r []
    , EX_MOD_SLEEP          = 4     // mod sleep 1 1 <F> []
    , EX_MOD_GET_VER        = 5     // mod get_ver\r [ex_str]
    , EX_MOD_GET_DEVEUI     = 6     // mod get_hw_deveui\r [ex_hex]
    , EX_MOD_SET_ECHO_OFF   = 7     // mod set_echo off\r []
    , EX_MOD_SET_ECHO_ON    = 8     // mod set_echo on\r []
    , EX_MOD_SET_BAUD       = 9     // mod set_baudrate <F> [ex_resetAfter]
    , EX_LORA_SAVE          = 10    // lorawan save\r [ex_resetAfter]
    , EX_LORA_GET_DEVADDR   = 11    // lorawan get_devaddr\r [ex_hex]
    , EX_LORA_JOIN_OTAA     = 12    // lorawan join otaa\r []
    , EX_LORA_JOIN_ABP      = 13    // lorawan join abp\r []
    , EX_LORA_TX_UCNF       = 14    // lorawan tx ucnf <F> [ex_tx]
    , EX_LORA_TX_CNF        = 15    // lorawan tx cnf <F> [ex_tx]
    , EX_LORA_GET_DR        = 16    // lorawan get_dr\r [ex_dec F]
    , EX_LORA_GET_UPCNT     = 17    // lorawan get_upcnt\r [ex_dec T]
    , EX_LORA_GET_DWCNT     = 18    // lorawan get_downcnt\r [ex_dec T]
    , EX_LORA_GET_ADR       = 19    // lorawan get_adr\r [ex_onoff]
    , EX_LORA_SET_DR        = 20    // lorawan set_dr <F> []
    , EX_LORA_SET_ADR_OFF   = 21    // lorawan set_adr off\r []
    , EX_LORA_SET_ADR_ON    = 22    // lorawan set_adr on\r []
    , EX_LORA_SET_LINK      = 23    // lorawan set_linkchk\r []
    , EX_LORA_GET_APPSKEY   = 24    // lorawan get_appskey\r [ex_hex]
} tlmex_t;

#endif

// end of header
