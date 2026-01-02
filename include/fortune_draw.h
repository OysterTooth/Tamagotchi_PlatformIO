#ifndef FORTUNE_DRAW_H
#define FORTUNE_DRAW_H

#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <U8g2_for_TFT_eSPI.h>
#include "fortune.h"  // 你的 fortune[] 句子数组
#include "wrapped_text.h"  // drawWrappedChineseText 函数
#include "select.h"   // drawSelect()
#include "colors.h"  // CANDY_BLUE 等颜色

extern TFT_eSPI tft;
extern TJpg_Decoder TJpgDec;
extern U8g2_for_TFT_eSPI u8g2;



#define BUTTON_Left 0
#define BUTTON_Right 47

void drawFortune();

#endif
