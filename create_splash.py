#!/usr/bin/env python3
"""
EdgeTXスプラッシュスクリーン作成ツール

EdgeTX対応のスプラッシュスクリーン画像を生成します。
対応サイズ:
- 212x64ピクセル (カラースクリーン対応)
- 128x64ピクセル (モノクロスクリーン)
"""

from PIL import Image, ImageDraw, ImageFont
import argparse
import sys
import os


def create_splash_screen(width=212, height=64, text="EdgeTX", bg_color="black", text_color="white", output_file="splash.bmp"):
    """
    スプラッシュスクリーン画像を作成
    
    Args:
        width: 画像の幅（デフォルト: 212）
        height: 画像の高さ（デフォルト: 64）
        text: 表示するテキスト（デフォルト: "EdgeTX"）
        bg_color: 背景色（デフォルト: "black"）
        text_color: テキスト色（デフォルト: "white"）
        output_file: 出力ファイル名（デフォルト: "splash.bmp"）
    """
    # 画像を作成（モノクロモード）
    # EdgeTXは通常モノクロまたはグレースケールを使用
    img = Image.new('L', (width, height), color=0 if bg_color == "black" else 255)
    draw = ImageDraw.Draw(img)
    
    # フォントのサイズを計算（画像サイズに応じて調整）
    font_size = int(height * 0.4)
    
    try:
        # システムフォントを試す
        font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", font_size)
    except:
        try:
            font = ImageFont.truetype("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf", font_size)
        except:
            # フォントが見つからない場合はデフォルトフォントを使用
            font = ImageFont.load_default()
    
    # テキストのサイズを取得
    bbox = draw.textbbox((0, 0), text, font=font)
    text_width = bbox[2] - bbox[0]
    text_height = bbox[3] - bbox[1]
    
    # テキストを中央に配置
    x = (width - text_width) // 2
    y = (height - text_height) // 2
    
    # テキストを描画
    text_color_value = 255 if text_color == "white" else 0
    draw.text((x, y), text, fill=text_color_value, font=font)
    
    # ファイル形式を自動検出（拡張子から判定）
    file_ext = os.path.splitext(output_file)[1].lower()
    if file_ext == '.png':
        # PNG形式で保存
        # EdgeTXはPNG形式をサポートしていますが、BMP形式の方が互換性が高いです
        img.save(output_file, "PNG")
    elif file_ext == '.bmp':
        # BMP形式で保存（EdgeTX推奨形式）
        # EdgeTXは通常、8ビットグレースケールまたは1ビットモノクロBMPをサポート
        # 8ビットグレースケールで保存（より互換性が高い）
        img.save(output_file, "BMP")
    else:
        # 拡張子がない、または不明な場合はBMP形式で保存（EdgeTX推奨）
        if not file_ext:
            output_file = output_file + '.bmp'
        img.save(output_file, "BMP")
        print(f"注意: 拡張子が不明なため、BMP形式で保存しました（EdgeTX推奨）")
    
    print(f"スプラッシュスクリーンを作成しました: {output_file}")
    print(f"サイズ: {width}x{height}ピクセル")
    return output_file


def main():
    parser = argparse.ArgumentParser(description='EdgeTXスプラッシュスクリーン作成ツール')
    parser.add_argument('--width', type=int, default=212, help='画像の幅（デフォルト: 212）')
    parser.add_argument('--height', type=int, default=64, help='画像の高さ（デフォルト: 64）')
    parser.add_argument('--text', type=str, default='EdgeTX', help='表示するテキスト（デフォルト: EdgeTX）')
    parser.add_argument('--bg-color', type=str, default='black', choices=['black', 'white'], help='背景色（デフォルト: black）')
    parser.add_argument('--text-color', type=str, default='white', choices=['black', 'white'], help='テキスト色（デフォルト: white）')
    parser.add_argument('--output', type=str, default='splash.bmp', help='出力ファイル名（デフォルト: splash.bmp、.bmpまたは.png形式）')
    parser.add_argument('--format', type=str, choices=['png', 'bmp'], help='出力形式を明示的に指定（デフォルト: 拡張子から自動判定）')
    parser.add_argument('--mono', action='store_true', help='モノクロスクリーン用（128x64）')
    
    args = parser.parse_args()
    
    # モノクロモードが指定された場合
    if args.mono:
        args.width = 128
        args.height = 64
    
    # フォーマットが明示的に指定された場合、拡張子を変更
    output_file = args.output
    if args.format:
        base_name = os.path.splitext(output_file)[0]
        output_file = f"{base_name}.{args.format}"
    
    create_splash_screen(
        width=args.width,
        height=args.height,
        text=args.text,
        bg_color=args.bg_color,
        text_color=args.text_color,
        output_file=output_file
    )


if __name__ == "__main__":
    main()
