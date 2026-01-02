#include "Out.h"
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>

const uint32_t T_LightDown = 10000;         // 10 s
uint32_t lastLightDown   = 0;                // 上次移动的时间戳
int LightNum = 3;
const char* LightBar[3] = { "/Light1.jpg", "/Light2.jpg", "/Light3.jpg"};

const uint32_t T_PetFoodDown = 10000;         // 10 s
uint32_t lastPetFoodDown   = 0;                // 上次移动的时间戳
int PetFoodNum = 3;
const char* PetFoodBar[3] = { "/PetFood1.jpg", "/PetFood2.jpg", "/PetFood3.jpg"};

void Light_Bar(){
  uint32_t Now = millis();
  if (Now - lastLightDown >= T_LightDown)         // 到 10 s 了
  {
    if (LightNum > 0){
       LightNum --;
    }
    lastLightDown = Now;                        // 重新计时
  }
}
void PetFood_Bar(){
  uint32_t Now = millis();
  if (Now - lastPetFoodDown >= T_PetFoodDown)         // 到 10 s 了
  {
    if (PetFoodNum > 0){
       PetFoodNum --;
    } 
    lastPetFoodDown = Now;                        // 重新计时
  }
}
void drawLightBar(){
  uint32_t Now = millis();
  if (Now - lastLightDown >= T_LightDown)         // 到 10 s 了
  {
    TJpgDec.drawFsJpg(20, 2, LightBar[LightNum]); 
  }
}
void drawPetFoodBar(){
    uint32_t Now = millis();
    if (Now - lastPetFoodDown >= T_PetFoodDown)         // 到 10 s 了
  {
    TJpgDec.drawFsJpg(20, 14, PetFoodBar[PetFoodNum]); 
  }
}

void Garden(){
    int GardenSelect_x = 5;
    bool GardenExit = false;
    lastInput = millis();
    delay(300);  // 避免多次触发
    TJpgDec.drawFsJpg(0, 0, "/Garden.jpg");
    TJpgDec.drawFsJpg(20, 2, LightBar[LightNum]); 
    TJpgDec.drawFsJpg(20, 14, PetFoodBar[PetFoodNum]);

    while(GardenExit != true){
        uint32_t Now = millis();
        if (digitalRead(BUTTON_Left) == LOW){
            lastInput = Now;
            TJpgDec.drawFsJpg(0, 0, "/Garden.jpg");
            tft.fillRect(5, 101, 104, 6, 0xACD3);//重新覆盖选择小三角的位置
            if (GardenSelect_x > 83){
                GardenSelect_x = 5;  // 超出范围则回到起点
            }
            tft.fillTriangle(GardenSelect_x, 101, GardenSelect_x+16, 101, GardenSelect_x+8, 106, 0x8266);//画选择小三角
            GardenSelect_x += 26;
            delay(300);
        }
        
        if (digitalRead(BUTTON_Right) == LOW){
            lastInput = Now;
            switch(GardenSelect_x){
            case 5:
                if (LightNum < 4){
                 LightNum ++;
                }
                tft.fillScreen(TFT_BLACK);
                tft.setTextDatum(MC_DATUM);
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.setTextSize(2);
                tft.drawString("Lighting...", 64, 64);
                delay(1000);
                TJpgDec.drawFsJpg(0, 0, "/Garden.jpg");
                TJpgDec.drawFsJpg(20, 2, LightBar[LightNum]); 
                TJpgDec.drawFsJpg(20, 14, PetFoodBar[PetFoodNum]);
                break;
            case 31:
                if (PetFoodNum < 4){
                 PetFoodNum ++;
                }
                tft.fillScreen(TFT_BLACK);
                tft.setTextDatum(MC_DATUM);
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.setTextSize(2);
                tft.drawString("Eatting...", 64, 64);
                delay(1000);
                TJpgDec.drawFsJpg(0, 0, "/Garden.jpg");
                TJpgDec.drawFsJpg(20, 2, LightBar[LightNum]); 
                TJpgDec.drawFsJpg(20, 14, PetFoodBar[PetFoodNum]);
                break;
            case 57:
                GardenExit = true;
                OutMenu();
                break;
            case 83:
                return;
            }
        }
              // ── 超时自动息屏 + Light-sleep ──────────────
       if (millis() - lastInput > idleTimeout) {
            goLightSleep();               // 进入休眠
            lastInput = millis();         // 醒来后lastInput=现在的时间
            TJpgDec.drawFsJpg(0, 0, "/Garden.jpg");
            TJpgDec.drawFsJpg(20, 2, LightBar[LightNum]); 
            TJpgDec.drawFsJpg(20, 14, PetFoodBar[PetFoodNum]);
       }
       drawLightBar();
       drawPetFoodBar();
    }
}

void OutMenu(){
  uint32_t Now = millis();
  int OutSelect_y = 15;
  bool OutExit = false;
  TJpgDec.drawFsJpg(0, 0, "/Out_Menu.jpg");
  delay(300);  // 避免多次触发
  
    while(OutExit != true){
        if (digitalRead(BUTTON_Left) == LOW){
            lastInput = millis();
            OutSelect_y += 29;
            tft.fillRect(8, OutSelect_y - 29, 9, 9, 0xFF19);//覆盖选择小方点
            if (OutSelect_y > 102){
                OutSelect_y = 15;  // 超出范围则回到起点
            }
            tft.fillRect(8, OutSelect_y, 9, 9, 0x8266);//画选择小方点
            delay(300);
        }
        
        if (digitalRead(BUTTON_Right) == LOW){
            lastInput = millis();
            switch(OutSelect_y){
            case 15:
                Garden();
                break;
            case 44:
                OutExit = true;
                BackMainMnue();
                break;
            case 73:
                //OutExit = true;
                BackMainMnue();
                //break;
                return;
            case 102:
                //OutExit = true;
                BackMainMnue();
                //break;
                return;
            }
        }
              // ── 超时自动息屏 + Light-sleep ──────────────
       if (millis() - lastInput > idleTimeout) {
            goLightSleep();               // 进入休眠
            lastInput = millis();         // 醒来后lastInput=现在的时间
       }
    }
}
