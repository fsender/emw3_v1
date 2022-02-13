#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#define LGFX_USE_V1
#include "LovyanGFX.hpp"
#include "emw3.h"
#include "gb2312.h"
#include "listMenuV2.h"
EMW3 emw3;

listMenuV2 menu(&emw3);
const lgfx::U8g2font cityFont  ( chinese_city_gb2312  );
const char icon1[] = "/EMW3/sys/icon/icon1.bmp";
const char icon2[] = "/EMW3/sys/icon/icon2.bmp";
const char *icons[] = {
  icon1,icon2,icon1,icon2,nullptr,icon2,icon1,icon2,icon1
};
const char *test_txt[17]={
  "标题数据","菜单项目1","支持很多功能","这很奇怪","\x03\x00\x00\x10\020icon1.bmp\xff选项10带有SD卡的图标文件",
  "亲自写出来的","过年啦不想写代码了","咋办","支持UTF-8编码"
  "标题德莉莎数据","菜单可莉项目1","支持优拉la很多功能","神里绫人凌华雷电将军八重神子",
  "亲自写黯色空壳出来的","过年啦不想写代码了","咋办,渊上,不凹分了","不支持GBK编码","空"
};
void setup(){
  Serial.begin(115200);
  Serial.println("\nINIT!\n");
  emw3.init(false);
  emw3.setFont(&cityFont);
  Serial.println("START E-INK DISPLAY!\n");
  emw3.fillScreen(0xffff);
  emw3.setTextColor(0);
  emw3.setCursor(0,0);
  emw3.print("测试-EMW3 Version 0.36");
  emw3.setCursor(50,45);
  emw3.print("Hello, 别来无恙啊!");
  emw3.setCursor(55,60);
  emw3.print("我是墨水时钟 EMW3,");
  emw3.setCursor(20,75);
  emw3.print("来自FriendshipEnder大神之手!");
  if(!SD.begin(EMW3_SD_CS_PIN)){
    Serial.println("SD NOT AVAILABLE!!\n");
    emw3.setCursor(60,90);
    emw3.print("SD卡未插入!!请检查");
  }
  else{
    Serial.println("SD AVAILABLE.\n");
    //emw3.setCursor(85,90);
    //emw3.print("SD卡已插入。");
  }
  
  emw3.display(2);
  delay(1000);
  emw3.drawBmpFile(SDFS,"/bmp/xiangqi1.bmp",0,0);
  emw3.display(2);
  delay(1000);
  
  menu.setSDIcon(9,icons);
  menu.setDrawMulti(1);
  uint8_t sel = menu.listMenu(10,0,5,16,4,test_txt);
  emw3.clearDisplay(0xffff);
  emw3.setCursor(40,40);
  emw3.print(test_txt[sel]);
  emw3.display(2);
  delay(1000);
}
int iok = 0;
int lastbatv=0;
void loop(){
  emw3.fillScreen(0xffff);
  emw3.setCursor(9,16);
  //emw3.fillScreen(0);
  //emw3.setTextColor(0xffff);
  emw3.print(iok);
  emw3.print(" 电池电压(V): ");
  pinMode(D4,OUTPUT);
    digitalWrite(D4,LOW);
  int batv = 0;
  for(int i=0;i<5;i++){
    delay(10);
    batv += analogRead(A0);
    //digitalWrite(D4,HIGH);
  }
  emw3.print(batv*4.3/5120.0,4);
  pinMode(D4,INPUT_PULLUP);
  if(batv != lastbatv) //emw3.displayWindow(80,8,24,240);
    emw3.display(3);
  lastbatv = batv;
  delay(2500);
  iok ++;
}