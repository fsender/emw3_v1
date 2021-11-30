#include "GxEPD2_EPD.h"
#include "emw3_defines.h"
#include <pgmspace.h>

namespace emw3_gxepd2{
GxEPD2_EPD::GxEPD2_EPD(int16_t busy_level, uint32_t busy_timeout,
                       uint16_t w, uint16_t h, GxEPD2::Panel p, bool c, bool pu, bool fpu) :
  WIDTH(w), HEIGHT(h), panel(p), hasColor(c), hasPartialUpdate(pu), hasFastPartialUpdate(fpu),
  _busy_level(busy_level), _busy_timeout(busy_timeout), _diag_enabled(false),
  _spi_settings(4000000, MSBFIRST, SPI_MODE0)
{ 
  _cs = EMW3_EPD_CS_PIN;
  _dc = EMW3_EPD_DC_PIN;
  _rst = EMW3_EPD_RST_PIN;
  _busy = EMW3_EPD_BUSY_PIN;
  _initial_write = true;
  _initial_refresh = true;
  _power_is_on = false;
  _using_partial_mode = false;
  _hibernating = false;
  _reset_duration = 20;
}

void GxEPD2_EPD::init(bool initial, uint16_t reset_duration, bool pulldown_rst_mode)
{
  _initial_write = initial;
  _initial_refresh = initial;
  _pulldown_rst_mode = pulldown_rst_mode;
  _power_is_on = false;
  _using_partial_mode = false;
  _hibernating = false;
  _reset_duration = reset_duration;
  if (_cs >= 0)
  {
    digitalWrite(_cs, HIGH);
    pinMode(_cs, OUTPUT);
  }
  if (_dc >= 0)
  {
    digitalWrite(_dc, HIGH);
    pinMode(_dc, OUTPUT);
  }
  _reset();
  if (_busy >= 0)
  {
    pinMode(_busy, INPUT);
  }
  SPI.begin();
  if (_busy == MISO) // may be overridden, to be verified
  {
    pinMode(_busy, INPUT);
  }
}

void GxEPD2_EPD::_reset()
{
    _hibernating = false;
}
/*
void GxEPD2_EPD::_waitWhileBusy(const char* comment, uint16_t busy_time)
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
void GxEPD2_EPD::_writeCommand(uint8_t c)
{
  //SPI.beginTransaction(_spi_settings);
  digitalWrite(_dc, LOW);
  digitalWrite(_cs, LOW);
  SPI.transfer(c);
  digitalWrite(_cs, HIGH);
  digitalWrite(_dc, HIGH);
  //SPI.endTransaction();
}

void GxEPD2_EPD::_writeData(uint8_t d)
{
  //SPI.beginTransaction(_spi_settings);
  digitalWrite(_cs, LOW);
  SPI.transfer(d);
  digitalWrite(_cs, HIGH);
  //SPI.endTransaction();
}

void GxEPD2_EPD::_writeData(const uint8_t* data, uint16_t n)
{
  //SPI.beginTransaction(_spi_settings);
  digitalWrite(_cs, LOW);
  for (uint16_t i = 0; i < n; i++)
  {
    SPI.transfer(*data++);
  }
  digitalWrite(_cs, HIGH);
  //SPI.endTransaction();
}

void GxEPD2_EPD::_writeDataPGM(const uint8_t* data, uint16_t n, int16_t fill_with_zeroes)
{
  //SPI.beginTransaction(_spi_settings);
  digitalWrite(_cs, LOW);
  for (uint16_t i = 0; i < n; i++)
  {
    SPI.transfer(pgm_read_byte(&*data++));
  }
  while (fill_with_zeroes > 0)
  {
    SPI.transfer(0x00);
    fill_with_zeroes--;
  }
  digitalWrite(_cs, HIGH);
  //SPI.endTransaction();
}

void GxEPD2_EPD::_writeDataPGM_sCS(const uint8_t* data, uint16_t n, int16_t fill_with_zeroes)
{
  //SPI.beginTransaction(_spi_settings);
  for (uint8_t i = 0; i < n; i++)
  {
    digitalWrite(_cs, LOW);
    SPI.transfer(pgm_read_byte(&*data++));
    digitalWrite(_cs, HIGH);
  }
  while (fill_with_zeroes > 0)
  {
    digitalWrite(_cs, LOW);
    SPI.transfer(0x00);
    fill_with_zeroes--;
    digitalWrite(_cs, HIGH);
  }
  //SPI.endTransaction();
}

void GxEPD2_EPD::_writeCommandData(const uint8_t* pCommandData, uint8_t datalen)
{
  //SPI.beginTransaction(_spi_settings);
  digitalWrite(_dc, LOW);
  digitalWrite(_cs, LOW);
  SPI.transfer(*pCommandData++);
  digitalWrite(_dc, HIGH);
  for (uint8_t i = 0; i < datalen - 1; i++)  // sub the command
  {
    SPI.transfer(*pCommandData++);
  }
  digitalWrite(_cs, HIGH);
  //SPI.endTransaction();
}

void GxEPD2_EPD::_writeCommandDataPGM(const uint8_t* pCommandData, uint8_t datalen)
{
  //SPI.beginTransaction(_spi_settings);
  digitalWrite(_dc, LOW);
  digitalWrite(_cs, LOW);
  SPI.transfer(pgm_read_byte(&*pCommandData++));
  digitalWrite(_dc, HIGH);
  for (uint8_t i = 0; i < datalen - 1; i++)  // sub the command
  {
    SPI.transfer(pgm_read_byte(&*pCommandData++));
  }
  digitalWrite(_cs, HIGH);
  //SPI.endTransaction();
}

void GxEPD2_EPD::_startTransfer()
{
  SPI.beginTransaction(_spi_settings);
  digitalWrite(_cs, LOW);
}

void GxEPD2_EPD::_transfer(uint8_t value)
{
  SPI.transfer(value);
}

void GxEPD2_EPD::_endTransfer()
{
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}
}