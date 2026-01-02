#pragma once
#include <Arduino.h>

#define BUTTON_Left 0
#define BUTTON_Right 47

class TFT_eSPI;                // 前向声明
extern TFT_eSPI tft;           // 主程序里唯一实例

extern void goLightSleep();
extern uint32_t lastInput;
extern const uint32_t idleTimeout;

extern void BackMainMnue();

/*—— 对外接口 ——*/
void OutMenu();
void Light_Bar();
void PetFood_Bar();
