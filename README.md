# LoRaWAN_TLM922S

Kiwi Technology TLM922S-P01A LoRaWAN Module シリーズのための実装（for Arduino IDE）

# 概要

本Arduino向けライブラリは Kiwi Technology社の製造する
TLM922S-P01A LoRaWAN Module シリーズのための Arduino IDE 用実装である。

この TLM922S-P01A LoRaWAN Module の基本操作は、ターミナル対話型になっている。
このため PCに直結して、試しに人間が直接読み書きするにはわかりやすいのだが、
大量の文字列解析処理を伴うため、ATコマンド実装のように自動処理化するのは難しい。
ましてや CPU速度もメモリ量も制約の多い Arduino / 8bit AVR ではかくもいわんやである。

本Arduino向けライブラリは、LoRaWAN操作コマンドをラップしたメソッド群を備え、
プロンプト応答をメタコード化して、スケッチから扱いやすく工夫したものである。
コマンドとメソッドは基本的に 1対1で直交している。

一方でフットプリントの削減と小メモリ化のためもあり、対応コマンドは最小限に絞った。
特に、日本で使用すると電波法に抵触する可能性のある電波設定系コマンド等には対応しない。

一般的な用法では、8KB程度のフットプリントと、0.5KB程度の SRAM空き容量があれば動作する。
これは Arduino UNO R3 で、GPSと SDシールドをどうにか併用できるレベルである。

LoRaWANがとはなにか、についてはここでは触れないが、
他の電波方式とはおおむね次のような棲み分けである。

|#|BLE/Bluetooth|WiFi|3G/LTE|LoRaWAN|
|---|---|---|---|---|
|電波帯|2.4Ghz|2.4G/5Ghz|～1Ghz～|920Mhz（元アマ無線帯）|
|実用通信距離|近距離（～10m）|短距離（～50m）|中距離（～2km）|**超遠距離**（～100km）|
|スループット|超低速～低速（～10kbps）|超高速（ギガ級）|高速（メガ級）|**超低速**（～100bps）|
|情報量|小|超大|中|超小|
|消費電力|超々低～超低（～数mA）|高（数百mA）|高（数百mA）|低（～数10mA）|
|電波出力|微小|小～中|中～高|小～大|
|電波免許|不要（微小なので）|不要または屋内限定|免許者と許諾契約|用途により国家申請|
|基地対端末比|1:5～10|1:10～20|1:1000～5000|1:10000～50000|
|端末運用コスト|低（購入費）|中（主に電力）|超高（通信料）|低（購入＋通信費）|
|適不適|オーディオや操作機器|なんでも|頑張れば|センサー収拾一択|

日本向け Region AS923 に対応した TLM922S-P01A を搭載した製品は、日本では以下で販売されている。
いずれも Kiwi Tech 製造の正規 OEM品だが、書き込まれている Firmwareには各社毎に差異がある。

- SenseWay Inc. [ADB922S](http://amzn.asia/6cFHc8C)
- SORACOM / A-BIT [AL-050](https://soracom.jp/products/lora/al-050/)

いずれも推奨動作電圧は 3.3Vだが、5V系の Arduinoに直結しても壊れることはない。
ただし互換機によっては 3.3V出力電力が要求に満たず（純正 UNO R3が 150mAに対し、50mA以下の物がある）正常動作を害する。
安定して運用するには 150mA以上の定電圧レギュレータと、33uF以上の平滑コンデンサを必要とする。

# Arduino IDE への導入

1. .ZIPアーカイブをダウンロードする。[Click here](https://github.com/askn37/LoRaWAN_TLM922S/archive/master.zip)

2. ライブラリマネージャで読み込む

	スケッチ -> ライブラリをインクルード -> .ZIP形式のライブラリをインストール...

3. 依存関係があるライブラリも同様に読み込む

	[MultiUART](https://github.com/askn37/MultiUART) -- SoftwareSerial で代用も可能（後述）

# 使い方

## とっかかり

OTAAで JOINし、unconfirmdモードで 32bitのデータ送信を約10秒間隔で実行する、
単純な例を示す。

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

getReady() は TLM922Sモジュールがコマンドを受け付けられる状態になるのを確認するときに使う。

join() の前に factoryReset() を行っているのは必ずしも必要ではないが、
以前の通信パラメータの残滓をすべて破棄して
次の join() 実行の確実を期すためである。
join() は一種の認証手続きであり、サーバとのセッションを確立する。

join() には JOIN_OTAA または JOIN_ABP いずれかのモード指定を指示する。
OTAAはセッション確立時にのみ一意の初期化を行う軽量通信モード、
ABPは txRequest() 実行毎に暗号化処理と通信パラメータの更新を行うモードである。
どちらか（あるいはいずれもが）使えるかは LoRaWANゲートウェイ／サーバ側の通信設定に依存する。
そして多くは特定のデバイスに付き、どちらかしか受け付けないようにされているだろう。

joinおよび tx送信コマンドは、いずれも2段階の応答を返す。
すなわちコマンドの構文チェックなどを行う第1プロンプト応答と、
実際に電波の送信／受信に成功／失敗したことを示す第2プロンプト応答である。
それぞれの間には数秒の無応答時間があるため（その間に CPUは他のことが出来るので）
joinResult() と txResult() はこの第2プロンプトが返ってくるのを待つために使う。

tx送信コマンドは、ここでは tx() txData() txRequest() の三組メソッドで使用している。

tx() メソッドは TX_UCNFまたは TX_CNFのモード指定と、送信ポート番号を指示する。
送信ポート番号は 0および 250前後以上の特別な予約番号があり、それらは指定ができない。
それ以外は続くペイロードデータと共にサーバ側で読み取ることができ、データ内容の処理分別に自由に使用できる。

txData() メソッドは送信ペイロードを準備する。
ペイロードは最終的にはバイナリデータに組み立てられるため、もろもろのデータ型を区別して受け付けるように設計されている。
ペイロードサイズは送受信のために選択された周波数変調設定（データレート）によって上限が規定されている。

txRequest() メソッドはそれらの情報を取りまとめて、
TLM922Sモジュールに渡して実際の送信コマンドを発行し、第1プロンプトを受け取る。
送信実行結果である第2プロンプトは、txResult() メソッドで受け取られる。

# リファレンス

ここでは Arduinoスタイルではなく、型を含めた C/C++スタイルで各メソッドを記述する。

## コンストラクタ

### LoRaWAN_TLM922S::LoRaWAN_TLM922S(uint8\_t RX\_PIN, uint8\_t TX\_PIN)

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

### operator LoRaWAN_TLM922S::bool (void)

真偽値として評価された場合、通信準備ができているかを返す。
実際には常に真を返す。

```c
if (LoRaWAN) Serial.println("true");
```

## インターフェイスメソッド

### void begin (long SPEED)

UARTの通信ボーレートを設定する。

```c
LoRaWAN.begin(9600);
```

工場出荷時初期値、および推奨値は 9600 である。

### void setEchoThrough (bool ECHO)

TLM922Sモジュールからエコーバックキャラクタが送られてきた場合に、
それをコンソールに転送するか、破棄するかを真偽値で設定する。

```c
LoRaWAN.setEchoThrough(ECHO_ON);        // エコーバックスルー
LoRaWAN.setEchoThrough(ECHO_OFF);       // エコーバックドロップ（初期値）
```

転送先のコンソールは Serial である。
Serial.begin() が未実行のときにエコーバックがあると、Arduino はハングアップする。

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

### tlmps_t getResult(void)

最後の（最新の）TLM922Sモジュール応答プロンプト文字列を、
timps_t列挙型（整数）にして返す。

```c
timps_t ps = LoRaWAN.getResult();
if (ps != PS_READY) {
    switch (ps) {
        case : PS_ON  { "..."; break; };
        case : PS_OFF { "..."; break; };
    }
}
```

timps_t は以下のように定義されている。

|番号|メンバー名|種別|意味|対応プロンプト|
|---:|:---|---|:---|:---|
|0|PS_NOOP|制御|タイムアウト等|（空）|
|1|PS_READY|制御|入力待ち|\r> |
|2|PS_PREFIX|制御|パラメータ応答前置詞|\r>> |
|3|PS_ENDRESET|制御|リセット完了| <\n|
|4|PS_MODRESET|制御|機種名表示|\r> TLM922S <\n|
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

TLM922S の UARTは、改行区切りが "\n\r" になっているため、
これを考慮しなければならない。

### bool getReady (void)

TLM922Sモジュールがコマンド入力受付状態にあるかを調べ、可能なら真を返す。
具体的には "\r" を送信して、PS_READY が直ちに返却されれば真とみなす。

```c
while (!LoRaWAN.getReady());    // PS_READY
```

### uint32_t getValue (void)

TLM922Sモジュールから取得した最後の数値データを返却する。

```c
uint32_t valueInt = LoRaWAN.getValue();
```

### String getData (void)

TLM922Sモジュールから取得した最後の文字列データを返却する。

```c
if (LoRaWAN.isData()) {
    String resultData = LoRaWAN.getData();
}
```

### bool isData (void)

TLM922Sモジュールから文字列データが取得されているかを調べる

```c
if (LoRaWAN.isData()) {
    String resultData = LoRaWAN.getData();
}
```

## コントローラモジュールメソッド

### bool factoryReset (void)

TLM922Sモジュールを、工場出荷時設定に戻す。
コマンド送信に成功すると真を返す。（==PS_OK）

```c
if (LoRaWAN.factoryReset()) {
    // LoRaWAN.getResult() == PS_OK;
}
```

### bool getVersion (void)

TLM922Sモジュールからファームウェアバージョンを取得する。
成功すると真を返し、getData() で文字列を取得できる。

```c
if (LoRaWAN.getVersion() &&
    LoRaWAN.isData()) {
    String version = LoRaWAN.getData();
}
```

### bool getDevEUI (void)

TLM922Sモジュールから devEUI を取得する。
成功すると真を返し、getData() で 16文字の文字列を取得できる。

```c
if (LoRaWAN.getDevEUI() &&
    LoRaWAN.isData()) {
    String deveui = LoRaWAN.getData();  // "0123456789abcdef"
}
```

### bool sleep (uint16_t SECONDS)

TLM922Sモジュールをディープスリープモードにする。
スリープ時間には 1～65535秒と、0==無制限が指定できる。

このメソッドの実行前には、TLM922Sモジュールの D7/INT2ピンを LOWにしなければならない。
スリープは指定時間を経るか、D7/INT2ピンを HIGHにすることで解除される。
スリープの終了確認には、wakeUp() メソッドを用いる。

```c
pinMode(7, OUTPUT);
digitalWrite(7, LOW);

// 10秒間スリープさせる
if (LoRaWAN.sleep(10)) {
    while (!LoRaWAN.wakeUp());  // PS_OK
    // 10秒経過
}
```

```c
pinMode(7, OUTPUT);
digitalWrite(7, LOW);

// コンソールにキー入力があるまでスリープさせ続ける
if (LoRaWAN.sleep(0)) {
    while (Serial.available()) Serial.read();
    Serial.println("Press any key, continue.");

    digitalWrite(7, HIGH);
    while (!LoRaWAN.wakeUp());  // PS_OK
}
```

### bool wakeUp (void)

TLM922Sモジュールのスリープ解除を確認する。
成功すると真を返す（==PS_OK）

スリープが実行された瞬間にモジュールは停止するが、
本来その前に返ってくるべき応答（PS_OK）はスリープ解除後に受信バッファへ送られてくる。
このメソッドはそれを検出する。
（要するに通信完了前に寝てしまう）

### void setBaudRate (long SPEED)

コントローラに通信ボーレート変更コマンドを送る。
コマンドが実行された時点で通信が切れるため、このメソッドに返値はない。

```c
LoRaWAN.setBaudRate(19200);
LoRaWAN.begin(19200);
while (!LoRaWAN.getReady());    // PS_READY

```

設定可能な値は 9600、19200、57600、115200 である。
しかし Arduino の SoftwareSerial（or MultiUART）を使用する限り、
使用可能なのは 9600 か 19200 に限られる。

## ラジオモジュールメソッド





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
