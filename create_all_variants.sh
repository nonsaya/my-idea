#!/bin/bash
# EdgeTX 2.11.0用 全バリエーション作成スクリプト
# 全ての可能な組み合わせでスプラッシュスクリーンを作成します

TEXT="${1:-My Radio}"
OUTPUT_DIR="${2:-./splash_variants}"

mkdir -p "$OUTPUT_DIR"

echo "EdgeTX 2.11.0用スプラッシュスクリーンを作成します..."
echo "テキスト: $TEXT"
echo "出力先: $OUTPUT_DIR"
echo ""

# RGB形式（デフォルト）
echo "=== RGB形式（デフォルト）==="
python3 create_splash.py --text "$TEXT" --output "$OUTPUT_DIR/ splash.bmp"
python3 create_splash.py --text "$TEXT" --output "$OUTPUT_DIR/splash.bmp"
python3 create_splash.py --text "$TEXT" --format png --output "$OUTPUT_DIR/ splash.png"
python3 create_splash.py --text "$TEXT" --format png --output "$OUTPUT_DIR/splash.png"

echo ""
echo "=== グレースケール形式 ==="
python3 create_splash_grayscale.py --text "$TEXT" --output "$OUTPUT_DIR/ splash_grayscale.bmp"
python3 create_splash_grayscale.py --text "$TEXT" --output "$OUTPUT_DIR/splash_grayscale.bmp"
python3 create_splash_grayscale.py --text "$TEXT" --format png --output "$OUTPUT_DIR/ splash_grayscale.png"
python3 create_splash_grayscale.py --text "$TEXT" --format png --output "$OUTPUT_DIR/splash_grayscale.png"

echo ""
echo "=== 大文字ファイル名 ==="
python3 create_splash.py --text "$TEXT" --output "$OUTPUT_DIR/ SPLASH.BMP"
python3 create_splash.py --text "$TEXT" --output "$OUTPUT_DIR/SPLASH.BMP"

echo ""
echo "完了しました！"
echo ""
echo "作成されたファイル:"
ls -la "$OUTPUT_DIR"
echo ""
echo "試す順序:"
echo "1. $OUTPUT_DIR/ splash.bmp (先頭スペース付きRGB BMP)"
echo "2. $OUTPUT_DIR/splash.bmp (スペースなしRGB BMP)"
echo "3. $OUTPUT_DIR/ splash_grayscale.bmp (先頭スペース付きグレースケール BMP)"
echo "4. $OUTPUT_DIR/splash_grayscale.bmp (スペースなしグレースケール BMP)"
echo ""
echo "各ファイルをSPLASHフォルダに1つずつ配置して試してください。"
