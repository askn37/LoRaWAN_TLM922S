#ifndef __LoRaWAN_TLM922S_T_H
#define __LoRaWAN_TLM922S_T_H

#include <Arduino.h>
#include <avr/pgmspace.h>

#define PS_DICT_MAX (197)
#define CMD_DIC_LEN (171)
#define CMD_TABLE_LEN (24)

extern const PROGMEM uint8_t CMD_DICT[];
extern const PROGMEM uint16_t CMD_TABLE[];
extern const PROGMEM uint32_t PS_DICT[];

#define PS_NOOP             0
#define PS_READY            1		// >
#define PS_PREFIX           2		// >>
#define PS_ENDRESET         3		// <
#define PS_MODRESET         4		// > TLM922S <
#define PS_DEMODMARG        5		// >> DemodMargin =
#define PS_NBGATEWAYS       6		// >> NbGateways =
#define PS_RX               7		// >> rx
#define PS_OK               8		// >> Ok
#define PS_TXOK             10		// >> tx_ok
#define PS_ACCEPTED         11		// >> accepted
#define PS_JOINED           12		// >> joined
#define PS_ADVANCE          13		// >> Advance Mode
#define PS_ON               14		// >> on
#define PS_OFF              15		// >> off
#define PS_INVALID          16		// >> Invalid
#define PS_UNKOWN           17		// >> Unknown command!
#define PS_ERR              18		// >> err
#define PS_UNSUCCESS        19		// >> unsuccess
#define PS_UNJOINED         20		// >> unjoined
#define PS_INVDLEN          21		// >> invalid_data_length
#define PS_KEYNOTINIT       22		// >> keys_not_init
#define PS_NOFREECH         23		// >> no_free_ch
#define PS_BUSY             24		// >> busy
#define PS_NOTJOINED        25		// >> not_joined
#define PS_PASSWORDERR      26		// >> Password Error

#define EX_NOOP                  0x00	//  [ex_noop]
#define EX_MOD_SAVE              0x01	// mod save\r []
#define EX_MOD_RESET             0x02	// mod reset\r [ex_resetAfter]
#define EX_MOD_FACTRY            0x03	// mod factory_reset\r []
#define EX_MOD_SLEEP             0x04	// mod sleep 1 1 <F> []
#define EX_MOD_GET_VER           0x05	// mod get_ver\r [ex_str]
#define EX_MOD_GET_DEVEUI        0x06	// mod get_hw_deveui\r [ex_hex]
#define EX_MOD_SET_ECHO_OFF      0x07	// mod set_echo off\r []
#define EX_MOD_SET_ECHO_ON       0x08	// mod set_echo on\r []
#define EX_LORA_SAVE             0x09	// lorawan save\r [ex_resetAfter]
#define EX_LORA_GET_DEVADDR      0x0a	// lorawan get_devaddr\r [ex_hex]
#define EX_LORA_JOIN_OTAA        0x0b	// lorawan join otaa\r []
#define EX_LORA_JOIN_ABP         0x0c	// lorawan join abp\r []
#define EX_LORA_TX_UCNF          0x0d	// lorawan tx ucnf <F> [ex_tx]
#define EX_LORA_TX_CNF           0x0e	// lorawan tx cnf <F> [ex_tx]
#define EX_LORA_GET_DR           0x0f	// lorawan get_dr\r [ex_dec F]
#define EX_LORA_GET_UPCNT        0x10	// lorawan get_upcnt\r [ex_dec T]
#define EX_LORA_GET_DWCNT        0x11	// lorawan get_downcnt\r [ex_dec T]
#define EX_LORA_GET_ADR          0x12	// lorawan get_adr\r [ex_onoff]
#define EX_LORA_SET_DR           0x13	// lorawan set_dr <F> []
#define EX_LORA_SET_ADR_OFF      0x14	// lorawan set_adr off\r []
#define EX_LORA_SET_ADR_ON       0x15	// lorawan set_adr on\r []
#define EX_LORA_SET_LINK         0x16	// lorawan set_linkchk\r []
#define EX_MOD_SET_BAUD          0x17	// mod set_baudrate <F> [ex_resetAfter]

#endif

/* end of header */
