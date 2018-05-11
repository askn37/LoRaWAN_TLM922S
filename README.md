# LoRaWAN_TLM922S

Kiwi Technology TLM922S-P01A LoRaWAN Module シリーズのための実装（for Arduino IDE）

# 概要

本Arduino用ライブラリは Kiwi Technology社の製造する
TLM922S-P01A LoRaWAN Module シリーズのための Arduino IDE 用実装である。

この TLM922S-P01A LoRaWAN Module の基本操作は、ターミナル対話型になっている。
このため PCに直結して試しに人間が直接読み書きするにはわかりやすいのだが、
大量の文字列解析処理を伴うので ATコマンド実装のように自動処理化するのは難しい。
ましてや CPU速度もメモリ量も制約の多い Arduino / 8bit AVR ではかくもいわんやである。

本Arduino用ライブラリは、LoRaWAN操作コマンドを抽象化したメソッド群を備え、
プロンプト応答を列挙型にマップして、スケッチから扱いやすくしたものである。
組み込み用途向けに対応コマンドはよく使うものに限り、フットプリントとSRAM消費を節約した。
一般的な用法では、8KB程度のコードと、0.5KB程度のメモリ空きがあれば動作する。
これは Arduino UNO R3 で、GPSと SDシールドを併用できるレベルである。

日本向け Region AS923 に対応した TLM922S-P01A を搭載した製品は、日本では以下で販売されている。
いずれも Kiwi Tech 製造の正規 OEM品だが、書き込まれている Firmwareには各社毎に差異がある。

- SenseWay Inc. [ADB922S](http://amzn.asia/6cFHc8C)
- SORACOM / A-BIT [AL-050](https://soracom.jp/products/lora/al-050/)

いずれも推奨動作電圧は 3.3Vだが、5Vトレラントであるかは不明であるものの、5V系の Arduinoに直結しても壊れることはない。
ただし互換機によっては 3.3V出力電力が要求に満たず（純正 UNO R3が 150mAに対し、50mA以下の物がある）正常動作を害する。
安定して運用するには 150mA以上の定電圧レギュレータと、33uF以上の平滑コンデンサを必要とする。

なお TLM922S は ClassB ダウンリンクに準拠してはいるが、本ライブラリは ClassAのみの対応となる。

SenseWay の製品は、ClassCの Rx2-Rx受信フローに対応していないと思われる。
（有効化コマンドがユーザに解放されていない）

# LoRaWANについて

LoRaWAN は前提として半二重ハンドシェイク通信方式をとる。
まず事前に joinコマンドで LoRaWANゲートウェイとのセッションを宣言したあとは、
txコマンドで指定のポート番号にペイロードデータをアップリンクするのを適宜繰り返す。
LoRaWANゲートウェイはアップリンクを受け取ると、
単にバックエンドサーバへペイロードを転送するだけか、
加えて LoRaWANデバイスへ ACK応答をダウンリンクで返すか、
さらに LoRaWANデバイス向けのダウンリンクペイロードをキューに保持しているならば
それを LoRaWANデバイスへダウンリンクする。

アップリンクとダウンリンクは共に同じ周波数チャンネルを使用するのが特徴的で、
Bluetooth/WiFi/3G/LTE等はいずれもアップ・ダウンに
別のチャンネルを使用することで全二重通信を実現しているのとは対照的である。
LoRaWANの通信方式はトランシーバーや、外惑星探査機のそれに近い。
さらにアップ・ダウン両パケットとも、相手に必ず届くことを期待も保証もしておらず
これはインターネットにおけるUDP方式と照応するものだ。
電波帯域の活用も QoSに当たるものがなく、ベストエフォートのみとなる。

LoRaWAN はあえてこの方式を採用しており、
ペイロードデータ量も小さく制限し、
かつエラー訂正を強化しつつビットレートをごく低く抑えることで、
乾電池駆動による数mW/hの低送信電力でありながら、
直線見通しと受信側アンテナの利得次第では 100kmにも達する超遠距離通信を可能にしている。
これにより単一基地局（ゲートウェイ）で
数千から数万デバイスのアップリンクを処理することも難しくはない。
つまりわずかなデータを間欠的に短時間送信するだけの廉価なセンサーデバイスを大量に配布し、
それを集約収集・分析する用途に特化している。

# Arduino IDE への導入

1. .ZIPアーカイブをダウンロードする。[Click here](https://github.com/askn37/LoRaWAN_TLM922S/archive/master.zip)

2. ライブラリマネージャで読み込む

	スケッチ -> ライブラリをインクルード -> .ZIP形式のライブラリをインストール...

3. 依存関係があるライブラリも同様に読み込む

	[MultiUART](https://github.com/askn37/MultiUART) -- SoftwareSerial で代用も可能（後述）

# 使い方

## とっかかり

```c
#include <Arduino.h>
#include <LoRaWAN_TLM922S.h>

#define TX_PIN	12	// D12 O to I TLM_MISO/RX(12)
#define RX_PIN	11	// D11 I to O TLM_MOSI/TX(11)

LoRaWAN_TLM922S LoRaWAN(RX_PIN, TX_PIN);

void setup (void) {
    while (!Serial);
    Serial.begin(9600);

    LoRaWAN.begin(9600);
    while (!LoRaWAN.getReady())     delay(1000);
    while (!LoRaWAN.factoryReset()) delay(1000);

    while (!(LoRaWAN.join(JOIN_OTAA) &&
             LoRaWAN.joinResult())) delay(1000);
}

void loop (void) {
    uint32_t ms = millis();		// or user senser data

    if (LoRaWAN.tx(TX_UCNF, 1)) {
        LoRaWAN.txData(ms);
        if (LoRaWAN.txRequest() &&
            LoRaWAN.txResult()) {
            Serial.println(F("tx_ok"));
        }
    }
    delay(10000);
}
```

これは OTAAで JOINし、unconfirmdモードで
32bitのデータ送信を約10秒間隔で実行する、単純なコードである。

各メソッドは TLM922S の UARTに向けてコマンドを発行し、その応答プロンプトを調べ、成功か否かを真偽値で返す。
エラー応答の詳細は getResult() で知ることが出来る。

joinおよび tx送信コマンドは、いずれも2段階の応答を返す。
すなわちコマンドの構文チェックなどを行う第1プロンプト応答と、
実際に電波の送信／受信に成功／失敗したことを示す第2プロンプト応答である。
それぞれの間には数秒の無応答時間があるため（その間 CPUは他の作業に回せる）
joinResult() と txResult() はこの第2プロンプトを受け付けるために使う。

tx送信コマンドは、さらに tx() txData() txRequest() の三つ組のメソッドで使用している。
それぞれ送信開始宣言、ペイロード構築、送信実行である。

# リファレンス

ここでは Arduinoスタイルではなく、型を含めた C/C++スタイルで各メソッドを記述する。

## コンストラクタ

### LoRaWAN_TLM922S::LoRaWAN_TLM922S (uint8\_t RX\_PIN, uint8\_t TX\_PIN)

クラスオブジェクト生成にはこのコンストラクタを使用する。
受信ピン、送信ピンの指定は必須である。
ピン番号は Arudiono で規定されたものとする。
それぞれのピンのIO設定は直ちに行われる。

```c
#define TX_PIN 12
#define RX_PIN 11

// もっぱらグローバルなオブジェクトとして
LoRaWAN_TLM922S LoRaWAN(RX_PIN, TX_PIN);
LoRaWAN.begin(9600);

// スコープから抜けると破棄されるようなオブジェクトとして
auto LoRaWAN = new LoRaWAN_TLM922S(RX_PIN, TX_PIN);
LoRaWAN->begin(9600);
```

本クラスは、規定値では MultiUART クラスを継承している。
これを SoftwareSerial に変えるには、
LoRaWAN_TLM922S.h ヘッダファイル内の以下の定義を変更する。

```c
//
// 継承するシリアルクラスを選択（MultiUART or SoftwareSerial）
//
#define LORAWAN_TLM922S_USED_MULTIUART
// #define LORAWAN_TLM922S_USED_SOFTWARESERIAL
```

### operator bool (void)

真偽値として評価された場合、通信準備ができているかを返す。
実際には常に真を返す。

```c
if (LoRaWAN) Serial.println("true");
```

## インターフェイスメソッド

UART送受信にかかわる操作を抽象化したメソッド群である。

### void begin (long SPEED)

UARTの通信ボーレートを設定し、Arduinoとモジュールとの通信を開始する。

```c
LoRaWAN.begin(9600);
```

工場出荷時初期値、および推奨値は 9600 である。

### void setEchoThrough (bool ECHO_MODE)

TLM922Sモジュールからエコーバックキャラクタが送られてきた場合に、
それをコンソールに転送するか、破棄するかを設定する。

```c
LoRaWAN.setEchoThrough(ECHO_ON);        // エコーバックスルー
LoRaWAN.setEchoThrough(ECHO_OFF);       // エコーバックドロップ（初期値）
```

転送先のコンソールは Serial である。
Serial.begin() がまだ未実行のときにエコーバックがあると、Arduino はハングアップするだろう。

Serial 以外を使用するには、
LoRaWAN_TLM922S.h ヘッダファイル内の以下の定義を変更する。
あるいはヘッダファイル読込前に、このプリプロセッサ変数を定義する。

```c
//
// エコーバックスルー先コンソールの指定（HardwareSerial）
//
#ifndef LORAWAN_TLM922S_DEBUG
#define LORAWAN_TLM922S_DEBUG Serial
#endif
```

### tlmps_t getResult (void)

最後の（最新の）TLM922S応答プロンプト文字列を、
tlmps_t列挙型（整数）にして返す。

```c
tlmps_t ps = LoRaWAN.getResult();
if (ps != PS_READY) {
    switch (ps) {
        case : PS_ON  { "..."; break; };
        case : PS_OFF { "..."; break; };
    }
}
```

tlmps_t の定義は以下の通り。

|定義値|メンバー名|種別|意味|対応プロンプト|
|---:|:---|---|:---|:---|
|0|PS_NOOP|制御|タイムアウト等||
|1|PS_READY|制御|入力待ち|\r> |
|2|PS_PREFIX|制御|パラメータ応答前置詞|\r>> |
|3|PS_ENDRESET|制御|リセット完了| <\n|
|4|PS_MODRESET|制御|再起動出力|\r> TLM922S <\n|
|5|PS_DEMODMARG|制御|復調マージン応答前置詞|\r>> DemodMargin = |
|6|PS_NBGATEWAYS|制御|ゲートウェイ応答前置詞|\r>> NbGateways = |
|7|PS_RX|制御|rx受信データ前置詞|\r>> rx |
|8|PS_OK|第1|コマンド受付完了|\r>> Ok|
|9|(未定義)||||
|10|PS_TXOK|第2|tx完了|\r>> tx_ok|
|11|PS_ACCEPTED|第2|join完了|\r>> accepted|
|12|PS_JOINED|第1|join中|\r>> joined|
|13|(未使用)||||
|14|PS_ON|第1|設定有効|\r>> on|
|15|PS_OFF|第1|設定無効|\r>> off|
|16|PS_INVALID|第1|構文エラー|\r>> Invalid|
|17|PS_UNKOWN|第1|不明なコマンド|\r>> Unknown command!|
|18|PS_ERR|第2|送受信失敗|\r>> err|
|19|PS_UNSUCCESS|第2|join失敗|\r>> unsuccess|
|20|PS_UNJOINED|第1|非join中|\r>> unjoined|
|21|PS_INVDLEN|第1|ペイロード長誤り|\r>> invalid_data_length|
|22|PS_KEYNOTINIT|第2|SHAキー未初期化|\r>> keys_not_init|
|23|PS_NOFREECH|第1|キャリアセンス失敗|\r>> no_free_ch|
|24|PS_BUSY|第1|RF応答なし|\r>> busy|
|25|PS_NOTJOINED|第1|join未実行|\r>> not_joined|
|26|(未使用)||||

TLM922S の UARTは、改行区切りが "\n\r" になっていることには注意が必要。

PS_MODRESET は部分文字列に PS_READY と PS_ENDRESET の両方を含んでいる特殊な応答である。
状況によっては正常な解釈はできない。

PS_RX は ClassA/ClassC のダウンリンクでは PS_TXOKとセットで現れるが、
ClassBのダウンリンクで発生した場合、
あらゆる局面で UART通信に割り込んでくる可能性がある。
検証が不完全のため、本ライブラリでは現状 ClassBは対応外とする。

### bool getReady (void)

TLM922Sがコマンド入力受付状態にあるかを調べ、可能なら真を返す。
具体的には "\r" を送信して、PS_READY が直ちに返却されれば真とみなす。

```c
while (!LoRaWAN.getReady());    // PS_READY
```

### int32_t getValue (void)

TLM922Sから取得した最後の数値データを返す。
取得に失敗していると -1 を返す。

```c
int32_t valueInt = LoRaWAN.getValue();
```

### uint8_t isData (void)

TLM922Sから文字列データが取得されているを調べる。
返されるのは保持しているキャラクタ数である。（最大値 242）

### String getData (void)

TLM922Sから取得した文字列データを String型で返す。

```c
if (LoRaWAN.isData()) {
    String resultData = LoRaWAN.getData();
}
```

## コントローラモジュールメソッド

TLM922Sの mod で始まるコマンド群を操作する。

### bool factoryReset (void)

TLM922Sを工場出荷時設定に戻す。
コマンド送信に成功すると真を返す。（==PS_OK）

```c
if (LoRaWAN.factoryReset()) {
    // LoRaWAN.getResult() == PS_OK;
}
```

### bool reset (void)

TLM922Sをリセットする。
成功すると TLM922Sは起動文字列（PS_MODRESET）を返すので、これを検出して真を返す。

```c
if (LoRaWAN.reset()) {
    // LoRaWAN.getResult() == PS_MODRESET;
}
```

### bool getVersion (void)

TLM922Sから最大 32byteのファームウェアバージョン文字列を取得する。
成功すると真を返し、getData() で文字列を取得できる。

```c
if (LoRaWAN.getVersion() &&
    LoRaWAN.isData()) {
    // LoRaWAN.getResult() == PS_PREFIX;
    String version = LoRaWAN.getData();
}
```

### bool getDevEUI (void)

TLM922Sから devEUI を取得する。
成功すると真を返し、getData() で 16文字の文字列を取得できる。

```c
if (LoRaWAN.getDevEUI() &&
    LoRaWAN.isData()) {
    // LoRaWAN.getResult() == PS_PREFIX;
    String deveui = LoRaWAN.getData();  // "0123456789abcdef"
}
```

devEUI値は、OTAAモードで joinするために必須のデバイス固有値である。
同じ値をゲートウェイが事前に知らなければ、LoRaWAN通信は開始できない。

### bool getAppsKey (void)

TLM922Sから アプリケーションキー を取得する。
成功すると真を返し、getData() で文字列を取得できる。

```c
if (LoRaWAN.getAppsKey() &&
    LoRaWAN.isData()) {
    // LoRaWAN.getResult() == PS_PREFIX;
    String deveui = LoRaWAN.getData();
}
```

AppsKeyは、ABPモードで txするために必須のデバイス固有鍵である。
同じ値をゲートウェイが事前に知らなければ、LoRaWAN通信は開始できない。

### bool sleep (uint16_t SECONDS)

TLM922Sをディープスリープモードにする。
スリープ時間には 1～65535秒と、0==無制限が指定できる。
不正な値には偽を返し、PS_INVALID となる。

このメソッドの実行前には、TLM922Sの D7/INT2ピンを LOWにしなければならない。
スリープは指定時間を経るか、D7/INT2ピンを HIGHにすることで解除される。
スリープの終了確認には、wakeUp() メソッドを用いる。

無限時間スリープさせて任意のアクションでスリープ解除する例。
```c
#define WAKE_PIN    7	// D7  O to I TLM_INT2/WakeUp/~Sleep(7)

pinMode(WAKE_PIN, OUTPUT);
digitalWrite(WAKE_PIN, LOW);

// コンソールにキー入力があるまでスリープさせ続ける
if (LoRaWAN.sleep(0)) {
    while (Serial.available()) Serial.read();
    Serial.println("Press any key, continue.");
    while(!Serial.available());

    digitalWrite(WAKE_PIN, HIGH);
    while (!LoRaWAN.wakeUp());  // PS_OK
}
```

指定時間だけスリープさせる例。
```c
#define WAKE_PIN    7	// D7  O to I TLM_INT2/WakeUp/~Sleep(7)

pinMode(WAKE_PIN, OUTPUT);
digitalWrite(WAKE_PIN, LOW);

// 10秒間スリープさせる
if (LoRaWAN.sleep(10)) {
    // スリープ終了を待つ
    while (!LoRaWAN.wakeUp());  // PS_OK
}
```

指定時間スリープと任意スリープ解除を併用したい場合は、
汎用ピン変化群割込を使用すると両立できる。
ただしこの方法は SoftwareSerial をインクルードした場合は使えない。

```c
#define RX_PIN	    11	// D11 I to O TLM_MOSI/TX(11)
#define WAKE_PIN    7	// D7  O to I TLM_INT2/WakeUp/~Sleep(7)

// Arduino UNO の D8～D13ピンに対応するピン変化群割込ハンドラ
volatile bool wakeup = false;
ISR(PCINT0_vect) {
    wakeup = true;
}

// D7 のピン変化群割込を有効にする
*digitalPinToPCMSK(RX_PIN) |= _BV(digitalPinToPCMSKbit(RX_PIN));
PCIFR |= _BV(digitalPinToPCICRbit(RX_PIN));
PCICR |= _BV(digitalPinToPCICRbit(RX_PIN));

pinMode(WAKE_PIN, OUTPUT);
digitalWrite(WAKE_PIN, LOW);

if (LoRaWAN.sleep(60)) {
    // Serialの受信バッファをクリア
    while (Serial.available()) Serial.read();

    // 割込フラグをクリア
    wakeup = false;

    // キー入力があるか
    while(!Serial.available()) {
        // 割込みあったらスリープ終了なのでここを抜ける
        if (wakeup) break;
    }

    // スリープ解除
    digitalWrite(WAKE_PIN, HIGH);
    while (!LoRaWAN.wakeUp());  // PS_OK
}
```

電力不足・規定電圧不足の時に本メソッドを実行すると、
まれに失敗してハードリセットがかかることがある。

### bool wakeUp (void)

TLM922Sのスリープが解除されたかを確認する。
スリープそのものを解除するわけではない。
成功すると真を返す（==PS_OK）

スリープが実行された瞬間にモジュールは停止するが、
本来その前に返ってくるべき応答（PS_OK）はスリープ解除後に受信バッファへ送られてくる。
このメソッドはそれを検出する。
（要するに応答を返しきる前に寝てしまう）

### void setBaudRate (long SPEED)

TLM922Sに通信ボーレート変更コマンドを送る。
コマンドが正常実行された時点で通信が切れてしまうため、このメソッドには返値がない。
例のように UARTの通信ボーレートを変え、getReady() で通信が正常再開されたかを確認するのがよい。

```c
LoRaWAN.setBaudRate(19200);
LoRaWAN.begin(19200);
while (!LoRaWAN.getReady());    // PS_READY
```

設定可能な値は 9600、19200、57600、115200 である。
28800、38400 は（なぜか）使えない。
しかし Arduino の SoftwareSerial（or MultiUART）を使用する限り、
使用可能なのは事実上 9600 か 19200 に限られる。
不正な値では PS_INVALID となる。

実は、設定した値どおりのボーレートを正しく出力することがないようで、数％の変動が生じている。
このため規定値の 9600bpsから変更しないほうがトラブルになりにくい
（これでも1～2％のスキューが生じている）

### bool setEcho (bool ECHO_MODE)

TLM922Sのエコーバック応答を設定する。
規定値は ECHO_ON である。
設定変更に成功すると真を返す（==PS_OK）

```c
LoRaWAN.setEcho(ECHO_ON);        // エコーバック開始（初期値）
LoRaWAN.setEcho(ECHO_OFF);       // エコーバック停止
```

得られたエコーバックをコンソールに送るかは setEchoThrough() で別に設定する。
両方を ECHO_ON にしなければ、コンソールには何も表示されない。

### bool modSave (void)

TLM922Sの不揮発メモリに、setBaudRate() と setEcho() の設定値を保存させる。
保存された設定は reset() および電源 ON/OFFしても維持される。
保存に成功すると真を返す（==PS_OK）

```c
if (LoRaWAN.modSave()) Serial.println("mod save ok");
```

電力不足・規定電圧不足の時に本メソッドを実行すると、
まれに失敗してハードリセットがかかることがある。
この場合の保存可否については保証されない（普通は失敗している）

## ラジオモジュールメソッド

TLM922Sの lorawan で始まるコマンド群を操作する。

### bool setDataRate (uint8_t DATARATE)

TLM922Sに DR値を設定する。
成功すると真を返す（==PS_OK）

```c
LoRaWAN.setDataRate(2);
```

### int8_t getDataRate (void)

TLM922Sから現在設定されている DR値を取得する。
返値は 0から 6の整数である。
取得に失敗すると -1 が返る。

### int16_t getMaxPayloadSize (int8_t DATARATE)

TLM922Sに指定の DR値で送受信可能な最大ペイロードサイズを問い合わせる。
返値は 0から 242の整数で、単位は byteである。

```c
int dr = LoRaWAN.getDataRate();
int size = LoRaWAN.getMaxPayloadSize(dr);
```

設定された DataRate値によって電波の送信出力と変調ファクターが変わり、
より大きな数値であるほどスループットが上がる。
LoRaWANでは 1回に送れる電波発信時間に制約があるため、
送信可能なペイロードサイズは DataRate値に応じて変化する。

Region AS923 では日本の電波法違反規定により、上限と設定が以下のように定まる。デフォルト値は DR2である。

|番号|送信出力|Dwell=0|Dwell=1|変調設定|ビットレート|
|---:|---:|---:|---:|---|---|
|DR0|13 dBm|51 byte|0 byte|SF12 BW125K|250 bps|
|DR1|11 dBm|51 byte|0 byte|SF11 BW125K|440 bps|
|DR2|9 dBm|51 byte|11 byte|SF10 BW125K|980 bps|
|DR3|7 dBm|115 byte|53 byte|SF9 BW125K|1760 bps|
|DR4|5 dBm|222 byte|125 byte|SF8 BW125K|3125 bps|
|DR5|3 dBm|222 byte|242 byte|SF7 BW125K|5470 bps|
|DR6|3 dBm|222 byte|242 byte|SF7 BW250K|11000 bps|

Dwell値（ドエルタイム＝休止時間挿入）はサブバンドによって制約が異なる。
Dwell=0 が選べるのは 923.4Mhz以下のチャネル限定という既定があるため、
TLM922Sでは汎用性の高い Dwell=1 が標準値である。
（ファームウェアによっては、設定変更自体が不可）

Dwell=0 で DR4以上が頭打ちなのはドエルタイムがないため
（送信時間×出力の上限を超えるので）電波法制約の打ち切りによる。

Dwell=1 でDR0/DR1のペイロードサイズがゼロバイトなのは、
13byteのヘッダも含めたパケットの送信が発信制限時間内に収まらないことによる。

DR6は送信に同時に2チャンネル分の帯域を使うことでビットレートを倍速にする方式だが、
ヘッダを含めた最大パケット長が 255byteなのでペイロードサイズ自体は DR5と変わらない。
また PS_NOCHFREE応答が返る可能性も高まるため、ほとんど実用価値はない。

瞬間最大ビットレートは以上の通りだが、
電波発信時間の10倍の強制休止時間を挟まなければならない規定があるため、
4秒に1パケット（DR6は2秒に1パケット）が最大スループットとなる。

### bool getAdr (void)

TLM922Sに設定された現在の Adaptive Data Rate スケジュール（ADRモード）設定を問い合わせる。
結果が PS_ONなら真を返す。

```c
if (LoRaWAN.getAdr()) Serial.println("lorawan get_adr on");
```

### bool setAdr (bool ADR_MODE)

TLM922Sに Adaptive Data Rate スケジュール（ADRモード）の使用を指示する。
成功すれば真（==PS_OK）

```c
LoRaWAN.setAdr(ADR_ON);		// ADRセット
LoRaWAN.setAdr(ADR_OFF);	// ADRクリア
```

ADRがセットされているあいだ、setDataRate() の設定は無視され、
getDataRate() の取得値も無意味となる。
データレートは適切な回数または時間経過により動的に調整される。
ADR_ON は固定局でかつ ABP_ON と併用するのが望ましい。

### bool loraSave (void)

TLM922Sの不揮発メモリに、setDataRate() と setAdr() の設定値を保存させる。
保存された設定は reset() および電源 ON/OFFしても維持される。
保存に成功すると真を返す（==PS_OK）

```c
if (LoRaWAN.loraSave()) Serial.println("lorawan save ok");
```

電力不足・規定電圧不足の時に本メソッドを実行すると、
まれに失敗してハードリセットがかかることがある。
この場合の保存可否については保証されない（普通は失敗している）

### bool join (bool JOIN_MODE)

TLM922Sに指定のモードで、joinリクエストの発行を指示する。
第1プロンプトが PS_OK であれば真が返り、joinResult() を実行することができる。

joinモードには JOIN_OTAA（Over the air activation）と、
JOIN_ABP（Activation By Personalization）とがある。

```c
LoRaWAN.join(JOIN_OTAA) && LoRaWAN.joiResult();
// または
LoRaWAN.join(JOIN_ABP)  && LoRaWAN.joiResult();
```

OTAAは join() 実行時に一度だけ共通鍵暗号化を決定し、以後の tx() にはそれを使用する。
ABPは join() では何もせず、tx() 毎に暗号化処理を再計算する（パーソナライズする）
後者はデバイスだけでなくゲートウェイ／サーバにも相応の負担がかかる。
OTAAは電波環境が刻々と変わる移動体向き、
ABPは電波環境が不変の固定局向きである。

getResult() の返値は以下の通り。

|真偽|メンバー名|意味|
|---|---|---|
|true|PS_OK|コマンド受領・送信開始|
|false|PS_INVALID|コマンド不正|
|false|PS_KEYNOTINIT|AppsKey等未設定|
|false|PS_NOFREECH|キャリアセンス失敗|
|false|PS_BUSY|RFモジュールが応答しない|
|false|PS_MODRESET|再起動|
|false|PS_NOOP|UART応答なし(タイムアウト)|

電力不足・規定電圧不足の時に本メソッドを実行すると、
まれに失敗してハードリセットがかかることがある。

### bool joinResult (void)

TLM922Sに指示したjoinコマンドの、実行結果を受け取る。
第2プロンプトが PS_ACCEPTED であれば真が返る。

```c
Serial.println(F("try join otaa"));
while (!(LoRaWAN.join(JOIN_OTAA) &&
         LoRaWAN.joinResult())) delay(1000);
Serial.println(F("accepted"));
```

getResult() の返値は以下の通り。

|真偽|メンバー名|意味|
|---|---|---|
|true|PS_ACCEPTED|join成功|
|false|PS_UNSUCCESS|join失敗|
|false|PS_NOOP|UART応答なし(タイムアウト)|

join() + joinResult() を実行する前に factoryReset() を実行するのが望ましい。
joinに成功するとゲートウェイから使用してもよい電波帯チャンネルリストがダウンリンクされ、
現在デバイスが保持しているチャンネルリストに「追加」される。
使用可能なチャンネルはjoin先のゲートウェイによって異なり、
あるチャンネルは他のゲートウェイでは使えないということもある。

### bool getDevAddr (void)

TLM922Sから最新の DevAddr文字列を取得する。
成功すると真を返す（==PS_PREFIX）

```c
if (LoRaWAN.getDevAddr() &&
    LoRaWAN.isData()) {
    // LoRaWAN.getResult() == PS_PREFIX;
    String devaddr = LoRaWAN.getData();
}
```

DevAddr値は、
OTAA では joinするたびに、
ABP では txするたびに変化する。

### uint32_t getUpCount (void)

TLM922Sから現在のアップリンクカウント値を取得する。
アップリンクカウント値は join実行成功時に 1に初期化され、
以後 tx実行時にインクリメントされる。

```c
uint32_t upcount = LoRaWAN.getUpCount(dr);
```

この値は 32bit幅だが、
送信パケット内のフィールド幅は 16bitしかないため
ゲートウェイ／サーバとは調歩同期を行っている。
同期が外れるとゲートウェイ側は以後の txをドロップする。

### uint32_t getDownCount (void)

TLM922Sから現在のダウンリンクカウント値を取得する。
ダウンリンクカウント値は join実行成功時に 0に初期化され、
以後ダウンリンクを受けとるたびにインクリメントされる。

```c
uint32_t downcount = LoRaWAN.getDownCount(dr);
```

### bool setLinkCheck (void)

TLM922Sにリンクチェック（MACレイヤーテスト）を指示する。
成功すると真を返す（==PS_OK）

このメソッドは通信品質試験用である。通常の運用において使用するべきではない。

リンクチェックが指示されると
TLM922Sは次回の tx送信パケットに 1byteの拡張ペイロードを付加する。
このため getMaxPayloadSize() で得られる値より
1byte少ないペイロードデータしか送れなくなるので注意すること。

### bool tx (bool TX_MODE, uint8_t FPORT)

TLM922Sに送る txコマンドの送信準備を行う。
成功すると真を返す。

```c
if (!tx(TX_UCNF, 1)) return false;
```

getResult() の返値は以下の通り。

|真偽|メンバー名|意味|
|---|---|---|
|true|PS_READY|コマンド受付可|
|false|PS_NOOP|UART応答なし(タイムアウト)|

TX_MODE には TC_UCNF（unconfirmd）か TX_CNF（confirmd）を指定する。
TX_CNF はゲートウェイ／サーバに ACKダウンリンクを要求し、
当該 ACKを txResult() が受信できない場合は PS_ERR となる。

FPORTには任意のアプリケーションポート番号を指定する。
指定可能範囲は 1から 223で、それ以外は予約番号である。

### void txData (char* HEXSTRING)

TLM922Sにペイロードデータを直接送る。
返値はない。

このメソッドは write(char* str) メソッドと等価である。
書き込むデータは HEX文字列でなければならない。

### void txData (char* HEXSTRING, int BYTELENGTH)

TLM922Sにペイロードデータを直接無変換で送る。
返値はない。

このメソッドは write(char* str, int len) メソッドと等価である。
書き込むデータは HEX文字列でなければならない。

### void txData (signed char HEXCHAR)

TLM922Sにペイロードデータを直接無変換で送る。
返値はない。

このメソッドは write(char character) メソッドと等価である。
書き込むデータは HEX文字列でなければならない。

### void txData (uint8_t VALUE)

TLM922Sへ、8bitの無符号整数を HEX文字列 2byte（bigendian）に変換して送る。
返値はない。

### void txData (int VALUE)

TLM922Sへ、16bitの符号付整数を HEX文字列 4byte（bigendian）に変換して送る。
返値はない。

### void txData (uint16_t VALUE)

TLM922Sへ、16bitの無符号整数を HEX文字列 4byte（bigendian）に変換して送る。
返値はない。

### void txData (long VALUE)

TLM922Sへ、32bitの符号付整数を HEX文字列 8byte（bigendian）に変換して送る。
返値はない。

### void txData (uint32_t VALUE)

TLM922Sへ、32bitの無符号整数を HEX文字列 8byte（bigendian）に変換して送る。
返値はない。

### void txData (uint32_t VALUE, NIBBLE)

TLM922Sへ、32bitの無符号整数（littleendian）の下位から指定桁数ニブルを取り出し
HEX文字列（bigendian）に変換して送る。
返値はない。

```c
uint32_t value = 0x12345678;
txData(value, 1);	// "8"
txData(value, 2);	// "78"         == txData((uint8_t) value);
txData(value, 3);	// "678"
txData(value, 4);	// "45678"      == txData((uint16_t) value);
txData(value, 5);	// "345678"
txData(value, 6);	// "2345678"
txData(value, 7);	// "12345678"
txData(value, 8);	// "012345678"  == txData((uint32_t) value);
```

### bool txRequest (void)

tx() および txData() で準備されたペイロードの送信を指示する。
第1プロンプトが PS_OK であれば真が返り、txResult() を実行することができる。

getResult() の返値は以下の通り。

|真偽|メンバー名|意味|
|---|---|---|
|true|PS_OK|コマンド受領・送信開始|
|false|PS_INVALID|コマンド不正|
|false|PS_NOTJOINED|join未実行|
|false|PS_NOFREECH|キャリアセンス失敗|
|false|PS_BUSY|RFモジュールが応答しない|
|false|PS_INVDLEN|ペイロードデータ長不正|
|false|PS_MODRESET|再起動|
|false|PS_NOOP|UART応答なし(タイムアウト)|

txData() で組み立てたペイロードデータがHEX文字列でない、
あるいは 2の倍数長ではない、
または getMaxPayLoadSize() の制限を超えている場合は PS_INVDLEN になる。

電力不足・規定電圧不足の時に本メソッドを実行すると、
まれに失敗してハードリセットがかかることがある。

### bool tx (bool TX_MODE, uint8_t FPORT, char* DATA)
### bool txRequest (bool TX_MODE, uint8_t FPORT, char* DATA)

この二つのメソッドはいずれも以下の三つ組メソッドと等価のショートカットである。
返値は txRequest() を参照のこと。

```c
// char data[] = "0123456789abcdef";
tx(confirm, fport);
txData(data);
return txRequest();
```

送信するペイロードデータは、HEX文字列に変換済でなければならない。

### bool txResult (void)

### bool isLinkCheck (void)
### int8_t getMargin (void)
### int8_t getGateways (void)
### int8_t getRxPort (void)




## ユーティリティ／実験的実装

## その他

Stream と Print クラスを継承しているため、以下のメソッドが機能する。

	print() println()

以下のメソッドは使えるはずだが、充分にはテストされていない。

	setTimeout() find() findUntil() readBytes() readBytesUntil()
	readString() readStringUntil() parseInt() parseFloat()

以下のコールバック関数宣言はサポートされない。

	serialEvent()

# 応用



# 既知の不具合／制約／課題

- 受信割込、serialEvent() に相当する機能がない。
- 主要な AVR 以外はテストされていない。
- 古い Arduino IDE には対応しない。1.8.5で動作確認。少なくとも C++11 は使えなければならない。
- 英文マニュアルが未整備である。

# 改版履歴

- 0.1.1

# 使用許諾

MIT

# 著作表示

朝日薫/askn
(SenseWay Inc.)
Twitter: [@askn37](https://twitter.com/askn37)
GitHub: https://github.com/askn37
