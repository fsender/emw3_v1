/******* FRIENDSHIPENDER *******
 * @file emw3_defines.h
 * @author fsender (f_ender@163.com)
 * @brief provide emw3 brick pin definations.
 * 
 * @version Beta 1.0.0

 * Update: 2022-3-29
 * 开发工具包 1.0 版本正式发布
 * 
 * update: 2022-2-13
 * 新增: 串口模拟显示, 在电脑的串口助手上也能显示画面, 不会缩短屏幕寿命
 * 修改 DEBUG_DISPLAY_SERIAL即可
 * update: 2021-11-27
 * 初次创建
 */
#ifndef _CTG_EMW3_DEFINES_H
#define _CTG_EMW3_DEFINES_H
#include <Arduino.h>
//#define DEBUG_DISPLAY_SERIAL //使用串口控制台调试显示部分,取代墨水屏显示.
// * 需要搭配特定的字体和较高的串口波特率使用,如921600

#define _BOARD_RELEASE

#ifdef _BOARD_RELEASE
//最新的硬件
#define EMW3_SD_CS_PIN 0 //D3, SD卡的片选引脚
#define EMW3_EPD_CS_PIN 15 //D8, 显示屏的片选引脚
#define EMW3_EPD_DC_PIN 5 //D1, 显示屏的DC引脚
#define EMW3_EPD_RST_PIN -1 //reserved 保留的重置引脚
#define EMW3_EPD_BUSY_PIN 4 //D2 显示屏的Busy引脚
#define EMW3_BtnL 5 //D1, 左键
#define EMW3_BtnM 12 //中间键 D6引脚
#define EMW3_BtnR 2 //D4 右键

#define HARDWARE_NEED_SD_END 0
#define REMIND_LED_PIN LED_BUILTIN

#elif defined(_BOARD_PRE_RELEASE_NO_USB)
//在拿到pcb之前的硬件, 尽管不能连接SD卡, 
//但是必须要能使用 load from SD 功能
#define EMW3_SD_CS_PIN 2
#define EMW3_EPD_CS_PIN 15
#define EMW3_EPD_DC_PIN 0
#define EMW3_EPD_RST_PIN 2 // real: 2
#define EMW3_EPD_BUSY_PIN 4
#define EMW3_BtnL 5
#define EMW3_BtnM D9 //RX
#define EMW3_BtnR D10 //TX  
#define _HARDWARE_NEED_SD_END 0
#define _REMIND_LED_PIN LED_BUILTIN

#else 
//最初的硬件
#define EMW3_SD_CS_PIN 4
#define EMW3_EPD_CS_PIN 15
#define EMW3_EPD_DC_PIN 0
#define EMW3_EPD_RST_PIN -1
#define EMW3_EPD_BUSY_PIN -1
#define EMW3_BtnL 5
#define EMW3_BtnM 0
#define EMW3_BtnR 2
#define _HARDWARE_NEED_SD_END 0
#define _REMIND_LED_PIN LED_BUILTIN

#endif

#define EMW3_REAL_WIDTH 122
#define EMW3_WIDTH 128
#define EMW3_HEIGHT 250
#define EMW3_WHITE 65535
#define EMW3_BLACK 0
#endif