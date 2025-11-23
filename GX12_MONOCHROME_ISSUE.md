# RadioMaster GX12（モノクロスクリーン）でのスプラッシュスクリーン問題

## 重要な発見

EdgeTXのソースコードを確認した結果、**モノクロスクリーン対応トランスミッター（RadioMaster GX12を含む）では、SDカードからカスタムスプラッシュスクリーンを読み込む機能が実装されていません。**

## ソースコードの確認結果

### カラースクリーン用のコード（`gui/colorlcd/startup_shutdown.cpp`）

```cpp
void drawSplash()
{
  if (!sdMounted()) sdInit();
  
  auto bg = new StaticImage(splashScreen, {0, 0, LCD_W, LCD_H},
                            BITMAPS_PATH "/" SPLASH_FILE);
  bg->show(bg->hasImage());
  // SDカードから画像を読み込む処理がある
}
```

**カラースクリーン用では、SDカードから画像を読み込む処理が実装されています。**

### モノクロスクリーン用のコード（`gui/128x64/splash.cpp`）

```cpp
const unsigned char splashdata[]  = {
  'S','P','S',0,
  #include "bitmaps/128x64/splash.lbm"
  'S','P','E',0 };

void drawSplash()
{
  lcdClear();
  lcdDraw1bitBitmap(0, 0, splash_lbm, 0, 0);
  lcdRefresh();
}
```

**モノクロスクリーン用では、ビルトインのデータ（`bitmaps/128x64/splash.lbm`）のみを使用しており、SDカードから画像を読み込む処理がありません。**

## 結論

**RadioMaster GX12などのモノクロスクリーン対応トランスミッターでは、EdgeTX 2.11.0の時点で、SDカードからカスタムスプラッシュスクリーンを読み込む機能がサポートされていません。**

## 対処方法

### 方法1: EdgeTXのバージョンを確認

EdgeTXの最新バージョンで、モノクロスクリーン用のカスタムスプラッシュスクリーン機能が追加されている可能性があります。最新バージョンに更新してみてください。

### 方法2: EdgeTXのフォーラムで確認

EdgeTXの公式フォーラムで、モノクロスクリーン対応トランスミッターでのカスタムスプラッシュスクリーン機能について確認してください：
- EdgeTXフォーラム: https://discuss.edgetx.org/

### 方法3: 機能リクエスト

この機能が必要な場合は、EdgeTXのGitHubリポジトリで機能リクエストを提出してください：
- EdgeTX GitHub: https://github.com/EdgeTX/edgetx

### 方法4: ファームウェアのカスタマイズ

技術的に可能であれば、EdgeTXのソースコードを修正して、モノクロスクリーン用にもSDカードから画像を読み込む機能を追加することができます。

## 確認事項

1. **EdgeTXのバージョン**: 最新バージョンで機能が追加されているか確認
2. **フォーラムでの情報**: 他のユーザーが同様の問題を報告しているか確認
3. **機能リクエスト**: この機能がリクエストされているか確認

## 注意事項

- カラースクリーン対応トランスミッターでは、SDカードからカスタムスプラッシュスクリーンを読み込む機能が実装されています
- モノクロスクリーン対応トランスミッターでは、現時点でこの機能が実装されていない可能性が高いです
