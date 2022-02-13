/**
 * @file eddrv_1.h
 * @author fsender
 * @brief 
 * @version 1.0
 * Update: 2021-11-27
 * 初次创建
 */
#ifndef _EinkDriver_H_
#define _EinkDriver_H_

#include <Arduino.h>
#include <SPI.h>
#pragma GCC diagnostic ignored "-Wunused-parameter"

namespace emw3_EinkDriver{
class EinkDriver{
  public:
    // attributes
    const uint16_t WIDTH;
    const uint16_t HEIGHT;
    const bool _EinkDrv_Has_Colors;
    const bool _EinkDrv_Has_Part_Show;
    const bool _EinkDrv_Has_Part_Show_Fast;
    // constructor
    EinkDriver(int16_t busy_level, uint32_t busy_timeout,
               uint16_t w, uint16_t h, bool c, bool pu, bool fpu);
    virtual void init(bool initial, uint16_t reset_duration = 20, bool pulldown_rst_mode = false);
    //  Support for Bitmaps (Sprites) to Controller Buffer and to Screen
    //virtual void clearScreen(uint8_t value) = 0; // init controller memory and screen (default white)
    virtual void writeScreenBuffer(uint8_t value) = 0; // init controller memory (default white)
    // write to controller memory, without screen refresh; x and w should be multiple of 8
    virtual void writeImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    /*
    virtual void writeImageForFullRefresh(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false)
    {
      // writeImage is independent from refresh mode for most controllers, exception e.g. SSD1681
      writeImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
    }
    */
    virtual void writeImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    //    virtual void writeImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    //    virtual void writeImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
    //                                int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    // write sprite of native data to controller memory, without screen refresh; x and w should be multiple of 8
    //    virtual void writeNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    // for differential update: set current and previous buffers equal (for fast partial update to work correctly)
    /*
    virtual void writeScreenBufferAgain(uint8_t value = 0xFF) // init controller memory (default white)
    {
      // most controllers with differential update do switch buffers on refresh, can use:
      writeScreenBuffer(value);
    }
    virtual void writeImageAgain(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false)
    {
      // most controllers with differential update do switch buffers on refresh, can use:
      writeImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
    }
    virtual void writeImagePartAgain(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                     int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false)
    {
      // most controllers with differential update do switch buffers on refresh, can use:
      writeImagePart(bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
    }
    */
    // write to controller memory, with screen refresh; x and w should be multiple of 8
    //    virtual void drawImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    //    virtual void drawImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
    //                                int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    //    virtual void drawImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    //    virtual void drawImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
    //                                int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    // write sprite of native data to controller memory, with screen refresh; x and w should be multiple of 8
    //    virtual void drawNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false) = 0;
    virtual void refresh(bool partial_update_mode = false) = 0; // screen refresh from controller memory to full screen
    virtual void refresh(int16_t x, int16_t y, int16_t w, int16_t h) = 0; // screen refresh from controller memory, partial screen
    virtual void powerOff() = 0; // turns off generation of panel driving voltages, avoids screen fading over time
    virtual void hibernate() = 0; // turns powerOff() and sets controller to deep sleep for minimum power use, ONLY if wakeable by RST (rst >= 0)
    virtual void setPaged() {}; // for EinkDrv_154c paged workaround
    
  protected:
    void _reset();
    //void _waitWhileBusy(const char* comment = 0, uint16_t busy_time = 5000);
    void _writeCommand(uint8_t c);
    void _writeData(uint8_t d);
    void _writeData(const uint8_t* data, uint16_t n);
    void _writeDataPGM(const uint8_t* data, uint16_t n, int16_t fill_with_zeroes = 0);
    void _writeDataPGM_sCS(const uint8_t* data, uint16_t n, int16_t fill_with_zeroes = 0);
    void _writeCommandData(const uint8_t* pCommandData, uint8_t datalen);
    void _writeCommandDataPGM(const uint8_t* pCommandData, uint8_t datalen);
  protected:
    int16_t _cs, _dc, _rst, _busy, _busy_level;
    uint32_t _busy_timeout;
    bool _diag_enabled, _pulldown_rst_mode;
    SPISettings _spi_settings;
    bool _initial_write, _initial_refresh;
    bool _power_is_on, _using_partial_mode, _hibernating;
    uint16_t _reset_duration;
 /**
  * @brief 标记刷新状态: 0 未正在刷新 1 在传输数据  2 在等待刷新完毕 
  * 中断会更改此数值 */
    volatile uint8_t _refreshing;
};
}
#endif
