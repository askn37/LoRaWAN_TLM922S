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
#define CMD_DIC_LEN (203)
#define CMD_TABLE_LEN (27)

extern const PROGMEM uint32_t PS_DICT[];    // 668 bytes
extern const PROGMEM uint8_t CMD_DICT[];    // 609 bytes
extern const PROGMEM uint16_t CMD_TABLE[];  // 54 bytes

// total 1331 bytes

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
    , EX_MOD_SLEEP          = 4     // mod sleep 0 1 <F> []
    , EX_MOD_DEEP_SLEEP     = 5     // mod sleep 1 1 <F> []
    , EX_MOD_GET_VER        = 6     // mod get_ver\r [ex_str]
    , EX_MOD_GET_DEVEUI     = 7     // mod get_hw_deveui\r [ex_hex]
    , EX_MOD_SET_ECHO_OFF   = 8     // mod set_echo off\r []
    , EX_MOD_SET_ECHO_ON    = 9     // mod set_echo on\r []
    , EX_MOD_SET_BAUD       = 10    // mod set_baudrate <F> [ex_resetAfter]
    , EX_LORA_SAVE          = 11    // lorawan save\r [ex_resetAfter]
    , EX_LORA_GET_DEVADDR   = 12    // lorawan get_devaddr\r [ex_hex]
    , EX_LORA_JOIN_OTAA     = 13    // lorawan join otaa\r []
    , EX_LORA_JOIN_ABP      = 14    // lorawan join abp\r []
    , EX_LORA_TX_UCNF       = 15    // lorawan tx ucnf <F> [ex_tx]
    , EX_LORA_TX_CNF        = 16    // lorawan tx cnf <F> [ex_tx]
    , EX_LORA_GET_DR        = 17    // lorawan get_dr\r [ex_dec F]
    , EX_LORA_GET_UPCNT     = 18    // lorawan get_upcnt\r [ex_dec T]
    , EX_LORA_GET_DWCNT     = 19    // lorawan get_downcnt\r [ex_dec T]
    , EX_LORA_GET_ADR       = 20    // lorawan get_adr\r [ex_onoff]
    , EX_LORA_SET_DR        = 21    // lorawan set_dr <F> []
    , EX_LORA_SET_ADR_OFF   = 22    // lorawan set_adr off\r []
    , EX_LORA_SET_ADR_ON    = 23    // lorawan set_adr on\r []
    , EX_LORA_SET_LINK      = 24    // lorawan set_linkchk\r []
    , EX_LOLA_GET_MAX_PAY   = 25    // lorawan get_max_payload_table <F> [ex_dec]
    , EX_MP_GETKEY          = 26    // mp get_key\r [ex_hex]
} tlmex_t;

#endif

// end of header
