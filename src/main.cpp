#include "lcd_cmd.h"
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include "SPIFFS.h"
#include "SD.h"  // 直接使用 ESP32 SD 版本，这个不能删，也不一定不能删，写上了"SD.h" 对应多个库的报错也还在
#include <Preferences.h> // ESP32 的 NVS 存储库
#include <U8g2_for_TFT_eSPI.h>//显示中文文字要用的库
#include <WiFi.h>//Wifi
#include <esp_bt.h>///蓝牙
#include <esp_sleep.h>//休眠

#include "driver/gpio.h"//睡眠用的
#include "esp_sleep.h"//睡眠用的

#include "select.h" 
#include "fortune.h"
#include "wrapped_text.h"
#include "fortune_draw.h"
#include "food.h"
#include "hunger.h"
#include "emptyHungerBar.h"
#include "Char1_F_Stand1.h"
#include "Char1_F_Stand2.h"
#include "Char1_F_Stand3.h"
#include "CharBg.h"
#include "Out.h"
#include "menu.h"

#define LCD_BL   10 // 背光 GPIO

#define BUTTON_Right 47//右边按钮define
#define BUTTON_Left 0 //左边按钮define

#define LCD_BL_ON  LOW   // 板子是低电平点亮
#define LCD_BL_OFF HIGH

TFT_eSPI tft = TFT_eSPI();//显示图片用的
TFT_eSprite  spr = TFT_eSprite(&tft);//用于动画去闪，用 Sprite 先在 RAM 里合成，再一次推屏
TFT_eSprite MenuSpr = TFT_eSprite(&tft);
uint32_t Now = millis();

U8g2_for_TFT_eSPI u8g2;  // 声明中文显示的对象
Preferences preferences;  // NVS 存储实例
extern TJpg_Decoder TJpgDec; // 使用外部定义的实例

/////////////////自动息屏///////////////////////////////////
const uint64_t ONE_HOUR = 3600ULL * 1000000ULL;
const uint32_t idleTimeout = 1200000;   // 1200 s 无操作自动息屏
uint32_t lastInput = 0;
//////////////////////////////////////////////////////////

int brightness_x = 29;  // 全局变量，存储亮度的 X 位置
int brightness_y = 19;
int sound_x = 33;       // 全局变量，存储音量的 X 位置
int sound_y = 51;

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {//显示图片用的，初始化设置
    if (y >= tft.height()) return 0;//检测图片是不是太大了，感觉没什么用
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

void drawTransparentImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *image, uint16_t transparentColor) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            uint16_t color = image[j * w + i];
            if (color != transparentColor) {  // 跳过背景色
                tft.drawPixel(x + i, y + j, color);
            }
        }
    }
}

/////////////////////////////////  select & main menu  ////////////////////////////////////////////////////////////////////////////////////////////
void initMenuSprite()
{
  MenuSpr.setColorDepth(16);//1 bit(黑白）,16bit(RGB565)(默认），24/32 bit(RGB888/ARGB8888)
  MenuSpr.createSprite(128, 29);          // Menu尺寸
  MenuSpr.setSwapBytes(true); //对调RB,要不颜色会乱
}

int mainSelect_x = 1;  // 起始select框位置

void drawSelect() {
    //drawTransparentImage(mainSelect_x, 101, 25, 26, select_img, 0x0000); // 设置黑色替换成透明色
    MenuSpr.pushImage(mainSelect_x, 2, 25, 26, select_img, 16);  // 画select框
    MenuSpr.pushSprite(0, 99, TFT_BLACK);  // 一次性推上屏幕,最后一个参数TFTblack是透明
}

void clearSelect() {
    //TJpgDec.drawFsJpg(0, 99, "/menu.jpg");    // 用背景覆盖原位置
    MenuSpr.fillSprite(TFT_BLACK);
    MenuSpr.pushImage(0,0, 128, 29, menu, 16);  // Menu
}

void moveSelect() {
    clearSelect();  // 清除旧位置
    mainSelect_x += 25;  // 向右移动 25 像素

    if (mainSelect_x > 101){
      mainSelect_x = 1;  // 超出范围则回到起点
    }
    drawSelect(); // 在新位置重绘  
}

void Exit() {
    int caseSelect_x = 90, caseSelect_y = 10;
    tft.drawString("NO Exit", 90, 10, 2);
    tft.drawString("Exit", 90, 90, 2);

    bool exitLoop = false;  // 控制退出逻辑

    while (!exitLoop) {  // 仅在 exitLoop = true 时才退出
        if (digitalRead(BUTTON_Left) == LOW) {  
            caseSelect_y = (caseSelect_y == 10) ? 90 : 10;  // 在 10 和 90 之间切换
            tft.fillScreen(TFT_BLACK);  // 清除之前的选项
            tft.drawString("NO Exit", 90, 10, 2);
            tft.drawString("Exit", 90, 90, 2);
            tft.drawString(">", 70, caseSelect_y, 2);  // 显示选择的 `>`
            delay(300);
        }

        if (digitalRead(BUTTON_Right) == LOW) {  
            if (caseSelect_y == 90) {  // 选择 Exit
                TJpgDec.drawFsJpg(0, 0, "/background.jpg");
                drawSelect();
                exitLoop = true;  // 退出 `while`
                delay(500);
            }
            if (caseSelect_y == 10) {  // 选择 NO Exit
                tft.drawString("Still Here", 90, 50, 2);
                delay(500);
            }
        }
    }
}

void mainselect()
{
  if (digitalRead(BUTTON_Left) == LOW) {  // 左边移动按钮被按下
        moveSelect();  // 移动select框
        delay(300);  // 避免多次触发
    }
    
  if (digitalRead(BUTTON_Right) == LOW) {  // 右边确认按钮被按下    
       switch(mainSelect_x)
        {
          case 1: 
            drawFortune();
            break;
      
          case 26:
            enterFoodMenu();       // 进入 Food 菜单
            break;
      
          case 51:
            TJpgDec.drawFsJpg(0, 0, "/full_size_number_3.jpg"); 
            Exit();
            break;
      
          case 76:
            OutMenu();
            break;
      
          case 101:
            TJpgDec.drawFsJpg(0, 0, "/full_size_number_5.jpg");
            Exit(); 
            break;
        }
    }
}

/////////////////////////////////////////////   setting   //////////////////////////////////////////////////////////

void setting_memory(){
  TJpgDec.drawFsJpg(0, 0, "/setting_bg.jpg");
  
  // 读取上一次的值（以防 setup 中没有成功读取）
  preferences.begin("settings", false);
  brightness_x = preferences.getInt("brightness", 29);
  sound_x = preferences.getInt("sound", 33);
  preferences.end();

  // 显示上一次存储的亮度图像
  for (int x = 29; x < brightness_x; x += 32) {
    TJpgDec.drawFsJpg(x, brightness_y, "/setting_Brightness.jpg");
  }

  // 显示上一次存储的音量图像
  if(sound_x <= 129){
    for (int x = 33; x < sound_x; x += 32) {
        TJpgDec.drawFsJpg(x, sound_y, "/setting_sound.jpg");
        }
  }
  
}

void setting(){
  setting_memory(); 
  int settingSelect_y = 23;
  bool exitLoop = false;  // 控制退出逻辑
  int exitBrightness = 0;
  int exitSound = 0;
  
    while (!exitLoop) {  // 仅在 exitLoop = true 时才退出
      if (digitalRead(BUTTON_Left) == LOW) {
        if(settingSelect_y > 85){
          settingSelect_y = 23;
          }
          setting_memory();
          TJpgDec.drawFsJpg(5, settingSelect_y, "/setting_select.jpg");
          settingSelect_y += 31;
          delay(300);
      }
      
      if (digitalRead(BUTTON_Right) == LOW) {
        setting_memory(); 
        delay(300);
        switch(settingSelect_y)
        {
          case 54: 
            exitBrightness = 0;
            TJpgDec.drawFsJpg(5, 23, "/setting_select.jpg");
            while (exitBrightness == 0) {
              if (digitalRead(BUTTON_Left) == LOW) {
                if (brightness_x > 120){
                  brightness_x = 29;  // 超出范围则回到起点
                  TJpgDec.drawFsJpg(0, 0, "/setting_bg.jpg"); //清除之前的选择
                  TJpgDec.drawFsJpg(5, 23, "/setting_select.jpg");
                  delay(300);
                }
                TJpgDec.drawFsJpg(brightness_x, brightness_y, "/setting_Brightness.jpg");
                brightness_x += 32;
                delay(300);
              }
              if (digitalRead(BUTTON_Right) == LOW) {
                exitBrightness = 1;
                delay(300);
              }
            }
            // **存储 brightness_x 到 NVS**
            preferences.begin("settings", false);
            preferences.putInt("brightness", brightness_x);
            preferences.end();
            break;
            
            
          case 85: 
            exitSound = 0;
            TJpgDec.drawFsJpg(5, 54, "/setting_select.jpg");
            sound_x = 33;
            while (exitSound == 0) {
              if (digitalRead(BUTTON_Left) == LOW) {
                if (sound_x > 129){
                  sound_x = 33;  // 超出范围则回到起点
                  TJpgDec.drawFsJpg(0, 0, "/setting_bg.jpg"); //清除之前的选择
                  TJpgDec.drawFsJpg(5, 54, "/setting_select.jpg");
                  delay(300);
                }
                if (sound_x == 129){
                  TJpgDec.drawFsJpg(0, 0, "/setting_bg.jpg"); //清除之前的选择
                  TJpgDec.drawFsJpg(5, 54, "/setting_select.jpg");
                  delay(300);
                }
                TJpgDec.drawFsJpg(sound_x, sound_y, "/setting_sound.jpg");
                sound_x += 32;
                //tft.drawString(String(sound_x), 50, 30, 4);
                delay(300);
              }
              if (digitalRead(BUTTON_Right) == LOW) {
                exitSound = 1;
                delay(300);
              }
            }
            // **存储 sound_x 到 NVS**
            preferences.begin("settings", false);
            preferences.putInt("sound", sound_x);
            preferences.end();
            break;
            
          case 116: 
            TJpgDec.drawFsJpg(0, 0, "/background.jpg");
            drawSelect();
            exitLoop = true;  // 退出 `while`
            delay(500);
            break;         
         }
      }
    }
}

///////////////////////////休眠/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
void lcdSleep()
{
  digitalWrite(LCD_BL, LCD_BL_OFF);          // ① 关背光
  //pinMode(LCD_BL, INPUT);  // 进入 Hi-Z，让背光驱动彻底断电，好像不需要这个，LCD_BL_OFF设置对了就不会黑屏但有背光了，
  //之前黑屏但是有背光可能是因为背光设成low（打开背光），但是diaplay off了
  tft.writecommand(TFT_DISPOFF);      // ② LCD 关闭输出
  tft.writecommand(TFT_SLPIN);        // ③ 进入休眠
  delay(5);
}

void lcdWake()
{
  tft.writecommand(TFT_SLPOUT);       // ① 退出休眠
  delay(120);                         // GC9107 典型 120 ms
  tft.writecommand(TFT_DISPON);       // ② 打开输出
  digitalWrite(LCD_BL, LCD_BL_ON);         // ③ 开背光
}

void wifiBtOff()
{
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  btStop();                 // 若未启用 BT 可省略
}

void armLightSleepGpioWakeup()
{

  /* 允许这个 GPIO 作唤醒源（低电平触发）*/
  gpio_wakeup_enable((gpio_num_t)BUTTON_Left, GPIO_INTR_LOW_LEVEL);

  /* 打开 GPIO 唤醒功能（整个芯片只有一次）*/
  esp_sleep_enable_gpio_wakeup();
}

void goLightSleep()
{
  lcdSleep();
  wifiBtOff();

  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON); //保持 RTC 外设上电
  armLightSleepGpioWakeup();
  
  esp_light_sleep_start();   // ——进入 Light-sleep——
  /* 醒来后从这里继续 */

  lcdWake();

  lastInput = millis(); // 重新计时防止立刻再睡lastInput=现在的时间
}

///////////////////////////////////hunger//////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
const uint32_t T_hungerDown = 10000;         // 10 s
uint32_t lastHungerDown   = 0;                // 上次移动的时间戳
int hungerNum = 6;
const char* HungerBar[7] = { "/HungerBar1.jpg", "/HungerBar2.jpg", "/HungerBar3.jpg", "/HungerBar4.jpg", "/HungerBar5.jpg", "/HungerBar6.jpg", "/HungerBar7.jpg"};

void drawHunger(){
  TJpgDec.drawFsJpg(4, 4, HungerBar[hungerNum]);
}

void hungerBar(){
  Now = millis();
  if (Now - lastHungerDown >= T_hungerDown)         // 到 10 s 了
  {
    if (hungerNum > 0){
       hungerNum --;
    }
    drawHunger();
    lastHungerDown = Now;                        // 重新计时
  }
}

///////////////////////////////     人物     ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
uint32_t lastFlipChar = 0;                   // 上次切换的时间戳
uint8_t  Char1_F_Stand_Idx     = 0;

// 定义要循环的序列：索引 0→1→2→1，然后再重复
const uint8_t CharPattern[] = { 1, 2, 3, 2 };

void initCharSprite()
{
  spr.setColorDepth(16);//1 bit(黑白）,16bit(RGB565)(默认），24/32 bit(RGB888/ARGB8888)
  spr.createSprite(120, 78);          // 背景尺寸
  spr.setSwapBytes(true); //对调RB,要不颜色会乱
}

void CharStand(){
  if (Now - lastFlipChar >= 400) {
    Char1_F_Stand_Idx = (Char1_F_Stand_Idx + 1) % 4;
    lastFlipChar = Now;
    spr.fillSprite(TFT_BLACK);
    spr.pushImage(0, 0, 120, 78, CharBg, 16);
    switch (CharPattern[Char1_F_Stand_Idx]){
      case 1:
        spr.pushImage(42, 26, 36, 46, Char1_F_Stand1, 16);  // Char贴图
        spr.pushSprite(4, 21, TFT_BLACK);  // 一次性推上屏幕,最后一个参数TFTblack是透明
        break;
      case 2:
        spr.pushImage(42, 26, 36, 46, Char1_F_Stand2, 16);  // Char贴图
        spr.pushSprite(4, 21, TFT_BLACK);  // 一次性推上屏幕,最后一个参数TFTblack是透明
        break;
      case 3:
        spr.pushImage(42, 26, 36, 46, Char1_F_Stand3, 16);  // Char贴图
        spr.pushSprite(4, 21, TFT_BLACK);  // 一次性推上屏幕,最后一个参数TFTblack是透明
        break;
    }
  }
}



//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void BackMainMnue(){
  TJpgDec.drawFsJpg(0, 0, "/background.jpg");
  CharStand(); 
  drawHunger(); 
  drawSelect();
  delay(300);
}
void setup() {
    Now = millis();//现在的时间
    Serial.begin(9600);//初始化设置别碰
    tft.init();//这个也是
    tft.setRotation(0);//也是
    tft.setSwapBytes(true);//tft pushimg修正颜色
    u8g2.begin(tft);//显示中文用的
    tft.fillScreen(TFT_BLACK);//先显示个黑屏？

    if (!SPIFFS.begin(true)) { //检查按钮初始化有没有成功
        Serial.println("SPIFFS 初始化失败");
        return;
    }

    pinMode(BUTTON_Right, INPUT_PULLUP);  // 按钮输入（上沿）不知道为什么得在（ESP32 JPEG 解码库）的配置选项上面
    pinMode(BUTTON_Left, INPUT_PULLUP);  // 按钮输入（上沿）不知道为什么得在（ESP32 JPEG 解码库）的配置选项上面

    pinMode(LCD_BL, OUTPUT);      // 提前声明为输出
    //digitalWrite(LCD_BL, LCD_BL_ON);   // 正常工作时先开背光
  
    //TJpg_Decoder（ESP32 JPEG 解码库）的配置选项
    TJpgDec.setCallback(tft_output);//设定 解码回调，让 JPEG 画到屏幕
    TJpgDec.setJpgScale(1);//100%显示jpg，如果2就是缩小1/2,4就是缩小1/4
    //TJpgDec.setSwapBytes(true);//修正 RGB 颜色顺序
    
    TJpgDec.drawFsJpg(0, 0, "/background.jpg");
    
    initMenuSprite();
    drawSelect();// 初始绘制select
    drawHunger();
    initCharSprite();
    CharStand();

    // **读取 NVS 里的亮度 & 音量**
    preferences.begin("settings", false);  // 打开 NVS
    brightness_x = preferences.getInt("brightness", 29);  // 读取亮度（默认29）
    sound_x = preferences.getInt("sound", 33);  // 读取音量（默认33）
    preferences.end();  // 关闭 NVS
    
    //到点自动息屏
    lastInput = millis();
}

void loop() {
  Now = millis();//现在的时间
  mainselect();
  CharStand();
  hungerBar();
  Light_Bar();
  PetFood_Bar();

    // ── 检测按键 ────────────────────────────────
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
