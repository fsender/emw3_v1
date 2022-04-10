/********* FRIENDSHIPENDER *********
 * @file main.cpp
 * @author FriendshipEnder
 * @brief EMW3基础示例程序
 * 此模块被设计为不适合经常频繁重启
 * @version 1.0
 */
#include "emw3.h"
#include "listMenuV2.h"
#include "emw3keyboard.h"
#include "EMW3App.h"
EMW3 emw3;
listMenuV2 menu(&emw3);
EMW3App app(emw3,menu);
emw3Keyboard kbd(&emw3);

void setup(){
  Serial.begin(115200);
  if(!emw3.begin()) Serial.println("SD NOT AVAILABLE!");
  app.init();//初始化模块
  emw3.setFont(&cn_font);
}
void loop(){
  emw3.clearDisplay(1);
  emw3.drawString("Hello World! 你好, 世界, 你好,EMW3",16,16);
  emw3.display(3);
  while(1){
    if(emw3.getBtn(keyR) == 0) {
      app.drawStatusBar(8);
      break;
    }
    yield();
  }
}