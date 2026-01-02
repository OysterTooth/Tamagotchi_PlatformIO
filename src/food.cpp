#include "food.h"
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include "food_select.h"

extern void drawSelect();  // 声明 drawSelect 函数

const Pos kCursor[4] = { {4,4}, {66,4}, {4,66}, {66,66} };

const char* const kBgPath[] PROGMEM = { "/food1.jpg", "/food2.jpg" };
constexpr uint8_t kBgCnt = sizeof(kBgPath) / sizeof(kBgPath[0]); // ★放在所有函数之前

static uint8_t bgIdx  = 0;
static uint8_t curIdx = 0;

/*------------------ 辅助绘图 ------------------*/
void drawBackground(uint8_t page){
  TJpgDec.drawFsJpg(0,0,kBgPath[page]);
}
void drawSelector(const Pos& p){
  drawTransparentImage(p.x-1, p.y-1,
                       FOOD_SEL_W, FOOD_SEL_H,
                       food_select, FOOD_SEL_COLOR);
}

/*------------------ 对外接口 ------------------*/
void enterFoodMenu(){
  uint32_t Now = millis();
  bool foodExit = false;
  delay(300);  // 避免多次触发
  bgIdx = curIdx = 0;
  drawBackground(bgIdx);
  drawSelector(kCursor[curIdx]);
  while(foodExit != true){
    if (digitalRead(BUTTON_Left) == LOW){
        food_onLeft();
        lastInput = millis();
        delay(300);  // 避免多次触发
    }
    
    if (digitalRead(BUTTON_Right) == LOW){
      lastInput = millis(); 
      foodExit = true;// 跳出while，切回主菜单
      tft.fillScreen(TFT_BLACK);
      tft.setTextDatum(MC_DATUM);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(2);
      tft.drawString("EATING...", 64, 64);
      if (hungerNum < 9){
       hungerNum ++;
      }
      delay(1000);
      BackMainMnue();
      }    
      // ── 超时自动息屏 + Light-sleep ──────────────
      if (millis() - lastInput > idleTimeout) {
        goLightSleep();               // 进入休眠
        lastInput = millis();         // 醒来后lastInput=现在的时间
     }
}
}

void food_onLeft(){
  curIdx = (curIdx + 1) & 0x03;
  if (curIdx == 0) bgIdx = (bgIdx + 1) % kBgCnt;   // ★kBgCnt 现在可见
  drawBackground(bgIdx);
  drawSelector(kCursor[curIdx]);
}

bool food_onRight(){
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("EATING...", 64, 64);

  delay(1000);
  return true;
}
