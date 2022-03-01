/**
 * @file emw3keyboard.cpp
 * @author fsender (Bilibili FriendshipEnder)
 * @brief 虚拟全键盘输入
 * @version 0.1
 * @date 2022-02-14
 */
#include "emw3keyboard.h"

int emw3Keyboard::getchs(char * inString, int maxlen, int x , int y , bool multiline){
  LGFX_Sprite dest(in_tft);
  dest.setColorDepth(1);
  dest.setFont(&cn_font);
  in_tft->drawRect(x,y,87,17,0);
  in_tft->fillRect(x+1,y+1,85,15,1);
  dest.createSprite(85,13);
  return getchs(dest,inString,maxlen,x,y+18,x+1,y+2,multiline);
}
int emw3Keyboard::getchs(LGFX_Sprite dest, char * inString, int maxlen, 
  int x, int y, int destx, int desty, bool multiline){
  //LGFX_Sprite dest (in_tft);
  uint8_t panel = 0; //选择键盘面板
  uint8_t cur = 0; //选择光标
  uint8_t dispUpdate = 3; //屏幕刷新标志
  int strptr = strlen(inString); //指向最后一个字符后面的\0字符
  auto inFont = in_tft->getFont(); //备份设置
  auto inDatum = in_tft->getTextDatum();
  in_tft->setDepth(11);
  in_tft->setFont(&fonts::Font0); //还原设置
  in_tft->setTextDatum(TL_DATUM);
  //in_tft->fillRect(x+1,y+1,85,52,1); //白色矩形作为工作区
  //in_tft->drawRect(x,y,87,54,0); //边框
  dest.setTextColor(0);
  destDraw(dest,inString,destx,desty);
  for(;;){
    if(in_tft->getBtn(EMW3_BtnL)==0){
      if(cur>=30) cur -=30;
      else cur+=10;
      dispUpdate = 1;
    }
    else if(in_tft->getBtn(EMW3_BtnR)==0){
      if(cur%10==9) cur-=9;
      else cur++;
      dispUpdate = 2;
    }
    else if(in_tft->getBtn(EMW3_BtnM)==0){
      char selected = keys[panel][cur];
      if(selected == 0x00){ //回车符,单行显示为结束输入
        in_tft->display(3);
        break;
      }
      else if(selected == 0x01){ //更换键盘/输入法, 在更新中文输入前, 同更换字符面板
        panel = (panel+1)%3;
        dispUpdate = 3;
      }
      else if(selected == 0x02){ //退格
        if(strptr) {
          do strptr--;
          while(inString[strptr]>=0x80 && inString[strptr]<0xc0);
          inString[strptr] = '\0';
          dispUpdate = 4;
        }
      }
      else if(strptr<maxlen){ //写入字符
        inString[strptr] = selected;
        strptr++;
        inString[strptr] = '\0';
        dispUpdate = 4;
      }
    }
    if(dispUpdate){
      switch(dispUpdate){
        case 3: //case 3 需要执行case 1的内容
        in_tft->setColor(0);
        in_tft->fillRect(x+1,y+1,85,56,1); //白色矩形作为工作区
        in_tft->drawRect(x,y,87,58,0); //边框
          for(int i=0;i<40;i++){
            if(i == 39 || i == 29) {
              int iy = i == 29?32:45;
              in_tft->drawFastVLine(x+77,y+iy,2);
              in_tft->drawFastVLine(x+78,y+iy-1,4);
              in_tft->drawFastVLine(x+79,y+iy-2,6);
              in_tft->drawFastVLine(x+80,y+iy,2);
              if(i == 39) in_tft->drawFastVLine(x+81,y+41,6);
              else in_tft->drawFastVLine(x+81,y+32,2);
            }
            else if(i == 30){
              ;
            }
            else in_tft->drawChar(keys[panel][i],5+8*(i%10)+x,5+12*(i/10)+y);
          }
        case 1: {
          int tx = (cur%10)*8;
          int curv = cur/10;
          in_tft->drawRect(x+3+tx,y+2+(curv>=1?curv-1:curv+3)*12,9,14,1);
          in_tft->drawRect(x+3+tx,y+2+                 curv*12,9,14,0);
        } 
        break;
        case 2: {
          int ty = y+2+(cur/10)*12;
          in_tft->drawRect(x+3+((cur+9)%10)*8,ty,9,14,1);
          in_tft->drawRect(x+3+    (cur%10)*8,ty,9,14,0);
        } 
        break;
        case 4: destDraw(dest,inString,destx,desty);
        break;
      }
      in_tft->display(3);
      dispUpdate = 0;
    }
    yield();
  }
  in_tft->setFont(inFont); //还原设置
  in_tft->setTextDatum(inDatum);
  in_tft->setDepth(14); //初始值14.
  return strptr;
}
void emw3Keyboard::destDraw(LGFX_Sprite dest, const char * inString, int dx, int dy){
          dest.fillScreen(1);
          if(dest.textWidth(inString)>dest.width()-2){
            dest.setTextDatum(top_right);
            dest.drawString(inString,dest.width()-2,0);
            dest.setTextDatum(top_left);
            dest.fillRect(dest.width()-2,0,2,dest.height());
          }
          else {
            dest.drawString(inString,1,0);
            dest.fillRect(dest.textWidth(inString)+1,0,2,dest.height());
          }
          dest.pushSprite(dx,dy);
}