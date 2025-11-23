#!/usr/bin/env python3
"""
EdgeTXスプラッシュスクリーン作成ツール（ファイル名に先頭スペース付き）

EdgeTXの一部のバージョンでは、ファイル名の先頭にスペースが必要な場合があります。
このスクリプトは、先頭スペース付きのファイル名でスプラッシュスクリーンを作成します。
"""

import subprocess
import sys
import os

def main():
    # 元のスクリプトを呼び出し、ファイル名に先頭スペースを追加
    script_dir = os.path.dirname(os.path.abspath(__file__))
    create_splash_script = os.path.join(script_dir, "create_splash.py")
    
    # 引数を取得
    args = sys.argv[1:]
    
    # --outputオプションを探して、先頭スペースを追加
    output_index = None
    for i, arg in enumerate(args):
        if arg == '--output' and i + 1 < len(args):
            output_index = i + 1
            break
    
    if output_index is not None:
        # ファイル名に先頭スペースがない場合は追加
        filename = args[output_index]
        if not filename.startswith(' '):
            args[output_index] = ' ' + filename
    else:
        # --outputオプションがない場合は追加
        args.extend(['--output', ' splash.bmp'])
    
    # 元のスクリプトを実行
    subprocess.run([sys.executable, create_splash_script] + args)

if __name__ == "__main__":
    main()
