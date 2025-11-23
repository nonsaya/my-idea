# EdgeTX スプラッシュスクリーン 最終解決策

## 問題の原因

ソースコードを詳しく分析した結果、以下のことが判明しました：

### 1. 画像読み込みの仕組み

`drawSplash()`関数では以下のコードが実行されます：

```cpp
auto bg = new StaticImage(splashScreen, {0, 0, LCD_W, LCD_H},
                          BITMAPS_PATH "/" SPLASH_FILE);
bg->show(bg->hasImage());
```

**重要なポイント：**
- `bg->show(bg->hasImage())`は、`hasImage()`が`true`の場合のみ画像を表示します
- `hasImage()`が`false`を返す場合、画像は表示されず、デフォルトのロゴが表示されます

### 2. StaticImage::setSource()の処理

```cpp
void StaticImage::setSource(std::string filename)
{
  if (!filename.empty()) {
    std::string fullpath = std::string("A" PATH_SEPARATOR) + filename;
    lv_img_set_src(image, fullpath.c_str());
    if (!hasImage()) {
      TRACE_ERROR("could not load image '%s'", filename.c_str());
      clearSource();
    }
  }
}
```

**問題点：**
- `fullpath`は`A:/IMAGES/splash.png`のようになります
- `A:`はLVGLのファイルシステムドライバのプレフィックスです
- 画像が読み込めない場合、エラーメッセージが出力されますが、ユーザーには見えません

### 3. SDカードのマウントタイミング

```cpp
void drawSplash()
{
  if (!sdMounted()) sdInit();
  // ...
}
```

**問題点：**
- SDカードがマウントされていない場合、`sdInit()`が呼ばれますが、マウントが完了する前に画像を読み込もうとする可能性があります

## 解決策

### 方法1: 画像ファイルの形式を確認

現在の画像形式を確認し、LVGLがサポートする形式であることを確認してください。

**推奨される形式：**
- PNG形式（RGB、24-bit）
- 非インターレース
- 透明度なし（アルファチャンネルなし）

### 方法2: ファイルパスの確認

SDカード内のファイル構造を確認してください：

```
SDカードのルート/
└── IMAGES/
    └── splash.png
```

**確認事項：**
- フォルダ名は`IMAGES`（大文字）であること
- ファイル名は`splash.png`（すべて小文字）であること
- ファイルが正しく配置されていること

### 方法3: SDカードのフォーマット確認

SDカードがFAT32形式であることを確認してください。

### 方法4: 画像サイズの確認

トランスミッターの種類に応じたサイズを使用してください：
- カラースクリーン: 212x64ピクセル
- モノクロスクリーン: 128x64ピクセル

### 方法5: デバッグ方法

EdgeTXのログを確認できる場合は、以下のエラーメッセージがないか確認してください：

```
TRACE_ERROR("could not load image '/IMAGES/splash.png'");
```

## 試すべき手順

### ステップ1: 画像を再作成

```bash
python3 create_splash.py --text "My Radio" --format png --output splash.png
```

### ステップ2: 画像形式を確認

```bash
file splash.png
```

出力例：
```
splash.png: PNG image data, 212 x 64, 8-bit/color RGB, non-interlaced
```

### ステップ3: SDカードに配置

1. SDカードをPCで読み込む
2. SDカードのルートに`IMAGES`フォルダを作成（存在しない場合）
3. `splash.png`を`IMAGES`フォルダにコピー
4. SDカードを安全に取り外す

### ステップ4: トランスミッターで確認

1. SDカードをトランスミッターに挿入
2. トランスミッターを正常にシャットダウン
3. トランスミッターを再起動
4. スプラッシュスクリーンが表示されるか確認

### ステップ5: それでも表示されない場合

1. SDカードを取り外してPCで確認
2. `IMAGES/splash.png`が正しく配置されているか確認
3. ファイル名が正確であるか確認（大文字小文字を含む）
4. 別のSDカードで試す
5. EdgeTXのバージョンを確認（2.11.0であることを確認）

## 追加の確認事項

### 1. EdgeTXのバージョン

EdgeTX 2.11.0を使用していることを確認してください。バージョンが異なる場合、動作が異なる可能性があります。

### 2. トランスミッターの種類

カラースクリーンとモノクロスクリーンで、スプラッシュスクリーンの処理が異なる可能性があります。

### 3. SDカードの互換性

一部のSDカードがEdgeTXと互換性がない可能性があります。別のSDカードで試してください。

## 最後の手段

上記の方法を全て試しても表示されない場合、以下の可能性があります：

1. **EdgeTX 2.11.0のバグ**: スプラッシュスクリーンの読み込みに問題がある可能性
2. **トランスミッター固有の問題**: 特定のトランスミッターで問題が発生している可能性
3. **SDカードの問題**: SDカードが正しく認識されていない可能性

この場合、EdgeTXのフォーラムやGitHubのIssuesで報告することをお勧めします。
