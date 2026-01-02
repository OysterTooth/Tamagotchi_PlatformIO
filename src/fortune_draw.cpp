#include "fortune_draw.h"
extern const char* fortunes[];
extern const int fortuneCount;
extern void drawSelect();  // 声明 drawSelect 函数
extern const unsigned short select_img[396];



void drawFortune() {
  // 背景
  TJpgDec.drawFsJpg(0, 0, "/fortune_bg.jpg");

  // 显示文字内容边框
  tft.drawRect(7, 7, 113, 86, TFT_BLACK);  // 黑色边框框住文字区域

  // fortune内容
  u8g2.setFont(u8g2_font_wqy15_t_gb2312);
  u8g2.setFontMode(1);
  u8g2.setFontDirection(0);
  u8g2.setForegroundColor(TFT_BLACK);
  u8g2.setCursor(12, 28);
  int index = random(fortuneCount);
  drawWrappedChineseText(u8g2, fortunes[index], 12, 36, 90, 18);

  // 按钮状态
  bool selectAgain = true;
  bool lastState = !selectAgain;
  bool inMenu = true;

  while (inMenu) {
    if (lastState != selectAgain) {
      // 清除按钮区域
      tft.fillRect(7, 98, 49, 25, TFT_WHITE);
      tft.fillRect(72, 98, 49, 25, TFT_WHITE);

      // Again 按钮
      if (selectAgain) {
        tft.fillRect(7, 98, 49, 25, CANDY_BLUE);
        tft.drawRect(7, 98, 49, 25, TFT_BLACK);
        tft.setTextColor(TFT_BLACK, CANDY_BLUE);
        tft.drawString("Again", 12, 103, 2);
      } else {
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
        tft.drawString("Again", 12, 103, 2);
      }

      // Exit 按钮
      if (!selectAgain) {
        tft.fillRect(72, 98, 49, 25, CANDY_BLUE);
        tft.drawRect(72, 98, 49, 25, TFT_BLACK);
        tft.setTextColor(TFT_BLACK, CANDY_BLUE);
        tft.drawString("Exit", 77, 103, 2);
      } else {
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
        tft.drawString("Exit", 77, 103, 2);
      }

      lastState = selectAgain;
    }

    // 按键操作
    if (digitalRead(BUTTON_Left) == LOW) {
      selectAgain = !selectAgain;
      delay(300);
    }

    if (digitalRead(BUTTON_Right) == LOW) {
      delay(300);
      if (selectAgain) {
        drawFortune();  // 重新抽签
      } else {
        TJpgDec.drawFsJpg(0, 0, "/background.jpg");
        drawSelect();
      }
      inMenu = false;
    }
  }
}
