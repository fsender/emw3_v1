#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#define LGFX_USE_V1
#include "LovyanGFX.hpp"
#include "emw3.h"
#include "gb2312.h"
#include "listMenuV2.h"
EMW3 emw3;

listMenuV2 menu(&emw3);
const lgfx::U8g2font cn_font  ( chinese_city_gb2312  );

  const char *list_title = "Title 标题";
  const char *txt1 = "Sel1 Simple list";
  const char *character2 = "Sel2 Symbols IF|!@#{}[]:\";\'\\|";
  const char *unicode3 = "选项3 Unicode 是受支持的";
  const char *cn4 = "选项4 不简单的UTF8字体";
  const char *ltxt5 =
  "更多的选项5 和更长，更长，更长，更长，更卡，更长的字符串支持。";
  const char *ltxt6 =   "选项6 以下是空白的选项7";
  const char *empty7 = "";
  const char *bmptxt8 = 
  "\x02\x00\x00\x10\x10\x83\xaf\x03\x40\x9c\xaf\x1c\x00\x9c\x7f\x00\x00\xb7\x3b\x92\x10\xb2\x03\x12\x3a\xb2\x43\x00\x90\x80\xb2\x15\x82\x41\x44\x07\x38\xff选项8可以带有常量图标";
  const char *parambmp9 = "\x1e\x00\x00\x10\x10选项9可以带有图标参数";
  const char *SDbmp10 = "\x03\x00\x00\x20\020Busy0.bmp\xff选项10带有SD卡的图标文件"; // \020就是 \x10 为了防止字符冲突
  const char *anywhere11 = 
  "\x02\xed\x04\x10\x10\x83\xaf\x03\x40\x9c\xaf\x1c\x00\x9c\x7f\x00\x00\xb7\x3b\x92\x10\xb2\x03\x12\x3a\xb2\x43\x00\x90\x80\xb2\x15\x82\x41\x44\x07\x38\xff选项11图标画到任何地方(支持8位补码)";
  const char *multibmp12 = 
  "\x02\x14\x00\x10\x10\x83\xaf\x03\x40\x9c\xaf\x1c\x00\x9c\x7f\x00\x00\xb7\x3b\x92\x10\xb2\x03\x12\x3a\xb2\x43\x00\x90\x80\xb2\x15\x82\x41\x44\x07\x38\xff"
  "\x03\xef\x00\x20\020Busy0.bmp\xff"
  "\x1d\x00\x00\x10\x10" //参数里面的位图文件
  "               选项12带有多种不同的图标";
  const char *line13 = "\x05\xb9\x00\xc5\x0a选项13可以带有简单绘图功能";
  const char *drawmore14 = 
  "\x04\x00\x0f"         //像素点,    (0,15)
  "\x05\x00\x03\x0f\x0d" //线段,      (0,3,    15,13)
  "\x06\x10\x03\x0a\x05" //矩形,      (16,3,    10,5)
  "\x07\x20\x00\x05\x0a" //fill矩形,  (32,0,    5,10)
  "\x08\x30\x0f\x33\x00\x3d\x08" //三角,
  "\x09\x40\x0f\x43\x00\x4d\x08" //fill三角,
  "\x0a\x57\x07\x06\x06" //椭圆,
  "\x0b\x6d\x07\x0a\x04"; //fill椭圆,
  const char *bmpAndDraw15 = "\x02\x07\x00\x10\x10\x83\xaf\x03\x40\x9c\xaf\x1c\x00\x9c\x7f\x00\x00\xb7\x3b\x92\x10\xb2\x03\x12\x3a\xb2\x43\x00\x90\x80\xb2\x15\x82\x41\x44\x07\x38\xff"
  "\x0b\xaf\x07\x0a\x04" //fill椭圆,
  "选项15可以绘图+图标";
  const char *bold16 = "\x12\x0c\x01可以加粗选项16的字体";
  const char *centre17 = "\x0d\x05\x0e\x10\xff\xff选项17可以居中显示"; 
    // baseline_center 为 17, 需要同时设置显示坐标, 0x30为XY两个方向的居中模式, 0x10为X方向的居中模式
  const char *position18 = "\x0e\x00\x12\x11选项18可以更改文本显示坐标";//模式位为0x00,正常模式
  const char *features19 = 
  "\x02\x04\x00\x10\x10\x83\xaf\x03\x40\x9c\xaf\x1c\x00\x9c\x7f\x00\x00\xb7\x3b\x92\x10\xb2\x03\x12\x3a\xb2\x43\x00\x90\x80\xb2\x15\x82\x41\x44\x07\x38\xff"
  "\x0b\x9d\x07\x0a\x04" //fill椭圆,
  "\x0c\x01"
  "\x0d\x05"
  "\x0e\x04\x12\xff选项19=15+16+17+18"; //mode = 0x04 即x轴为从右往左计数
  const char *toggle20 = "\x10选项20带一个 Toggle 开关";
  const char *banned21 = "\x11选项21被禁用了,上面还有形状";
  const char *boolparam22 = "\x13选项22的 bool 参数值:";
  const char *intparam23 = "\x14选项23的 int 参数值:";
  const char *doubleparam24 = "\x17选项24的 double 参数值:";
  const char *bar25 = "\x14\x18\x26选项25有进度条:"; // \x18后面带有一个长度参数,单位是像素
  const char *minusbar26 = "\x19\x10选项26的进度条值是负的"; //后面带有一个长度参数,单位是像素
  const char *charparam27 = "\x1a 27的字符参数值:";
  const char *cstring28 = "\x1b 28的C字符串:";
  const char *invert29 = "\x01选项29可以反色";
  const char *string30 = "\x1c 30的String字符串:";
  const char *anything31 = 
  "\x02\x04\x00\x10\x10\x83\xaf\x03\x40\x9c\xaf\x1c\x00\x9c\x7f\x00\x00\xb7\x3b\x92\x10\xb2\x03\x12\x3a\xb2\x43\x00\x90\x80\xb2\x15\x82\x41\x44\x07\x38\xff"
  "\x0b\x9d\x07\x0a\x04" //fill椭圆,
  "\x1e\x12\x00\x10\x10" //参数图标,
  "\x0c\x01" //加粗
  "\x0d\x11\x0e\x10\xff\xff" //居中显示
  "选项31可以做所有事情:";

  const char *txtptr[32] ;
  const char *txtptr2[16];
  void *interactionPtr[32] = {nullptr};
const unsigned char test_bmp[32] = { 
  0xff,0x01,0x01,0x03,0x1d,0x05,0x5d,0x09,
  0x1d,0x1f,0x01,0x10,0xfd,0x17,0x01,0x10,
  0x1d,0x10,0xc1,0xff,0x59,0x80,0x42,0xc0,
  0x22,0x60,0x22,0x78,0x94,0x3f,0xf8,0x1f};
const char *test_bmp_file_path = "/bmp/bb4.bmp";
bool test_bool = true;
int32_t test_int = 2021;
double test_double = -0.4;
char test_char = 'A';
char test_cstring[8] = "Hello!";
String test_String = "World!";

void setup(){
  Serial.begin(115200);
  Serial.println("\nInit!");
  emw3.init(false);
  Serial.println("Display!");
  emw3.fillScreen(0xffff);
  emw3.setTextColor(0);
  emw3.print("Hello");
  Serial.println("Display 2!");
  emw3.display(2);
  Serial.println("Display 3!");
  //delay(1000);
  Serial.println(SD.begin(EMW3_SD_CS_PIN));
  pinMode(keyR,INPUT_PULLUP);
  //emw3.fillScreen(TFT_DARKCYAN);
  //emw3.drawBmpFile(SDFS,"/bmp/bb4.bmp",40,40);
  //Serial.println("load pic success1");
  //emw3.drawBmpFile(SDFS,"/EMW3/sys/icon/Busy0.bmp",80,40);
  //Serial.println("load pic success2");
  //emw3.display(3);
  //delay(1000);
  int16_t sizedata[12];
  sizedata[0]=220;    //width_max  最大宽度, 0为禁用
  sizedata[1] =20;   //height_max每个选项最大高度, 0为默认, <0为禁用
  sizedata[2] =-8;    //spr_width drawMulti 显示区域最大宽度, 0为默认值
  sizedata[3] =-4;   //spr_height drawMulti 显示区域最大高度, 0为默认值
  sizedata[4] =4;  //spr_offset_x drawMulti 显示坐标与选项位置坐标的偏移值 为负则保持默认
  sizedata[5] =2;  //spr_offset_y drawMulti 显示坐标与选项位置坐标的偏移值 为负则保持默认
  sizedata[6] =0;           //border_w边框左右宽度, 仅当使用了loadTheme 之后可用
  sizedata[7] =0;           //border_h边框上下高度, 仅当使用了loadTheme 之后可用
  sizedata[8]  = 22; //title_height_max标题最大高度, 0为未指定(默认值 = height_max), <0为禁用
  sizedata[9] =-4;   //title_spr_height标题显示区域高度, 0为未指定(使用默认值, 默认等于spr_height), 
  sizedata[10] = 4;   //title_spr_offset_y 标题显示区域偏移值, 
  sizedata[11]  = 1;       //titleInvert使能对标题反色
  menu.setSize(sizedata);
  /*
  emw3.setCursor(10,60);
  const char *bpath[3] = {
    "border1.bmp", "border2.bmp", "bo`rder3.bmp"
  };
  emw3.print(menu.setThemeFromFile(menu.themeNormal,3,bpath));
  const char *bpathTitle[3] = {
    "tborder1.bmp", "tborder2.bmp", "tborder3.bmp"
  };
  emw3.print(menu.setThemeFromFile(menu.themeTitle,3,bpathTitle));
  emw3.display(3);
  delay(1300);
*/
  txtptr[0] = list_title ;
  txtptr[1] = txt1 ;
  txtptr[2] = character2 ;
  txtptr[3] = unicode3 ;
  txtptr[4] = cn4 ;
  txtptr[5] = ltxt5;
  txtptr[6] = ltxt6;
  txtptr[7] = empty7;
  txtptr[8] = bmptxt8;
  txtptr[9] = parambmp9 ;
  txtptr[10] = SDbmp10 ;
  txtptr[11] = anywhere11 ;
  txtptr[12] = multibmp12 ;
  txtptr[13] = line13 ;
  txtptr[14] = drawmore14 ;
  txtptr[15] = bmpAndDraw15 ;
  txtptr[16] = bold16 ;
  txtptr[17] = centre17 ;
  txtptr[18] = position18 ;
  txtptr[19] = features19 ;
  txtptr[20] = toggle20 ;
  txtptr[21] = banned21 ;
  txtptr[22] = boolparam22 ;
  txtptr[23] = intparam23 ;
  txtptr[24] = doubleparam24 ;
  txtptr[25] = bar25 ;
  txtptr[26] = minusbar26 ;
  txtptr[27] = charparam27;
  txtptr[28] = cstring28;
  txtptr[29] = invert29;
  txtptr[30] = string30;
  txtptr[31] = anything31;
  interactionPtr[9] = (void *) test_bmp;
  interactionPtr[12] = (void *) test_bmp_file_path;
  interactionPtr[16] = &test_bool;
  interactionPtr[20] = &test_bool;
  interactionPtr[21] = &test_bool;
  interactionPtr[22] = &test_bool;
  interactionPtr[23] = &test_int;
  interactionPtr[24] = &test_double;
  interactionPtr[25] = &test_int;
  interactionPtr[26] = &test_double;
  interactionPtr[27] = &test_char;
  interactionPtr[28] = test_cstring;
  interactionPtr[30] = &test_String;
  interactionPtr[31] = (void *) test_bmp;
}

const char list_titlef[] PROGMEM = "标题 使用 PGMSPACE";
const char txt1f[] PROGMEM = "选项1";
const char character2f[] PROGMEM = "选项2";
const char unicode3f[] PROGMEM = "选项3";
const char cn4f[] PROGMEM = "选项4";
const char ltxt5f[] PROGMEM = "选项5";
const char ltxt6f[] PROGMEM = "选项6";
const char empty7f[] PROGMEM = "选项7";
const char bmptxt8f[] PROGMEM = "选项8";
const char parambmp9f[] PROGMEM = "选项9";
const char SDbmp10f[] PROGMEM = "选项10";
const char anywhere11f[] PROGMEM = "选项11";
const char multibmp12f[] PROGMEM = "选项12";
const char line13f[] PROGMEM = "选项13";
const char drawmore14f[] PROGMEM = "选项14";
const char bmpAndDraw15f[] PROGMEM = "选项15";
void loop(void)
{
  txtptr2[0] = list_titlef ;
  txtptr2[1] = txt1f ;
  txtptr2[2] = character2f ;
  txtptr2[3] = unicode3f ;
  txtptr2[4] = cn4f ;
  txtptr2[5] = ltxt5f;
  txtptr2[6] = ltxt6f;
  txtptr2[7] = empty7f;
  txtptr2[8] = bmptxt8f;
  txtptr2[9] = parambmp9f ;
  txtptr2[10] = SDbmp10f ;
  txtptr2[11] = anywhere11f ;
  txtptr2[12] = multibmp12f ;
  txtptr2[13] = line13f ;
  txtptr2[14] = drawmore14f ;
  txtptr2[15] = bmpAndDraw15f ;
  int res ;
  const char *txtptr3[2]={
    "从TXT文件导入数据标题","/txt/selec.txt"
  };
  emw3.fillScreen(0xffff);
  res = menu.listMenu(4,0,5,70,28,txtptr3);
  emw3.fillScreen(0xaaaa);
  emw3.setCursor(20,20);
  emw3.setTextColor(0);
  emw3.print(res);
  emw3.display(0);
  delay(1000);
  
  menu.setDrawMulti(1);
  res = menu.listMenu(4,0,5,31,20,txtptr,interactionPtr);
  emw3.fillScreen(0xaaaa);
  emw3.setCursor(20,20);
  emw3.setTextColor(0);
  emw3.print(res);
  emw3.display(0);
  delay(1000);
  menu.setDrawMulti(0);
   res = menu.listMenu(4,0,5,15,21,txtptr2);
  emw3.fillScreen(0xaaaa);
  emw3.setCursor(20,20);
  emw3.setTextColor(0);
  emw3.print(res);
  emw3.display(0);
  delay(1000);
   res = menu.listMenu(4,0,5,1,21,txtptr2);
  emw3.fillScreen(0xaaaa);
  emw3.setCursor(20,20);
  emw3.setTextColor(0);
  emw3.print(res);
  emw3.display(0);
  delay(1000);
   res = menu.listMenu(4,0,5,0,21,txtptr2);
  emw3.fillScreen(0xaaaa);
  emw3.setCursor(20,20);
  emw3.setTextColor(0);
  emw3.print(res);
  emw3.display(0);
  delay(1000);
}
