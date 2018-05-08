# LoRaWAN_TLM922S

Kiwi Technology TLM922S-P01A LoRaWAN Module シリーズのための実装（for Arduino IDE）

# 概要

本Arduino向けモジュールは Kiwi Technology社の製造する
TLM922S-P01A LoRaWAN Module シリーズのための Arduino IDE 用実装である。

この TLM922S-P01A LoRaWAN Module の基本操作は、ターミナル対話型になっている。
このため人間が直接読み書きするにはわかりやすいのだが、
大量の文字列解析処理を伴うため、ATコマンド実装のように自動処理化するのは難しい。
ましてや CPU速度もメモリ量も制約の多い Arduino / 8bit AVR ではかくもいわんやである。

本Arduino向けモジュールの実装は、LoRaWAN操作コマンドをラップしたメソッド群を備え、
プロンプト応答をメタコード化して、スケッチから扱いやすくしたものである。
コマンドとメソッドは 1対1で直交しており、乖離してはいない。

一方でフットプリントの削減と小メモリ化のためもあり、対応コマンドは最小限に絞った。
特に、日本で使用すると電波法に抵触する可能性のある電波設定系コマンド等には対応しない。

一般的な用法では、9KB程度のフットプリントと、1KB程度の SRAM空き容量があれば動作する。
これは Arduino UNO R3 で、GPS とMicroSDシールドがぎりぎりで併用できる程度である。

日本向け Region AS923 に対応した TLM922S-P01A を搭載した製品は、日本では以下で販売されている。
いずれも Kiwi Tech の正規 OEM品だが、書き込まれている Firmwareには各社毎の差異がある。

  SenseWay Inc. [ADB922S](http://amzn.asia/6cFHc8C)
  SORACOM / A-BIT [AL-050](https://soracom.jp/products/lora/al-050/)

# Arduino IDE への導入

1. .ZIPアーカイブをダウンロードする。[Click here](https://github.com/askn37/LoRaWAN_TLM922S/archive/master.zip)

2. ライブラリマネージャで読み込む

  スケッチ -> ライブラリをインクルード -> .ZIP形式のライブラリをインストール...

3. 依存関係があるライブラリも同様に読み込む

  [MultiUART](https://github.com/askn37/MultiUART)

# 使い方

## とっかかり

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
