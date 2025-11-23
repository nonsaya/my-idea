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
python3 create_splash.py
```

### カスタムテキストを指定

```bash
python3 create_splash.py --text "My Radio"
```

### モノクロスクリーン用（128x64ピクセル）

```bash
python3 create_splash.py --mono
```

### カスタムサイズを指定

```bash
python3 create_splash.py --width 212 --height 64 --text "EdgeTX"
```

### 白背景・黒文字

```bash
python3 create_splash.py --bg-color white --text-color black
```

### 出力ファイル名を指定

```bash
python3 create_splash.py --output my_splash.png
```

### PNG形式で出力（デフォルト）

```bash
python3 create_splash.py --text "EdgeTX" --output splash.png
```

### BMP形式で出力

```bash
python3 create_splash.py --text "EdgeTX" --format bmp --output splash.bmp
```

## オプション

- `--width`: 画像の幅（デフォルト: 212）
- `--height`: 画像の高さ（デフォルト: 64）
- `--text`: 表示するテキスト（デフォルト: "EdgeTX"）
- `--bg-color`: 背景色（black/white、デフォルト: black）
- `--text-color`: テキスト色（black/white、デフォルト: white）
- `--output`: 出力ファイル名（デフォルト: splash.png、.bmpまたは.png形式）
- `--format`: 出力形式を明示的に指定（png/bmp、デフォルト: 拡張子から自動判定）
- `--mono`: モノクロスクリーン用（128x64）を指定

## EdgeTXへの適用方法

1. SDカードをトランスミッターに挿入し、PCで読み込む
2. SDカードのルートに`SPLASH`フォルダを作成（存在しない場合）
3. 生成された画像ファイル（BMPまたはPNG）を`SPLASH`フォルダにコピー
4. EdgeTXの設定メニューからスプラッシュスクリーンを選択
5. トランスミッターを再起動して確認

**注意**: `SPLASH`フォルダが存在しない場合は、SDカードのルートに手動で作成してください。

## 注意事項

- EdgeTXのスプラッシュスクリーンは通常モノクロまたはグレースケール形式です
- カラースクリーン対応のトランスミッターは212x64ピクセル
- モノクロスクリーンのトランスミッターは128x64ピクセル
- PNG形式とBMP形式の両方をサポートしています（PNG推奨）
- `SPLASH`フォルダはSDカードのルートに手動で作成する必要があります
