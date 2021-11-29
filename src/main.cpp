#include <Arduino.h>
#include <SPI.h>
#define LGFX_USE_V1
#include "LovyanGFX.hpp"
#include "emw3.h"
EMW3 emw3;
void setup(){
  Serial.begin(115200);
  Serial.println("\nWill init...");
  emw3.init();
  Serial.println("everything done.");
}
int rota=0;
void loop(){
  emw3.rotation(rota);
  Serial.println("generating LGFX Sprite.");
  emw3.setCursor(0,12);
  emw3.fillSprite(GxEPD_WHITE);
  emw3.setFont(&fonts::efontCN_12);
  Serial.println("Hello World will be printed soon.");
  emw3.setTextColor(GxEPD_BLACK,GxEPD_WHITE);
  emw3.fillRect(0,0,10,10);
  emw3.println(F("Hello World!"));
  emw3.println(F("你好, 世界!"));
  emw3.print(F("Rotation: "));
  emw3.print(rota);
  Serial.println("Hello World printed.");
  emw3.display(rota>0);
  rota++;
  delay(500);
  if(rota ==4) delay(5000);
  if(rota ==8){
    emw3.fillScreen(EMW3_BLACK);
    for(int i=0;i<20;i++){
      emw3.displayWindow(5,i*10,20,10);
      delay(500);
    }
    while(1) yield();
  } 
}