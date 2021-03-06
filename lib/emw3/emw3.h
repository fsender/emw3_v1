/******* FRIENDSHIPENDER *****
 * @file emw3.h
 * @author FriendshipEnder
 * @brief EMW3 基础驱动封装库
 * @version 1.1
 * Update: 2022-3-26
 * 增加getBtnAll函数, 一次可以读取更多按钮
 * 
 * Update: 2022-3-13
 * 重新封装了init()函数(初始化函数),更方便使用,加入了功能相同的begin()函数
 * 
 * Update: 2022-3-9
 * push16bitSprite函数, 能更方便的显示16bit的灰度图像
 * 现在按钮定义随屏幕旋转方向自动旋转(旋转180°时, 左键变成了右键, etc.)
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
#include <SDFS.h>
#include <LittleFS.h>
#include "emw3_defines.h"
#include "eddrv_2.h"
#include "TimeLib.h" //时间库
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/LGFX_Sprite.hpp>
#include "gb2312.h"   //字体
#include "osmall5_tf.h"   //字体
#include "o_bold8.h"   //字体
using namespace emw3_EinkDriver;
//using namespace emw3epd;
#ifdef DEBUG_DISPLAY_SERIAL
#define SERIAL_REFRESH_TIME 300
#endif
#define NEED_FULL_UPDATE 20 //每20次局部刷新需要一次全局刷新

const lgfx::U8g2font cn_font  ( chinese_city_gb2312  );          // 中文字体
const lgfx::U8g2font osmall5_font  ( ctg_u8g2_font_osmall5_tf ); // 3x5像素超小字体
const lgfx::U8g2font obold8_font (ctg_u8g2_font_o_bold8);        // 5x8像素超粗字体

//按键定义, 使用我一般不用的全局变量
extern uint8_t keyL,keyM,keyR;
class EMW3 : public EinkDrv_213, public LGFX_Sprite {
  public:
    EMW3();
    /** @brief 初始化EMW3各个组件
     *  @param initOptions 参数选项
     * Bit 0: 是否初始化SD卡
     * Bit 1: 是否在初始化完成后清屏(初始化屏幕缓冲区)
     * Bit 2: 是否在初始化完成后刷屏(以全刷显示)
     */
    bool init(uint8_t initOptions = 7);
    /** @brief 初始化EMW3各个组件
     *  @param initOptions 参数选项
     * Bit 0: 是否初始化SD卡
     * Bit 1: 是否在初始化完成后清屏(初始化屏幕缓冲区)
     * Bit 2: 是否在初始化完成后刷屏(以全刷显示)
     */
    bool begin(uint8_t initOptions = 7){ return init(initOptions); }
    /**  @brief 刷屏, 如果屏幕忙会直接跳过
     *   @param part 0:无延时全刷    1:无延时快刷     2: 阻塞式全刷     3: 阻塞式快刷
     *   @return 0:显示成功   1: 刷屏失败
     */
    uint8_t display(uint8_t part = 1) ;
    /**  @brief 设置旋转方向
     *   @note  注意: 尽量不要使用 setRotation 函数, 会出现按键问题
     *   @param rot 0-3: 旋转, 4-7: 镜像+旋转
     */
    void rotation(int rot);
    /** @brief 读取按钮的电平值
     *  @note  注意: 尽量不要使用 digitalRead 函数, 会出现SPI干扰问题
     *  @param btn 按钮代号
     *  @return uint8_t 读到的电平值, 没按下为1, 按下为0
     */
    uint8_t getBtn(uint8_t btn);
    /** @brief 读取所有按钮的电平值
     *  @return uint8_t 读到的电平值, 全都没有按下为111, 全都按下为000, 只按下左键为011
     */
    inline uint8_t getBtnAll(){ return (getBtn(keyL)<<2) | (getBtn(keyM)<<1) | (getBtn(keyR)); }
    /**  @brief 检查屏幕是否还在刷屏
     *   @return uint8_t 0 可用, 1,正在刷新,为0时将会可用
     */
    inline uint8_t epdBusy(){ 
#ifdef DEBUG_DISPLAY_SERIAL
      return millis() - lastRefresh <= SERIAL_REFRESH_TIME;
#else
      return digitalRead(EMW3_EPD_BUSY_PIN);
#endif
    }
    /** @brief 设置是否在刷屏时启用中断(自动进行), 默认禁用
     *  @param en 
     */
    inline void setInterruptDisplay(bool en = 1) { interruptDisplay = en; _refreshing = 0; }
    inline bool getInterruptDisplay(           ) { return interruptDisplay; }
    /** @brief 显示一个16位灰度的图块
     *  @param spr16bit 目标图块
     *  @param x 显示的x坐标
     *  @param y 显示的y坐标 */
    void push16bitSprite(LGFX_Sprite spr16bit, int x, int y);
    /// @brief 获取电池电压
    uint32_t getVoltage();
    /// @brief 获取电池电压, 百分比
    uint8_t getVoltagePercent();
    /// @brief 设置是否自动全刷, 默认开启
    void setAutoFullRefresh(bool af) { autoFullRefresh=af; }
    /// @brief 获取是否开启了自动全刷
    bool getAutoFullRefresh() { return autoFullRefresh; }
    /// @brief 重置局部刷新计数器
    void resetRefreshCombo() { refreshCombo = 0; }
  private:
    //for 250 * 122 sized buffer
    static unsigned char buff [4000];
    bool sd_ok;
    bool autoFullRefresh; 
    uint8_t refreshCombo;
#ifdef DEBUG_DISPLAY_SERIAL
    uint32_t lastRefresh = 0;
#endif
};

#endif