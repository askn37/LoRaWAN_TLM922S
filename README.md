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

一般的な用法では、9KB程度のフットプリントと、1KB程度の SRAM空き容量があれば動作する。
これは Arduino UNO R3 で、GPS とMicroSDシールドをどうにか併用できるレベルである。

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
単純な例を示す。

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

getReady() は LoRaWANモジュールがコマンドを受け付けられる状態になるのを確認するときに使う。

join() の前に factoryReset() を行っているのは必ずしも必須ではないが、
以前の通信パラメータの残滓をすべて破棄して
次の join() 実行の確実を期すためである。
join() は一種の認証手続きであり、サーバとのセッションを確立する。

join() には JOIN_OTAA または JOIN_ABP いずれかのモード指定を指示する。
OTAAはセッション確立時にのみ一意の暗号化キー交換を行う軽量通信モード、
ABPは txRequest() 実行毎に異なるスクランブルを行う高暗号強度モードである。
いずれが使えるかはゲートウェイ（サーバ）側の通信設定に依存する。
（そして多くは特定のデバイスに付き、どちらかしか受け付けないだろう）

joinおよび tx送信コマンドは、いずれも2段階の応答を返す。
すなわちコマンドの構文チェックなどを行う第1プロンプト応答と、
実際に電波の送信／受信に成功したことを示す第2プロンプト応答である。
それぞれの間には数秒の無応答時間があるため（その間に CPUは他のことが出来るので）
joinResult() と txResult() はこの第2プロンプトが返ってくるのを待つために使う。

tx送信コマンドは、ここでは tx() txData() txRequest() の三組メソッドで使用している。

tx() メソッドは TX_UCNFまたは TX_CNFのモード指定と、送信ポート番号を指示する。
送信ポート番号は 0および 250前後以上の特別な予約番号があり、それらは指定ができない。
それ以外は続くペイロードデータと共にサーバ側で読み取ることができ、データ内容の処理分別に自由に使用できる。

txData() メソッドは送信ペイロードを準備する。
ペイロードは最終的にはバイナリデータに組み立てられるため、もろもろのデータ型を区別して受け付けるように設計されている。
ペイロードサイズは送受信のために選択された周波数変調設定（データレート）によって上限が規定されている。

txRequest() メソッドはそれらの情報を取りまとめて、
LoRaWANモジュールに渡して実際の送信コマンドを発行し、第1プロンプトを受け取る。
送信実行結果である第2プロンプトは、txResult() メソッドで受け取られる。

# リファレンス

ここでは Arduinoスタイルではなく、型を含めた C/C++スタイルで各メソッドを記述する。

## コンストラクタ

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
