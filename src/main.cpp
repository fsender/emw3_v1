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
  emw3.fillScreen(0xffff);
  Serial.print("everything done.");
  emw3.display(2);
  Serial.print(digitalRead(EMW3_EPD_BUSY_PIN));
  delay(1500);
  Serial.println(digitalRead(EMW3_EPD_BUSY_PIN));
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
  Serial.print("Hello World printed.");
  emw3.display(3);
  Serial.print(digitalRead(EMW3_EPD_BUSY_PIN));
  delay(500);
  Serial.println(digitalRead(EMW3_EPD_BUSY_PIN));
  rota++;
  if(rota ==4){
    emw3.fillScreen(EMW3_BLACK);
    for(int i=0;i<20;i++){
      emw3.displayWindow(5,i*10,20,10);
    }
    while(1) yield();
  } 
}