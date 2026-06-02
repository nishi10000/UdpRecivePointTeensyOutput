# UdpRecivePointTeensyOutput

UDPで受け取った座標/制御コマンドをもとに、TeensyからGalvoとレーザーを制御するためのPlatformIOプロジェクトです。

## 概要

レーザープロジェクター用の受信・出力側プログラムです。
送信側アプリからUDPでコマンドを受け取り、以下を制御します。

- GalvoのX/Y電圧出力
- LaserのON/OFF
- WIZ850io経由のEthernet UDP受信

フレーム管理は送信側に依存し、このプロジェクトでは受信したデータを即時処理します。

## ハードウェア構成

主な接続先は `src/main.cpp` のコメントにも記載しています。

- WIZ850io Ethernet module
- Teensy
- Galvo X/Y output
- Laser ON/OFF output

## 主なファイル

- `platformio.ini` - PlatformIO設定
- `src/main.cpp` - UDP受信、コマンド解析、Galvo/Laser制御
- `include/`, `lib/`, `test/` - PlatformIO標準ディレクトリ

## セットアップ

PlatformIO環境でビルド/書き込みを行います。

```bash
pio run
pio run --target upload
```

## 関連プロジェクト

映像からポイントデータを生成してUDP送信する側のアプリです。

- [SpoutToOpenCvPointCreate](https://github.com/nishi10000/SpoutToOpenCvPointCreate)
