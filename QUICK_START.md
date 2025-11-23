# EdgeTXスプラッシュスクリーン クイックスタートガイド

## 基本的な使い方

### 1. スプラッシュスクリーンを作成

```bash
# 基本的な作成（RGB形式、BMP）
python3 create_splash.py --text "My Radio" --output " splash.bmp"
```

### 2. SDカードに配置

1. SDカードをPCで読み込む
2. **SDカードのルートに`SPLASH`フォルダを作成**（存在しない場合）
3. 作成したファイル（例：` splash.bmp`）を**`SPLASH`フォルダにコピー**
4. トランスミッターを再起動

### 3. 表示されない場合

複数のバリエーションを作成して試してください：

```bash
# 全バリエーションを作成（splash_variants/フォルダに作成される）
./create_all_variants.sh "My Radio"
```

その後、`splash_variants/`フォルダ内のファイルを**1つずつ**`SPLASH`フォルダにコピーして試してください。

## 重要なポイント

- ✅ **`SPLASH`フォルダ**: EdgeTXが読み込むフォルダ（SDカードのルートに作成）
- ✅ **`splash_variants`フォルダ**: 作業用フォルダ（複数のバリエーションを保存）
- ⚠️ **一度に1つのファイルだけ**を`SPLASH`フォルダに配置してください
- ⚠️ **ファイル名が重要**です（先頭スペースの有無など）

## フォルダ構造の例

```
SDカード/
├── SPLASH/              ← EdgeTXが読み込むフォルダ
│   └──  splash.bmp      ← ここにファイルを配置
│
ワークスペース/
├── splash_variants/      ← 作業用フォルダ（複数のバリエーション）
│   ├──  splash.bmp
│   ├── splash.bmp
│   ├──  splash_grayscale.bmp
│   └── ...
└── create_splash.py
```
