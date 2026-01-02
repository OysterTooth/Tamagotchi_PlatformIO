#include "wrapped_text.h"

void drawWrappedChineseText(U8g2_for_TFT_eSPI &u8g2, const String &text, int x, int y, int maxWidth, int lineHeight) {
  String currentLine = "";
  int cursorX = x, cursorY = y;

  for (uint16_t i = 0; i < text.length(); i++) {
    currentLine += text[i];

    int linePixelWidth = u8g2.getUTF8Width(currentLine.c_str());
    if (linePixelWidth > maxWidth || text[i] == '\n') {
      u8g2.setCursor(cursorX, cursorY);
      u8g2.print(currentLine);
      currentLine = "";
      cursorY += lineHeight;
    }
  }

  if (currentLine.length() > 0) {
    u8g2.setCursor(cursorX, cursorY);
    u8g2.print(currentLine);
  }
}
