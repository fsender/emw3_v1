/**
 * @file listMenuV2.h
 * @author FriendshipEnder (Q:3253342798)
 * @brief 菜单显示程序,可以提供一个功能丰富的菜单功能,仅使用于EMW3
 * @version beta 1.0
 * @date 2022-02-03
 * @copyright Copyright (c) 2022 
 */
#ifndef _LISTMENU_V2_H
#define _LISTMENU_V2_H
//注意事项: 使用PCtoLCD2002取模时,需要阴码+顺向+C51输出
#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "emw3.h"
#include "emw3_defines.h"
#include "gb2312.h"
//功能支持
#define LMV2_USE_DRAWMULYI_CANVAS_SUPPORT
//#define LMV2_USE_THEME_SUPPORT
#define LMV2_USE_SD_BMP_SUPPORT
#define LMV2_USE_SD_TXT_SUPPORT
#define _EMW3_ICON_PATH "/EMW3/sys/icon/"
#define _EMW3_THEME_PATH "/EMW3/sys/theme/" // E:\EMW3\sys\theme/border1.bmp
  //0 plain   常规文本
  //1 printf plain 支持 printf,只能带一个参数
  //2 可以反色
  //4 可以带 toggle 按钮和数值条,也可以禁用某个选项
//#define LMV2_USE_ADVANCED_PLAIN_SUPPORT

//系统常量
#define DEBOUNCE_DELAY_MS 25
#define LONGPRESS_DELAY_MS 500
//extern LGFX tft;
extern const lgfx::U8g2font cityFont;

class listMenuV2{
  public:
    listMenuV2(EMW3 *intft = nullptr) : in_tft(intft) {}
    listMenuV2(EMW3 *intft, const char **text, void **interaction = nullptr);
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
    uint8_t setThemeFromFile(themeselection themesel,int16_t styleBmps, const char **bmpFilePath, fs::FS Fs = SDFS);
#endif
#endif
/** @brief 显示一个多级菜单
 * @param x 显示坐标X
 * @param y 显示坐标Y
 * @param hlines 菜单行数,不含标题行
 * @param selections 菜单选项数
 * @param settings 菜单设置
 * bit 0 : 是否对 text 使用 progmem
 * bit 1 : 是否绘制标题栏 ,(形参会在内部更改) 0 绘制   1 不绘制
 * bit 2 : 是否显示模拟滑动指示条
 * bit 3 : 是否使用SD卡上的txt文件来显示内容
 * 如 bit 3 为1,则参数 text将会只读取text[0]作为标题, text[1]作为文件路径
 * 若此位为0且iconsfilepath不为nullptr 显示iconsfilepath对应的图标
 * 若此位为1且iconsfilepath不为nullptr 显示文件/文件夹图标
 * bit 4 : 滑动指示条宽度
 * 
 * @param title 菜单标题
 * @param text 每一行的内容 支持 drawMulti 函数的 单行内容指令 详见 drawMulti 函数
 * @param interactions 交互选项, 规则比较复杂
 * @return uint16_t 选择的条目编号, 1为第一条消息, 0为退出
 */
  uint16_t listMenu(int16_t x,int16_t y,uint8_t hlines,int16_t numitem,uint8_t settings,
    const char **text, void ** interactions = nullptr);
/// @brief 内部调用, 绘制菜单的GUI
  void listMenuGUI(int16_t x,int16_t y,int16_t w,uint8_t hlines,uint8_t settings,
    const char **text, void ** interactions = nullptr, const char ** bmpsrc = nullptr);
/// @brief 绘制圆角矩形,圆角半径固定为2
  void themeRect(themeselection themesel, int16_t x,int16_t y,int16_t w,int16_t h,uint16_t color);
/// @brief 绘制填充圆角矩形,圆角半径固定为2, 只用于标题 (已经被 themeRect() 取代)
//   void fillArcRect(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t color);
/// @brief 绘制返回对话框
  void drawBackturn(int16_t x,int16_t y);
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
 若不以'/'开头则为相对路径(/EMW3/sys/icon), 否则为绝对路径, 数据段以\xff结尾.
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
#endif

private:
  EMW3 *in_tft;
  const char ** _text = nullptr;
  void ** _intera = nullptr;
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
#endif
#ifdef LMV2_USE_SD_TXT_SUPPORT
  fs::File txtf;
#endif
  //是否使用drawMulti解锁更多功能
  uint16_t useDrawMulti = 0;
#ifdef LMV2_USE_THEME_SUPPORT
  //theme 位图数据,应该,...
  LGFX_Sprite *styleData[8] = {nullptr};
  LGFX_Sprite *titleStyleData[8] = {nullptr};
#endif
};
#endif