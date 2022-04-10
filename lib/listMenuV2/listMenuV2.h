/******* FRIENDSHIPENDER *****
 * 重新封装了init()函数(初始化函数),现在用init_epd函数替代原init函数
 * @file listMenuV2.h
 * @author fsender (Bilibili FriendshipEnder) (Q:3253342798)
 * @brief 菜单显示程序,可以提供一个功能丰富的菜单功能,仅使用于EMW3
 * @version beta 1.3
 * 
 * update 2022-04-07
 * 1.现在可以为 listMenu 指定初始值了, API不再兼容, 就是指定一开始选项在哪里
 * 2.新增listMenu的演示模式, 仅显示内容并且刷屏, 但不可以交互
 * 此bit为1则仅显示GUI部分,显示选项等内容, 不能交互, 不能选择选项, 瞬间执行完
 * ***** 注意: 此功能使用了 goto 语句, 如果不需要此功能, 建议关闭.*****
 */
#define LMV2_USE_LISTMENU_DEMO_MODE_SUPPORT
/* 
 * update 2022-03-13
 * 允许使用 const String * 类型作为参数使用此库中的函数
 * 优化了drawDialog显示对话框的时候对多行文本的处理
 * 加入了选项回调函数, 使用后自动释放
 * 
 * update 2022-03-07
 * 修复图标缓存机制的bug,(图标缓存重复使用时导致花屏)
 * 
 * update 2022-02-28
 * 即将更新 浮动键盘: 在EMW3Keyboard库中, 暂不开源
 * 
 * update 2022-02-25
 * 窗口,对话框,滑动条
 * 增加菜单快速度跳转(同时按住左右键触发滑动条)
 * 修复了关于 drawMulti 中"\x01 - 对此条目绘制颜色反色" 不反色的bug
 * 
 * update 2022-02-14
 * 增加图标数据缓存,减少读写SD卡的次数
 * 增加动态lut, 来实现加速滑动(原来需要 ~350ms, 目标 ~200ms)
 * 记得在listMenuV2.cpp的第269行修改刷新需要的时间(根据lut数据动态修整)
 * 
 * update 2022-02-13
 * 修复了SD卡与刷屏互相干扰导致屏幕或者SD卡死机问题
 * 
 * update 2022-02-03
 * 初次创建
 * @copyright Copyright (c) 2022 
 */
#ifndef _LISTMENU_V2_H
#define _LISTMENU_V2_H
//注意事项: 使用PCtoLCD2002取模时,需要阴码+顺向+C51输出
#include <Arduino.h>
#include <SDFS.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "emw3.h"
#include "emw3_defines.h"
#include "gb2312.h"
//功能支持
//#define LMV2_USE_PROGMEM_SUPPORT
#define LMV2_USE_DRAWMULYI_CANVAS_SUPPORT
//#define LMV2_USE_THEME_SUPPORT //SD卡主题设置(未实现)
#define LMV2_USE_SD_BMP_SUPPORT
#define LMV2_USE_SD_TXT_SUPPORT
#define MAX_XV_LINE 8 //hlines支持的最大值

#define _EMW3_ICON_PATH "/EMW3/icon/"
#define _EMW3_THEME_PATH "/EMW3/theme/" // E:\EMW3\sys\theme/border1.bmp
#define _EMW3_DIALOG_PATH "/EMW3/dialog/"
  //0 plain   常规文本
  //1 printf plain 支持 printf,只能带一个参数
  //2 可以反色
  //4 可以带 toggle 按钮和数值条,也可以禁用某个选项
//#define LMV2_USE_ADVANCED_PLAIN_SUPPORT

//系统常量
#define DEBOUNCE_DELAY_MS 25
#define LONGPRESS_DELAY_MS 500
//extern LGFX tft;
extern const lgfx::U8g2font cn_font;

class listMenuV2{
  public:
    listMenuV2(EMW3 *intft = nullptr) : in_tft(intft), emw3_cnt_fs(&SDFS) {}
    ~listMenuV2();
/// @brief 设置每个选项的宽度/高度, 所有数据如果设为0即为使用默认值,标题高度设为1则禁用标题栏
    void setSize(const int16_t * sizedata); 
/// @brief 设置单个选项的宽度/高度
  enum sizeOption_t{
    widthMax = 0,
    heightMax,
    sprWidth,
    sprHeight,
    sprOffsetX,
    sprOffsetY,
    borderWidth ,
    borderHeight,
    titleHeightMax,
    titleSpriteHeight,
    titleSprOffsetY,
    titleInvert,
  };
    void setSize(sizeOption_t option, int16_t sizedata);

enum themeselection{
  themeNull = 0,
  themeTitle = 1,
  themeNormal = 2,
};
#ifdef LMV2_USE_THEME_SUPPORT
/// @brief 设置选项卡位图信息,指针为NULL则代表使用默认主题
//    void setStyle(const int16_t * styleSizedata, const uint8_t ** styleXBmp);
/// @brief 设置标题栏位图信息 注意默认标题栏是反色显示的,指针为NULL则代表使用默认主题
//    void setTitleStyle();
#ifdef LMV2_USE_SD_BMP_SUPPORT
/// @brief 从SD卡图片文件设置选项卡位图信息
/// @return uint8_t 设置是否成功: 0 成功 1 参数错误 2 文件不存在或者内存不足
  /*styleSizedata 数据结构: 
  [0]: 图像个数 可选 3 或者 8
  3张图片时,对应数字代表的位置
  0 1 0
  2   2
  0 1 0
  8张图片时,对应数字代表的位置
  0 1 3
  2   4
  5 6 7                   */
    //uint8_t setThemeFromFile(int16_t styleBmps, const char **bmpFilePath);
/// @brief 从SD卡图片文件设置标题栏位图信息
    uint8_t setThemeFromFile(themeselection themesel,int16_t styleBmps, const char **bmpFilePath, fs::FS Fs = * emw3_cnt_fs);
#endif
#endif
/** @brief 显示一个多级菜单
 * @param x 显示坐标X
 * @param y 显示坐标Y
 * @param hlines 菜单行数,不含标题行
 * @param selections 菜单选项数, 不含标题选项
 * @param init_pos 初始位置, 默认是 0
 * @param settings 菜单设置
 * bit 0 : 是否对 text 使用 progmem
 * bit 1 : 是否绘制标题栏 ,(形参会在内部更改) 0 绘制   1 不绘制
 * bit 2 : 是否显示模拟滑动指示条
 * bit 3 : 是否使用SD卡上的txt文件来显示内容
 * 如 bit 3 为1,则参数 text将会只读取text[0]作为标题, text[1]作为文件路径, text[2]表示从第几行开始读
 * 若此位为0且iconsfilepath不为nullptr 显示iconsfilepath对应的图标
 * 若此位为1且iconsfilepath不为nullptr 显示文件/文件夹图标
 * bit 4 : 滑动指示条宽度
 * bit 5 : 此bit为1则仅显示GUI部分,显示选项等内容, 不能交互, 不能选择选项, 瞬间执行完
 * 注意: 此功能必须使用 goto 语句, 如果不需要, 可以关闭.
 * 
 * @param text 每一行的内容 支持 drawMulti 函数的 单行内容指令 详见 drawMulti 函数, 第一行为title
 * @param interactions 交互选项, 规则比较复杂
 * @return uint16_t 选择的条目编号, 1为第一条消息, 0为退出
 */
  uint16_t listMenu(int16_t x,int16_t y,uint8_t hlines,int16_t numitem, uint16_t selected,
    uint8_t settings, const char **text, void ** interactions = nullptr);
  uint16_t listMenu(int16_t x,int16_t y,uint8_t hlines,int16_t numitem, uint16_t selected,
    uint8_t settings, const String *text, void ** interactions = nullptr);
/// @brief 内部调用, 绘制菜单的GUI
  void listMenu_impl_GUI(int16_t x,int16_t y,int16_t w,uint8_t hlines,uint8_t settings,
    const char **text, void ** interactions = nullptr, const char ** bmpsrc = nullptr);
/// @brief 绘制圆角矩形,圆角半径固定为2
  void themeRect(themeselection themesel, int16_t x,int16_t y,int16_t w,int16_t h,uint16_t color = 0);
/// @brief 绘制填充圆角矩形,圆角半径固定为2, 只用于标题 (已经被 themeRect() 取代)
//   void fillArcRect(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t color);
/// @brief 绘制返回对话框 注意是居中显示的, 设置h为负的数则为黑色
  void drawKeyText(int16_t x,int16_t y,const char *_2ch = nullptr,int16_t w = 40,int16_t h = 24);
inline bool getDrawMulti() { return (bool)useDrawMulti; }
inline void setDrawMulti(bool en) { useDrawMulti = en; }
#ifdef LMV2_USE_DRAWMULYI_CANVAS_SUPPORT
/**
 * @brief 在canvas或sprite中绘制一个单色字符图纹,可以包含多种形状,位图等数据
 * @param sprbase 基础canvas
 * @param cmds 单行绘制指令 
 * 可以使用单行绘制指令 
 * \x01: 对此条目绘制颜色反色, 无结尾
 * \x02: 后面是一个位图数据, 紧接着是位图x显示位置(1 byte), y显示位置(1 byte), 宽度(1 byte), 
 高度(1 byte), 然后是位图数据(高度一定小于等于16), 数据段以\xff结尾.
 * \x03: 后面是4字节, 标识绘制位置x和y以及宽度高度w和h, 然后是一个SD卡上的位图文件的路径, 只支持单色位图, 
 若不以'/'开头则为相对路径(/EMW3/icon), 否则为绝对路径, 数据段以\xff结尾.
 若以'\\'开头则为littleFS上的文件, (未实现)
 * \x04: 绘制点,后面跟着2个单字节参数,分别是x1,y1 无结尾
 * \x05: 绘制直线段,后面跟着4个单字节参数,分别是x1,y1,x2,y2 无结尾
 * \x06: 绘制矩形,后面跟着4个单字节参数,分别是x1,y1,x2,y2, 无结尾
 * \x07: 绘制填充矩形,后面跟着4个单字节参数,分别是x1,y1,x2,y2 无结尾
 * \x08: 绘制三角形,后面跟着3个单字节参数,分别是x1,y1,x2,y2,x3,y3 无结尾
 * \x09: 绘制填充三角形,后面跟着3个单字节参数,分别是x1,y1,x2,y2,x3,y3 无结尾
 * \x0a: 绘制椭圆(绘制圆时设置需要两个半径相等), 后面跟着4个单字节参数,分别是x1,y1,r1,r2 无结尾
 * \x0b: 绘制填充椭圆(绘制圆时设置需要两个半径相等), 后面跟着4个单字节参数,分别是x1,y1,r1,r2 无结尾
 * \x0c: 对后面的文字加粗/斜体/加下划线/加删除线 后面跟着1字节参数, 无结尾
   参数: bit 0 是否加粗
         bit 1 是否斜体
         bit 3:2 是否下划线  00 无  01 直线  10 波浪线  11 双直线
         bit 5:4 是否删除线  00 无  01 直线  10 波浪线  11 双直线
 * \x0d: 设置文本对齐方式 后面跟着1字节参数, 无结尾
   参数: top_left        =  0  // Top left (default)
    , top_center      =  1  // Top center
    , top_right       =  2  // Top right
    , middle_left     =  4  // Middle left
    , middle_center   =  5  // Middle center
    , middle_right    =  6  // Middle right
    , bottom_left     =  8  // Bottom left
    , bottom_center   =  9  // Bottom center
    , bottom_right    = 10  // Bottom right
    , baseline_left   = 16  // Baseline left (Line the 'A' character would sit on)
    , baseline_center = 17  // Baseline center
    , baseline_right  = 18  // Baseline right
 * \x0e: 设置文本显示位置 后面跟着3字节参数, mode,x,y 无结尾 
 mode:   0xxxxxxx
          |||||||----> x方向为负
          ||||||-----> y方向为负
          |||||----> x方向为从右向左计数
          ||||-----> y方向为从右向左计数
          |||------> 居中显示X方向
          ||-------> 居中显示Y方向
          |--------> 使用相对位置坐标
 * \x0f: 条目文本正文, 缺省时直接显示正文,正文只能显示一行
 -------------------------以下三个选项只能在文本条目最前面(首字节)使用----------------------------
 . \x10: 以bool形式读取条目对应的interactions的值, 然后在文本最后显示拨动开关, 可点击确定来打开或关闭
 . \x11: 以bool形式读取条目对应的interactions的值, 若为true则此选项不可被选中
 . \x12: 以bool形式读取条目对应的interactions的值, 若为false则此选项不可被选中
 ----------------------------------------------------------------------------------------------
 * \x13: 以bool形式读取条目对应的interactions的值, 然后显示在文本最后
 * \x14: 以int32形式读取条目对应的interactions的值, 然后显示在文本最后
 * \x15: 以uint32形式读取条目对应的interactions的值, 然后显示在文本最后
 * \x16: 以float形式读取条目对应的interactions的值, 然后显示在文本最后
 * \x17: 以double形式读取条目对应的interactions的值, 然后显示在文本最后
 * \x18: 以int32形式读取条目对应的interactions的值, 后面跟着1个单字节参数,分别是w(长度: 像素)
 然后显示为进度条, 0为空, 10000为满, -10000为反方向满
 * \x19: 以double形式读取条目对应的interactions的值, 后面跟着1个单字节参数,分别是w(长度: 像素)
 然后显示为进度条, 0为空, 1.0为满, -1.0为反方向满
 * \x1a: 以字符形式读取条目对应的interactions的值, 然后显示在文本最后
 * \x1b: 以C字符串形式读取条目对应的interactions的值, 然后显示在文本最后
 * \x1c: 以String字符串形式读取条目对应的interactions的值, 然后显示在文本最后
 * \x1d: 以C字符串形式读取interactions的值作为位图在SD卡上的路径
 * , 后面跟着4个单字节参数,分别是x1,y1,w,h 然后显示此位图文件
 * \x1e: 以位图数组形式读取interactions的值, 后面跟着4个单字节参数,分别是x1,y1,w,h 然后显示此位图
 * @param intera 交互数据
 */
  void drawMulti(lgfx::LGFX_Sprite &sprbase,const char * cmds,void * intera);
#endif
  /// @brief 显示文本,没有更多的交互选项.
  void drawPlain(lgfx::LGFX_Sprite &sprbase,const char * cmds,void * intera);
  /** @brief 设置图标库, 需要保持bmps对应的数组始终可用
   *  @param bmps 图标数据,仅支持16x16大小位图
      @param num 为设置的图标个数, 0为禁用图标库
  */
  void setIcon(uint16_t num,const uint8_t **bmps);
#ifdef LMV2_USE_SD_BMP_SUPPORT
  /** @brief 设置SD卡上的图标库, 需要保持bmps对应的字符串始终可用
   *  @param bmps 图标文件名数据
      @param num 为设置的图标个数, 0为禁用图标库
  */
  void setSDIcon(uint16_t num,const char **path);
  void clearSDIconCache() {
    for(uint8_t i=0;i<16;i++) iconDataCacheLabel[i]=nullptr; //字典索引
  }
#endif

//适用于多语言情况
#define CTG_CN_okText "确定"
#define CTG_CN_backText "返回"
#define CTG_CN_moreText "更多"
/**
 * @brief 显示对话框
 * @param str 字符串指针
 * @param dx 对话框模式
 * Bit 1-0: 显示的按钮个数, 
 * 0, 不显示按钮, 函数将会瞬间执行完,不会自动刷屏
 * 1, 显示一个'确定'按钮,自动刷屏,将会在按钮松开后继续执行代码(带去抖)
 * 2, 显示'确定','取消' 按钮,自动刷屏,将会在按钮松开后继续执行代码(带去抖)
 * 3, 显示'确定','取消','更多'三个按钮,自动刷屏,将会在按钮松开后继续执行代码(带去抖)
 * Bit 2==1, 自定义按钮文本
 * 按钮文本将会定位在str[2],str[3],str[4]
 * Bit 3==1, 强制支持多行文本.
 * Bit 4==1, 自动刷屏模式强行设置为全局刷新.
 * Bit 5==1, 显示SD卡上的图标,预留图标大小为64x64图标路径为str[1].
 * Bit 6==1, 显示图标,图标数组,数据为str[1].默认尺寸16x16, 
 * 如果Bit 5==1则为32x32, 如果Bit 7==1则为48x48, 如果Bit 5和Bit 7==1则为64x64
 * @param dmultiw,dmultih 若非0,则用drawMulti渲染对话框, 此数值为渲染宽度 (不支持多行文本)
 * @return uint8_t 选中的值, 0 中间, 1 右边, 2 左边
 */
  uint8_t drawDialog(const char **str,uint8_t dx,uint8_t dmultiw = 0, uint8_t dmultih = 0,int16_t x=-32768,int16_t y=-32768);
  uint8_t drawDialog(const String *str,uint8_t dx,uint8_t dmultiw = 0,uint8_t dmultih = 0,int16_t x=-32768,int16_t y=-32768);

/**
 * @brief 设置 menu 的默认文件系统, 启动默认是SD卡文件系统
 * @param ing_FS 文件系统, 支持SD卡(SDFS), SPIFFS, LittleFS
 */
  inline void setFS(fs::FS &ing_FS){ emw3_cnt_fs = &ing_FS; }
/**
 * @brief 单项功能选择框,一行两个功能对应两个返回值,最多10个,没有标题,不可返回
 * @param sel 选项个数
 * @param str 选项字符串
 * @param btnlen 选项按钮长度
 * @return 选择了的选项
 */
  uint8_t selectionList(uint8_t sel, const char ** str, int16_t btnlen = 0);
  uint8_t selectionList(uint8_t sel, const String* str, int16_t btnlen = 0);

#define LMV2_SLIDER_D_WIDTH 64
/**
 * @brief 滑动条 长按中间键确定输入, 同时按住左右键退出
 * @param str 选项文本
 * @param minv,maxv 最大/最小值
 * @param initialVal 初始值
 * @return 选择了的值
 */
  int32_t slider(const char * str, int32_t minv=0, int32_t maxv=100, int32_t initialVal=0);

  //键盘输入函数, 已经实现, 在emw3Keyboard中
  //int32_t getKNum(int32_t initialVal = 0);
  //size_t getKString(char * srcstr, int32_t maxlen = 0);
  //size_t getKChinese(char * srcstr, int32_t maxlen = 0);

/**  @brief 设置回调函数
  void (*listMenu_cb)(uint16_t,void *);
  void (*selectionList_cb)(uint8_t,void *);
  void (*slider_cb)(int32_t,void *);
 *   @param _listMenu_cb 回调函数名
 */
  inline void setListMenuCallback(void (*_listMenu_cb)(uint16_t,void *),void * _userdata){
    listMenu_cb = _listMenu_cb;
    userdata = _userdata;
  }
  inline void setSelectionListCallback(void (*_selectionList_cb)(uint8_t,void *),void * _userdata){
    selectionList_cb = _selectionList_cb;
    userdata = _userdata;
  }
  inline void setSliderCallback(void (*_slider_cb)(int32_t,void *),void * _userdata){
    slider_cb = _slider_cb;
    userdata = _userdata;
  }
  inline void drawList(const char *ch1,const char *ch2 = nullptr,
      const char *ch3 = nullptr,const char *ch4 = nullptr){
    drawListBtn(0,ch1,ch2,ch3,ch4);
  }

  inline uint8_t drawListBtn(uint8_t btns, const char *ch1, const char *ch2 = nullptr,
      const char *ch3 = nullptr,const char *ch4 = nullptr){
    String ch_f = String(ch1);
    if(ch2 != nullptr) ch_f += String('\n') + String(ch2);
    if(ch3 != nullptr) ch_f += String('\n') + String(ch3);
    if(ch4 != nullptr) ch_f += String('\n') + String(ch4);
    uint8_t wr = drawDialog(&ch_f,btns);
    if(!btns) in_tft->display(3);
    return wr;
  }







/*------------------------PRIVATE-----东西太多了,让我理清一下...------------------*/
private:
  EMW3 * in_tft;
  fs::FS * emw3_cnt_fs;
  //const char ** _text = nullptr;
  //void ** _intera = nullptr;
/*!
未被选中的选项
  |<---- width_max is the maximum of this width ---->|  这是 width_max 指定的宽度
      |<-------------- spr_width -------------->|  这是 spr_width 指定的宽度
  1---------------------------------------------------            ----------------
  |   2------------------------------------------    |  --------    ^ 这是
  |   |drawMulti display area                   |    | spr_height   | height_max
  |   -------------------------------------------    |  --------    v 指定的高度
  ----------------------------------------------------            ----------------

被选中的选项
  |<->|  这是 spr_offset_x 指定的宽度  spr_offset_x is equal to the width between these two lines
  1===================================================   -------- spr_offset_y is equal to the
  ||  2------------------------------------------   ||   -------- height between these two lines
  ||  |Selected drawMulti display area          |   ||            这是 spr_offset_y 指定的高度
  ||  -------------------------------------------   ||
  ====================================================
      1代表的点是 选项位置坐标, 2代表的点是 drawMulti 显示位置坐标
*/
  //设置Style参数
  //所有值为(参数为255) 时代表默认值
  int16_t width_max=200;    //每个选项最大宽度, 0为禁用
  int16_t height_max=20;   //每个选项最大高度, 0为默认, <0为禁用
  int16_t spr_width=-8;    //drawMulti 显示区域最大宽度, 0为默认值
  //此数值为负代表spr_width = width_max - 参数
  int16_t spr_height=-4;   //drawMulti 显示区域最大高度, 0为默认值
  //此数值为负代表spr_height = height_max - 参数
  int16_t spr_offset_x=4;  //drawMulti 显示坐标与选项位置坐标的偏移值 为负则保持默认
  int16_t spr_offset_y=2;  //drawMulti 显示坐标与选项位置坐标的偏移值 为负则保持默认
  int16_t border_w=0;           //边框左右宽度, 仅当使用了loadTheme 之后可用
  int16_t border_h=0;           //边框上下高度, 仅当使用了loadTheme 之后可用
  //int16_t title_width_max = -1;  标题最大宽度, 等于width_max
  int16_t title_height_max = 20; //标题最大高度, 0为未指定(默认值 = height_max), <0为禁用
  //int16_t title_spr_width=-8;    标题显示区域宽度, 等于spr_width
  int16_t title_spr_height=-4;   //标题显示区域高度, 0为未指定(使用默认值, 默认等于spr_height), 
  int16_t title_spr_offset_y = spr_offset_y; //标题显示区域偏移值, 
  int16_t title_Invert = 1;       //使能对标题反色
  //此数值为负代表spr_width = width_max - 参数
  //LGFX_Sprite borders[8];
  
  //是否在每个选项前使用图标
  uint16_t useGlobalIcon=0;
  const uint8_t** globalIcon = nullptr;
#ifdef LMV2_USE_SD_BMP_SUPPORT
  uint16_t useGlobalSDIcon=0;
  const char** globalSDIcon = nullptr;
  //SD卡图标缓存数据,此部分将会常驻内存, 所以不用的listMenuV2对象需要析构.
  //常驻内存需要580字节.
  const char* iconDataCacheLabel[16]={nullptr}; //字典索引
  uint8_t iconDataCache[16][32]; //默认缓存最近使用的16个图标,占用内存为512字节,未被缓存的图标数据将以常规方式绘制.
  uint32_t cacheUsage=0; //缓存使用情况
#endif
#ifdef LMV2_USE_SD_TXT_SUPPORT
  fs::File txtf;
#endif
  //是否使用drawMulti解锁更多功能
  bool useDrawMulti = 0;
#ifdef LMV2_USE_THEME_SUPPORT
  //theme 位图数据,应该,...
  LGFX_Sprite *styleData[8] = {nullptr};
  LGFX_Sprite *titleStyleData[8] = {nullptr};
#endif
  void (*listMenu_cb)(uint16_t,void *) = nullptr;
  void (*selectionList_cb)(uint8_t,void *) = nullptr;
  void (*slider_cb)(int32_t,void *) = nullptr;
  void *userdata = nullptr;
  void end_listMenu_settings_impl(uint8_t d);
  static String dispspecial[MAX_XV_LINE];
};
#endif