#ifndef _CTG_EMW3_V1_H
#define _CTG_EMW3_V1_H
#include <Arduino.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include "emw3_defines.h"
#include "GxEPD2_213.h"
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
using namespace emw3_gxepd2;
//using namespace emw3epd;
class EMW3 : public GxEPD2_213, public LGFX_Sprite {
  public:
    EMW3();
    /**
     * @brief 刷屏
     * @param part 0:无延时全刷    1:无延时快刷     2: 全刷     3: 快刷
     */
    inline void display(uint8_t part = 3) { _display(part); }
    void rotation(int rot);
    // void dispWindow(int16_t x,int16_t y,int16_t w,int16_t h);
  private:
    //for 250 * 122 sized buffer
    static unsigned char buff [4000];
};
// preset lut mode.

#endif