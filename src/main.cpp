/********* FRIENDSHIPENDER *********
 * @file main.cpp
 * @author FriendshipEnder
 * @brief EMW3文件管理器, 文件应该是可以被索引到的
 * @version 1.0
 */
#include "emw3.h"
#include "listMenuV2.h"
#include "emw3keyboard.h"
#include "EMW3App.h"

#define TIME_ADJUST 15 //表明多少分钟校准一次, 最好可以被60整除

EMW3 emw3;
listMenuV2 menu(&emw3);
EMW3App app(emw3,menu);
emw3Keyboard kbd(&emw3);

uint8_t cmb [4] = {0};
const char * weekchar[]={
  "","天","一","二","三","四","五","六",
};

void setup(){
  //serial.begin(74880);
    if(emw3.getBtn(keyM) == 0){
      emw3.init();
      app.init();
      ESP.rtcUserMemoryWrite(32,(uint32_t *) &cmb,4);
      for(;;) {
        app.settingPage();
        yield();
      }
    }
  app.init(1);//初始化模块, 但是是快速初始化, 不初始化app列表和WiFi
  app.setAppNameManual("简易电子时钟");
  if(app.getSetting(EMW3_WAKEUP_FROM_DEEPSLEEP)){
    emw3.init(2);
    app.readParameter(cmb);
    if((cmb[2]%TIME_ADJUST)!=(TIME_ADJUST-1)){ //-------------每15分钟校正一次
      //serial.println("Wake from sleep!");
      cmb[2]++;
      setTime(cmb[1],cmb[2],cmb[3],day(),month(),year());
      //serial.printf("cmb[2]: %d, div: %d\n",cmb[2],((cmb[2]%TIME_ADJUST)==(TIME_ADJUST-1))?48000:60000);
    
      emw3.clearDisplay(1);
      emw3.setFont(&fonts::Font7);
      emw3.setCursor(16,24);
      emw3.printf("%02d:%02d",hour(),minute());
      //serial.printf("Wake: %02d:%02d:%02d\n",hour(),minute(),second());
      emw3.setCursor(16,80);
      emw3.setFont(&cn_font);
      emw3.printf("%d 年 %d 月 %d 日, 星期%s",year(),month(),day(),weekchar[weekday()]);
      //serial.printf("%d 年 %d 月 %d 日, 星期%s",year(),month(),day(),weekchar[weekday()]);
      emw3.setCursor(16,100);
      emw3.setFont(&osmall5_font);
      emw3.print("First EMW3 application. By FriendshipEnder BETA V0.08");
      emw3.setCursor(16,110);
      emw3.print("Build: ");
      emw3.print(__DATE__);
      emw3.print(" ");
      emw3.print(__TIME__);
      app.drawStatusBar(0);
      emw3.display(3);
      cmb[0] = 0;
      cmb[1] = hour();
      cmb[2] = minute();
      cmb[3] = second();

      app.deepSleep((((cmb[2]%TIME_ADJUST)==(TIME_ADJUST-1))?55000:60000)-millis(),0,cmb,4);
    }
    else{
      app.init(1);//初始化模块
      app.setAppNameManual("简易电子时钟");
      WiFi.mode(WIFI_STA);
      WiFi.begin();
      while(!WiFi.isConnected()) {
        yield();
      }
      setTime(app.wifiSettings().getNTPTime());
      WiFi.mode(WIFI_OFF);
      while(minute()==cmb[2]) yield();
    }
  }
  else{
    const char *d_fail[4] = {
      "欢迎使用低功耗智能表\n走时中如需调用菜单栏,\n请按住中间键按复位,\n直到闪屏之后松开中间键\n按确定键自动连接到WiFi",
      "info.bmp","连接失败","warn.bmp"
    };
  //emw3.begin();
    emw3.init();
    app.init();//初始化模块
    app.setAppNameManual("简易电子时钟");
    emw3.setFont(&cn_font);
  //emw3.setCursor(16,12);
  //emw3.print(F("简单的可休眠时钟"));
    menu.drawDialog(d_fail,33);
    if(app.connectToWiFi(1)){
      menu.drawDialog(d_fail+2,32);
      delay(2000);
      while(app.connectToWiFi(0)){
        menu.drawDialog(d_fail+2,32);
        delay(2000);
      }
    }
  }
}
void loop(){
  int tsec = second();
  while((tsec = second())>57) yield();
  emw3.clearDisplay(1);
  emw3.setFont(&fonts::Font7);
  emw3.setCursor(16,24);
  emw3.printf("%02d:%02d",hour(),minute());
  //serial.printf("%02d:%02d:%02d\n",hour(),minute(),tsec);
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
  app.drawStatusBar(0);
  emw3.display(7);
  emw3.display(3);
  cmb[0] = 0;
  cmb[1] = hour();
  cmb[2] = minute();
  cmb[3] = 0;
  app.deepSleep(58500-tsec*1000,0,cmb,4);
}