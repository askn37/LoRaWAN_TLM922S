#!/usr/bin/env perl
# use strict;
# use warnings;
use Data::Dumper;
STDOUT->autoflush(1);
my $basename = "LoRaWAN_TLM922S";

my @PSSOURCE = (
    # 0
    # Result
    ["PS_READY", 1, "\r> "],
    ["PS_PREFIX", 2, "\r>> "],
    ["PS_ENDRESET", 3, " <\n"],
    ["PS_MODRESET", 4, "\r> TLM922S <\n"],
    ["PS_DEMODMARG", 5, "\r>> DemodMargin = "],
    ["PS_NBGATEWAYS", 6, "\r>> NbGateways = "],
    ["PS_RX", 7, "\r>> rx "],
    ## Success
    ["PS_OK", 8, "\r>> Ok"],
    ["PS_TXOK", 10, "\r>> tx_ok"],
    ["PS_ACCEPTED", 11, "\r>> accepted"],
    ["PS_JOINED", 12, "\r>> joined"],
    ["PS_ADVANCE", 13, "\r>> Advance Mode"],
    ["PS_ON", 14, "\r>> on"],
    ## Failed
    ["PS_OFF", 15, "\r>> off"],
    ["PS_INVALID", 16, "\r>> Invalid"],
    ["PS_UNKOWN", 17, "\r>> Unknown command!"],
    ["PS_ERR", 18, "\r>> err"],
    ["PS_UNSUCCESS", 19, "\r>> unsuccess"],
    ["PS_UNJOINED", 20, "\r>> unjoined"],
    ["PS_INVDLEN", 21, "\r>> invalid_data_length"],
    ["PS_KEYNOTINIT", 22, "\r>> keys_not_init"],
    ["PS_NOFREECH", 23, "\r>> no_free_ch"],
    ["PS_BUSY", 24, "\r>> busy"],
    ["PS_NOTJOINED", 25, "\r>> not_joined"],
    ["PS_PASSWORDERR", 26, "\r>> Password Error"],
);

# 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59,
# 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131,
# 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197,
# 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271,
# 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353,
# 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433,
# 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509,

my $PSDICTMAX = 197;

my @DICTSOURCE = (
    ["EX_NOOP", "", "ex_noop"],
    ["EX_MOD_SAVE" ,"mod save\r", ""],
    ["EX_MOD_RESET", "mod reset\r", "ex_resetAfter"],
    ["EX_MOD_FACTRY", "mod factory_reset\r", ""],
    ["EX_MOD_SLEEP" ,"mod sleep 1 1 ", ""],
    ["EX_MOD_GET_VER" ,"mod get_ver\r", "ex_str"],
    ["EX_MOD_GET_DEVEUI" ,"mod get_hw_deveui\r", "ex_hex"],
    ["EX_MOD_SET_ECHO_OFF" ,"mod set_echo off\r", ""],
    ["EX_MOD_SET_ECHO_ON" ,"mod set_echo on\r", ""],
    ["EX_LORA_SAVE" ,"lorawan save\r", "ex_resetAfter"],
    ["EX_LORA_GET_DEVADDR" ,"lorawan get_devaddr\r", "ex_hex"],
    ["EX_LORA_JOIN_OTAA" ,"lorawan join otaa\r", ""],
    ["EX_LORA_JOIN_ABP" ,"lorawan join abp\r", ""],
    ["EX_LORA_TX_UCNF" ,"lorawan tx ucnf ", "ex_tx"],
    ["EX_LORA_TX_CNF" ,"lorawan tx cnf ", "ex_tx"],

    ["EX_LORA_GET_DR" ,"lorawan get_dr\r", "ex_dec F"],
    ["EX_LORA_GET_UPCNT" ,"lorawan get_upcnt\r", "ex_dec T"],
    ["EX_LORA_GET_DWCNT" ,"lorawan get_downcnt\r", "ex_dec T"],
    ["EX_LORA_GET_ADR" ,"lorawan get_adr\r", "ex_onoff"],
    ["EX_LORA_SET_DR" ,"lorawan set_dr ", ""],
    ["EX_LORA_SET_ADR_OFF" ,"lorawan set_adr off\r", ""],
    ["EX_LORA_SET_ADR_ON" ,"lorawan set_adr on\r", ""],
    ["EX_LORA_SET_LINK" ,"lorawan set_linkchk\r", ""],

    # ["EX_MOD_GET_MODEL" ,"mod get_hw_model\r", "ex_str"],
    # ["EX_LORA_GET_DEVEUI" ,"lorawan get_deveui\r", "ex_hex"],

    # ["EX_LORA_GET_DL_FREQ" ,"lorawan get_dl_freq ", "ex_dec F"],
    # ["EX_LORA_GET_PWR_TBL" ,"lorawan get_pwr_table ", "ex_dec"],
    # ["EX_LOLA_GET_MAX_PAY" ,"lorawan get_max_payload_table ", "ex_dec"],
    # ["EX_LORA_GET_CH_PARA" ,"lorawan get_ch_para ", "ex_chpara Q,F,S,T"],
    # ["EX_LORA_GET_DC_BAND" ,"lorawan get_dc_band ", "ex_dcband"],
    # ["EX_LORA_GET_CH_STAT" ,"lorawan get_ch_status ", "ex_onoff"],
    # ["EX_LORA_GET_JOIN_ST" ,"lorawan get_join_status\r", ""],
    # ["EX_LORA_GET_CLASS" ,"lorawan get_class\r", "ex_str"],
    # ["EX_LORA_GET_CH_PLAN" ,"lorawan get_ch_plan\r", "ex_str"],
    # ["EX_LORA_GET_DEVEUI" ,"lorawan get_deveui\r", "ex_hex"],
    # ["EX_LORA_GET_DEVADDR" ,"lorawan get_devaddr\r", "ex_hex"],
    # ["EX_LORA_GET_APPSKEY", "lorawan get_appskey\r", "ex_hex"],
    # ["EX_LORA_GET_NWKSKEY", "lorawan get_nwkskey\r", "ex_hex"],
    # ["EX_LORA_GET_RX2" ,"lorawan get_rx2\r", "ex_dec S,T"],
    # ["EX_LORA_GET_BATT" ,"lorawan get_battery\r", "ex_dec F"],
    # ["EX_LORA_GET_PWR" ,"lorawan get_pwr\r", "ex_dec F"],
    # ["EX_LORA_GET_JOIN_CH" ,"lorawan get_join_ch\r", "ex_dec F,S"],
    # ["EX_LORA_GET_DC" ,"lorawan get_dc_ctl\r", "ex_onoff"],
    # ["EX_LORA_GET_TXR" ,"lorawan get_txretry\r", "ex_dec FS"],
    # ["EX_LORA_GET_RXD" ,"lorawan get_rxdelay\r", "ex_dec T,Q"],
    # ["EX_LORA_SET_TXR" ,"lorawan set_txretry ", ""],
    # ["EX_LORA_SET_BATT" ,"lorawan set_battery ", ""],

    # ["EX_ENABLE_ADVANCE", "Enable PASSWORD\r", ""],
    # ["EX_MP_GETKEY", "mp get_key\r", "ex_hex"],
    # ["EX_SHIP_SETKEY", "ship set_key ", ""],
    # ["EX_SHIP_GETKEY", "ship get_key\r", "ex_hex"],
    # ["EX_LORA_SET_CH_FREQ", "lorawan set_ch_freq ", ""],
    # ["EX_LORA_SET_CH_DR_RNG", "lorawan set_ch_dr_range ", ""],
    # ["EX_LORA_SET_CH_STAT", "lorawan set_ch_status ", ""],
    # ["EX_LORA_SET_DC_CTL", "lorawan set_dc_ctl ", ""],
    # ["EX_LORA_SET_DC_BAND", "lorawan set_dc_band ", ""],
    # ["EX_LORA_SET_JOIN_CH", "lorawan set_join_ch ", ""],

    # ["EX_LORA_GET_AS923" ,"lorawan get_as923_para\r", "ex_dec Q4"],
    # ["EX_MOD_SET_BAUD" ,"mod set_baudrate ", "ex_resetAfter"],
);

my(@DICT, $PSDICT, @START);
sub makeDict {
    @DICT = @START = ();
    my $DICTMAX = shift // 1023;
    my $count = 0;
    my $idx = 1;
    foreach my $obj (@DICTSOURCE) {
        my($n, $p, $x) = @$obj;
        next unless $p;
        my $prevword = 0;
        my $newword;
        my $index;
        my $hash;
        foreach my $c (split //, $p) {
            $c = ord $c;
            if (!$prevword) {
                $prevword = $c;
                next;
            }
            $newword = ($prevword << 8) | $c;
            $index = $newword % $DICTMAX;
            while ($hash = $DICT[$index]) {
                if ($hash == $newword) {
                    last;
                }
                $index = ($index + 1) % $DICTMAX;
            }
            if (!$hash) {
                $count++;
                $DICT[$index] = $newword;
            }
            $prevword = ($index * 3) + 0x100;
        }
        $START[$idx++] = ($index * 3) + 0x100;
    }
    return $count;
}
my $DICTMAX = makeDict(makeDict());
# printf "<%s>\n", $DICTMAX;
# print Dumper(\@DICT, \@START);

foreach my $obj (@PSSOURCE) {
    my($n, $d, $p) = @$obj;
    my $prevword = 0;
    my $index = 0;
    my $hash = 0;
    my $newword = 0;
    foreach my $c (split //, $p) {
        $c = ord $c;
        if (!$prevword) {
            $prevword = $c;
            next;
        }
        $newword = ($prevword) | ($c << 16);
        $index = $newword % $PSDICTMAX;
        while ($hash = $PSDICT[$index]) {
            last if 0 == (($hash - $newword) & 0xFFFFFF);
            $index = ($index + 1) % $PSDICTMAX;
        }
        if (!$hash) {
            $PSDICT[$index] = $newword;
        }
        $prevword = $index + 256;
    }
    $PSDICT[$index] = $newword | ($d << 24);
    # printf "%s, %s, %x, %x\n", $d, $n, $index, $PSDICT[$index];
}
my $CMDLEN = scalar @START;

open $FH, ">", "${basename}_Table.h";
# open $FH, ">", "CON";
print $FH <<_EOL;
#ifndef __${basename}_T_H
#define __${basename}_T_H

#include <Arduino.h>
#include <avr/pgmspace.h>

#define PS_DICT_MAX ($PSDICTMAX)
#define CMD_DIC_LEN ($DICTMAX)
#define CMD_TABLE_LEN ($CMDLEN)

extern const PROGMEM uint8_t CMD_DICT[];
extern const PROGMEM uint16_t CMD_TABLE[];
extern const PROGMEM uint32_t PS_DICT[];

#define PS_NOOP             0
_EOL

my %ps;
foreach my $obj (@PSSOURCE) {
    my($n, $d, $p) = @$obj;
    unless ($ps{$d}) {
        $p =~ s{^\s|\s$}{}g;
        printf $FH "#define %-20s%d\t\t// %s\n", $n, $d, $p;
        $ps{$d}++;
    }
}

print $FH "\n";

my $idx = 0;
foreach my $obj (@DICTSOURCE) {
    my($n, $p, $x) = @$obj;
    my $q = "";
    $p =~ s/^Enable (\S+)/Enable ADVANCE/;
    $p =~ s/\r+/\\r/;
    $q = "<F>" if $p =~ m/\s$/;
    printf $FH "#define %-24s 0x%02x\t// %s%s [%s]\n", $n, $idx++, $p, $q, $x;
}

print $FH <<_EOL;

#endif

/* end of header */
_EOL
close $FH;

open $FH, ">", "${basename}_Table.cpp";
# open my $FH, ">", "CON";
binmode $FH;

print $FH <<_EOL;
#include <Arduino.h>
#include <avr/pgmspace.h>
#include "${basename}_Table.h"

const PROGMEM uint32_t PS_DICT[] = {
_EOL
for (my $i = 0; $i < $PSDICTMAX; $i++) {
    my $d = $PSDICT[$i];
    print $FH "\t" unless $i % 8;
    if ($d) {
        printf $FH "0x%08xU", $d;
    }
    else {
        print $FH "         0U";
    }
    last if $i + 1 == $PSDICTMAX;
    if ($i % 8 == 7) {
        print $FH ",\n";
    }
    else {
        print $FH ", ";
    }
}
print $FH <<_EOL;

};

const PROGMEM uint8_t CMD_DICT[] = {
_EOL
my $max = scalar @DICT;
for (my $i = 0; $i < $max; $i++) {
    my $d = $DICT[$i] // 0;
    # print $FH "\t" unless $i % 4;
    my @chk = ((0xff & $d), (0xff00 & $d) >> 8, (0xff0000 & $d) >> 16);
    my $str = sprintf "\t0x%02xU, 0x%02xU, 0x%02xU", @chk;
    # $str =~ s/0x00U/   0U/g;
    print $FH $str;
    push @CHK, @chk;
    if ($i + 1 == $max) {
        print $FH "\n";
        last;
    }
    if ($i % 4 == 3) {
        printf $FH ",\t// 0x%04x\n", ($i & 0xFFFC) * 3 + 0x100;
    }
    else {
        print $FH ", ";
    }
}

my $len = scalar @START;
print $FH <<_EOL;
};

const PROGMEM uint16_t CMD_TABLE[] = {
_EOL

for (my $i = 0; $i < $len; $i++) {
    print $FH "\t" unless $i % 8;
    printf $FH "0x%04xU", ($START[$i] // 0);
    if ($i + 1 == $len) {
        print $FH "\n";
        last;
    }
    if ($i % 8 == 7) {
        printf $FH ",\n";
    }
    else {
        print $FH ", ";
    }
}

print $FH <<_EOL;
};

/* end of code */
_EOL

close $FH;

__END__
