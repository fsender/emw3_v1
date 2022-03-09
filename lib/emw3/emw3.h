/**
 * @file emw3.h
 * @author fsender
 * @brief EMW3 基础驱动封装库
 * @version 1.0.4
 * Update: 2022-3-9
 * push16bitSprite函数, 能更方便的显示16bit的灰度图像
 * 
 * Update: 2022-02-25
 * 现在中断刷新可以自己开启或关闭了
 * 
 * Update: 2022-02-15
 * 修复在当 hlines < 3 的时候指示条显示错位的问题
 * 
 * Update: 2022-02-13
 * 修复了SD卡与刷屏互相干扰导致屏幕或者SD卡死机问题
 * 由"中断刷屏"导致, 现已修复, 在系统函数中仅使用阻塞刷屏
 * 修复了SD卡图片不能直接显示,必须间接显示的bug
 * 原因: 未创建 palette, 未在初始化时调用 createPalette 函数
 * 
 * Update: 2022-02-09
 * 新增: 串口模拟显示, 在电脑的串口助手上也能显示画面, 不会缩短屏幕寿命
 * 发现问题: SD卡与刷屏互相干扰导致屏幕或者SD卡死机
 * 
 * Update: 2021-11-27
 * 初次创建
 */
#ifndef _CTG_EMW3_V1_H
#define _CTG_EMW3_V1_H

#include <Arduino.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include "emw3_defines.h"
#include "eddrv_2.h"
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/LGFX_Sprite.hpp>
#include "gb2312.h"   //字体
#include "osmall5_tf.h"   //字体
using namespace emw3_EinkDriver;
//using namespace emw3epd;

const lgfx::U8g2font cn_font  ( chinese_city_gb2312  );
const lgfx::U8g2font osmall5_font  ( ctg_u8g2_font_osmall5_tf  );

class EMW3 : public EinkDrv_213, public LGFX_Sprite {
  public:
    EMW3();
    /**  @brief 刷屏
     *   @param part 0:无延时全刷    1:无延时快刷     2: 阻塞式全刷     3: 阻塞式快刷
     */
    uint8_t display(uint8_t part = 1) ;
    /**  @brief 设置旋转方向
     *   @param rot 0-3: 旋转, 4-7: 镜像+旋转
     */
    void rotation(int rot);
    /** @brief 读取按钮的电平值
     *  @param btn 按钮代号
     *  @return uint8_t 读到的电平值, 没按下为1, 按下为0
     */
    uint8_t getBtn(uint8_t btn);
    /**  @brief 检查屏幕是否还在刷屏
     *   @return uint8_t 0 可用, 1,正在刷新,为0时将会可用
     */
    inline uint8_t epdBusy(){ 
#ifdef DEBUG_DISPLAY_SERIAL
      return 0;
#else
      return digitalRead(EMW3_EPD_BUSY_PIN);
#endif
    }
    /** @brief 设置是否在刷屏时启用中断(自动进行)
     *  @param en 
     */
    inline void setInterruptDisplay(bool en = 1) { interruptDisplay = en; _refreshing = 0; }
    inline bool getInterruptDisplay(           ) { return interruptDisplay; }
    /** @brief 显示一个16位灰度的图块
     *  @param spr16bit 目标图块
     *  @param x 显示的x坐标
     *  @param y 显示的y坐标 */
    void push16bitSprite(LGFX_Sprite spr16bit, int x, int y);
  private:
    //for 250 * 122 sized buffer
    static unsigned char buff [4000];
};
// preset lut mode.

#endif