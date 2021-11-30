#include "emw3.h"
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include "emw3_defines.h"
#include "GxEPD2_213.h"
//using namespace emw3epd;
using namespace emw3_gxepd2;

unsigned char EMW3::buff [4000] = {};
EMW3::EMW3(){
  setColorDepth(1);
  setBuffer(static_cast<uint8_t*>(buff),EMW3_REAL_WIDTH,EMW3_HEIGHT,1);
  setPaletteColor(0,GxEPD_BLACK);
  setPaletteColor(1,GxEPD_WHITE);
  setRotation(7); // real val:0
  //createSprite(EPD_WIDTH,EPD_HEIGHT);
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
  if(btn == D6) pinMode(D6,SPECIAL);
  else {
    pinMode(btn,OUTPUT);
    digitalWrite(btn,HIGH);
  }
  return readb;
}