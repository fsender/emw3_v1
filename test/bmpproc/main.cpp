#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#define LGFX_USE_V1
#include "LovyanGFX.hpp"
#include "emw3.h"
#include "listMenuV2.h"
#include "emw3keyboard.h"

#include "bmpproc.h"

EMW3 emw3;

listMenuV2 menu(&emw3);
uint32_t chksum = 1234567890, chk0 = 0;
uint32_t batdat = 0;

void setup(){
  Serial.begin(115200);
  emw3.init(false);
  emw3.setFont(&cn_font);
  emw3.fillScreen(1);
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
  /*
  emw3.fillScreen(1);
  emw3.setCursor(0,0);
  emw3.print("灰度测试 Beta, 需要分段修改lut");
  emw3.display(2);
  //emw3.drawBmpFile(SDFS,"/bmp/xiangqi1.bmp",0,0);
  for(int i=1;i<16;i++){
    emw3.setLut(1,i,16);
    emw3.fillRect((i<<4)-12,32,16,64,0);
    emw3.display(3);
  }
  delay(3000);
  emw3.fillScreen(0xffff);
  emw3.display(2);
  delay(1000);*/
}
void loop(){
  bmpproc draw(SDFS, "/bmp/shoot.bmp");
  draw.drawBW(emw3,0,0,emw3.width(),emw3.height());
  int ix=0,iy=0;
  uint8_t changed=1;
  while(1){
    if(emw3.getBtn(keyL) == 0){
      if(emw3.getBtn(keyM) == 0) {
        if(iy) changed = 1;
        iy+=48;
        if(iy>0) iy = 0;
      }
      else if(iy > emw3.height()-draw.height()) {
        iy-=48;
        if(emw3.height()<draw.height() && iy <= emw3.height()-draw.height())
          iy = emw3.height()-draw.height();
        changed = 1;
      }
    }
    else if(emw3.getBtn(keyR) == 0){
      if(emw3.getBtn(keyM) == 0) {
        if(ix) changed = 1;
        ix+=48;
        if(ix>0) ix = 0;
      }
      else if(ix > emw3.width()-draw.width()) {
        ix-=48;
        if(emw3.width()<draw.width() && ix <= emw3.width()-draw.width())
          ix = emw3.width()-draw.width();
        changed = 1;
      }
    }
    if(changed) {
      //emw3.display(7);
      draw.drawBW(emw3,ix,iy);
      changed = 0;
    }
    yield();
  }
}
