/**
 * @file emw3_definas.h
 * @author fsender (f_ender@163.com)
 * @brief provide emw3 brick pin definations.
 * 
 * @version 1.0.0 pre-release
 * @date 2021-11-27
 */
#ifndef _CTG_EMW3_DEFINES_H
#define _CTG_EMW3_DEFINES_H
#define _BOARD_RELEASE

#ifdef _BOARD_RELEASE
//最新的硬件
#define EMW3_SD_CS_PIN 0
#define EMW3_EPD_CS_PIN 15
#define EMW3_EPD_DC_PIN 5
#define EMW3_EPD_RST_PIN -1
#define EMW3_EPD_BUSY_PIN 4
#define EMW3_BtnL 5
#define EMW3_BtnM 12
#define EMW3_BtnR 2

#define HARDWARE_NEED_SD_END 0
#define REMIND_LED_PIN LED_BUILTIN

#elif defined(_BOARD_PRE_RELEASE_NO_USB)
//在拿到pcb之前的硬件
#define EMW3_SD_CS_PIN 0
#define EMW3_EPD_CS_PIN 15
#define EMW3_EPD_DC_PIN 5
#define EMW3_EPD_RST_PIN -1
#define EMW3_EPD_BUSY_PIN 4
#define EMW3_BtnL 5
#define EMW3_BtnM 12
#define EMW3_BtnR 2
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