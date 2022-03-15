/**
 * @file eddrv_1.cpp
 * @author fsender
 * @brief 
 * @version 1.0
 * Update: 2021-11-27
 * 初次创建
 */
#include "eddrv_1.h"
#include "emw3_defines.h"
#include <pgmspace.h>

namespace emw3_EinkDriver{
EinkDriver::EinkDriver(int16_t busy_level, uint32_t busy_timeout,
                       uint16_t w, uint16_t h, bool c, bool pu, bool fpu) :
  WIDTH(w), HEIGHT(h), _EinkDrv_Has_Colors(c), _EinkDrv_Has_Part_Show(pu), _EinkDrv_Has_Part_Show_Fast(fpu),
  _busy_level(busy_level), _busy_timeout(busy_timeout), _diag_enabled(false),
  _spi_settings(4000000, MSBFIRST, SPI_MODE0){ 
  _cs = EMW3_EPD_CS_PIN;
  _dc = EMW3_EPD_DC_PIN;
  _rst = EMW3_EPD_RST_PIN;
  _busy = EMW3_EPD_BUSY_PIN;
  _init_stat = true;
  _initial_refresh = true;
  _power_is_on = false;
  _using_partial_mode = false;
  _hibernating = false;
  _reset_duration = 20;
}

void EinkDriver::init_epd(bool initial, uint16_t reset_duration, bool pulldown_rst_mode){
  _init_stat = initial;
  _initial_refresh = initial;
  _pulldown_rst_mode = pulldown_rst_mode;
  _power_is_on = false;
  _using_partial_mode = false;
  _hibernating = false;
  _reset_duration = reset_duration;
  if (_cs){
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
  }
  if (_dc){
    pinMode(_dc, OUTPUT);
    digitalWrite(_dc, HIGH);
  }
  _reset();
  SPI.begin();
  if (_busy)
    pinMode(_busy, INPUT);
}

void EinkDriver::_reset(){
    _hibernating = false;
}
/*
void EinkDriver::_waitWhileBusy(const char* comment, uint16_t busy_time)
{
  if (_busy >= 0)
  {
    delay(1); // add some margin to become active
    unsigned long start = micros();
    while (1)
    {
      if (digitalRead(_busy) != _busy_level) break;
      delay(1);
      if (micros() - start > _busy_timeout)
      {
        Serial.println("Busy Timeout!");
        break;
      }
    }
    if (comment)
    {
#if !defined(DISABLE_DIAGNOSTIC_OUTPUT)
      if (_diag_enabled)
      {
        unsigned long elapsed = micros() - start;
        Serial.print(comment);
        Serial.print(" : ");
        Serial.println(elapsed);
      }
#endif
    }
    (void) start;
  }
  else delay(busy_time);
}
*/
void EinkDriver::_writeCommand(uint8_t c){
  //SPI.beginTransaction(_spi_settings);
  digitalWrite(_dc, LOW);
  digitalWrite(_cs, LOW);
  SPI.transfer(c);
  digitalWrite(_cs, HIGH);
  digitalWrite(_dc, HIGH);
  //SPI.endTransaction();
}

void EinkDriver::_writeData(uint8_t d){
  //SPI.beginTransaction(_spi_settings);
  digitalWrite(_cs, LOW);
  SPI.transfer(d);
  digitalWrite(_cs, HIGH);
  //SPI.endTransaction();
}

void EinkDriver::_writeData(const uint8_t* data, uint16_t n){
  //SPI.beginTransaction(_spi_settings);
  digitalWrite(_cs, LOW);
  for (uint16_t i = 0; i < n; i++)
    SPI.transfer(*data++);
  digitalWrite(_cs, HIGH);
  //SPI.endTransaction();
}

void EinkDriver::_writeDataPGM(const uint8_t* data, uint16_t n, int16_t fill_with_zeroes){
  //SPI.beginTransaction(_spi_settings);
  digitalWrite(_cs, LOW);
  for (uint16_t i = 0; i < n; i++)
    SPI.transfer(pgm_read_byte(&*data++));
  while (fill_with_zeroes > 0){
    SPI.transfer(0x00);
    fill_with_zeroes--;
  }
  digitalWrite(_cs, HIGH);
  //SPI.endTransaction();
}

void EinkDriver::_writeDataPGM_sCS(const uint8_t* data, uint16_t n, int16_t fill_with_zeroes){
  //SPI.beginTransaction(_spi_settings);
  for (uint8_t i = 0; i < n; i++){
    digitalWrite(_cs, LOW);
    SPI.transfer(pgm_read_byte(&*data++));
    digitalWrite(_cs, HIGH);
  }
  while (fill_with_zeroes > 0){
    digitalWrite(_cs, LOW);
    SPI.transfer(0x00);
    fill_with_zeroes--;
    digitalWrite(_cs, HIGH);
  }
  //SPI.endTransaction();
}

void EinkDriver::_writeCommandData(const uint8_t* pCommandData, uint8_t datalen){
  //SPI.beginTransaction(_spi_settings);
  digitalWrite(_dc, LOW);
  digitalWrite(_cs, LOW);
  SPI.transfer(*pCommandData++);
  digitalWrite(_dc, HIGH);
  for (uint8_t i = 0; i < datalen - 1; i++)  // sub the command
    SPI.transfer(*pCommandData++);
  digitalWrite(_cs, HIGH);
  //SPI.endTransaction();
}

void EinkDriver::_writeCommandDataPGM(const uint8_t* pCommandData, uint8_t datalen)
{
  //SPI.beginTransaction(_spi_settings);
  digitalWrite(_dc, LOW);
  digitalWrite(_cs, LOW);
  SPI.transfer(pgm_read_byte(&*pCommandData++));
  digitalWrite(_dc, HIGH);
  for (uint8_t i = 0; i < datalen - 1; i++)  // sub the command
    SPI.transfer(pgm_read_byte(&*pCommandData++));
  digitalWrite(_cs, HIGH);
  //SPI.endTransaction();
}
} //end namespace