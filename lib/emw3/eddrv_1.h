/**
 * @file eddrv_1.h
 * @author fsender
 * @brief 
 * @version 1.0
 * Update: 2022-2-21
 * 大幅度精简了没有必要的函数定义
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
    bool _init_stat, _initial_refresh;
    bool _power_is_on, _using_partial_mode, _hibernating;
    uint16_t _reset_duration;
 /**
  * @brief 标记刷新状态: 0 未正在刷新 1 在传输数据  2 在等待刷新完毕 
  * 中断会更改此数值 */
    volatile uint8_t _refreshing;
};
}
#endif
