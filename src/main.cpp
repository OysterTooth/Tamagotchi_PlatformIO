#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include "SPIFFS.h"
#include <U8g2_for_TFT_eSPI.h>
#include <Preferences.h>

#define BUTTON_Right 47//右边按钮define
#define BUTTON_Left 0 //左边按钮define


/*int brightness_x = 29;  // 全局变量，存储亮度的 X 位置
int brightness_y = 19;
int sound_x = 33;       // 全局变量，存储音量的 X 位置
int sound_y = 51;*/

TFT_eSPI tft = TFT_eSPI();//显示图片用的
U8g2_for_TFT_eSPI u8g2;  // 声明中文显示的对象
Preferences preferences;  // NVS 存储实例
extern TJpg_Decoder TJpgDec; // 使用外部定义的实例

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {//显示图片用的，初始化设置
    if (y >= tft.height()) return 0;//检测图片是不是太大了，感觉没什么用
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

void setup() {
  Serial.begin(115200);//初始化，9600也行，不知道两个数有什么区别
  tft.init();//这个也是
  tft.setRotation(0);//也是
  u8g2.begin(tft);//显示中文用的
  tft.fillScreen(TFT_BLACK);//先显示个黑屏？

  if (!SPIFFS.begin(true)) { //检查按钮初始化有没有成功
      Serial.println("SPIFFS 初始化失败");
      return;
  }
  pinMode(BUTTON_Right, INPUT_PULLUP);  // 按钮输入（上沿）不知道为什么得在（ESP32 JPEG 解码库）的配置选项上面
  pinMode(BUTTON_Left, INPUT_PULLUP);  // 按钮输入（上沿）不知道为什么得在（ESP32 JPEG 解码库）的配置选项上面

  //TJpg_Decoder（ESP32 JPEG 解码库）的配置选项
  TJpgDec.setCallback(tft_output);//设定 解码回调，让 JPEG 画到屏幕
  TJpgDec.setJpgScale(1);//100%显示jpg，如果2就是缩小1/2,4就是缩小1/4
  TJpgDec.setSwapBytes(true);//修正 RGB 颜色顺序，这个和下一行的二选一开就行
  //tft.setSwapBytes(true);
    
  TJpgDec.drawFsJpg(0, 0, "/background.jpg");

}

  //tft.drawString("JPG done", 10, 10, 2);

void loop() {
  delay(1000);
}
