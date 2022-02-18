/**
 * @file emw3.h
 * @author fsender
 * @brief 
 * @version 1.0.4
 * Update: 2022-02-15
 * 修复在当 hlines < 3 的时候指示条显示错位的问题
 * 
 * Update: 2022-02-13
 * 修复了SD卡与刷屏互相干扰导致屏幕或者SD卡死机问题
 * 由"中断刷屏"导致, 现已修复, 在系统函数中仅使用阻塞刷屏
 * 
 * Update: 2022-02-09
 * 新增: 串口模拟显示, 在电脑的串口助手上也能显示画面, 不会缩短屏幕寿命
 * 发现问题: SD卡与刷屏互相干扰导致屏幕或者SD卡死机
 * 
 * Update: 2021-11-27
 * 初次创建
 */
#ifndef _CTG_EMW3_V1_H
#define _CTG_EMW3_V1_H

#include <Arduino.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include "emw3_defines.h"
#include "eddrv_2.h"
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
using namespace emw3_EinkDriver;
//using namespace emw3epd;
class EMW3 : public EinkDrv_213, public LGFX_Sprite {
  public:
    EMW3();
    /**
     * @brief 刷屏
     * @param part 0:无延时全刷    1:无延时快刷     2: 阻塞式全刷     3: 阻塞式快刷
     */
    inline uint8_t display(uint8_t part = 1) { 
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
      uint8_t res = _display(part&1); 
      if(part>=2) 
        while(digitalRead(EMW3_EPD_BUSY_PIN)==HIGH) ESP.wdtFeed();
      Serial.print(F("TIME COST (US): "));
      Serial.println(micros() - tm);
      Serial.print('\n');
      return res;
#endif
    }
    void rotation(int rot);
    // void dispWindow(int16_t x,int16_t y,int16_t w,int16_t h);
    
    uint8_t getBtn(uint8_t btn);
    uint8_t epdBusy(){ 
#ifdef DEBUG_DISPLAY_SERIAL
      return 0;
#else
      return digitalRead(EMW3_EPD_BUSY_PIN);
#endif
    }
  private:
    //for 250 * 122 sized buffer
    static unsigned char buff [4000];
};
// preset lut mode.

#endif