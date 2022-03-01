/**
 * @file emw3keyboard.h
 * @author fsender (Bilibili FriendshipEnder)
 * @brief 虚拟全键盘输入
 * @version 0.1
 * 
 * update 2022-02-28
 * 初次加入
 */
#ifndef Emw3keyboard_H_FILE
#define Emw3keyboard_H_FILE

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "emw3.h"
#include "emw3_defines.h"
#include "gb2312.h"
#define DEBOUNCE_DELAY_MS 25
#define LONGPRESS_DELAY_MS 500

extern const lgfx::U8g2font cn_font;

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
    int getchs(char * inString, int maxlen, int x = 0, int y = 0, bool multiline = false);


    /** @brief 同上,输入字符串, 返回输入的字符数
     * @param dest 显示缓冲区,输入的字符串将会临时显示在这里
     */
    int getchs(LGFX_Sprite dest, char * inString, int maxlen, 
    int x = 0, int y = 0, int destx = 0, int desty = 0, bool multiline = false);
  private:
    EMW3 * in_tft;
    int lastx = 0;
    int lasty = 0;
    const char *keys[3] = {"1234567890"
                           "qwertyuiop"
                           "asdfghjkl\x02"
                        "\x01zxcvbnm ",

                           "!@#$%^&*()"
                           "QWERTYUIOP"
                           "ASDFGHJKL\x02"
                        "\x01ZXCVBNM ",

                           "1234567890"
                          "\t`<>()[]{}"
                           "\\|\'\"+-*/=\x02"
                        "\x01,._?!:;~"  };
    /**  @brief 辅助绘图函数  */
    void destDraw(LGFX_Sprite dest, const char * inString, int dx, int dy);
};
#endif