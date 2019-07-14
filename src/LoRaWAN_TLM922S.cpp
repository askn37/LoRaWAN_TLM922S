/***************
 *
 * LoRaWAN_TLM922S.cpp - Kiwi-tec.com TLM922S-P01A LoRaWAN Module connector for Arduino
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/LoRaWAN_TLM922S
 * maintainer: askn https://twitter.com/askn37
 *
 */

#include <Arduino.h>
#include "LoRaWAN_TLM922S.h"

//
// プライベートメソッド
//

// プロンプト解析
// 1キャラクタずつ与え、プロンプトが辞書に見つかったら
// そのインデックス番号＝プロンプト番号を返す
// 見つからなければ PS_NOOP
uint8_t LoRaWAN_TLM922S::parsePrompt (const uint8_t c) {
    static uint32_t prevword = 0;
    uint32_t hash;
    uint32_t newword = prevword | ((uint32_t)c << 16);
    uint16_t index = newword % PS_DICT_MAX;
    if (!c) return ( prevword = PS_NOOP );
    while (hash = pgm_read_dword_near(PS_DICT + index)) {
        prevword = index + 0x100;
        if (((hash - newword) << 8) == 0) {
            return ( hash >> 24 );
        }
        index = (index + 1) % PS_DICT_MAX;
    }
    prevword = c;
    return PS_NOOP;
}

// コマンド定型文送信
// コマンド番号を与えるとそれに応じた文字列を送信する
// 辞書から得られる文字列は逆順なので末尾から取り出す
void LoRaWAN_TLM922S::putCommand (const uint8_t command) {
    int s;
    uint16_t addr;
    uint16_t next;
    uint8_t c;
    if (command >= CMD_TABLE_LEN) return;
    _terminal = "";
    if (_rxData) {
        free(_rxData);
        _rxDataLen = 0;
        _rxData = nullptr;
    }
    addr = pgm_read_word_near(CMD_TABLE + command);
    if (!addr) return;
    while (addr >= 0x100) {
        addr -= 0x100;
        c = pgm_read_byte_near(CMD_DICT + addr++);
        next = pgm_read_byte_near(CMD_DICT + addr++)
            | (pgm_read_byte_near(CMD_DICT + addr) << 8);
        addr = next;
        _terminal += (char) c;
    }
    _terminal += (char) addr;
    s = _terminal.length();
    while (s > 0) write(_terminal[--s]);
}

// 分割時間待ち
// 指定値はms
// 指定値無しで前回の続き
bool LoRaWAN_TLM922S::wait (uint16_t timeout) {
    static uint32_t start;
    static uint16_t ms;
    if (timeout) {
        start = micros();
        ms = timeout;
        return true;
    }
    if (ms > 0) {
        // 127msの空きがあるなら yield() を呼ぶ
        if ((ms & 0x7F) == 0x7F) yield();
        while (ms > 0 && (micros() - start) >= 1000) {
            ms--;
            start += 1000;
        }
    }
    return (ms > 0);
}

// 次のプロンプトを待つ
// タイムアウトすると PS_NOOP を返す
// 見つかったプロンプトは this->_current
// 直前のプロンプトは this->_result
// タイムアウト既定値は10ms(10000)
tlmps_t LoRaWAN_TLM922S::nextPrompt (uint16_t timeout) {
    if (timeout) wait(timeout);
    while (wait()) {
        if (available()) {
            uint8_t c = read();
            pushEchoBack(c);
            uint8_t r = parsePrompt(c);
            if (r != PS_NOOP) {
                _result = _current;
                return (tlmps_t)(_current = r);
            }
        }
    }
    return PS_NOOP;
}

// 改行を1回打って PS_READY が現れるかを調べる
bool LoRaWAN_TLM922S::getReady (void) {
    while (available()) {
        read();
    }
    write('\r');
    uint8_t f = (nextPrompt(100) == PS_READY);
    putEchoBack();
    return f;
}

// いずれかのプロンプトが現れるまで進める
// 見つかったプロンプトを返す（this->_current）
// 直前のプロンプトは this->_result にある
tlmps_t LoRaWAN_TLM922S::skipPrompt (uint8_t ps1, uint8_t ps2, uint16_t timeout) {
    uint8_t r;
    while ((r = nextPrompt(timeout)) != PS_NOOP) {
        if (r == ps1 || r == ps2) break;
        timeout = 0;
    }
    return (tlmps_t)(r);
}

// 指定のコマンドを実行して PS_OK を確認する
// 第1プロンプト応答に指定時間待つ
bool LoRaWAN_TLM922S::runCommand (uint8_t command, uint16_t timeout) {
    if (!getReady()) return false;
    putCommand(command);
    uint8_t r = skipPrompt(PS_OK, PS_READY, timeout);
    uint8_t f = r == PS_OK;
    if (r != PS_READY) skipPrompt();
    putEchoBack();
    return f;
}

// 指定のコマンドを実行して PS_ON を確認する
// 第1プロンプト応答に指定時間待つ
bool LoRaWAN_TLM922S::runBoolCommand (uint8_t command, uint16_t timeout) {
    if (!getReady()) return false;
    putCommand(command);
    uint8_t r = skipPrompt(PS_ON, PS_OFF, timeout);
    uint8_t f = r == PS_ON;
    skipPrompt(timeout);
    putEchoBack();
    return f;
}

// 指定のコマンドを実行して 文字列結果 を取得する
// 第1プロンプト応答に指定時間待つ
bool LoRaWAN_TLM922S::getStringCommand (uint8_t command, uint16_t timeout) {
    if (!getReady()) return false;
    putCommand(command);
    return parseValue(true, timeout);
}

// 指定のコマンドを実行して 数値結果 を取得する
// 第1プロンプト応答に指定時間待つ
uint32_t LoRaWAN_TLM922S::getValueCommand (uint8_t command, uint16_t timeout) {
    if (!getReady()) return false;
    putCommand(command);
    return parseValue(false, timeout);
}

// 受信バッファの現在位置から10進数値を読み取る
// 空白を見つけたら読み捨てて次の文字で止める
uint32_t LoRaWAN_TLM922S::parseDecimal (void) {
    uint32_t value = 0;
    while (wait()) {
        if (available()) {
            uint8_t c = peek();
            if (isDigit(c)) {
                pushEchoBack(c);
                value *= 10;
                value += c - '0';
            }
            else if (c == ' ') {
                pushEchoBack(read());
                break;
            }
            else break;
            read();
        }
    }
    return value;
}

// 現在の位置から HEXDATA を読み取る
// 結果は _rxData に格納する
// 非 HexDigit を見つけたらその位置で止める
// エコーバックは行わない（代わりに取得文字数のサマリを出力する）
void LoRaWAN_TLM922S::parseHexData (void) {
    uint16_t recv = 0;
    uint8_t x = 0;
    while (wait()) {
        if (available()) {
            uint8_t c = peek();
            if (isHexadecimalDigit(c)) {
                c -= '0';
                if (c >= 17) c -= 7;
                if (c >= 42) c -= 32;
                c &= 15;
                if ((++recv) & 1)          x = c << 4;
                else if (_rxDataLen < 242) _rxData[_rxDataLen++] = x | c;
            }
            else break;
            read();
        }
    }
    if (_echo) {
        _echoBack += '*';
        _echoBack += _rxDataLen;
        _echoBack += '*';
    }
}

// 指定の取得結果を得られたら返す
// t=false で数値結果、t=true で文字列結果真偽
uint32_t LoRaWAN_TLM922S::parseValue (bool t, uint16_t timeout) {
    bool f = false;
    _value = -1;
    _terminal = "";
    wait(timeout);
    while (wait()) {
        if (available()) {
            uint8_t c = read();
            uint8_t r = parsePrompt(c);
            pushEchoBack(c);
            if (f) {
                if (c < ' ') {
                    f = false;
                    continue;
                }
                _terminal += (char) c;
            }
            if (r != PS_NOOP) {
                _result = _current;
                _current = r;
                switch (r) {
                    case PS_PREFIX :
                        if (t) f = true;
                        else _value = parseDecimal();
                        break;
                    case PS_READY : goto _parseStringLoop;
                }
            }
        }
    }
    _parseStringLoop:
    putEchoBack();
    return (t ? _result == PS_PREFIX : _value);
}

//
// パブリックメソッド
//
size_t LoRaWAN_TLM922S::write (const uint8_t c) {
    size_t r = writeRaw(c);
    if (available() &&
        peek() != '\r') {
        uint8_t c = read();
        if (_echo) LORAWAN_TLM922S_DEBUG.write(c);
    }
    return r;
}

// write時のエコーバックを有効・無効にする
void LoRaWAN_TLM922S::setEchoThrough (bool through) {
    _echo = through;
}

// リセットコマンドを実行する
bool LoRaWAN_TLM922S::reset (void) {
    if (!getReady()) return false;
    putCommand(EX_MOD_RESET);
    uint8_t r = skipPrompt(PS_MODRESET, PS_ENDRESET, 2000);
    uint8_t f = r == PS_MODRESET;
    if (r != PS_READY) skipPrompt();
    putEchoBack();
    return f;
}

// ディープスリープを実行する
bool LoRaWAN_TLM922S::sleep (uint16_t seconds) {
    if (!getReady()) return false;
    putCommand(EX_MOD_SLEEP);
    print(seconds, DEC);
    write('\r');
    while (available()) {
        if (peek() == '\r') break;
        pushEchoBack(read());
    }
    putEchoBack();
    return true;
}

// スリープから起きたことを確認する
bool LoRaWAN_TLM922S::wakeUp (void) {
    uint8_t r = skipPrompt(PS_OK, PS_READY);
    uint8_t f = r == PS_OK;
    if (r != PS_READY) skipPrompt();
    putEchoBack();
    return f;
}

// 通信ボーレートを変更する
void LoRaWAN_TLM922S::setBaudRate (long baudrate) {
    if (!getReady()) return;
    putCommand(EX_MOD_SET_BAUD);
    print(baudrate, DEC);
    write('\r');
}

//
// LoRaWAN通信
//

// DR値を設定する
bool LoRaWAN_TLM922S::setDataRate (uint8_t datarate) {
    if (!getReady()) return false;
    putCommand(EX_LORA_SET_DR);
    print(datarate, DEC);
    write('\r');
    uint8_t r = skipPrompt(PS_OK, PS_READY);
    uint8_t f = r == PS_OK;
    if (r != PS_READY) skipPrompt();
    putEchoBack();
    return f;
}

// DR値を指定して送信可能なペイロードサイズを取得する
int16_t LoRaWAN_TLM922S::getMaxPayloadSize (int8_t dr) {
    if (!getReady()) return false;
    putCommand(EX_LOLA_GET_MAX_PAY);
    print(dr, DEC);
    write('\r');
    return parseValue(false, 1000);
}

// join実行
// 第1プロンプト応答に 500ms待つ
bool LoRaWAN_TLM922S::join (bool abp) {
    if (!getReady()) return false;
    putCommand(abp ? EX_LORA_JOIN_ABP : EX_LORA_JOIN_OTAA);
    uint8_t r = skipPrompt(PS_OK, PS_READY);
    uint8_t f = r == PS_OK;
    // if (r != PS_READY) skipPrompt();
    putEchoBack();
    return f;
}

// join実行結果
// 第2プロンプト応答に最大 10000ms待つ
bool LoRaWAN_TLM922S::joinResult (void) {
    bool f = false;
    wait(10000);
    while (wait()) {
        if (available()) {
            uint8_t c = read();
            uint8_t r = parsePrompt(c);
            pushEchoBack(c);
            if (r != PS_NOOP) {
                _result = _current;
                _current = r;
                switch (r) {
                    case PS_ACCEPTED : f = true; break;
                    case PS_READY : goto _joinLoop;
                }
            }
        }
    }
    _joinLoop:
    putEchoBack();
    return f;
}

// tx送信準備
// 準備できなければ偽を返す
bool LoRaWAN_TLM922S::tx (bool confirm, uint8_t fport) {
    if (!getReady()) return false;
    putCommand(confirm ? EX_LORA_TX_CNF : EX_LORA_TX_UCNF);
    print(fport, DEC);
    write(' ');
    return true;
}

// tx送信データ
// 指定のニブル幅を倍整数下位からビッグエンディアン HEXDIGIT で送る
bool LoRaWAN_TLM922S::txData (uint32_t value, int nibble) {
    for (int i = 8; i > 0; i--) {
        if (i <= nibble) {
            uint8_t c = value >> 28;
            print(c, HEX);
        }
        value <<= 4;
    }
    return true;
}

// tx送信データ
// String型をビッグエンディアン HEXDIGIT で送る
bool LoRaWAN_TLM922S::txData (String str) {
    for (auto c : str) {
        print(c >> 4, HEX);
        print(c & 15, HEX);
    }
    return true;
}
bool LoRaWAN_TLM922S::txData (const char* str, int len) {
    for (int i = 0; i < len; i++) {
        print((uint8_t)str[i] >> 4, HEX);
        print((uint8_t)str[i] & 15, HEX);
    }
    return true;
}

// tx送信実行
// 第1プロンプト応答に 500ms待つ
bool LoRaWAN_TLM922S::txRequest (void) {
    write('\r');
    uint8_t r = skipPrompt(PS_OK, PS_READY);
    uint8_t f = r == PS_OK;
    if (r != PS_READY) skipPrompt();
    putEchoBack();
    return f;
}

// tx実行結果
// 第2プロンプト応答に最大 10000ms待つ
bool LoRaWAN_TLM922S::txResult (void) {
    uint8_t f = false;
    _margin = _gateways = -1;
    _rxPort = 0;
    wait(10000);
    while (wait()) {
        if (available()) {
            uint8_t c = read();
            uint8_t r = parsePrompt(c);
            pushEchoBack(c);
            if (r != PS_NOOP) {
                _result = _current;
                _current = r;
                switch (r) {
                    case PS_READY : goto _txLoop;
                    case PS_DEMODMARG :
                        _margin = parseDecimal();
                        break;
                    case PS_NBGATEWAYS :
                        _gateways = parseDecimal();
                        break;
                    case PS_TXOK :
                        f = true;
                        break;
                    case PS_RX : {
                        void *ptr = malloc(242);
                        if (ptr != nullptr) {
                            _rxDataLen = 0;
                            _rxData = (uint8_t*) ptr;
                            _rxPort = parseDecimal();
                            parseHexData();
                        }
                        break;
                    }
                }
            }
        }
    }
    _txLoop:
    putEchoBack();
    return f;
}

// end of code
