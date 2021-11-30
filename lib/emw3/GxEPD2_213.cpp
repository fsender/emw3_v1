#include "GxEPD2_213.h"
#include "emw3_defines.h"
namespace emw3_gxepd2{

  #define gx_uint16_min(a,b) ((a) < (b) ? (a) : (b))
  #define gx_uint16_max(a,b) ((a) > (b) ? (a) : (b))

GxEPD2_213::GxEPD2_213() :
  GxEPD2_EPD(HIGH, 10000000, WIDTH, HEIGHT, panel, hasColor, hasPartialUpdate, hasFastPartialUpdate)
{
      _page_height = EMW3_HEIGHT;
      _pages = 1;
      _reverse = 1;
      _mirror = false;
      _using_partial_mode = false;
      _current_page = 0;
      _refreshing = 0;
      _rSf.driver = this;
}
void GxEPD2_213 ::startTr(){
  _refreshing = 1;
  SPI.beginTransaction(_spi_settings);
}
void GxEPD2_213 ::endTr(){
  SPI.endTransaction();
  _refreshing = 0;
}
    void GxEPD2_213::init(bool initial, uint16_t reset_duration, bool pulldown_rst_mode)
    {
      GxEPD2_EPD::init(initial, reset_duration, pulldown_rst_mode);
      _using_partial_mode = true;
      _current_page = 0;
      next_frame = 0;
      _rSf.px = 0;
      _rSf.py = 0;
      _rSf.pw = 0;
      _rSf.ph = 0;
      //setFullWindow();
    }
/*
    void GxEPD2_213::fill(uint16_t color){
      uint8_t data = (color == GxEPD_BLACK) ? 0x00 : 0xFF;
      for (uint16_t x = 0; x < sizeof(_buffer); x++)
      {
        _buffer[x] = data;
      }
    }
*/

IRAM_ATTR void _rSfr_Cal_lBack_(_refresh_status_t *_fb){
  uint8_t bkup = _fb->driver->_refreshing;
  detachInterrupt(EMW3_EPD_BUSY_PIN);
  SPI.beginTransaction(_fb->driver->_spi_settings);
  _fb->driver->_writeCommand(0xff);
  if(bkup>=4) {
    _fb->driver->writeImagePart(_fb->driver->_buffer,0,0,_fb->driver->WIDTH,_fb->driver->HEIGHT,
    _fb->px,_fb->py,_fb->pw,_fb->ph);
    _fb->driver->writeImagePart(_fb->driver->_buffer,0,0,_fb->driver->WIDTH,_fb->driver->HEIGHT,
    _fb->x,_fb->y,_fb->w,_fb->h);
  }
  else _fb->driver->writeImage(_fb->driver->_buffer,0,0,_fb->driver->WIDTH,_fb->driver->HEIGHT);
  if (2==bkup) _fb->driver->powerOff();
  SPI.endTransaction();
  _fb->driver->_refreshing = 0;
  if(_fb->driver->next_frame ==1) _fb->driver->_display(bkup-2);
  else if(_fb->driver->next_frame ==2) _fb->driver->displayWindow(_fb->x,_fb->y,_fb->w,_fb->h,1);
}
  void GxEPD2_213::_display(uint8_t partial_update_mode ){
      if(_refreshing){ //上一次刷新还没结束
        next_frame = 1;
        return;
      }
      /*
    if(partial_update_mode&2){
      startTr();
      writeImage(_buffer, 0, 0, WIDTH, _page_height);
      refresh(partial_update_mode&1);
      writeImage(_buffer, 0, 0, WIDTH, _page_height);
      if (!partial_update_mode&1) powerOff();
      endTr();
    }
    else {*/
      startTr();
      writeImage(_buffer, 0, 0, WIDTH, _page_height);
      refreshNoDelay(partial_update_mode);
      endTr();
      _refreshing = 2 | (partial_update_mode);
      attachInterruptArg(EMW3_EPD_BUSY_PIN,(void(*)(void *))_rSfr_Cal_lBack_,&_rSf,FALLING);
    //}
    next_frame = 0;
  }

  void GxEPD2_213::displayWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool async){
      if(_refreshing){ //上一次刷新还没结束
        next_frame = 2;
        _rSf.x = x;
        _rSf.y = y;
        _rSf.w = w;
        _rSf.h = h;
        return;
      }
      x = gx_uint16_min(x, EMW3_WIDTH);
      y = gx_uint16_min(y, EMW3_HEIGHT);
      w = gx_uint16_min(w, EMW3_WIDTH - x);
      h = gx_uint16_min(h, EMW3_HEIGHT - y);
      uint16_t y_part = _reverse ? HEIGHT - h - y : y;
    if(async){
      startTr();
      //writeImagePart(_buffer, x, y_part, WIDTH, _page_height, _rSf.px, _rSf.py, _rSf.pw, _rSf.ph);
      writeImagePart(_buffer, x, y_part, WIDTH, _page_height, x, y, w, h);
      refreshNoDelay(x, y, w, h);
      endTr();
      _refreshing = 4;
      _rSf.px = x;
      _rSf.py = y;
      _rSf.pw = w;
      _rSf.ph = h;
      attachInterruptArg(EMW3_EPD_BUSY_PIN,(void(*)(void *))_rSfr_Cal_lBack_,&_rSf,FALLING);
    }
    else {
      startTr();
      writeImagePart(_buffer, x, y_part, WIDTH, _page_height, x, y, w, h);
      refresh(x, y, w, h);
      writeImagePart(_buffer, x, y_part, WIDTH, _page_height, x, y, w, h);
      endTr();
    }
    next_frame = 0;
  }
    void GxEPD2_213::setFullWindow(){
      _using_partial_mode = false;
      _pw_x = 0;
      _pw_y = 0;
      _pw_w = WIDTH;
      _pw_h = HEIGHT;
    }

    void GxEPD2_213::setPartialWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
    {
      _pw_x = gx_uint16_min(x, EMW3_WIDTH);
      _pw_y = gx_uint16_min(y, EMW3_HEIGHT);
      _pw_w = gx_uint16_min(w, EMW3_WIDTH - _pw_x);
      _pw_h = gx_uint16_min(h, EMW3_HEIGHT - _pw_y);
      _using_partial_mode = true;
      // make _pw_x, _pw_w multiple of 8
      _pw_w += _pw_x % 8;
      if (_pw_w % 8 > 0) _pw_w += 8 - _pw_w % 8;
      _pw_x -= _pw_x % 8;
    }

/* void GxEPD2_213::clearScreen(uint8_t value)
{
  _initial_write = false; // initial full screen buffer clean done
  if (_initial_refresh)
  {
    _Init_Full();
    _setPartialRamArea(0, 0, WIDTH, HEIGHT);
    _writeCommand(0x24);
    for (uint32_t i = 0; i < uint32_t(WIDTH) * uint32_t(HEIGHT) / 8; i++)
    {
      _writeData(value);
    }
    _Update_Full();
    _initial_refresh = false; // initial full update done
  }
  else
  {
    if (!_using_partial_mode) _Init_Part();
    _setPartialRamArea(0, 0, WIDTH, HEIGHT);
    _writeCommand(0x24);
    for (uint32_t i = 0; i < uint32_t(WIDTH) * uint32_t(HEIGHT) / 8; i++)
    {
      _writeData(value);
    }
    _Update_Part();
  }
  if (!_using_partial_mode) _Init_Part();
  _setPartialRamArea(0, 0, WIDTH, HEIGHT);
  _writeCommand(0x24);
  for (uint32_t i = 0; i < uint32_t(WIDTH) * uint32_t(HEIGHT) / 8; i++)
  {
    _writeData(value);
  }
  _Update_Part();
}*/

void GxEPD2_213::writeScreenBuffer(uint8_t value)
{
  _initial_write = false; // initial full screen buffer clean done
  // this controller has no command to write "old data"
  /*if (_initial_refresh) clearScreen(value);
  else */
  _writeScreenBuffer(value);
}

void GxEPD2_213::_writeScreenBuffer(uint8_t value)
{
  if (!_using_partial_mode) _Init_Part();
  _setPartialRamArea(0, 0, WIDTH, HEIGHT);
  _writeCommand(0x24);
  for (uint32_t i = 0; i < uint32_t(WIDTH) * uint32_t(HEIGHT) / 8; i++)
  {
    _writeData(value);
  }
}

void GxEPD2_213::writeImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  if (_initial_write) writeScreenBuffer(); // initial full screen buffer clean
  int16_t wb = (w + 7) / 8; // width bytes, bitmaps are padded
  x -= x % 8; // byte boundary
  w = wb * 8; // byte boundary
  int16_t x1 = x < 0 ? 0 : x; // limit
  int16_t y1 = y < 0 ? 0 : y; // limit
  int16_t w1 = x + w < int16_t(WIDTH) ? w : int16_t(WIDTH) - x; // limit
  int16_t h1 = y + h < int16_t(HEIGHT) ? h : int16_t(HEIGHT) - y; // limit
  int16_t dx = x1 - x;
  int16_t dy = y1 - y;
  w1 -= dx;
  h1 -= dy;
  if ((w1 <= 0) || (h1 <= 0)) return;
  if (!_using_partial_mode) _Init_Part();
  _setPartialRamArea(x1, y1, w1, h1);
  _writeCommand(0x24);
  for (int16_t i = 0; i < h1; i++)
  {
    for (int16_t j = 0; j < w1 / 8; j++)
    {
      uint8_t data;
      // use wb, h of bitmap for index!
      int16_t idx = mirror_y ? j + dx / 8 + (h - 1 - i) * wb : j + dx / 8 + i * wb;
      if (pgm) data = pgm_read_byte(&bitmap[idx]);
      else data = bitmap[idx];
      if (invert) data = ~data;
      _writeData(data);
    }
  }
}

void GxEPD2_213::writeImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  if (_initial_write) writeScreenBuffer(); // initial full screen buffer clean
  if ((w_bitmap < 0) || (h_bitmap < 0) || (w < 0) || (h < 0)) return;
  if ((x_part < 0) || (x_part >= w_bitmap)) return;
  if ((y_part < 0) || (y_part >= h_bitmap)) return;
  int16_t wb_bitmap = (w_bitmap + 7) / 8; // width bytes, bitmaps are padded
  x_part -= x_part % 8; // byte boundary
  w = w_bitmap - x_part < w ? w_bitmap - x_part : w; // limit
  h = h_bitmap - y_part < h ? h_bitmap - y_part : h; // limit
  x -= x % 8; // byte boundary
  w = 8 * ((w + 7) / 8); // byte boundary, bitmaps are padded
  int16_t x1 = x < 0 ? 0 : x; // limit
  int16_t y1 = y < 0 ? 0 : y; // limit
  int16_t w1 = x + w < int16_t(WIDTH) ? w : int16_t(WIDTH) - x; // limit
  int16_t h1 = y + h < int16_t(HEIGHT) ? h : int16_t(HEIGHT) - y; // limit
  int16_t dx = x1 - x;
  int16_t dy = y1 - y;
  w1 -= dx;
  h1 -= dy;
  if ((w1 <= 0) || (h1 <= 0)) return;
  if (!_using_partial_mode) _Init_Part();
  _setPartialRamArea(x1, y1, w1, h1);
  _writeCommand(0x24);
  for (int16_t i = 0; i < h1; i++)
  {
    for (int16_t j = 0; j < w1 / 8; j++)
    {
      uint8_t data;
      // use wb_bitmap, h_bitmap of bitmap for index!
      int16_t idx = mirror_y ? x_part / 8 + j + dx / 8 + ((h_bitmap - 1 - (y_part + i + dy))) * wb_bitmap : x_part / 8 + j + dx / 8 + (y_part + i + dy) * wb_bitmap;
      if (pgm) data = pgm_read_byte(&bitmap[idx]);
      else data = bitmap[idx];
      if (invert) data = ~data;
      _writeData(data);
    }
  }
}

void GxEPD2_213::writeImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  if (black)
  {
    writeImage(black, x, y, w, h, invert, mirror_y, pgm);
  }
}

void GxEPD2_213::writeImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                                int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  if (black)
  {
    writeImagePart(black, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
  }
}

void GxEPD2_213::writeNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  if (data1)
  {
    writeImage(data1, x, y, w, h, invert, mirror_y, pgm);
  }
}

void GxEPD2_213::drawImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  startTr();
  writeImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
  refresh(x, y, w, h);
  writeImage(bitmap, x, y, w, h, invert, mirror_y, pgm);
  endTr();
}

void GxEPD2_213::drawImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                               int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  startTr();
  writeImagePart(bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
  refresh(x, y, w, h);
  writeImagePart(bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
  endTr();
}

void GxEPD2_213::drawImage(const uint8_t* black, const uint8_t* color, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  startTr();
  writeImage(black, color, x, y, w, h, invert, mirror_y, pgm);
  refresh(x, y, w, h);
  writeImage(black, color, x, y, w, h, invert, mirror_y, pgm);
  endTr();
}

void GxEPD2_213::drawImagePart(const uint8_t* black, const uint8_t* color, int16_t x_part, int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                               int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  startTr();
  writeImagePart(black, color, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
  refresh(x, y, w, h);
  writeImagePart(black, color, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h, invert, mirror_y, pgm);
  endTr();
}

void GxEPD2_213::drawNative(const uint8_t* data1, const uint8_t* data2, int16_t x, int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm)
{
  startTr();
  writeNative(data1, data2, x, y, w, h, invert, mirror_y, pgm);
  refresh(x, y, w, h);
  writeNative(data1, data2, x, y, w, h, invert, mirror_y, pgm);
  endTr();
}

void GxEPD2_213::refresh(bool partial_update_mode)
{
  if (partial_update_mode) refresh(0, 0, WIDTH, HEIGHT);
  else
  {
    if (_using_partial_mode) _Init_Full();
    _initial_refresh = false; // initial full update done
    _Update_Full();
  }
}
void GxEPD2_213::refreshNoDelay(bool partial_update_mode)
{
  if (partial_update_mode) refreshNoDelay(0, 0, WIDTH, HEIGHT);
  else
  {
    if (_using_partial_mode) _Init_Full();
    _initial_refresh = false; // initial full update done
    _Update_Full_noDelay();
  }
}


void GxEPD2_213::refresh(int16_t x, int16_t y, int16_t w, int16_t h)
{
  if (_initial_refresh) return refresh(false); // initial update needs be full update
  // intersection with screen
  int16_t w1 = x < 0 ? w + x : w; // reduce
  int16_t h1 = y < 0 ? h + y : h; // reduce
  int16_t x1 = x < 0 ? 0 : x; // limit
  int16_t y1 = y < 0 ? 0 : y; // limit
  w1 = x1 + w1 < int16_t(WIDTH) ? w1 : int16_t(WIDTH) - x1; // limit
  h1 = y1 + h1 < int16_t(HEIGHT) ? h1 : int16_t(HEIGHT) - y1; // limit
  if ((w1 <= 0) || (h1 <= 0)) return; 
  // make x1, w1 multiple of 8
  w1 += x1 % 8;
  if (w1 % 8 > 0) w1 += 8 - w1 % 8;
  x1 -= x1 % 8;
  if (!_using_partial_mode) _Init_Part();
  _setPartialRamArea(x1, y1, w1, h1);
  _Update_Part();
}
void GxEPD2_213::refreshNoDelay(int16_t x, int16_t y, int16_t w, int16_t h)
{
  if (_initial_refresh) return refresh(false); // initial update needs be full update
  // intersection with screen
  int16_t w1 = x < 0 ? w + x : w; // reduce
  int16_t h1 = y < 0 ? h + y : h; // reduce
  int16_t x1 = x < 0 ? 0 : x; // limit
  int16_t y1 = y < 0 ? 0 : y; // limit
  w1 = x1 + w1 < int16_t(WIDTH) ? w1 : int16_t(WIDTH) - x1; // limit
  h1 = y1 + h1 < int16_t(HEIGHT) ? h1 : int16_t(HEIGHT) - y1; // limit
  if ((w1 <= 0) || (h1 <= 0)) return; 
  // make x1, w1 multiple of 8
  w1 += x1 % 8;
  if (w1 % 8 > 0) w1 += 8 - w1 % 8;
  x1 -= x1 % 8;
  if (!_using_partial_mode) _Init_Part();
  _setPartialRamArea(x1, y1, w1, h1);
  _Update_Part_noDelay();
}

void GxEPD2_213::powerOff(void)
{
  _PowerOff();
}

void GxEPD2_213::hibernate()
{
  _PowerOff();
  if (_rst >= 0)
  {
    _writeCommand(0x10); // deep sleep mode
    _writeData(0x1);     // enter deep sleep
    _hibernating = true;
  }
}

void GxEPD2_213::_setPartialRamArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  _writeCommand(0x11); // set ram entry mode
  _writeData(0x01);    // x increase, y decrease : as in demo code
  _writeCommand(0x44);
  _writeData(x / 8);
  _writeData((x + w - 1) / 8);
  _writeCommand(0x45);
  _writeData((y + h - 1) % 256);
  _writeData((y + h - 1) / 256);
  _writeData(y % 256);
  _writeData(y / 256);
  _writeCommand(0x4e);
  _writeData(x / 8);
  _writeCommand(0x4f);
  _writeData((y + h - 1) % 256);
  _writeData((y + h - 1) / 256);
}

void GxEPD2_213::_PowerOn()
{
  if (!_power_is_on)
  {
    _writeCommand(0x22);
    _writeData(0xc0);
    _writeCommand(0x20);
    //_waitWhileBusy("_PowerOn", 0);
  //此处不可以使用 yield 或者 delay 因为这些函数在中断里面也会被调用
    while(digitalRead(EMW3_EPD_BUSY_PIN) == _busy_level) ESP.wdtFeed();
  }
  _power_is_on = true;
}

void GxEPD2_213::_PowerOff()
{
  _writeCommand(0x22);
  _writeData(0xc3);
  _writeCommand(0x20);
  //_waitWhileBusy("_PowerOff", 0);
  //while(digitalRead(EMW3_EPD_BUSY_PIN) == _busy_level) yield();
  _power_is_on = false;
  _using_partial_mode = false;
}

void GxEPD2_213::_InitDisplay()
{
  if (_hibernating) _reset();
  _writeCommand(0x01); // Panel configuration, Gate selection
  _writeData((HEIGHT - 1) % 256);
  _writeData((HEIGHT - 1) / 256);
  _writeData(0x00);
  _writeCommand(0x03); // Set Gate driving voltage
  _writeData(0x15);
  _writeData(0x0e);
  _writeCommand(0x04); // Set Source output voltage.
  _writeData(0x1d);

  _writeCommand(0x0c); // softstart
  _writeData(0xd7);
  _writeData(0xd6);
  _writeData(0x9d);
  _writeCommand(0x2d); // VCOM setting
  _writeData(0x55);    // * different
  _writeCommand(0x3a); // DummyLine
  _writeData(0x1a);    // 4 dummy line per gate
  _writeCommand(0x3b); // Gatetime
  _writeData(0x08);    // 2us per line
  _setPartialRamArea(0, 0, WIDTH, HEIGHT);
}
uint8_t GxEPD2_213::LUTDefault_full[40] = {   // command
  0x22, 0x55, 0xaa, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0f, 0x0f, 0x13, 0x13, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

uint8_t GxEPD2_213::LUTDefault_part[40] = {   // command
  0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0d, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
uint8_t GxEPD2_213:: LUTfullSize = 29;
uint8_t GxEPD2_213:: LUTpartSize = 29;
void GxEPD2_213::setLut(bool FullOrPart, const uint8_t *lut, uint8_t size){
  if(nullptr == lut|| size == 0 || size>40) return;
  if(FullOrPart){
    for(int i=0;i<size;i++) LUTDefault_part[i] = lut[i];
    LUTpartSize = size;
  }
  else{
    for(int i=0;i<size;i++) LUTDefault_full[i] = lut[i];
    LUTfullSize = size;
  }
}
void GxEPD2_213::_Init_Full()
{
  _InitDisplay();
  _writeCommand(0x32);
  _writeDataPGM(LUTDefault_full, LUTfullSize);
  _PowerOn();
  _using_partial_mode = false;
}

void GxEPD2_213::_Init_Part()
{
  _InitDisplay();
  _writeCommand(0x32);
  _writeDataPGM(LUTDefault_part, LUTpartSize);
  _PowerOn();
  _using_partial_mode = true;
}

void GxEPD2_213::_Update_Full()
{
  _writeCommand(0x22);
  _writeData(0xc4);
  _writeCommand(0x20);
  //_waitWhileBusy("_Update_Full", 0);
  //此处不可以使用 yield 或者 delay 因为这些函数在中断里面也会被调用
  while(digitalRead(EMW3_EPD_BUSY_PIN) == _busy_level) ESP.wdtFeed();
  _writeCommand(0xff);
}

void GxEPD2_213::_Update_Part()
{
  _writeCommand(0x22);
  _writeData(0x04);
  _writeCommand(0x20);
  //_waitWhileBusy("_Update_Part", 0);
  //此处不可以使用 yield 或者 delay 因为这些函数在中断里面也会被调用
  while(digitalRead(EMW3_EPD_BUSY_PIN) == _busy_level) ESP.wdtFeed();
  _writeCommand(0xff);
}

void GxEPD2_213::_Update_Full_noDelay()
{
  _writeCommand(0x22);
  _writeData(0xc4);
  _writeCommand(0x20);
  ESP.wdtFeed();
  //_waitWhileBusy("_Update_Full", 0);

}

void GxEPD2_213::_Update_Part_noDelay()
{
  _writeCommand(0x22);
  _writeData(0x04);
  _writeCommand(0x20);
  ESP.wdtFeed();
  //_waitWhileBusy("_Update_Part", 0);
}

} //namespace 