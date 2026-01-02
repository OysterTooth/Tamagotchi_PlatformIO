#ifndef WRAPPED_TEXT_H
#define WRAPPED_TEXT_H

#include <Arduino.h>
#include <U8g2_for_TFT_eSPI.h>

void drawWrappedChineseText(U8g2_for_TFT_eSPI &u8g2, const String &text, int x, int y, int maxWidth, int lineHeight);

#endif
