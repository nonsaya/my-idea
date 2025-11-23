# CursorでSTM32F030F4P6開発セットアップガイド

## 前提条件
- Cursorがインストールされていること
- STM32F030F4P6の開発ボードまたはカスタム基板
- ST-Linkデバッガ（ST-Link V2推奨）

## 方法1: PlatformIOを使用（推奨）

### 1. PlatformIO拡張機能のインストール

1. Cursorを開く
2. 拡張機能タブ（Ctrl+Shift+X / Cmd+Shift+X）を開く
3. "PlatformIO IDE"を検索してインストール
4. Cursorを再起動

### 2. プロジェクトの作成

1. Cursorで新しいフォルダを開く
2. コマンドパレット（Ctrl+Shift+P / Cmd+Shift+P）を開く
3. "PlatformIO: New Project"を選択
4. 以下の設定を入力:
   - **Board**: `ST STM32F030F4P6`
   - **Framework**: `STM32Cube`
   - **Location**: プロジェクトの保存場所

### 3. プロジェクト構造

```
project/
├── .pio/
├── include/
├── lib/
├── src/
│   └── main.c
├── platformio.ini
└── test/
```

### 4. platformio.iniの設定例

```ini
[env:genericSTM32F030F4P6]
platform = ststm32
board = genericSTM32F030F4P6
framework = stm32cube

; デバッガ設定
debug_tool = stlink
debug_init_break = tbreak main

; アップロード設定
upload_protocol = stlink

; ビルドフラグ
build_flags = 
    -DSTM32F030x6
    -DHSE_VALUE=8000000
    -DUSE_HAL_DRIVER
```

### 5. コード例（CRSF受信 + PWM出力）

**src/main.c:**
```c
#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"

UART_HandleTypeDef huart1;
TIM_HandleTypeDef htim1;

// UART初期化（420000bps、内蔵クロック使用）
void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 420000;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

// PWM初期化（LED制御用）
void MX_TIM1_Init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 999;  // 1kHz PWM（48MHz / 48000 = 1kHz）
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;  // 初期デューティ比0%
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_TIM_MspPostInit(&htim1);
}

// CRSFフレーム受信バッファ
uint8_t crsf_buffer[64];
uint8_t crsf_index = 0;

// CRSFフレーム処理
void ProcessCRSFFrame(uint8_t *data, uint8_t length)
{
    if (length < 2) return;
    
    uint8_t frame_type = data[2];
    if (frame_type == 0x16)  // RC Channels Packed
    {
        // チャンネルデータを抽出（22バイト目から）
        uint16_t channel[16];
        // パース処理...
        
        // チャンネル値をPWM値に変換（0%～100%）
        uint16_t pwm_value = (channel[0] - 172) * 1000 / 1638;  // 例
        
        // PWM出力更新
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_value);
    }
}

int main(void)
{
    HAL_Init();
    
    // システムクロック設定（内蔵8MHz → 48MHz PLL）
    SystemClock_Config();
    
    MX_USART1_UART_Init();
    MX_TIM1_Init();
    
    // PWM開始
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    
    while (1)
    {
        // UART受信
        uint8_t byte;
        if (HAL_UART_Receive(&huart1, &byte, 1, 10) == HAL_OK)
        {
            if (byte == 0xC8 && crsf_index == 0)  // フレーム開始
            {
                crsf_buffer[0] = byte;
                crsf_index = 1;
            }
            else if (crsf_index > 0)
            {
                crsf_buffer[crsf_index++] = byte;
                if (crsf_index >= crsf_buffer[1] + 2)  // フレーム終了
                {
                    ProcessCRSFFrame(crsf_buffer, crsf_index);
                    crsf_index = 0;
                }
            }
        }
    }
}
```

### 6. ビルドとアップロード

1. **ビルド**: コマンドパレット → "PlatformIO: Build"
2. **アップロード**: コマンドパレット → "PlatformIO: Upload"
3. **デバッグ**: コマンドパレット → "PlatformIO: Debug"

---

## 方法2: STM32CubeMX + VSCode拡張機能

### 1. STM32CubeMXのインストール

1. STMicroelectronicsの公式サイトからダウンロード
2. インストール

### 2. STM32CubeMXでプロジェクト生成

1. STM32CubeMXを起動
2. "New Project"を選択
3. STM32F030F4P6を検索して選択
4. ピン設定:
   - USART1: RX/TXピンを設定
   - TIM1: PWM出力ピンを設定
5. Clock Configuration:
   - HSI: 8MHz
   - PLL: 48MHz
6. Project Manager:
   - Toolchain: Makefile
   - Generate Code

### 3. Cursorでプロジェクトを開く

1. Cursorで生成されたプロジェクトフォルダを開く
2. STM32拡張機能をインストール（オプション）

### 4. ビルド設定

**Makefileを使用:**
```bash
# ビルド
make

# クリーン
make clean

# アップロード（ST-Link使用）
st-flash write build/project.bin 0x8000000
```

---

## 方法3: コマンドライン開発

### 1. ツールチェーンのインストール

**Linux:**
```bash
sudo apt-get install gcc-arm-none-eabi
```

**Mac:**
```bash
brew install arm-none-eabi-gcc
```

**Windows:**
- ARM公式サイトからダウンロード
- またはChocolatey: `choco install gcc-arm-embedded`

### 2. ST-Linkツールのインストール

**Linux:**
```bash
sudo apt-get install stlink-tools
```

**Mac:**
```bash
brew install stlink
```

**Windows:**
- ST-Link公式サイトからダウンロード

### 3. Makefileの作成

```makefile
# Makefile例
TARGET = project
MCU = STM32F030x6

# ツールチェーン
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# ソースファイル
SRCS = src/main.c \
       src/stm32f0xx_it.c \
       src/system_stm32f0xx.c

# インクルードパス
INCLUDES = -IInc \
           -IDrivers/STM32F0xx_HAL_Driver/Inc \
           -IDrivers/CMSIS/Device/ST/STM32F0xx/Include \
           -IDrivers/CMSIS/Include

# コンパイルフラグ
CFLAGS = -mcpu=cortex-m0 -mthumb \
         -DSTM32F030x6 \
         -DHSE_VALUE=8000000 \
         -DUSE_HAL_DRIVER \
         -O2 -Wall

# リンクフラグ
LDFLAGS = -mcpu=cortex-m0 -mthumb \
          -TSTM32F030F4P6_FLASH.ld \
          -Wl,--gc-sections

# ビルド
all: $(TARGET).bin

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	$(SIZE) $<

$(TARGET).elf: $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^

# アップロード
upload: $(TARGET).bin
	st-flash write $< 0x8000000

# クリーン
clean:
	rm -f $(TARGET).elf $(TARGET).bin
```

### 4. ビルドとアップロード

```bash
# ビルド
make

# アップロード
make upload
```

---

## デバッグ設定

### ST-Linkを使用したデバッグ

**OpenOCD設定ファイル（openocd.cfg）:**
```
source [find interface/stlink.cfg]
source [find target/stm32f0x.cfg]
reset_config srst_only
```

**デバッグコマンド:**
```bash
# OpenOCD起動
openocd -f openocd.cfg

# 別ターミナルでGDB起動
arm-none-eabi-gdb build/project.elf
(gdb) target remote localhost:3333
(gdb) monitor reset halt
(gdb) load
(gdb) continue
```

---

## 推奨開発フロー

### 1. プロジェクト初期化
- STM32CubeMXでプロジェクト生成
- ピン設定、クロック設定、周辺機能設定

### 2. コード開発
- Cursorでコード編集
- AI支援を活用したコード生成

### 3. ビルド・アップロード
- PlatformIOまたはMakefileでビルド
- ST-Linkでアップロード

### 4. デバッグ
- ST-Link + OpenOCD + GDBでデバッグ
- またはPlatformIOの統合デバッガ

---

## トラブルシューティング

### 1. アップロードエラー
- ST-Linkが認識されているか確認
- ボードの電源が供給されているか確認
- BOOT0ピンの状態を確認

### 2. ビルドエラー
- ツールチェーンが正しくインストールされているか確認
- パスの設定を確認

### 3. デバッグエラー
- OpenOCDの設定を確認
- ST-Linkのファームウェアを更新

---

## まとめ

### CursorでSTM32F030F4P6開発は可能か？

**答え: ✅ はい、可能です**

**推奨方法:**
1. **PlatformIO**（最も簡単、統合環境）
2. **STM32CubeMX + Makefile**（柔軟性が高い）
3. **コマンドライン**（完全な制御）

**必要なもの:**
- Cursor（エディタ）
- PlatformIO拡張機能 または ツールチェーン
- ST-Linkデバッガ
- STM32CubeMX（オプション、コード生成用）
