#ifndef _GxEPD2_213_H_
#define _GxEPD2_213_H_
#include <Arduino.h>
#include <SPI.h>
#include "GxEPD2_EPD.h"
#include "emw3_defines.h"
namespace emw3_gxepd2{
  
class GxEPD2_213;
typedef struct {
  GxEPD2_213 *driver;
  int16_t x;
  int16_t y;
  int16_t w;
  int16_t h;
  int16_t px;
  int16_t py;
  int16_t pw;
  int16_t ph;
  uint8_t partial_update_mode; // 0 无刷新  1 全刷  2 局刷  3 窗口局刷
} _refresh_status_t;

class GxEPD2_213 : public GxEPD2_EPD
{
  public:
    GxEPD2_213();
    void init(bool initial = true, uint16_t reset_duration = 20, bool pulldown_rst_mode = false);
    void fill(uint16_t color); // 0x0 black, >0x0 white, to buffer
    // display buffer content to screen, useful for full screen buffer
    void Display(bool partial_update_mode = false);
    void displayWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void setFullWindow();
    void setPartialWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
  private:
    template <typename T> static inline void
    _swap_(T & a, T & b)
    {
      T t = a;
      a = b;
      b = t;
    };
    static inline uint16_t gx_uint16_min(uint16_t a, uint16_t b)
    {
      return (a < b ? a : b);
    };
    static inline uint16_t gx_uint16_max(uint16_t a, uint16_t b)
    {
      return (a > b ? a : b);
    };
    
  private:
    bool _using_partial_mode, _second_phase, _mirror, _reverse;
    uint16_t _width_bytes, _pixel_bytes;
    int16_t _current_page;
    uint16_t _pages, _page_height;
    uint16_t _pw_x, _pw_y, _pw_w, _pw_h;
  protected:
    uint8_t *_buffer;
 /**
  * @brief 标记刷新状态: 0 未正在刷新 1 在传输数据  2 在等待刷新完毕 
  * 中断会更改此数值
  * >=4: 手动模式 */
    volatile uint8_t _refreshing;


    /// @brief 来自GxEPD2_213.h
  public:
    static const uint16_t WIDTH = 128;
    static const uint16_t HEIGHT = 250;
    static const GxEPD2::Panel panel = GxEPD2::GDE0213B1;
    static const bool hasColor = false;
    static const bool hasPartialUpdate = true;
    static const bool hasFastPartialUpdate = true;
    // constructor

    // methods (virtual)
    //  Support for Bitmaps (Sprites) to Controller Buffer and to Screen
    void clearScreen(uint8_t value = 0xFF); // init controller memory and screen (default white)
    void writeScreenBuffer(uint8_t value = 0xFF); // init controller memory (default white)
    // write to controller memory, without screen refresh; x and w should be multiple of 8
    void writeImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void writeImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                        int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void writeImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void writeImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                        int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    // write sprite of native data to controller memory, without screen refresh; x and w should be multiple of 8
    void writeNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    // write to controller memory, with screen refresh; x and w should be multiple of 8
    void drawImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void drawImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                       int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void drawImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void drawImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                       int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    // write sprite of native data to controller memory, with screen refresh; x and w should be multiple of 8
    void drawNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
    void refresh(bool partial_update_mode = false); // screen refresh from controller memory to full screen
    void refresh(int16_t x, int16_t y, int16_t w, int16_t h); // screen refresh from controller memory, partial screen
    void powerOff(); // turns off generation of panel driving voltages, avoids screen fading over time
    void hibernate(); // turns powerOff() and sets controller to deep sleep for minimum power use, ONLY if wakeable by RST (rst >= 0)
  private:
    void _writeScreenBuffer(uint8_t value);
    void _setPartialRamArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void _setRamEntryWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void _setRamArea(uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye);
    void _setRamPointer(uint16_t x, uint16_t y);
    void _PowerOn();
    void _PowerOff();
    void _InitDisplay();
    void _Init_Full();
    void _Init_Part();
    void _Update_Full();
    void _Update_Part();
  private:
    static const uint8_t LUTDefault_part[];
    static const uint8_t LUTDefault_full[];
};

}// namespace 
#endif