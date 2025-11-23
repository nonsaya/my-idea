#!/bin/bash
# EdgeTXスプラッシュスクリーン作成スクリプト
# 複数のファイル名パターンで作成します

TEXT="${1:-My Radio}"
OUTPUT_DIR="${2:-.}"

echo "EdgeTXスプラッシュスクリーンを作成します..."
echo "テキスト: $TEXT"
echo "出力先: $OUTPUT_DIR"
echo ""

# 1. 先頭スペース付きBMP（最も一般的）
echo "1. 先頭スペース付きBMP形式を作成中..."
python3 create_splash.py --text "$TEXT" --output "$OUTPUT_DIR/ splash.bmp"

# 2. スペースなしBMP
echo ""
echo "2. スペースなしBMP形式を作成中..."
python3 create_splash.py --text "$TEXT" --output "$OUTPUT_DIR/splash.bmp"

# 3. 先頭スペース付きPNG
echo ""
echo "3. 先頭スペース付きPNG形式を作成中..."
python3 create_splash.py --text "$TEXT" --format png --output "$OUTPUT_DIR/ splash.png"

# 4. スペースなしPNG
echo ""
echo "4. スペースなしPNG形式を作成中..."
python3 create_splash.py --text "$TEXT" --format png --output "$OUTPUT_DIR/splash.png"

echo ""
echo "完了しました！"
echo ""
echo "次の手順:"
echo "1. 作成されたファイルをSDカードのSPLASHフォルダにコピー"
echo "2. トランスミッターを再起動"
echo "3. 表示されない場合は、SPLASHフォルダ内のファイル名を確認してください"
echo ""
echo "推奨: まず ' splash.bmp'（先頭スペース付き）を試してください"
