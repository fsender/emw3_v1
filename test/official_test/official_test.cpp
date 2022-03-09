#include <Arduino.h>
#include <SPI.h>
#define LGFX_USE_V1
#include "LovyanGFX.hpp"
#include "emw3.h"
EMW3 emw3;
void setup(){
  Serial.begin(115200);
  Serial.println("\nWill init...");
  emw3.init(false);
  Serial.print("everything done.");
  Serial.println(digitalRead(EMW3_EPD_BUSY_PIN));
    delay(100);
    emw3.rotation(3);
    emw3.fillScreen(EMW3_WHITE);
    emw3.setCursor(0,0);
    emw3.println(F("SD card benchmark: complete."));
    emw3.println(F("Buttons test."));
    emw3.display(2);
    /*
    while(1){
      emw3.fillRect(10,97,30,30,emw3.getBtn(keyL)?EMW3_BLACK:EMW3_WHITE);
      emw3.fillRect(45,97,30,30,emw3.getBtn(keyM)?EMW3_BLACK:EMW3_WHITE);
      emw3.fillRect(80,97,30,30,emw3.getBtn(keyR)?EMW3_BLACK:EMW3_WHITE);
      emw3.displayWindow(8,96,112,32);
      yield();
    }*/
}
int rota=0;
void loop(){
  emw3.rotation(rota);
  Serial.println("generating LGFX Sprite.");
  emw3.setCursor(0,12);
  emw3.fillSprite(EMW3_WHITE);
  emw3.setFont(&fonts::efontCN_12);
  Serial.println("Hello World will be printed soon.");
  emw3.setTextColor(EMW3_BLACK,EMW3_WHITE);
  emw3.fillRect(0,0,10,10);
  emw3.println(F("Hello World!"));
  emw3.println(F("你好, 世界!"));
  emw3.print(F("Rotation: "));
  emw3.print(rota);
  Serial.println("Hello World printed.");
  emw3.display((rota>0)+2);
  rota++;
  if(rota ==4){
    emw3.fillScreen(EMW3_BLACK);
    for(int i=0;i<20;i++){
      emw3.displayWindow(5,i*10,20,10);
      delay(100);
    }
    delay(1600);
    emw3.fillScreen(EMW3_WHITE);
    emw3.setCursor(0,0);
    emw3.print(F("Benchmark: fast refresh"));
    Serial.print(F("Benchmark: fast refresh"));
    emw3.display(2);
    delay(800);
    for(int i=0;i<250;i++){
      emw3.drawFastHLine(0,i,14+i%108,EMW3_BLACK);
      uint32_t mic = micros();
      emw3.display();
      Serial.printf("fast refresh: %u us\n",micros()-mic);
      delay(15);
    }
    delay(1600);
    emw3.fillScreen(EMW3_WHITE);
    emw3.setCursor(0,0);
    emw3.print(F("Benchmark: no-delay full refresh"));
    Serial.print(F("Benchmark: no-delay full refresh"));
    emw3.display(2);
    delay(800);
    for(int i=0;i<250;i++){
      emw3.drawFastHLine(0,i,14+i%108,EMW3_BLACK);
      uint32_t mic = micros();
      emw3.display(0);
      Serial.printf("no-delay full refresh: %u us\n",micros()-mic);
      delay(40);
    }
    delay(1600);
    emw3.fillScreen(EMW3_WHITE);
    emw3.setCursor(0,0);
    emw3.print(F("Benchmark: part update"));
    Serial.print(F("Benchmark: part update"));
    emw3.display(2);
    delay(800);
    emw3.fillScreen(EMW3_BLACK);
    for(int i=0;i<25;i++){
      uint32_t mic = micros();
      emw3.displayWindow(5,i*10,20,10);
      Serial.printf("part update: %u us\n",micros()-mic);
      delay(100);
    }
    delay(1600);
    emw3.fillScreen(EMW3_WHITE);
    emw3.setCursor(0,0);
    emw3.print(F("Benchmark: blocked fast refresh"));
    Serial.print(F("Benchmark: blocked fast refresh"));
    emw3.display(2);
    delay(800);
    for(int i=0;i<5;i++){
      emw3.fillRect(0,i*50,i*10+40,50,EMW3_BLACK);
      uint32_t mic = micros();
      emw3.display(3);
      Serial.printf("blocked fast refresh: %u us\n",micros()-mic);
      delay(0);
    }
    delay(1600);
    emw3.fillScreen(EMW3_WHITE);
    emw3.setCursor(0,0);
    emw3.print(F("Benchmark: blocked full refresh"));
    Serial.print(F("Benchmark: blocked full refresh"));
    emw3.display(2);
    delay(800);
    for(int i=0;i<5;i++){
      emw3.fillRect(0,i*50,i*10+40,50,EMW3_BLACK);
      uint32_t mic = micros();
      emw3.display(2);
      Serial.printf("blocked full refresh: %u us\n",micros()-mic);
      delay(0);
    }
    delay(1600);
    emw3.fillScreen(EMW3_WHITE);
    emw3.setCursor(0,0);
    emw3.print(F("Benchmark: blocked part update"));
    Serial.print(F("Benchmark: blocked part update"));
    emw3.display(2);
    delay(800);
    emw3.fillScreen(EMW3_BLACK);
    for(int i=0;i<25;i++){
      uint32_t mic = micros();
      emw3.displayWindow(5,i*10,20,10,0);
      Serial.printf("blocked part update: %u us\n",micros()-mic);
      delay(0);
    }
    emw3.fillScreen(EMW3_WHITE);
    emw3.setCursor(0,0);
    emw3.println(F("EPD display benchmark: complete."));
    emw3.println(F("WiFi test."));
    emw3.println(F("Scanning..."));
    emw3.display(2);
    delay(1600);
    emw3.fillScreen(EMW3_WHITE);
    emw3.setCursor(0,0);
    emw3.println(F("WiFi benchmark: complete."));
    emw3.println(F("SD card test."));
    emw3.println(F("Opening..."));
    emw3.println(F("/hello.txt"));
    emw3.display(2);
    delay(1600);
    emw3.fillScreen(EMW3_WHITE);
    emw3.setCursor(0,0);
    emw3.println(F("SD card benchmark: complete."));
    emw3.println(F("Buttons test."));
    emw3.display(2);
    while(1){
      emw3.fillRect(10,97,30,30,emw3.getBtn(keyL)?EMW3_BLACK:EMW3_WHITE);
      emw3.fillRect(45,97,30,30,emw3.getBtn(keyM)?EMW3_BLACK:EMW3_WHITE);
      emw3.fillRect(80,97,30,30,emw3.getBtn(keyR)?EMW3_BLACK:EMW3_WHITE);
      emw3.displayWindow(8,96,112,32);
      delay(1);
    }
  } 
}