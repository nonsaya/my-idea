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

### BMP形式で出力（推奨・デフォルト）

```bash
python3 create_splash.py --text "EdgeTX" --output splash.bmp
```

### PNG形式で出力

```bash
python3 create_splash.py --text "EdgeTX" --format png --output splash.png
```

### ファイル名に先頭スペースを付ける（EdgeTXの一部のバージョンで必要）

```bash
python3 create_splash.py --text "My Radio" --output " splash.bmp"
```

## オプション

- `--width`: 画像の幅（デフォルト: 212）
- `--height`: 画像の高さ（デフォルト: 64）
- `--text`: 表示するテキスト（デフォルト: "EdgeTX"）
- `--bg-color`: 背景色（black/white、デフォルト: black）
- `--text-color`: テキスト色（black/white、デフォルト: white）
- `--output`: 出力ファイル名（デフォルト: splash.bmp、.bmpまたは.png形式）
- `--format`: 出力形式を明示的に指定（png/bmp、デフォルト: 拡張子から自動判定）
- `--mono`: モノクロスクリーン用（128x64）を指定

## EdgeTXへの適用方法

### 方法1: 自動認識タイプ（設定メニューにファイル選択項目がない場合）

1. SDカードをトランスミッターに挿入し、PCで読み込む
2. SDカードのルートに`SPLASH`フォルダを作成（存在しない場合）
3. 生成された画像ファイルを`SPLASH`フォルダにコピー
   - **重要**: ファイル名は` splash.bmp`（先頭にスペース）である必要があります
   - 複数のファイル名パターンを試す場合は、`create_edge_tx_splash.sh`スクリプトを使用してください
4. トランスミッターを再起動して確認

### 方法2: ファイル選択タイプ（設定メニューにファイル選択項目がある場合）

1. SDカードのルートに`SPLASH`フォルダを作成
2. 生成された画像ファイルを`SPLASH`フォルダにコピー
3. EdgeTXの設定メニュー（RADIO SETUP → Splash Screen）からファイルを選択
4. トランスミッターを再起動して確認

### 複数のファイル名パターンを一度に作成する

```bash
./create_edge_tx_splash.sh "My Radio"
```

このスクリプトは、以下の4つのパターンでファイルを作成します：
- ` splash.bmp`（先頭スペース付きBMP - 最も一般的）
- `splash.bmp`（スペースなしBMP）
- ` splash.png`（先頭スペース付きPNG）
- `splash.png`（スペースなしPNG）

**注意事項**:
- `SPLASH`フォルダが存在しない場合は、SDカードのルートに手動で作成してください
- EdgeTXのバージョンによっては、BMP形式の方が互換性が高い場合があります
- ファイル名に先頭スペースが必要な場合があります（例：` splash.bmp`）
- 設定メニューにファイル選択項目がない場合は、ファイル名が正確であることが重要です

## 注意事項

- EdgeTXのスプラッシュスクリーンは通常モノクロまたはグレースケール形式です
- カラースクリーン対応のトランスミッターは212x64ピクセル
- モノクロスクリーンのトランスミッターは128x64ピクセル
- PNG形式とBMP形式の両方をサポートしています（BMP形式推奨、互換性が高い）
- `SPLASH`フォルダはSDカードのルートに手動で作成する必要があります
