#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#define LGFX_USE_V1
#include "LovyanGFX.hpp"
#include "emw3.h"
#include "listMenuV2.h"
#include "emw3keyboard.h"
EMW3 emw3;

listMenuV2 menu(&emw3);
emw3Keyboard kbd(&emw3);
uint32_t chksum = 1234567890, chk0 = 0;
uint32_t batdat = 0;

extern const lgfx::U8g2font cn_font;
extern const lgfx::U8g2font osmall5_font;
const char icon1[] = "/EMW3/sys/icon/bmpf.bmp";
const char icon2[] = "/EMW3/sys/icon/emw3_2.bmp";
const char icon3[] = "/EMW3/sys/icon/keyboard.bmp";
const char icon4[] = "/EMW3/sys/icon/wifi.bmp";
const char *icons[] = {
  icon1,icon2,icon3,icon4,nullptr,icon4,icon1,icon3,icon2
};
const char *test_txt[17]={
  "标题数据","菜单项目1","支持很多功能","这很奇怪",
  //"\x03\x00\x00\x10\020icon1.bmp\xff选项10带有SD卡的图标文件",
  "选项10带有SD卡的图标文件",
  "亲自写出来的","过年啦不想写代码了","咋办","支持UTF-8编码"
  "标题德莉莎数据","菜单可莉项目1","支持优拉la很多功能","神里绫人凌华雷电将军八重神子",
  "亲自写黯色空壳出来的","过年啦不想写代码了","咋办,渊上,不凹分了","不支持GBK编码","空"
};
void setup(){
  ESP.rtcUserMemoryRead(0,&chksum,4);
  if(chksum == 1234567890){
    if(emw3.getBtn(EMW3_BtnM)==1) {
      emw3.init(false);
      emw3.setFont(&cn_font);
      emw3.setTextColor(0);
      loop();
    }
    chksum = 1234567891;
    ESP.rtcUserMemoryWrite(0,&chksum,4);
  }
  Serial.begin(115200);
  Serial.println("\nINIT!\n");
  emw3.init(false);
  emw3.setFont(&cn_font);
  Serial.println("START E-INK DISPLAY!\n");
  emw3.fillScreen(1);
  emw3.setTextColor(0);
  emw3.setCursor(0,0);
  emw3.print("测试-EMW3 Version 0.36");
  emw3.setCursor(50,45);
  emw3.print("Hello, 别来无恙啊!");
  emw3.setCursor(55,60);
  emw3.print("我是墨水时钟 EMW3,");
  emw3.setCursor(20,75);
  emw3.print("来自FriendshipEnder大神之手!");
  if(!SD.begin(EMW3_SD_CS_PIN)){
    Serial.println("SD NOT AVAILABLE!!\n");
    emw3.setCursor(60,90);
    emw3.print("SD卡未插入!!请检查");
  }
  else{
    Serial.println("SD AVAILABLE.\n");
    //emw3.setCursor(85,90);
    //emw3.print("SD卡已插入。");
  }
  
  emw3.display(2);
  delay(1000);
  /*
  emw3.fillScreen(1);
  emw3.drawBmpFile(SDFS,"/bmp/demo.bmp",0,0);
  emw3.display(2);
  delay(30000);*/
  
  char willPut[32] ="测试";
  Serial.print("On-Screen Keyboard Input: ");
  Serial.println(kbd.getstr(willPut,sizeof(willPut)-1,160,30));
  Serial.println(willPut);
  Serial.println();
  emw3.display(3);
  delay(1000);

  emw3.clearDisplay(1);
  int rtn = menu.slider("滑动滑动条滑条");
  emw3.setCursor(100,0);
  emw3.print(rtn);
  emw3.display(3);
  delay(1000);


  emw3.fillScreen(1);
  emw3.setCursor(0,0);
  emw3.print("灰度测试 Beta, 需要分段修改lut");
  
  const char * stg1="测试对话按钮";
  const char * stg2="确定按钮";
  const char * stg3="确(^_^)";
  const char * stg4="(-_-)消";
  const char * stg5="没事了";
  const char * stg[6] = {
    stg1,stg2,stg3,stg4,stg5
  };
    menu.selectionList(5,stg,96);
  /*
  emw3.drawBmpFile(SDFS,"/bmp/xiangqi1.bmp",0,0);
  for(int i=1;i<16;i++){
    emw3.setLut(1,i,16);
    emw3.fillRect((i<<4)-12,32,16,64,0);
    emw3.display(3);
  }
  delay(3000);
  emw3.fillScreen(1);
  emw3.display(2);
  */
  const char * msgbox1="测试对话框,神里绫人爱我哦!";
  const char msgbox2[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x80,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x03,0x01,0x00,0x00,0x00,0x01,0xe6,0x03,0x80,0x00,0x00,0x70,0x6c,0x02,0x80,0x00,0x00,0xd8,0xcf,0x86,0xc0,0x00,0x01,0x89,0x80,0x04,0x40,0x00,0x03,0x0d,0xe0,0x3c,0x78,0x00,0x02,0x04,0x01,0xe0,0x0f,0x00,0x06,0x04,0x03,0x02,0x81,0x80,0x04,0x06,0x01,0x80,0x03,0x00,0x0c,0x02,0x00,0xf3,0x9e,0x00,0x08,0x02,0x00,0x11,0x10,0x00,0x18,0x1b,0x00,0x30,0x18,0x00,0x10,0x39,0xc0,0x23,0x88,0x00,0x10,0x11,0x60,0x26,0xc8,0x00,0x30,0x01,0xa0,0x24,0x48,0x00,0x20,0x00,0xa0,0x2c,0x68,0x00,0x20,0x00,0xe0,0x38,0x38,0x00,0x20,0x00,0x40,0x10,0x10,0x00,0x20,0x00,0x60,0x00,0x00,0x00,0x20,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x30,0x00,0x00,0x00,0x20,0x00,0x18,0x00,0x00,0x00,0x20,0x01,0x8c,0x00,0x00,0x00,0x30,0x00,0xc6,0x00,0x00,0x00,0x10,0x00,0x73,0x00,0x00,0x00,0x10,0x00,0x1f,0x80,0x00,0x00,0x18,0x00,0x00,0xe0,0x00,0x00,0x08,0x00,0x00,0x38,0x00,0x00,0x0c,0x00,0x00,0x0e,0x00,0x00,0x04,0x00,0x00,0x57,0xc0,0x00,0x06,0x00,0x00,0xaa,0xf8,0x00,0x02,0x00,0x01,0x55,0x5f,0x00,0x03,0x00,0x0a,0xaa,0xab,0xc0,0x01,0x94,0x55,0x55,0x55,0x60,0x00,0xca,0xaa,0xaa,0xaa,0xe0,0x00,0x65,0x55,0x55,0x55,0xe0,0x00,0x32,0xaa,0xaa,0xaf,0xc0,0x00,0x1d,0x55,0x55,0x7f,0x80,0x00,0x0f,0xaa,0xab,0xff,0x00,0x00,0x03,0xf5,0x7f,0xfc,0x00,0x00,0x00,0xff,0xff,0xf0,0x00,0x00,0x00,0x3f,0xff,0xc0,0x00,0x00,0x00,0x03,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  const char * msgbox3="确(^_^)定";
  const char * msgbox4="取(-_-)消";
  const char * msgbox5="没事了";
  const char * msgbox[5] = {
    msgbox1,msgbox2,msgbox3,msgbox4,msgbox5,
  };
  menu.drawDialog(msgbox,192+7);
  
  menu.setSDIcon(9,icons);
  //menu.setDrawMulti(1);
  //menu.setSize(menu.titleInvert,0);
  uint8_t sel = menu.listMenu(10,0,5,16,4,test_txt);
  emw3.clearDisplay(1);
  emw3.setCursor(40,40);
  emw3.print(test_txt[sel]);
  emw3.display(2);
  delay(1000);
  chksum = 1234567890;
  ESP.rtcUserMemoryWrite(0,&chksum,4);
  ESP.rtcUserMemoryWrite(1,&batdat,4);
}
int lastbatv=0;
void loop(){
  ESP.rtcUserMemoryRead(1,&batdat,4);
  ESP.rtcUserMemoryRead(2,(uint32_t *)&lastbatv,4);
  emw3.fillScreen(1);
  emw3.setCursor(9,16);
  //emw3.fillScreen(0);
  //emw3.setTextColor(1);
  emw3.print(batdat);
  emw3.print(" 电池电压(V): ");
  pinMode(D4,OUTPUT);
  int batv = 0;
  for(int i=0;i<5;i++){
    delay(10);
    digitalWrite(D4,LOW);
    delayMicroseconds(8);
    batv += analogRead(A0);
    digitalWrite(D4,HIGH);
    //digitalWrite(D4,HIGH);
  }
  emw3.print(batv*4.3/5120.0,4);
  pinMode(D4,INPUT_PULLUP);
  if(batv != lastbatv) //emw3.displayWindow(80,8,24,240);
    emw3.display(3);
  lastbatv = batv;
  batdat ++;
  ESP.rtcUserMemoryWrite(1,&batdat,4);
  ESP.rtcUserMemoryWrite(2,(uint32_t *)&lastbatv,4);
  ESP.deepSleep(10000000);
}