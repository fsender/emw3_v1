/**
 * @file eddrv_2.h
 * @author fsender
 * @brief 
 * @version 1.0
 * 
 * Update: 2022-2-14
 * 增加 setLut 函数: 允许单独修改某一个Lut数据,而不只是以数组为参数一次修改全部数据
 * Update: 2021-11-27
 * 初次创建
 */
#ifndef _EinkDrv_213_H_
#define _EinkDrv_213_H_
#include <Arduino.h>
#include <SPI.h>
#include "eddrv_1.h"
#include "emw3_defines.h"
namespace emw3_EinkDriver{
  
class EinkDrv_213;
typedef struct {
  EinkDrv_213 *driver;
  int16_t x;
  int16_t y;
  int16_t w;
  int16_t h;
  int16_t px;
  int16_t py;
  int16_t pw;
  int16_t ph;
  //int16_t update_mode; // -1 无刷新  0 全刷  1 局刷  2 窗口局刷
} _refresh_status_t;

class EinkDrv_213 : public EinkDriver{
  public:
    EinkDrv_213();
    void init(bool initial = true, uint16_t reset_duration = 20, bool pulldown_rst_mode = false);
    //void fill(uint16_t color); // 0x0 black, >0x0 white, to buffer
    // display buffer content to screen, useful for full screen buffer
    uint8_t _display(uint8_t partial_update_mode);
    uint8_t displayWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool async = true);
    void setFullWindow();
    void setPartialWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void startTr();
    void endTr();
    friend IRAM_ATTR void _rSfr_Cal_lBack_(_refresh_status_t *);
  private:
    template <class T> static inline void _swap_(T & a, T & b) {
      T t = a;
      a = b;
      b = t;
    };
    /*
    static inline uint16_t emw3_u16min(uint16_t a, uint16_t b) {
      return (a < b ? a : b);
    };
    static inline uint16_t emw3_u16max(uint16_t a, uint16_t b) {
      return (a > b ? a : b);
    };
    */
  private:
    bool _using_partial_mode, _second_phase, _mirror, _reverse;
    uint16_t _width_bytes; 
    uint16_t _pixel_bytes;
    int16_t  _current_page;
    uint16_t _pages;
    uint16_t _page_height;
    uint16_t _pw_x, _pw_y, _pw_w, _pw_h;
  protected:
    uint8_t *_buffer;
    
    /// @brief 来自EinkDrv_213.h
  public:
    static const uint16_t WIDTH = EMW3_WIDTH;
    static const uint16_t HEIGHT = EMW3_HEIGHT;
    static const bool _EinkDrv_Has_Colors = false;
    static const bool _EinkDrv_Has_Part_Show = true;
    static const bool _EinkDrv_Has_Part_Show_Fast = true;
    // constructor

    // methods (virtual)
    //  Support for Bitmaps (Sprites) to Controller Buffer and to Screen
    //void clearScreen(uint8_t value = 0xFF); // init controller memory and screen (default white)
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
    void refreshNoDelay(bool partial_update_mode = false); // screen refresh without delay() function
    void refreshNoDelay(int16_t x, int16_t y, int16_t w, int16_t h); // screen refresh partial without delay() function
    
    void powerOff(); // turns off generation of panel driving voltages, avoids screen fading over time
    void hibernate(); // turns powerOff() and sets controller to deep sleep for minimum power use, ONLY if wakeable by RST (rst >= 0)
    void setLut(bool FullOrPart = 0, const uint8_t *lut = nullptr, uint8_t size = 0);
    void setLut(bool FullOrPart, uint8_t lut, uint8_t position = 0);
    const uint8_t * getLut(bool FullOrPart, uint8_t *size){
      *size = FullOrPart?_ed_lut_part_size:_ed_lut_full_size;
      return FullOrPart?_ed_lut_part:_ed_lut_full;
    }
  private:
    void _edbuff_w(uint8_t value);
    void _edpart_area(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void _edentry_nouse(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void _edset_area(uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye);
    void _edset_ptr(uint16_t x, uint16_t y);
    void _edpwr_begin();
    void _edpwr_end();
    void _edbegin();
    void _ed_fullscr_begin();
    void _ed_partscr_begin();
    void _ed_fullscr_disp();
    void _ed_partscr_disp();
    void _ed_fullscr_disp_noDelay();
    void _ed_partscr_disp_noDelay();
  private:
    static uint8_t _ed_lut_part[40];
    static uint8_t _ed_lut_full[40];
    static uint8_t _ed_lut_full_size;
    static uint8_t _ed_lut_part_size;
    _refresh_status_t _rSf;
    uint8_t next_frame;
};

}// namespace 
#endif