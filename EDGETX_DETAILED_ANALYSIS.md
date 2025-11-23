# EdgeTX スプラッシュスクリーン 詳細分析

## ソースコード分析結果

### 1. ファイルパスの処理

`StaticImage::setSource()`では以下のようにパスが処理されます：

```cpp
std::string fullpath = std::string("A" PATH_SEPARATOR) + filename;
lv_img_set_src(image, fullpath.c_str());
```

つまり、`BITMAPS_PATH "/" SPLASH_FILE`（`/IMAGES/splash.png`）が渡されると、実際には`A:/IMAGES/splash.png`としてLVGLに渡されます。

**重要**: `A:`はLVGLのファイルシステムドライバのプレフィックスです。

### 2. 画像読み込みの確認

`hasImage()`メソッドは以下のように実装されています：

```cpp
bool StaticImage::hasImage() const
{
  lv_img_t* img = (lv_img_t*)image;
  return img && img->w && img->h;
}
```

画像が正しく読み込まれていない場合、`img->w`と`img->h`が0になり、`hasImage()`は`false`を返します。

### 3. スプラッシュ表示の条件

`startSplash()`関数には以下の条件があります：

```cpp
void startSplash()
{
  if (!UNEXPECTED_SHUTDOWN()) {
    splashStartTime = get_tmr10ms();
    drawSplash();
  }
}
```

**予期しないシャットダウンの場合はスプラッシュが表示されません。**

### 4. splashModeの設定

`SPLASH_NEEDED()`関数は以下のように実装されています：

```cpp
inline bool SPLASH_NEEDED()
{
  return g_eeGeneral.splashMode != 3;
}
```

**`splashMode`が3の場合はスプラッシュが無効になります。**

設定メニュー（RADIO SETUP → Splash Screen）の「表示時間」設定は、実際には`splashMode`の値を変更しています。

## 確認すべきポイント

### 1. splashModeの設定値

EdgeTXの設定メニュー（RADIO SETUP → Splash Screen）で、表示時間が0（無効）になっていないか確認してください。

### 2. 予期しないシャットダウン

トランスミッターが正常にシャットダウンされていない場合、スプラッシュが表示されません。
正常にシャットダウンしてから再起動してください。

### 3. 画像ファイルの形式

LVGLがサポートする画像形式を確認する必要があります。PNG形式はサポートされていますが、以下の点を確認してください：

- **色深度**: 8-bit/color RGB（現在の形式）は問題ないはずですが、24-bitや32-bitも試す価値があります
- **透明度**: アルファチャンネルがある場合、問題が発生する可能性があります
- **インターレース**: 非インターレース（現在の形式）は問題ありません

### 4. 画像サイズ

- カラースクリーン: 212x64ピクセル
- モノクロスクリーン: 128x64ピクセル

画像サイズが正確であることを確認してください。

### 5. SDカードのマウント

`drawSplash()`関数の最初に以下のコードがあります：

```cpp
if (!sdMounted()) sdInit();
```

SDカードが正しくマウントされているか確認してください。

## トラブルシューティング手順

### ステップ1: splashModeの確認

1. EdgeTXの設定メニュー（RADIO SETUP → Splash Screen）を開く
2. 表示時間が0（無効）になっていないか確認
3. 表示時間を適切な値（例：2秒）に設定

### ステップ2: 正常なシャットダウン

1. トランスミッターを正常にシャットダウン
2. SDカードを取り外してPCで確認
3. `IMAGES/splash.png`が正しく配置されているか確認
4. SDカードを再挿入して再起動

### ステップ3: 画像形式の確認

現在の画像形式を確認し、必要に応じて再作成：

```bash
python3 create_splash.py --text "My Radio" --format png --output splash.png
```

### ステップ4: デバッグ情報の確認

可能であれば、EdgeTXのログを確認して、画像読み込みエラーがないか確認してください。

## 追加の確認事項

1. **SDカードのフォーマット**: FAT32形式であることを確認
2. **ファイル名の大文字小文字**: `splash.png`（すべて小文字）であることを確認
3. **ファイルの権限**: 読み取り可能であることを確認
4. **SDカードの破損**: SDカードが破損していないか確認
