#pragma once
#include <Arduino.h>

#define BUTTON_Left 0
#define BUTTON_Right 47

class TFT_eSPI;                // 前向声明
extern TFT_eSPI tft;           // 主程序里唯一实例

/*—— 对外接口 ——*/
void enterFoodMenu();
void food_onLeft();
bool food_onRight();
bool food_isBusy();

/*—— drawTransparentImage 在 Tmgz.ino 里实现 ——*/
void drawTransparentImage(int16_t x,int16_t y,int16_t w,int16_t h,
                          const uint16_t *img,uint16_t transparentColor);

extern void goLightSleep();
extern uint32_t lastInput;
extern const uint32_t idleTimeout;

extern int hungerNum;
extern void BackMainMnue();

/*—— 选框位图参数（改名前缀避免冲突） ——*/
#include "food_select.h"
const uint8_t  FOOD_SEL_W     = 58;
const uint8_t  FOOD_SEL_H     = 58;
const uint16_t FOOD_SEL_COLOR = 0x0000;

struct Pos { uint8_t x, y; };
extern const Pos kCursor[4];
