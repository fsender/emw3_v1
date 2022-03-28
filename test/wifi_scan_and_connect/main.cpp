#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#define LGFX_USE_V1
#include "LovyanGFX.hpp"
#include "emw3.h"
#include "listMenuV2.h"
#include "emw3keyboard.h"
#include <ESP8266WiFi.h>
#include "app.h"
EMW3 emw3;
EMW3App app;
listMenuV2 menu(&emw3);
emw3Keyboard kbd(&emw3);

extern const lgfx::U8g2font cn_font;
extern const lgfx::U8g2font osmall5_font;
void setup(){
  Serial.begin(115200);
  emw3.init(false);
  emw3.setFont(&cn_font);
}
void loop(){
}
void connectToWiFi(){
  int wifiScanResNum=0; //扫描到的wifi数量
  const char **wifiScanResult=nullptr; //扫描到的wifi结果

  if(WiFi.getMode() == WIFI_OFF) WiFi.mode(WIFI_STA);
  else if(WiFi.getMode() != WIFI_STA) WiFi.mode(WIFI_AP_STA);

  WiFi.scanNetworks(); //开始扫描
  wifiScanResNum = WiFi.scanComplete();
  if(wifiScanResNum){
    wifiScanResult = new const char* [wifiScanResNum];
    menu.listMenu(0,0,5,wifiScanResNum,0,wifiScanResult);
  }
}