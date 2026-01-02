#include "fortune_draw.h"
#include <TJpg_Decoder.h>
extern const char* fortunes[];
extern const int fortuneCount;
extern void drawSelect();  // 声明 drawSelect 函数
extern const unsigned short select_img[650];



void drawFortune() {
  delay(300);
  // 背景
  TJpgDec.drawFsJpg(0, 0, "/fortune_bg.jpg");

  // 显示文字内容边框
  //tft.drawRect(7, 7, 113, 86, TFT_BLACK);  // 黑色边框框住文字区域

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
      //tft.fillRect(7, 98, 49, 25, TFT_WHITE);
      //tft.fillRect(72, 98, 49, 25, TFT_WHITE);

      // Again 按钮
      if (selectAgain) {
        //tft.fillRect(7, 98, 49, 25, 0xAEDC);
        //tft.drawRect(7, 98, 49, 25, TFT_BLACK);
        //tft.setTextColor(TFT_BLACK, 0xAEDC);
        //tft.drawString("Again", 12, 103, 2);
        TJpgDec.drawFsJpg(7, 99, "/fortune_again.jpg");
      } else {
        //tft.setTextColor(TFT_BLACK, TFT_WHITE);
        //tft.drawString("Again", 12, 103, 2);
        TJpgDec.drawFsJpg(7, 99, "/fortune_exit.jpg");
      }

      // Exit 按钮
      if (!selectAgain) {
        //tft.fillRect(72, 98, 49, 25, 0xAEDC);
        //tft.drawRect(72, 98, 49, 25, TFT_BLACK);
        //tft.setTextColor(TFT_BLACK, 0xAEDC);
        //tft.drawString("Exit", 77, 103, 2);
        TJpgDec.drawFsJpg(7, 99, "/fortune_exit.jpg");
      } else {
        //tft.setTextColor(TFT_BLACK, TFT_WHITE);
        //tft.drawString("Exit", 77, 103, 2);
        TJpgDec.drawFsJpg(7, 99, "/fortune_again.jpg");
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
        selectAgain = !selectAgain;
      } else {
        BackMainMnue();
      }
      inMenu = false;
    }
    if (digitalRead(BUTTON_Left) == LOW) {
      lastInput = millis();         // 有操作 → 重置空闲计时,millis是计时器！！
      delay(20);                    // 简易去抖
    }
    else if (digitalRead(BUTTON_Right) == LOW) {
      lastInput = millis();         // 有操作 → 重置空闲计时,millis是计时器！！
      delay(20);                    // 简易去抖
    }
    
    // ── 超时自动息屏 + Light-sleep ──────────────
    if (millis() - lastInput > idleTimeout) {
      goLightSleep();               // 进入休眠
      lastInput = millis();         // 醒来后lastInput=现在的时间
    }
  }
}
