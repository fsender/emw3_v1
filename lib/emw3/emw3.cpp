/**
 * @file emw3.cpp
 * @author fsender
 * @brief 
 * @version 1.0.4
 * Update: 2022-02-13
 * 修复了SD卡图片不能直接显示,必须间接显示的bug
 * 原因: 未创建 palette, 未在初始化时调用 createPalette 函数
 * 
 * Update: 2021-11-27
 * 初次创建
 */
#include "emw3.h"
#include <SPI.h>
#include <FS.h>
#include <SD.h>
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
  ESP.wdtEnable(1000);
}
void EMW3::rotation(int rot){
  rot &=7;
  rot = 7-rot;
  if(rot<=1) rot+=2;
  else if(rot<=3) rot-=2;
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
  if(btn==EMW3_BtnL) BtnL_lastpress=1;
  if(btn==EMW3_BtnM) BtnM_lastpress=1;
  if(btn==EMW3_BtnR) BtnR_lastpress=1;
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