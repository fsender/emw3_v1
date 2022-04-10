/******* FriendshipEnder ******* EMW3 Clock *******
 * @file EMW3App.h
 * @author FriendshipEnder
 * @brief 应用程序基础类,也包含一些系统设置功能, 毕竟系统设置也算是一个app,
 *        你觉得呢, 灵炎?
 * 
 * 关于SD卡和应用程序
 * 每个应用程序存放在 /EMW3/app/ 文件夹内
 * 如某app名为 "记事本", 对应文件夹名为 notepad
 * 则存放路径为
 * /EMW3/
 *   /app/
 *     /notepad/
 *       /firmware.bin (应用本体)
 *       /data/ (应用数据, 可以是应用程序的外部数据资源, 
 *               其他应用相关的bin文件, 临时文件, 缓存等,可选)
 *       /logo.bmp (图标,单色位图,可选)
 *       /cover1.bmp (封面,彩色位图, 推荐全屏大小,可选)
 *       /cover2.bmp (封面,彩色位图, 方形,可选)
 *       /app.json (应用其他数据, 包含网址, MD5等数据,用于网上下载)
 * RTC使用分配情况
 * 0-31 禁止使用, 因为SDUpdater占用
 * 32-校验码1, 校验码2用于其他用途
 * 33-设置状态字
 * 34-日期时间,32-bit,单位为秒
 * 35- 对于SD卡加载程序, 此为RTC校准后时间,32-bit, 用于抵消SD卡app更新需要的时间,保持时间准确度,
 *     部分程序依赖此数值,单位为RTC周期数,~5.75微秒  对于深度睡眠, 此为深度睡眠时间,单位为毫秒;
 * 36-新的应用程序id
 * 37-新的应用程序用户设置
 * 38-新的应用程序参数长度
 * 39 保留, 用户可用
 * 40-127 用户可用RTC, 对于部分依赖参数的app 可以用于传递参数
 * 
 * 关于设置状态字, 通过Eeprom或者rtcUserMemory可以保留用于其他app
     * 应用加载前,需要先把设置项目存入RTC Memory
     * Byte0 WiFi相关   bit0: WiFi STA已启动
     * 默认值为0x00      bit1: WiFi自动连接, 默认为1
     *                  bit2: WiFi自动重新连接, 默认为1
     *                  bit3: WiFi数据自动保存Flash, 默认为1
     *                  bit4: WiFi AP已启动
     *                  bit5: WiFi WebServer已启动 (未实现)
     *                  bit6: 保留
     *                  bit7: 保留
     * 
     * Byte1 显示相关    bit2:0: 对比度(8-15) 默认值为111(15)
     * 默认值为0x67      bit3: 在状态栏显示日期
     *                  bit4: 12/24h制度显示 0 24h, 1 12h
     *                  bit5: 开机初始化SD卡 默认1
     *                  bit6: SD卡状态是否正常 EEPROM内总是1
     *                  bit7: 显示方向, 1为翻转180°
     * 
     * Byte2 app加载相关 bit0: 应用来源, 0 默认SD卡目录, 1 深度睡眠唤醒, 深度睡眠前,请将此位置1
     * 默认值为0x00      bit1: RTC内容是否可信 RTC内容继承自上一个app则为1, 否则为0, 
     *                         只有在ota升级时将此位置1, EEPROM内总是0
     *            [RTC] bit3:2: 应用带参数, 0 无参数 1 有参数 3 以String类的方法读取
     *                  bit4: 标记为OTA升级成功或者失败, 如果成功标记此位为1, 否则为0,视为串口更新
     *                        只有此bit为1时候, 参数才可用
     * Byte3 保留
 * 
 * @version Beta 1.0
 * 
 * 未完善的API
 * 1 随时召唤状态栏
 * 2 状态栏显示的WiFi为正常表示状态, 而不是始终显示在屏幕上
 * 3 可以截屏
 * 4 app可以查看预览图
 * 5 允许快捷跳转app, (带参数)
 * 6 message 功能:尽量完善
 * 7 热点功能:尽量完善, 可在应用内选择 是否开启服务器
 * 8 更多设置
 * 目标: 尽量把部分app库的非必须 API 做成可选的, 用define 指令控制
 * 问题: 极度依赖SD卡
 * 未来将会尝试无卡也可以运行
 * 
 * Update: 2022-03-27
 * 增加 deepSleep函数, 可以方便的实现deepsleep功能
 * Update: 2022-03-27
 * 功能性大更新 可以按app名称加载程序了
 * 
 * Update: 2022-03-18
 * 不再管理应用数据EEPROM相关内容,因为不好用, 所以将会继续采用系统的EEPROM, 
 * 此库不再干预用户使用EEPROM
 * Update: 2022-03-11
 * 初次创建
 */
#ifndef _EMW3_APP_H_FILE
#define _EMW3_APP_H_FILE
#include <Arduino.h>
#include <ArduinoJson.h>
#include <pgmspace.h>
#include <ESP8266WiFi.h>
#include <SDFS.h>
#include <Updater.h>
//#include <ESP8266SDUpdater.h>
#include <LittleFS.h>
#include <ESP_EEPROM.h>
#include <TimeLib.h>
#include "emw3.h"
#include "listMenuV2.h"
#include "emw3keyboard.h"
#include "wireless.h"

#define EMW3APP_BASE_PATH           "/EMW3/app/"              //应用默认路径
#define EMW3APP_BASE_LIST_PATH      "/EMW3/app/apps.txt"      //应用路径列表
//#define EMW3APP_BASE_NAME_PATH    "/EMW3/app/appnames.txt"  //应用名称列表
#define EMW3APP_CONTROL_BMPPATH     "/EMW3/theme/control.bmp" //控制台贴图路径
#define RTC_CheckSum1               3253342798ul              //RTC校验码
#define EEPROM_CheckSum             WIRELESS_EEPROM_CheckSum  //EEPROM校验码
#define INITIAL_SETTINGCONFIG       0x0000670eul              //默认设置
#define EMW3APP_MAX_APPS            40                        //最大可安装app数
#define EMW3APP_RTC_PARAM           0x60000                   //参数可用性校验

//#define EMW3APP_DEBUG                                       //开启debug模式

#define EMW3APP_WIFI_STA           (1)
#define EMW3APP_AUTO_CONNECT       (2)
#define EMW3APP_AUTO_RECONNECT     (4)
#define EMW3APP_WIFI_PERSISTENT    (8)
#define EMW3APP_WIFI_AP            (16)
#define EMW3APP_WIFI_WEB_SERVER    (32)

#define EMW3APP_EPD_CONTRAST       (1<<8)
#define EMW3APP_DISPLAY_DATE       (1<<11)
#define EMW3APP_12_HR              (1<<12)
#define EMW3APP_STARTUP_INIT_SD    (1<<13)
#define EMW3APP_SD_OK              (1<<14)
#define EMW3APP_ROTATION           (1<<15)

#define EMW3_WAKEUP_FROM_DEEPSLEEP (1<<16)
#define EMW3APP_RTC_TRUST          (1<<17)
#define EMW3APP_HAS_PARAMETER      (1<<18)
#define EMW3APP_STRING_PARAMETER   (1<<19)
#define EMW3APP_OTA_SUCCESSFUL     (1<<20)

class wireless;
class EMW3App{
  public:
    /** @brief Construct a new EMW3App object
     *  @param emw3 app runniong base 运行框架基础
     *  @param in_menu 菜单
     *  @param in_fs_norm 常规工作的文件系统, 通常在LittleFS
     *  @param in_fs_load 加载用文件系统, 通常在SD卡上
     */
    EMW3App(EMW3 &emw3, listMenuV2 &in_menu, fs::FS &in_fs_norm = SDFS, fs::FS &in_fs_load = SDFS):
    tft(&emw3), menu(&in_menu), fs_norm(&in_fs_norm), fs_load(&in_fs_load), wl(tft,menu), dispbuf(tft)
    {
      menu->setFS(in_fs_norm); //初始化各个模块的文件系统
      wl.setFS(in_fs_norm);
      dispbuf.setColorDepth(1);
      dispbuf.fillScreen(1);
    }
    /** @brief  初始化app库(用于加载程序等功能)
     *  @return 0:rtc内容无效, 1:正常初始化
     */
    bool begin(uint8_t _fast = 0) {return init(_fast);}
    /** @brief  初始化app库(用于加载程序等功能)
     *  @return 0:rtc内容无效, 1:正常初始化
     */
    bool init(uint8_t _fast = 0);
    /** @brief 加载程序中的user rtc memory中的 param 字段, 最多88块
     *  @return uint8_t 数据,长度256字节, 如果RTC内容不可信则返回0
     */
    uint8_t readParameter(uint8_t *dat);
    /** @brief 写入程序中的user rtc memory中的 param 字段, 最多88块
     *  @return uint8_t 数据,长度256字节, 如果RTC内容不可信则返回0
     */
    uint8_t writeParameter(const uint8_t *dat, uint16_t len);
    /// @brief 清空程序中的user rtc memory中的 param 字段
    void clearParameter(){ writeParameter(nullptr,0); }
    /** @brief 从EEPROM加载全局用户设置数据
     *  @return 返回设置参数
     */
    uint32_t appEepromRead() {  return wl.userAppData; }
    /** @brief 把全局用户设置数据写入EEPROM,但不保存
     *  @param dat 要写入的数据,数据写入后不会立即保存
     */
    void appEepromWrite(uint32_t dat){ wl.userAppData = dat; }
    /** @brief 立即把全局用户设置数据写入EEPROM
     *  @param dat 要写入的数据,数据写入后立即保存
     */
    void appEepromWriteImmediately(uint32_t dat){ wl.userAppData = dat; writeEeprom(); }
    /// @brief 加载应用程序列表(GUI), 需要带一个EMW3类
    uint16_t loadAppListGUI();
    /** @brief 加载应用程序列表(返回字符串), 需要两个字符串数组
     *  @param appstr 字符串数组, 输出app名称
     *  @param path 字符串数组, 输出app路径, 只包含对应文件夹名
     *  @param maxapps 最多显示多少个app
    */
    //void loadAppList(String *appstr, String *path, int maxapps);
    /** @brief 加载Bin文件
     *  @param path Bin文件路径
     */
    //void loadBin(String *path);
    /** @brief 加载一个app
     *  @param path app路径, 应当是一个文件夹
     *  @param param 应用参数
     *  @param paramlen 应用参数的长度
     */
    void loadApp(uint32_t app_id);//, const uint8_t *param = nullptr, uint16_t paramlen = 0
    /** @brief 加载Bin文件
     *  @param path Bin文件路径
     *  @param param 应用参数
     *  @param paramlen 应用参数的长度
     */
    void loadBin(String path);//, const uint8_t *param = nullptr, uint16_t paramlen = 0
    /** @brief 设置OTA更新时的回调函数
     *  @param fun_cb 要回调的函数
     */
    //void loadBinCallback(String *fun_cb);
    /// @brief 返回应用程序内部编号
    uint32_t getAppID() {return wl.userAppID;}
    /// @brief 返回应用程序代号/包名, 只能是英文,数字或者下划线
    String getAppPath() {return appPath; }
    /// @brief 返回应用程序完整路径
    String getFullAppPath() {return String(F(EMW3APP_BASE_PATH))+appPath; }
    /** @brief 返回指定应用程序代号/包名
     *  @param appID 指定的应用程序编号
     *  @return 应用程序代号/包名
     */
    String getAppPath(uint8_t appID);
    /** @brief 返回指定应用程序完整路径/位置
     *  @param appID 指定的应用程序编号
     *  @return 应用程序路径/位置
     */
    String getFullAppPath(uint8_t appID) {return String(F(EMW3APP_BASE_PATH))+getAppPath(appID); }

    /// @brief 手动设置当前应用程序名称
    void setAppNameManual(const char * nname) { appName = nname; };
    /// @brief 返回当前应用程序名称
    String getAppName() {return appName;}
    /// @brief 返回指定代号(包名)的应用程序名
    String getAppName(const String app_Path){
      return getAppName(app_Path.c_str());
    }
    /// @brief 返回指定代号(包名, String格式)的应用程序名
    String getAppName(const char *app_Path){ return getAppInfo(app_Path, "name"); }
    /// @brief 指定应用程序id, 返回应用程序名
    String getAppName(uint8_t appID) { return getAppName(getAppPath(appID)); };

    /// @brief 返回指定代号(包名)的应用程序名
    String getAppInfo(const String app_Path, const String _key){
      return getAppInfo(app_Path.c_str(), _key.c_str());
    }
    /// @brief 返回指定代号(包名, String格式)的应用程序名
    String getAppInfo(const char *app_Path, const char *_key);
    /// @brief 指定应用程序id, 返回应用程序名
    String getAppInfo(uint8_t appID, const char *_key) { return getAppInfo(getAppPath(appID).c_str(), _key); };
    /// @brief 指定应用程序id, 返回应用程序名
    String getAppInfo(uint8_t appID, const String _key) { return getAppInfo(getAppPath(appID), _key); };

    /** @brief 绘制状态栏, 有点像Mac风格... 此函数默认是阻塞函数,(允许读取按钮数据进入控制中心)
     *  @param part 绘制颜色, 0黑色, 1白色, 2只有右半边, 
     *  4移动显示位置(仅用于时钟程序) , 8缓存显示覆盖区,且阻塞程序
     * */
    void drawStatusBar(uint8_t part);
    /// @brief 此函数可用于进入设置页面
    void settingPage();
    /// @brief 截屏到SD卡,可以在drawStatusBar状态下调用
    void screenshot(bool useSpriteCache = 0);
    /// @brief 清理截屏缓存(就是那个LGFX_Sprite)
    void freeDisplayBuffer();
    /// @brief 设置数据
    void editSetting(uint32_t got, uint8_t val);
    /// @brief 设置数据并保存
    inline void saveSetting(uint32_t got, uint8_t val){
      editSetting(got, val);
      ESP.rtcUserMemoryWrite(33,&wl.emw3status,4);
      writeEeprom(wl.userAppData);
    }
    /// @brief 设置数据, 立即生效
    void editSettingImmediately(uint32_t got, uint8_t val);
    /// @brief 设置数据并保存, 立即生效
    inline void saveSettingImmediately(uint32_t got, uint8_t val){
      editSettingImmediately(got, val);
      ESP.rtcUserMemoryWrite(33,&wl.emw3status,4);
      writeEeprom(wl.userAppData);
    }
    /// @brief 读取所有设置数据
    inline uint32_t getSettings(){ return wl.emw3status; }
    /// @brief 读取设置数据
    inline uint8_t getSetting(uint32_t got){ 
      if(EMW3APP_EPD_CONTRAST == got) return ((wl.emw3status>>8) & 7)+8;
      return !!(wl.emw3status & got); 
    }
    /// @brief 保存设置数据于EEPROM中,目标地址为0, EEPROM默认使用1KB数据存储
    void commitSettings();
    
    /// @brief 返回wifi设置基础类, 可以由用户调用其函数
    wireless &wifiSettings() { return wl; }
    /// @brief 同步把需要的数据写入到EEPROM
    void writeEeprom(uint32_t dat = 0) { wl.userAppData = dat; wl.writeEeprom_impl(); };
    /// @brief 获取NTP时间
    time_t getNTPTime() { return wl.getNTPTime(); }
    /** @brief 可以带参数进行定时深度睡眠, 手动按下reset将会导致时钟不准
     *  @param sleepTimeMs 时间,单位是毫秒. 尝试一个过大的时间可能会导致模块无法唤醒.
     * */
    void deepSleep(unsigned int sleepTimeMs, bool disp = 1, const uint8_t *dat = nullptr, uint16_t len = 0);
    unsigned int getchs(char *ws,unsigned int wslen,const char * msg = nullptr);

  private:
    //在此存储基本设置
    String appName;
    String appPath;
    EMW3 * tft;
    listMenuV2 * menu;
    fs::FS * fs_norm;
    fs::FS * fs_load;
    wireless wl;
    LGFX_Sprite dispbuf;

    /// @brief 初始化应用程序模块
    void initAppInfo_impl(uint32_t id);
    /// @brief 进入设置页面的辅助绘图函数
    void settingPageGUI(uint8_t cp);
    /// @brief 进入设置页面的快速辅助绘图函数
    void settingPageGUIAgain(uint8_t cp);
    
    /** @brief 生成app名称列表, 当目标列表不存在时更新此列表
     *  @return uint32_t 生成的app个数
     */ 
    uint32_t generateAppsList();
    /// @brief 回调函数
    friend void dispinfo_callback(uint16_t sel, void *param);
    String *allAppNames;
    bool updateFromSD(String fileName);
};
#endif