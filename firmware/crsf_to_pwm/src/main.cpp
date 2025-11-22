#include <Arduino.h>

#ifndef CRSF_UART_NUM
#define CRSF_UART_NUM 1
#endif

#ifndef CRSF_UART_RX_PIN
#define CRSF_UART_RX_PIN 16
#endif

#ifndef PWM_PIN_CH1
#define PWM_PIN_CH1 18
#endif

#ifndef PWM_PIN_CH2
#define PWM_PIN_CH2 19
#endif

#ifndef PWM_PIN_CH3
#define PWM_PIN_CH3 21
#endif

namespace {

constexpr uint8_t CRSF_SYNC_BYTE = 0xC8;
constexpr uint8_t CRSF_TYPE_CHANNELS = 0x16;
constexpr uint32_t CRSF_BAUDRATE = 420000;
constexpr uint8_t CRSF_MAX_FRAME_SIZE = 64;
constexpr uint8_t CRSF_RC_PAYLOAD_SIZE = 22;
constexpr uint8_t CRSF_CHANNEL_COUNT = 16;

constexpr uint8_t PWM_FREQ_HZ = 50;
constexpr uint8_t PWM_RES_BITS = 16;
constexpr uint16_t PWM_PERIOD_US = 20000;
constexpr uint16_t FAILSAFE_US = 1500;
constexpr uint32_t FRAME_TIMEOUT_US = 100000;  // 100 ms

struct PwmMap {
  uint8_t ledcChannel;
  uint8_t pin;
  uint8_t crsfChannelIndex;
};

// CRSF チャネル番号と PWM ピンの対応をここで調整する。
const PwmMap PWM_CHANNELS[] = {
    {0, PWM_PIN_CH1, 0},
    {1, PWM_PIN_CH2, 1},
#if defined(PWM_PIN_CH3)
    {2, PWM_PIN_CH3, 2},
#endif
};
constexpr size_t PWM_OUTPUT_COUNT = sizeof(PWM_CHANNELS) / sizeof(PwmMap);

HardwareSerial& crsfSerial =
    (CRSF_UART_NUM == 0) ? Serial : ((CRSF_UART_NUM == 2) ? Serial2 : Serial1);

uint16_t latestChannels[CRSF_CHANNEL_COUNT]{};
uint32_t lastFrameMicros = 0;
uint32_t lastLogMillis = 0;
constexpr uint32_t LOG_INTERVAL_MS = 250;

uint8_t crc8D5(const uint8_t* data, uint8_t len) {
  uint8_t crc = 0;
  for (uint8_t i = 0; i < len; ++i) {
    crc ^= data[i];
    for (uint8_t b = 0; b < 8; ++b) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0xD5;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

uint16_t mapCrsfToMicros(uint16_t value) {
  const uint16_t minIn = 172;
  const uint16_t maxIn = 1811;
  value = constrain(value, minIn, maxIn);
  const int32_t numerator = static_cast<int32_t>(value) - 992;
  const int32_t micros = 1000 + (numerator * 1000) / 820;
  return constrain(micros, 1000, 2000);
}

uint32_t microsToDuty(uint16_t pulseUs) {
  const uint32_t maxDuty = (1u << PWM_RES_BITS) - 1;
  return static_cast<uint32_t>(pulseUs) * maxDuty / PWM_PERIOD_US;
}

void applyFailsafe() {
  for (size_t i = 0; i < PWM_OUTPUT_COUNT; ++i) {
    ledcWrite(PWM_CHANNELS[i].ledcChannel, microsToDuty(FAILSAFE_US));
  }
}

void updatePwmOutputs() {
  for (size_t i = 0; i < PWM_OUTPUT_COUNT; ++i) {
    const auto& pwm = PWM_CHANNELS[i];
    if (pwm.crsfChannelIndex >= CRSF_CHANNEL_COUNT) {
      continue;
    }
    const uint16_t pulse = mapCrsfToMicros(latestChannels[pwm.crsfChannelIndex]);
    ledcWrite(pwm.ledcChannel, microsToDuty(pulse));
  }
}

void decodeChannels(const uint8_t* payload) {
  static const uint32_t mask = 0x7FF;
  for (uint8_t ch = 0; ch < CRSF_CHANNEL_COUNT; ++ch) {
    const uint16_t bitOffset = ch * 11;
    const uint8_t byteOffset = bitOffset / 8;
    const uint8_t bitShift = bitOffset % 8;

    uint32_t value = payload[byteOffset];
    const uint8_t next1 = (byteOffset + 1 < CRSF_RC_PAYLOAD_SIZE) ? payload[byteOffset + 1] : 0;
    const uint8_t next2 = (byteOffset + 2 < CRSF_RC_PAYLOAD_SIZE) ? payload[byteOffset + 2] : 0;
    value |= static_cast<uint32_t>(next1) << 8;
    value |= static_cast<uint32_t>(next2) << 16;

    latestChannels[ch] = (value >> bitShift) & mask;
  }
}

void handleFrame(const uint8_t* frame, uint8_t frameLen) {
  if (frameLen < 5) {
    return;
  }
  const uint8_t lengthField = frame[1];
  if (lengthField + 2 != frameLen) {
    return;
  }
  const uint8_t type = frame[2];
  const uint8_t* payload = &frame[3];
  const uint8_t crc = frame[frameLen - 1];
  const uint8_t computed = crc8D5(&frame[2], lengthField - 1);
  if (crc != computed) {
    return;
  }

  if (type == CRSF_TYPE_CHANNELS && lengthField - 1 >= CRSF_RC_PAYLOAD_SIZE) {
    decodeChannels(payload);
    updatePwmOutputs();
    lastFrameMicros = micros();
  }
}

void pollCrsf() {
  static uint8_t frame[CRSF_MAX_FRAME_SIZE];
  static uint8_t index = 0;
  static uint8_t expected = 0;

  while (crsfSerial.available()) {
    const uint8_t byte = crsfSerial.read();

    if (index == 0) {
      if (byte != CRSF_SYNC_BYTE) {
        continue;
      }
      frame[index++] = byte;
      continue;
    }

    frame[index++] = byte;
    if (index == 2) {
      expected = byte + 2;
      if (expected > CRSF_MAX_FRAME_SIZE) {
        index = 0;
      }
      continue;
    }

    if (index == expected && expected != 0) {
      handleFrame(frame, index);
      index = 0;
      expected = 0;
    } else if (index >= CRSF_MAX_FRAME_SIZE) {
      index = 0;
      expected = 0;
    }
  }
}

void initPwm() {
  for (size_t i = 0; i < PWM_OUTPUT_COUNT; ++i) {
    const auto& pwm = PWM_CHANNELS[i];
    ledcSetup(pwm.ledcChannel, PWM_FREQ_HZ, PWM_RES_BITS);
    ledcAttachPin(pwm.pin, pwm.ledcChannel);
  }
  applyFailsafe();
}

void maybeLogChannels() {
  const uint32_t now = millis();
  if (now - lastLogMillis < LOG_INTERVAL_MS) {
    return;
  }
  lastLogMillis = now;

  Serial.print(F("[CRSF] PWM(us): "));
  for (size_t i = 0; i < PWM_OUTPUT_COUNT; ++i) {
    const auto& pwm = PWM_CHANNELS[i];
    const uint16_t pulse = mapCrsfToMicros(latestChannels[pwm.crsfChannelIndex]);
    Serial.print(F("CH"));
    Serial.print(pwm.crsfChannelIndex + 1);
    Serial.print('=');
    Serial.print(pulse);
    Serial.print(' ');
  }
  Serial.println();
}

void initUart() {
#if CRSF_UART_NUM == 0
  crsfSerial.begin(CRSF_BAUDRATE, SERIAL_8N1);
#elif CRSF_UART_NUM == 1
  crsfSerial.begin(CRSF_BAUDRATE, SERIAL_8N1, CRSF_UART_RX_PIN, -1);
#else
  crsfSerial.begin(CRSF_BAUDRATE, SERIAL_8N1, CRSF_UART_RX_PIN, -1);
#endif
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println(F("[CRSF] boot"));
  initUart();
  initPwm();
  lastFrameMicros = micros();
}

void loop() {
  pollCrsf();
  maybeLogChannels();
  if (micros() - lastFrameMicros > FRAME_TIMEOUT_US) {
    applyFailsafe();
    lastFrameMicros = micros();
  }
}
