#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include "SPIFFS.h"
#include <U8g2_for_TFT_eSPI.h>
#include <Preferences.h>

TFT_eSPI tft;
U8g2_for_TFT_eSPI u8f;
Preferences prefs;

// TJpg_Decoder 回调：把解码出来的块画到屏幕
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  if (y >= tft.height()) return 0;          // 超出屏幕就丢弃
  if (!bitmap) return 0;                    // 防御：防止空指针
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

void setup() {
  Serial.begin(115200);
  delay(200);

  #ifdef TFT_BL
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH); // 若黑屏改 LOW
  #endif

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
    while (1) delay(1000);
  }
  Serial.println("SPIFFS mounted");

  // 这俩通常建议打开（不一定是崩溃原因，但很常用）
  tft.setSwapBytes(true);
  TJpgDec.setSwapBytes(true);

  TJpgDec.setCallback(tft_output);

  Serial.println("Drawing /background.jpg ...");
  uint16_t rc = TJpgDec.drawFsJpg(0, 0, "/background.jpg");
  Serial.printf("draw rc = %u\n", rc);

  tft.drawString("JPG done", 10, 10, 2);
}

void loop() {
  delay(1000);
}
