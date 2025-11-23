# EdgeTXスプラッシュスクリーン作成ツール

EdgeTX対応のスプラッシュスクリーン画像を生成するためのツールです。

## 必要な環境

- Python 3.6以上
- Pillowライブラリ

## インストール

```bash
pip install -r requirements.txt
```

## 使用方法

### 基本的な使用方法（デフォルト: 212x64ピクセル）

```bash
python create_splash.py
```

### カスタムテキストを指定

```bash
python create_splash.py --text "My Radio"
```

### モノクロスクリーン用（128x64ピクセル）

```bash
python create_splash.py --mono
```

### カスタムサイズを指定

```bash
python create_splash.py --width 212 --height 64 --text "EdgeTX"
```

### 白背景・黒文字

```bash
python create_splash.py --bg-color white --text-color black
```

### 出力ファイル名を指定

```bash
python create_splash.py --output my_splash.bmp
```

## オプション

- `--width`: 画像の幅（デフォルト: 212）
- `--height`: 画像の高さ（デフォルト: 64）
- `--text`: 表示するテキスト（デフォルト: "EdgeTX"）
- `--bg-color`: 背景色（black/white、デフォルト: black）
- `--text-color`: テキスト色（black/white、デフォルト: white）
- `--output`: 出力ファイル名（デフォルト: splash.bmp）
- `--mono`: モノクロスクリーン用（128x64）を指定

## EdgeTXへの適用方法

1. 生成されたBMPファイルをSDカードの`/EDGETX/SPLASH/`フォルダにコピー
2. EdgeTXの設定メニューからスプラッシュスクリーンを選択
3. トランスミッターを再起動して確認

## 注意事項

- EdgeTXのスプラッシュスクリーンは通常モノクロまたはグレースケール形式です
- カラースクリーン対応のトランスミッターは212x64ピクセル
- モノクロスクリーンのトランスミッターは128x64ピクセル
