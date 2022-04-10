/********* FRIENDSHIPENDER *********
 * @file main.cpp
 * @author FriendshipEnder
 * @brief EMW3基础示例程序
 * 
 * @version 1.0.0
 * 正式发布SDK工具包
 */
#include "emw3.h"
#include "listMenuV2.h"
#include "emw3keyboard.h"
#include "EMW3App.h"
EMW3 emw3;
listMenuV2 menu(&emw3);
EMW3App app(emw3,menu);
emw3Keyboard kbd(&emw3);

/** @brief 连接到WiFi
 *  @param settingScanRes 设定是否自动连接已存WiFi, 自动删除连接结果
 *  0:不自动删除,不自动连接
 *  1:自动删除,不自动连接
 *  2,3:自动连接并删除
 *  @return String 
 */
const char * weekchar[]={
  "","天","一","二","三","四","五","六",
};
  char strg[24] = "";
void setup(){
  Serial.begin(115200);
  if(!emw3.begin()) Serial.println("SD NOT AVAILABLE!");
  app.init();//初始化模块
  Serial.print("app.getSettings():");
  Serial.println(app.getSettings(),BIN);
  Serial.print("app.getAppName():");
  Serial.println(app.getAppName());
  Serial.print("app.getAppPath():");
  Serial.println(app.getAppPath());
  Serial.print("app.loadParameter():");
  app.readParameter((uint8_t *)strg);
  Serial.println(strg);
  emw3.drawString(strg,0,0);
  if(app.getAppID() == 0){
    strcpy(strg,"parameter EPDreader");
  }
  else{
    strcpy(strg,"parameter MP3Player");
  }
  app.writeParameter((uint8_t *)strg,24);
  Serial.print("app.loadParameter after write():");
  app.readParameter((uint8_t *)strg);
  Serial.println(strg);
  app.wifiSettings().connectToWiFi();
  Serial.print("app.getSettings():");
  Serial.println(app.getSettings(),BIN);
  delay(1000);
  emw3.clearDisplay(1);
  const char *txt = "正在从NTP校准时间...";
  menu.drawDialog(&txt,0);
  emw3.display(3);
  //setTime(12,34,56,1970,1,1);
  setTime(app.getNTPTime());
}
void loop(){
  int lsecond = 0;
  int lminute = 0;
  emw3.clearDisplay(1);
  emw3.setFont(&fonts::Font7);
  emw3.setCursor(16,24);
  emw3.printf("%02d:%02d:%02d",hour(),(lminute = minute()),(lsecond = second()));
  emw3.setCursor(16,80);
  emw3.setFont(&cn_font);
  emw3.printf("%d 年 %d 月 %d 日, 星期%s",year(),month(),day(),weekchar[weekday()]);
  emw3.setCursor(16,100);
  emw3.setFont(&osmall5_font);
  emw3.print("First EMW3 application. By FriendshipEnder BETA V0.08");
  emw3.setCursor(16,110);
  emw3.print("Build: ");
  emw3.print(__DATE__);
  emw3.print(" ");
  emw3.print(__TIME__);
  app.drawStatusBar(1);
  emw3.display(3);
  while(second() == lsecond) {
    yield();
    if(emw3.getBtn(keyR) == 0) {
      app.drawStatusBar(8);
    }
  }
}
