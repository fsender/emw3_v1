/**
 * @file emw3.cpp
 * @author fsender (Bilibili FriendshipEnder) (Q:3253342798)
 * @brief  EMW3 基础驱动封装库
 * @version 1.0.4
 */
#include "emw3.h"
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <LittleFS.h>
#include "emw3_defines.h"
#include "eddrv_2.h"
//using namespace emw3epd;
using namespace emw3_EinkDriver;
unsigned char EMW3::buff [4000] = {};
EMW3::EMW3(){
  setColorDepth(1);
  setBuffer(static_cast<uint8_t *>(buff),EMW3_REAL_WIDTH,EMW3_HEIGHT,1);
  createPalette();
  //setPaletteColor(0,EMW3_BLACK);
  //setPaletteColor(1,EMW3_WHITE);
  setRotation(7); // real val:0
  //_buffer = (uint8_t *)createSprite(EMW3_REAL_WIDTH,EMW3_HEIGHT);
  _buffer = buff;
  keyL = EMW3_BtnL;
  keyM = EMW3_BtnM;
  keyR = EMW3_BtnR;
  ESP.wdtEnable(1000);
}

uint8_t EMW3::display(uint8_t part){ 
      Serial.println("DISPLAY FX");
#ifdef DEBUG_DISPLAY_SERIAL
      /*
      for(int i=6;i<128;i++){
        for(int j=6;j<256;j++){
          Serial.print(((this->buff[((j^255)<<4)|((i>>3)^15)])&(1<<(i&0x07)))?'.':'q');
        }
        Serial.print('\n');
        yield();
      }
      return 0;
      */
      char wp[64];
      wp[63] = '\0';
      for(int i=6;i<128;i++){
        char pixelprint = (((this->buff[((6^255)<<4)|((i>>3)^15)])&(1<<(i&0x07)))?0:2)
        |(((this->buff[((7^255)<<4)|((i>>3)^15)])&(1<<(i&0x07)))?0:1);
        //Serial.print(char('a'+pixelprint));
        wp[0]='a'+pixelprint;
        for(int j=8;j<256;j+=4){
          pixelprint = (((this->buff[((j^255)<<4)|((i>>3)^15)])&(1<<(i&0x07)))?0:8)
          |(((this->buff[(((j+1)^255)<<4)|((i>>3)^15)])&(1<<(i&0x07)))?0:4)
          |(((this->buff[(((j+2)^255)<<4)|((i>>3)^15)])&(1<<(i&0x07)))?0:2)
          |(((this->buff[(((j+3)^255)<<4)|((i>>3)^15)])&(1<<(i&0x07)))?0:1);
          //Serial.print(char('a'+pixelprint));
          wp[(j>>2)-1] = 'a'+pixelprint;
        }
        Serial.println(wp);
        yield();
      }
      return 0;
      
#else
      uint32_t tm = micros();
      uint8_t res = _display(part); 
      if(part>=2) 
        while(digitalRead(EMW3_EPD_BUSY_PIN)==HIGH) ESP.wdtFeed();
      Serial.print(F("TIME COST (US): "));
      Serial.println(micros() - tm);
      Serial.print('\n');
      return res;
#endif
    }

void EMW3::rotation(int rot){
  if(rot<=1){
    keyL = EMW3_BtnL;
    keyR = EMW3_BtnR;
  }
  else{
    keyR = EMW3_BtnL;
    keyL = EMW3_BtnR;
  }
  rot &=7;
  rot = 7-rot;
  if(rot<=1){
    rot+=2;
  }
  else if(rot<=3) {
    rot-=2;
  }
  //new 0  old 7
  //new 1  old 6
  //new 2  old 5
  //new 3  old 4

  //new 4  old 1
  //new 5  old 0
  //new 6  old 3
  //new 7  old 2
  setRotation(rot);
}
uint8_t EMW3::getBtn(uint8_t btn){
  /*
  if(btn==keyL) BtnL_lastpress=1;
  if(btn==keyM) BtnM_lastpress=1;
  if(btn==keyR) BtnR_lastpress=1;
  */
  pinMode(btn,INPUT_PULLUP);
  uint8_t readb = digitalRead(btn);
  if(btn == D6) pinMode(D6,SPECIAL); //针对MISO引脚进行专门的优化
  else {
    pinMode(btn,OUTPUT);
    digitalWrite(btn,HIGH);    //这些引脚的默认电平都是高电平
  }
  return readb;
}
void EMW3::push16bitSprite(LGFX_Sprite spr16bit, int x, int y){
  for(int coloridx=0;coloridx<16;coloridx++){
    setDepth(coloridx);
    for(int i=0;i<spr16bit.height();i++){
      for(int j=0;j<spr16bit.width();j++){
        if(spr16bit.readPixel(j,i)/0x1081 == 15-coloridx)
          drawPixel((x>0?x:0)+j,(y>0?y:0)+i,0);
        yield();
      }
    }
    display(3);
  }
}
uint8_t keyL,keyM,keyR;