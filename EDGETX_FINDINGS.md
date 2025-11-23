# EdgeTX スプラッシュスクリーン 調査結果

## 重要な発見！

EdgeTXのソースコードを調査した結果、以下のことが判明しました：

### 正しいフォルダとファイル名

**EdgeTXは以下のパスからスプラッシュスクリーンを読み込みます：**

```
/IMAGES/splash.png
```

**重要なポイント：**
1. **フォルダ名は`SPLASH`ではなく`IMAGES`です！**
2. **ファイル名は`splash.png`（PNG形式）です！**
3. **先頭スペースは不要です！**

### ソースコードの根拠

`radio/src/sdcard.h`に以下の定義があります：

```c
#define BITMAPS_PATH        ROOT_PATH "IMAGES"
#define SPLASH_FILE         "splash.png"
```

`radio/src/gui/colorlcd/startup_shutdown.cpp`では：

```cpp
auto bg = new StaticImage(splashScreen, {0, 0, LCD_W, LCD_H},
                          BITMAPS_PATH "/" SPLASH_FILE);
```

つまり、`/IMAGES/splash.png`というパスで読み込まれます。

### 正しい設定方法

1. **SDカードのルートに`IMAGES`フォルダを作成**（存在しない場合）
2. **`splash.png`というファイル名でPNG形式の画像を作成**
3. **`IMAGES`フォルダに配置**
4. **トランスミッターを再起動**

### 画像形式について

- **PNG形式が推奨**されています（ソースコードでは`SPLASH_FILE`が`"splash.png"`として定義）
- BMP形式も試す価値はありますが、PNG形式が標準です

### 画像サイズ

- カラースクリーン: 212x64ピクセル
- モノクロスクリーン: 128x64ピクセル

### 注意事項

- EdgeTXの設定メニュー（RADIO SETUP → Splash Screen）の「表示時間」設定は、スプラッシュスクリーンの表示時間を制御します
- ファイル選択機能はないため、ファイルを正しい場所に配置するだけで自動認識されます
