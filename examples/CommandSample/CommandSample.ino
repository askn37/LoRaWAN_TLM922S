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

#define SET_ADR     false        // adavtipe datarate mode
// or
#define SET_DR      3           // manual datarate

#define SET_FPORT   1
#define SET_TX      TX_CNF
#define SET_JOIN    JOIN_OTAA
#define SET_LCHK    false

#define LORAWAN_BAUD	9600
#define CONSOLE_BAUD	9600

// ピンレベル変化汎用割込
// このコードは SoftwareSerial とは共存できない
volatile bool wakeup = false;
ISR(PCINT0_vect) {              // handle pin change interrupt for D8 to D13 here
    wakeup = true;
}

// LoRaWANオブジェクト作成
LoRaWAN_TLM922S LoRaWAN(RX_PIN, TX_PIN);

void setup (void) {
    bool f;

    // RX_PINのレベル変化割込を有効にして、スリープ終了を検出可能にする
    *digitalPinToPCMSK(RX_PIN) |= _BV(digitalPinToPCMSKbit(RX_PIN));
    PCIFR |= _BV(digitalPinToPCICRbit(RX_PIN));
    PCICR |= _BV(digitalPinToPCICRbit(RX_PIN));

    // コンソール出力を有効化
    while (!Serial);
    Serial.begin(CONSOLE_BAUD);
    Serial.println(F("Startup"));
    Serial.print(F("FREQ_CPU:"));
    Serial.println(F_CPU);

    // LoRaWANモジュールに接続し
    // エコーバック表示を有効化
    LoRaWAN.begin(LORAWAN_BAUD);
    LoRaWAN.setEchoThrough(ECHO_ON);

    // Wakeupピンを上げてスリープ解除
    pinMode(WAKE_PIN, OUTPUT);
    digitalWrite(WAKE_PIN, HIGH);

    // デバイスの準備ができるのを待つ
    while (!LoRaWAN.getReady()) {
        printResult();
        Serial.println(F("=getReady:0"));
        delay(1000);
    }
    printResult();

    // リセットコマンドを試す
    // 成功すれば真
    f = LoRaWAN.reset();
    Serial.print(F("=reset:")); Serial.println(f);
    printResult();

    // // ボーレート設定を試す
    // // 結果は得られないが
    // // 実行後は wakeUp()を使う
    // LoRaWAN.setBaudRate(LORAWAN_BAUD);
    // LoRaWAN.begin(LORAWAN_BAUD);
    // LoRaWAN.wakeUp();

    // ファームウェアバージョン文字列取得
    // 成功すれば真
    f = LoRaWAN.getVersion();
    Serial.print(F("=getVersion:")); Serial.println(f);
    printResult();
    if (f) {
        Serial.print(F("=isData:")); Serial.println(LoRaWAN.isData());
        if (LoRaWAN.isData()) {
            Serial.print(F("=getData:["));
            Serial.print(LoRaWAN.getData());
            Serial.println(']');
        }
    }

    // DevEUI文字列取得
    // 成功すれば真
    f = LoRaWAN.getDevEUI();
    Serial.print(F("=getDevEUI:")); Serial.println(f);
    printResult();
    if (f) {
        Serial.print(F("=isData:")); Serial.println(LoRaWAN.isData());
        if (LoRaWAN.isData()) {
            Serial.print(F("=getData:["));
            Serial.print(LoRaWAN.getData());
            Serial.println(']');
        }
    }

    // 現在の（以前のjoinの）DevAddr文字列取得
    // 成功すれば真
    f = LoRaWAN.getDevAddr();
    Serial.print(F("=getDevAddr:")); Serial.println(f);
    printResult();
    if (LoRaWAN.isData()) {
        Serial.print(F("=getData:["));
        Serial.print(LoRaWAN.getData());
        Serial.println(']');
    }

    // joinの前に、工場出荷時リセットを試す
    // 成功すれば真
    f = LoRaWAN.factoryReset();
    Serial.print(F("=factoryReset:")); Serial.println(f);
    printResult();

    // エコーバック無効化（ON=工場出荷時デフォルト）
    // 成功すれば真
    f = LoRaWAN.setEcho(ECHO_OFF);
    Serial.print(F("=setEcho:")); Serial.println(f);
    printResult();
    LoRaWAN.setEchoThrough(ECHO_OFF);

    // EEPROMに保存されたAppsKey文字列取得（メモリを大量に使う）
    // 成功すれば真
    f = LoRaWAN.getAllKey();
    Serial.print(F("=getAllKey:")); Serial.println(f);
    printResult();
    if (LoRaWAN.isData()) {
        Serial.print(F("=getData:["));
        Serial.print(LoRaWAN.getData());
        Serial.println(']');
    }

    // エコーバック有効化（ON=工場出荷時デフォルト）
    // 成功すれば真
    f = LoRaWAN.setEcho(ECHO_ON);
    Serial.print(F("=setEcho:")); Serial.println(f);
    printResult();
    LoRaWAN.setEchoThrough(ECHO_ON);

    // joinが成功するまでループ
    do {
        if (!f) delay(2000);

        // OTAAでjoinする
        // 第1プロンプト結果が Okなら真
        f = LoRaWAN.join(SET_JOIN);
        Serial.print(F("=join:")); Serial.println(f);
        if (f) {
            // 第2プロンプトを待つ
            // 結果が acceptedなら真
            f = LoRaWAN.joinResult();
            Serial.print(F("=joinResult:")); Serial.println(f);
        }
        printResult();
    } while (!f);

    // join後のDevAddr文字列取得（成功すれば以前と変わっている）
    // 成功すれば真
    f = LoRaWAN.getDevAddr();
    Serial.print(F("=getDevAddr:")); Serial.println(f);
    printResult();
    if (LoRaWAN.isData()) {
        Serial.print(F("=getData:["));
        Serial.print(LoRaWAN.getData());
        Serial.println(']');
    }

    if (SET_ADR) {
        // ADRをオンにする
        // 成功なら真
        f = LoRaWAN.setAdr(ADR_ON);
        Serial.print(F("=setAdr:")); Serial.println(f);
        printResult();
    }
    else {
        // DR値を設定する
        // 成功なら真
        f = LoRaWAN.setDataRate(SET_DR);
        Serial.print(F("=setDataRate:")); Serial.println(f);
        printResult();
    }

    // lorawan設定を保存する（DR値など）
    // 成功なら真
    f = LoRaWAN.loraSave();
    Serial.print(F("=loraSave:")); Serial.println(f);
    printResult();

    // module設定を保存する（baudrateなど）
    // 成功なら真
    f = LoRaWAN.modSave();
    Serial.print(F("=modSave:")); Serial.println(f);
    printResult();
}

void loop (void) {
    uint32_t v;
    bool f;

    if (SET_LCHK) {
        // リンクチェックを要求する
        // 成功なら真
        f = LoRaWAN.setLinkCheck();
        Serial.print(F("=setLinkCheck:")); Serial.println(f);
        printResult();
    }

    // txコマンドを準備する
    // 成功すれば真
    f = LoRaWAN.tx(SET_TX, SET_FPORT);
    Serial.print(F("=tx:")); Serial.println(f);
    printResult();

    // 送信データを準備する
    LoRaWAN.txData("cafe");     // HEXDATA直書き これは LoRaWAN.write("cafe") と同じ
    LoRaWAN.txData(micros());   // uint32_t型をHEX 8桁で準備

    // 送信を実行
    // 第1プロンプト結果が Okなら真
    f = LoRaWAN.txRequest();
    Serial.print(F("=tx:")); Serial.println(f);
    printResult();
    if (f) {
        // 第2プロンプトを待つ
        // 結果が tx_okなら真
        f = LoRaWAN.txResult();
        Serial.print(F("=txResult:")); Serial.println(f);
        printResult();

        // リンクチェックが得られたなら真（ucnfでもダウンリンク発生）
        Serial.print(F("=isLinkCheck:")); Serial.println(LoRaWAN.isLinkCheck());
        if (LoRaWAN.isLinkCheck()) {
            // マージン値とゲートウェイ数を取得
            Serial.print(F("=getMargin:")); Serial.println(LoRaWAN.getMargin());
            Serial.print(F("=getGateways:")); Serial.println(LoRaWAN.getGateways());
        }

        // rxダウンリンクデータが得られたなら真
        Serial.print(F("=isData:")); Serial.println(LoRaWAN.isData());
        if (LoRaWAN.isData()) {
            // rxポート番号とバイナリデータを取得
            Serial.print(F("=getRxPort:")); Serial.println(LoRaWAN.getRxPort());
            Serial.print(F("=getData:["));
            Serial.print(LoRaWAN.getData());    // HEXからBINに変換済
            Serial.println(']');
        }
    }

    // 現在の DR値を取得する
    // 成功なら真
    v = LoRaWAN.getDataRate();
    Serial.print(F("=getDataRate:")); Serial.println(v);
    printResult();

    // 現在の upCount値を取得する
    // 成功なら真
    // joinで 1にリセット、以後txする度に増加
    // ただし no_freechだった場合は増加しない
    v = LoRaWAN.getUpCount();
    Serial.print(F("=getUpCount:")); Serial.println(v);
    printResult();

    // 現在の downCount値を取得する
    // 成功なら真
    // joinで 0にリセット、以後ACK受信またはリンクチェック返答がある度に増加
    v = LoRaWAN.getDownCount();
    Serial.print(F("=getDownCount:")); Serial.println(v);
    printResult();

    // Sleepピンを下げて、スリープ可能にする
    digitalWrite(WAKE_PIN, LOW);

    // 60秒ディープスリープを実行
    // 成功なら真
    f = LoRaWAN.sleep(60);
    Serial.println();
    Serial.print(F("=sleep:")); Serial.println(f);
    // printResult();   // no prompt

    // 任意のキー入力がコンソールにあるまで待機
    Serial.println();
    Serial.println(F("[Push any key or 60sec after wakeup]"));

    // 現在までのキー入力を読み捨てる
    while(Serial.available()) Serial.read();

    // 割り込みフラグを初期化
    wakeup = false;

    // 新たなキー入力があるか、割り込みがあるまで動作停止
    while(!Serial.available()) {
        if (wakeup) break;
    }

    // Wakeupピンを上げてスリープ解除
    // 成功なら真
    digitalWrite(WAKE_PIN, HIGH);
    f = LoRaWAN.wakeUp();
    Serial.print(F("=wakeUp:")); Serial.println(f);
    printResult();

    // 以後繰り返し
}

// リザルトを取得して番号と名前を表示する
void printResult (void) {
    tlmps_t ps = LoRaWAN.getResult();
    Serial.print(F("=getResult:"));
    Serial.print(ps, DEC);
    Serial.write(':');
    switch (ps) {
        case PS_NOOP       : Serial.println(F("PS_NOOP")); return;
        case PS_READY      : Serial.println(F("PS_READY")); return;
        case PS_PREFIX     : Serial.println(F("PS_PREFIX")); return;
        case PS_ENDRESET   : Serial.println(F("PS_ENDRESET")); return;
        case PS_MODRESET   : Serial.println(F("PS_MODRESET")); return;
        case PS_DEMODMARG  : Serial.println(F("PS_DEMODMARG")); return;
        case PS_NBGATEWAYS : Serial.println(F("PS_NBGATEWAYS")); return;
        case PS_RX         : Serial.println(F("PS_RX")); return;
        case PS_OK         : Serial.println(F("PS_OK")); return;
        case PS_TXOK       : Serial.println(F("PS_TXOK")); return;
        case PS_ACCEPTED   : Serial.println(F("PS_ACCEPTED")); return;
        case PS_JOINED     : Serial.println(F("PS_JOINED")); return;
        case PS_ON         : Serial.println(F("PS_ON")); return;
        case PS_OFF        : Serial.println(F("PS_OFF")); return;
        case PS_INVALID    : Serial.println(F("PS_INVALID")); return;
        case PS_UNKOWN     : Serial.println(F("PS_UNKOWN")); return;
        case PS_ERR        : Serial.println(F("PS_ERR")); return;
        case PS_UNSUCCESS  : Serial.println(F("PS_UNSUCCESS")); return;
        case PS_UNJOINED   : Serial.println(F("PS_UNJOINED")); return;
        case PS_INVDLEN    : Serial.println(F("PS_INVDLEN")); return;
        case PS_KEYNOTINIT : Serial.println(F("PS_KEYNOTINIT")); return;
        case PS_NOFREECH   : Serial.println(F("PS_NOFREECH")); return;
        case PS_BUSY       : Serial.println(F("PS_BUSY")); return;
        case PS_NOTJOINED  : Serial.println(F("PS_NOTJOINED")); return;
        default            : Serial.println(F("???"));
    }
}

// end of code
