## 目的
CRSF（Crossfire）受信機から得られるシリアル RC データのうち、2〜3 チャンネル分だけを通常の PWM サーボ信号（50 Hz、1000〜2000 µs）へ変換するための方針と実装メモです。PlatformIO + ESP32（`esp32dev`）を例にしていますが、420 kbaud の UART が扱えて 3 本の PWM 出力を持つ MCU であれば同じ手順で移植できます。

## ハード構成
- TBS/CRSF 互換受信機（Nano RX など）  
  - UART: 420000 bps, 8-N-1, 無フロー制御
  - 3.3 V ロジック
- マイコン: ESP32 DevKitC（UART1, LEDC PWM を使用）
- 接続例
  | CRSF | ESP32 | 備考 |
  | ---- | ----- | ---- |
  | TX   | GPIO16 (RX2) | CRSF → MCU |
  | RX   | 未使用 | 今回は送信不要 |
  | 5 V  | 5 V   | 受信機給電 |
  | GND  | GND   | 共通 GND |
  | PWM1 | GPIO18 | サーボ1 |
  | PWM2 | GPIO19 | サーボ2 |
  | PWM3 | GPIO21 | サーボ3（任意） |

## CRSF チャネルデータ概要
- フレーム構造: `[0xC8][Length][Type][Payload...][CRC]`
- チャネルフレーム Type: `0x16 (RC Channels Packed)`  
  - Payload 22 byte に 16ch × 11bit が連続格納（LSB first）
  - 有効値: 172〜1811（約 880〜2120 µs 相当）
- CRC: `CRC8-D5`（Poly 0xD5, 初期値 0）

## PWM 変換要件
1. UART バッファから 0x16 フレームだけ抽出
2. 必要な 2〜3 チャネル（例: CH1=スロットル, CH2=ロール, CH3=ピッチ）をデコード
3. `mapCrsfToMicros(value)` で 1000〜2000 µs に線形変換  
   ```text
   µs = 1000 + (value - 992) * (1000 / 820)  // 992〜1811 を 1000〜2000 に
   ```
4. 50 Hz PWM を生成（LEDC 14〜16bit 解像度を推奨）
5. 100 ms 以上フレームが途絶えたらフェイルセーフ（1500 µs）に戻す

## 実装手順
1. PlatformIO で `firmware/crsf_to_pwm` プロジェクトを開く
2. `platformio.ini` の `upload_port`, `monitor_port` を環境に合わせて変更
3. 受信機を ESP32 に配線し、電源投入前に GND を共通化
4. `pio run -t upload` で書き込み、`pio device monitor -b 115200` でログ確認
5. サーボテスタやオシロで PWM 幅（1〜2 ms）を検証

## ファイル
- `firmware/crsf_to_pwm/platformio.ini` : ESP32 用 PlatformIO 設定
- `firmware/crsf_to_pwm/src/main.cpp` : CRSF→PWM 変換ファームウェア

### チャネル割り当て
`main.cpp` の `PWM_CHANNELS` 配列（LEDC チャネル, ピン, CRSF チャネル番号）を編集すると、任意の 2〜3 系統へ出力できます。出力を 2 系統に減らしたい場合は対応エントリを削除するだけで構いません。

## テスト
1. 送信機の各操作を行い、`Serial` ログにチャネル値が追従することを確認
2. PWM 出力をサーボまたはロジックアナライザで測定し、1000〜2000 µs 範囲で連動することを確認
3. 受信機の電源を遮断して 100 ms 後に PWM が 1500 µs（もしくは設定値）で保持されるか確認
