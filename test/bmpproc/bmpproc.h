/**
 * @file bmpproc.h
 * @author friendshipEnder
 * @brief BMP图像解码, 只支持24位的位图( Windows画图默认格式 )
 * @version 0.1
 * update: 2022-03-07
 * 初次创建
 */
#ifndef BMP_PROC_H_FILE
#define BMP_PROC_H_FILE
//#define BAYER_GRAYSCALE_ALG //使用bayer的抖动算法
#define FLOYD_STEINBERG_GRAYSCALE_ALG //使用Floyd-Steinberg的抖动算法
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <LittleFS.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <math.h>
#include "emw3.h"
class bmpproc{
  public:
  /** @brief 创建 bmpproc 对象
   *  @param bmpfs 目标文件系统
   *  @param path  路径字符串
   */
  bmpproc(fs::FS &bmpfs, const char *path);
  /// @brief 关闭BMP文件
  void closeBmpFile(){ if(bmpf) bmpf.close(); }
  /// @brief 因为这个类打开了文件,需要在析构时关闭它
  ~bmpproc(){ closeBmpFile(); }
  /** @brief 获取某一像素颜色
   *  @param x, y 坐标
   *  @return uint32_t 颜色的RGB值
   */
  uint32_t get(uint32_t x, uint32_t y);
  /** @brief 获取某一像素颜色, 并转化为256阶灰度
   *  @param x, y 坐标
   *  @param gamma_on 是否对灰度值进行gamma校正(速度慢)
   *  @return uint32_t 颜色的灰度值
   */
  uint8_t getGray(uint32_t x, uint32_t y, bool gamma_on = 0);

  /** @brief 以黑白色显示这个BMP图像
   *  @note  此函数为EMW3的专有函数, 不可以移植到其他平台
   *  @param x,y,w,h: 显示位置*/
  void drawBW(EMW3 &emw3, int x = 0, int y = 0, int w = 0, int h = 0);

  /** @brief 以16阶灰度显示这个BMP图像
   *  @note  此函数为EMW3的专有函数, 不可以移植到其他平台
   *  @param x,y,w,h: 显示位置
   *  @param gamma_on 是否开启灰度值gamma校正(速度慢) */
  void draw16Gray(EMW3 &emw3, int x = 0, int y = 0, int w = 0, int h = 0, bool gamma_on = false);

  
  inline int32_t width()  {return w;}
  inline int32_t height() {return h;}
  inline int32_t bitPerPixel() {return bpp;}
  inline operator uint8_t() {return goError;}
  private:
  fs::File bmpf;
  uint8_t goError = 0; //错误原因
  uint32_t offset=0;
  uint32_t w=0;
  uint32_t w3=0; //存储每一行实际需要的字节数
  uint32_t h=0;
  uint32_t bpp=0;
#ifdef BAYER_GRAYSCALE_ALG
  static const uint8_t bayer_tab [];
#endif
};
#endif