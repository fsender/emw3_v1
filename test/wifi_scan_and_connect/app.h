/**
 * @file EMW3App.h
 * @author FriendshipEnder
 * @brief 应用程序基础类
 * 每个应用程序存放在 /EMW3/app/ 文件夹内
 * 如某app名为 "记事本", 对应文件夹名为 notepad
 * 则存放路径为
 * /EMW3/
 *   /app/
 *     /notepad/
 *       /app.bin (应用本体)
 *       /data/ (应用数据, 可以是应用程序的外部数据资源, 
 *               其他应用相关的bin文件, 临时文件, 缓存等,可选)
 *       /eeprom.dat (可选)
 *       /icon.bmp (图标,单色位图,可选)
 *       /cover1.bmp (封面,彩色位图, 推荐全屏大小,可选)
 *       /cover2.bmp (封面,彩色位图, 方形,可选)
 *       /app.json (应用其他数据, 包含网址, MD5等数据,用于网上下载)
 * @version 0.1
 * Update: 2022-03-11
 * 初次创建
 */
#ifndef _EMW3_APP_H_FILE
#define _EMW3_APP_H_FILE
#include <Arduino.h>
#include <pgmspace.h>
#include <ESP8266SDUpdater.h>
#include <ESP_EEPROM.h>
#include "emw3.h"
#define EMW3APP_BASE_PATH "/EMW3/app/"

class EMW3App{
  public:
    /** @brief 加载程序中的user rtc memory中的 param 字段
     *  @return uint8_t 数据,长度256字节
     */
    uint8_t loadParameter(uint8_t *dat);
    /** @note 不要频繁调用此类中的函数, 因为此EEPROM是由SD镜像文件实现的
     *  @brief 从EEPROM(SD卡的镜像文件)加载数据
     *  @param dat 数据
     *  @param offset 初始位置
     *  @param length 要读取/写入的数据长度
     *  @return uint8_t 成功加载到的字节数
     */
    uint8_t appEepromRead(uint8_t *dat, uint8_t offset, uint8_t length);
    uint8_t appEepromWrite(const uint8_t *dat, uint8_t offset, uint8_t length);
    /// @brief 加载应用程序列表(GUI), 需要带一个EMW3类
    void loadAppList(EMW3 &ebase);
    /** @brief 加载应用程序列表(返回字符串), 需要两个字符串数组
     *  @param appstr 字符串数组, 输出app名称
     *  @param path 字符串数组, 输出app路径, 只包含对应文件夹名
     *  @param maxapps 最多显示多少个app
    */
    void loadAppList(String *appstr, String *path, int maxapps);
    /** @brief 加载Bin文件
     *  @param path Bin文件路径
     */
    void loadBin(String *path);
    /// @brief 返回应用程序名
    String getAppName() {return appName;}
    /// @brief 返回应用程序路径
    String getAppPath() {return String(F(EMW3APP_BASE_PATH))+appPath; }
  private:
    //在此存储基本设置
    String appName;
    String appPath;
};
#endif