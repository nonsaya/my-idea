# RadioMaster GX12（モノクロスクリーン）でカスタムスプラッシュスクリーンを表示する方法

## 現状

EdgeTX 2.11.0では、モノクロスクリーン用のコード（`gui/128x64/splash.cpp`）にSDカードから画像を読み込む機能が実装されていません。

しかし、212x64スクリーン用のコード（`gui/212x64/bmp.cpp`）には`lcdLoadBitmap`という関数があり、SDカードからBMPファイルを読み込む機能が実装されています。

## 解決方法

### 方法1: EdgeTXのファームウェアをカスタマイズする（推奨）

モノクロスクリーン用のコードを修正して、212x64スクリーン用のコードを参考に、SDカードから画像を読み込む機能を追加します。

#### 実装手順

1. **EdgeTXのソースコードを取得**
   ```bash
   git clone https://github.com/EdgeTX/edgetx.git
   cd edgetx
   git checkout v2.11.0  # または最新のバージョン
   ```

2. **`radio/src/gui/128x64/splash.cpp`を修正**

   現在のコード：
   ```cpp
   void drawSplash()
   {
     lcdClear();
     lcdDraw1bitBitmap(0, 0, splash_lbm, 0, 0);
     lcdRefresh();
   }
   ```

   修正後のコード：
   ```cpp
   #include "sdcard.h"
   #include "bmp.h"  // 212x64のbmp.cppを参考にする
   
   void drawSplash()
   {
     lcdClear();
     
     // SDカードから画像を読み込む処理を追加
     if (sdMounted()) {
       char filename[] = BITMAPS_PATH "/splash.bmp";
       uint8_t *bmp = lcdLoadBitmap(nullptr, filename, LCD_W, LCD_H);
       
       if (bmp) {
         // 画像を表示
         lcdDrawBitmap(0, 0, bmp);
         // メモリを解放（必要に応じて）
         // free(bmp);
       } else {
         // SDカードから読み込めない場合はデフォルトを表示
         lcdDraw1bitBitmap(0, 0, splash_lbm, 0, 0);
       }
     } else {
       // SDカードがマウントされていない場合はデフォルトを表示
       lcdDraw1bitBitmap(0, 0, splash_lbm, 0, 0);
     }
     
     lcdRefresh();
   }
   ```

3. **必要な関数を追加**

   `gui/128x64/`ディレクトリに、212x64の`bmp.cpp`を参考にした画像読み込み関数を追加する必要があります。

4. **ファームウェアをビルド**

   EdgeTXのビルド環境をセットアップして、ファームウェアをビルドします。

5. **ファームウェアをフラッシュ**

   カスタマイズしたファームウェアをトランスミッターに書き込みます。

### 方法2: EdgeTXの最新バージョンを確認

EdgeTXの最新バージョンで、モノクロスクリーン用のカスタムスプラッシュスクリーン機能が追加されている可能性があります。

1. EdgeTXの最新リリースを確認: https://github.com/EdgeTX/edgetx/releases
2. 最新バージョンに更新
3. 機能が追加されているか確認

### 方法3: EdgeTXコミュニティに機能リクエスト

この機能が必要な場合は、EdgeTXのGitHubリポジトリで機能リクエストを提出してください：

1. EdgeTX GitHub: https://github.com/EdgeTX/edgetx
2. Issues セクションで「New Issue」をクリック
3. 「Feature Request」を選択
4. タイトル: 「Custom splash screen support for monochrome displays (128x64)」
5. 説明: RadioMaster GX12などのモノクロスクリーン対応トランスミッターで、SDカードからカスタムスプラッシュスクリーンを読み込む機能を追加してほしい旨を説明

### 方法4: ビルトインのスプラッシュスクリーンを置き換える

EdgeTXのビルド時に、ビルトインのスプラッシュスクリーンデータを自分の画像に置き換えることができます。

1. **画像をLBM形式に変換**

   128x64ピクセルのモノクロ画像をLBM形式に変換する必要があります。

2. **`bitmaps/128x64/splash.lbm`を置き換え**

   EdgeTXのソースコード内の`bitmaps/128x64/splash.lbm`を自分の画像に置き換えます。

3. **ファームウェアをビルド**

   ファームウェアをビルドして、トランスミッターに書き込みます。

## 画像形式の要件

モノクロスクリーン用のスプラッシュスクリーンは、以下の形式である必要があります：

- **形式**: BMP形式（1ビットモノクロ）
- **サイズ**: 128x64ピクセル
- **色深度**: 1ビット（モノクロ）

## 画像作成ツールの使用

このプロジェクトの`create_splash.py`を使用して、モノクロスクリーン用の画像を作成できます：

```bash
python3 create_splash.py --mono --text "My Radio" --format bmp --output splash.bmp
```

ただし、BMP形式で1ビットモノクロとして保存する必要があります。

## 推奨される方法

### 短期間の解決策

1. **EdgeTXの最新バージョンを確認** - 機能が追加されている可能性があります
2. **EdgeTXフォーラムで確認** - 他のユーザーが同様の問題を解決しているか確認
   - EdgeTXフォーラム: https://discuss.edgetx.org/

### 長期的な解決策

1. **機能リクエストを提出** - EdgeTXコミュニティに機能をリクエスト
2. **ファームウェアをカスタマイズ** - 技術的に可能であれば、自分で実装

## 注意事項

- ファームウェアのカスタマイズは、トランスミッターをブリックするリスクがあります
- 必ずバックアップを取ってから作業してください
- EdgeTXのライセンス（GPLv2）に従う必要があります
- ファームウェアのビルドには、適切な開発環境のセットアップが必要です

## 参考情報

- EdgeTX GitHub: https://github.com/EdgeTX/edgetx
- EdgeTXフォーラム: https://discuss.edgetx.org/
- EdgeTX開発Wiki: https://github.com/EdgeTX/edgetx/wiki
