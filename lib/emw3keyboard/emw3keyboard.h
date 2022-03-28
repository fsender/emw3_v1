/******* FRIENDSHIPENDER *******
 * @file emw3keyboard.h (即将开源)
 * @author fsender (Bilibili FriendshipEnder)
 * @brief 虚拟全键盘输入
 * 
 * @version Beta 1.0.6

 * Update: 2022-3-29
 * 开发工具包 1.0 版本正式发布
 * 
 * update 2022-02-28
 * 初次加入
 */
#ifndef Emw3keyboard_H_FILE
#define Emw3keyboard_H_FILE

#include <Arduino.h>
#include <FS.h>
#include <SDFS.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "emw3.h"
#include "emw3_defines.h"
#define KeyDEBOUNCE_DELAY_MS 25
#define KeyLONGPRESS_DELAY_MS 200
#define KEYBOARD_MODULE_WIDTH 87
#define KEYBOARD_MODULE_HEIGHT 58

extern const lgfx::U8g2font cn_font;
extern const lgfx::U8g2font osmall5_font;

class emw3Keyboard{
  public:
    emw3Keyboard(EMW3 *intft = nullptr) : in_tft(intft) {}

  /**  @brief 输入字符串, 返回输入的字符数
   * @param inString 要输入的字符串, 函数将会改写次字符串内容
   * @param maxlen 输入的字符串的最大长度,需要预留 maxlen+1 的空间,存储结束符
   * @param x, y: 窗口显示位置
   * @param multiline 是否为多行输入框(尚未实现)
   * @return int 输入的字符数
   */
    unsigned int getstr(char * inString, unsigned int maxlen, int x = 0, int y = 0, bool multiline = false);


    /** @brief 同上,输入字符串, 返回输入的字符数
     * @param dest 显示缓冲区,输入的字符串将会临时显示在这里
     */
    unsigned int getstr(LGFX_Sprite dest, char * inString, unsigned int maxlen, 
    int x = 0, int y = 0, int destx = 0, int desty = 0, bool multiline = false);
    /** @brief 中文输入 
     *  @param trapTo 是否允许跳转到英文输入
    */
    int getCNstr(LGFX_Sprite dest, char * inString, unsigned int maxlen, 
    int x = 0, int y = 0, int destx = 0, int desty = 0, bool trapTo = false);
  private:
    EMW3 * in_tft;
    int lastx = 0;
    int lasty = 0;
    static const char keys[];
    uint16_t key_cn_x, key_cn_y, key_des_x, key_des_y;
    static const uint8_t in_board[];
    String getCNString_T9(String workstr, unsigned int maxlen, LGFX_Sprite board, LGFX_Sprite dest);
    void themeRect(LovyanGFX *canvas,int16_t x,int16_t y,int16_t w,int16_t h,uint16_t color = 0);
    void dest_drawString(LGFX_Sprite dest,const char * inStr);
    void drawUI(LGFX_Sprite & canvas);
    void drawInputInfo(const char *comment, int kx, int ky, int num1, int num2);
};

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//拼音输入法 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   

//拼音码表与拼音的对应表
typedef struct
{
  const __FlashStringHelper *py_input;//输入的字符串
  const __FlashStringHelper *py;	   //对应的拼音
  const char *pymb;	   //码表
}py_index;

#define MAX_MATCH_PYMB 	6	//最大匹配数

//拼音输入法
typedef struct
{
  char(*getpymb)(char *instr);			//字符串到码表获取函数
  py_index *pymb[MAX_MATCH_PYMB];	//码表存放位置
}pyinput;

extern pyinput t9;
char str_match(char*str1,char*str2);
char get_matched_pymb(char *strin,py_index **matchlist);
char get_pymb(char* str);
void test_py(char *inputstr);
#endif