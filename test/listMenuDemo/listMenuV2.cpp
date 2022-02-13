#include "listMenuV2.h"

listMenuV2::listMenuV2(EMW3 *intft, const char **text, void **interaction) : in_tft(intft) {
  if(text == nullptr) return;
  _text = text;
  if(interaction != nullptr) _intera = interaction;
}
listMenuV2::~listMenuV2(){
#ifdef LMV2_USE_THEME_SUPPORT
  for(int i=0;i<8;i++)
    if(styleData[i] != nullptr) {
      styleData[i]->deleteSprite();
      delete styleData[i];
    }
  for(int i=0;i<8;i++)
    if(titleStyleData[i] != nullptr) {
      titleStyleData[i]->deleteSprite();
      delete titleStyleData[i];
    }
#endif
}
uint16_t listMenuV2::listMenu(int16_t x,int16_t y,uint8_t hlines,int16_t numitem,uint8_t settings,
  const char **text, void ** interactions ){
    Serial.println("listMenu fx");
  uint8_t long_pressed=0; //长按检测标签
  uint8_t cursor_changed=0; //标记 cursor 是否改变过, 
  // 0 无改变  1 向上   2 向下    >3: 需要刷新整个画面
  uint8_t initial = 1;  //初始化阶段此数值为1
  uint32_t refreshFlag = 0; //刷新检测
  uint32_t beginPress = 0; //标记开始按下 BtnM 的时间
  uint32_t pressMillis = 0; //标记开始按下 BtnL 或 BtnR 的时间
  int16_t selected = 1;
  int16_t cursor = hlines>2?2:1; //光标在第几个的位置上,当 hlines>=3 时,cursor只能为 2 到 hlines-1之间.
  int16_t cursor2 = 0; //当光标在最上面时,正好选择的是第一个选项,则此数值为0
  //hlines |= 1;
  int16_t h = height_max*hlines+title_height_max;
  /* uint8_t enable_pgm = 0;
  if(settings&8) enable_pgm = 8;      //SD卡
  else if(settings&1) enable_pgm = 1; //pgm
  uint8_t dispbar = settings&4;       //显示右侧进度条
  */
  const char *disp[32] = {nullptr};
  String dispspecial[32]={};
  void *intera[32] = {nullptr};
  if(settings&1) {
    dispspecial[0] = FPSTR(text[0]);
    disp[0] = dispspecial[0].c_str();
  }
  else disp[0] = text[0];
#ifdef LMV2_USE_SD_TXT_SUPPORT
  if(settings&8) {
    if(settings&1) {
      dispspecial[1] = FPSTR(text[1]);
      txtf = SD.open(dispspecial[1]);
    }
    else txtf = SD.open(text[1]);
    if(!txtf) return 0;
    else useDrawMulti = 0;  //使用SD卡文件时, 不能使用useDrawMulti
  }
#endif
  if(interactions != nullptr) intera[0] = interactions[0];
  while(in_tft->getBtn(EMW3_BtnM)==0) yield();
  if(numitem==0){ //无条目
    const char msg[] = "\0370个选项";
    if(cursor==2) disp[1] = msg;
    listMenuGUI(x,y,width_max,hlines,settings,disp,nullptr);
    themeRect(themeNormal, x,y+height_max*(cursor-1)+title_height_max,width_max,height_max,0);
    in_tft->display(3);
    while(in_tft->getBtn(EMW3_BtnM)==1) yield();
    delay(DEBOUNCE_DELAY_MS);
    while(in_tft->getBtn(EMW3_BtnM)==0) yield();
    delay(DEBOUNCE_DELAY_MS);
    return 0;
  }
  else if(numitem==1){ //仅有1条消息
    const char msg[] = "\0371个选项";
    if(settings&1) {
      dispspecial[1] = FPSTR(text[1]);
      disp[cursor] = dispspecial[1].c_str();
    }
#ifdef LMV2_USE_SD_TXT_SUPPORT
    else if(settings&8){
      dispspecial[1] = txtf.readStringUntil('\n');
      disp[cursor] = dispspecial[1].c_str();
    }
#endif
    else disp[cursor] = text[1];
    if(interactions != nullptr) intera[cursor] = interactions[1];
    if(cursor==2) disp[1] = msg;
    listMenuGUI(x,y,width_max,hlines,settings,disp,intera);
    themeRect(themeNormal, x,y+height_max*(cursor-1)+title_height_max,width_max,height_max,0);
    in_tft->display(3);
    while(in_tft->getBtn(EMW3_BtnM)==1) yield();
    delay(DEBOUNCE_DELAY_MS);
    while(in_tft->getBtn(EMW3_BtnM)==0) {
      if(!beginPress) {
        beginPress=millis();
        delay(DEBOUNCE_DELAY_MS);
      }
      if(millis()-beginPress>LONGPRESS_DELAY_MS){
        drawBackturn(x+width_max/2,y+h/2);
        while(in_tft->getBtn(EMW3_BtnM)==0) yield();
        delay(DEBOUNCE_DELAY_MS);
        return 0;
      }
      yield();
    }
    delay(DEBOUNCE_DELAY_MS);
    return 1;
  }
  for(;;){
    if((pressMillis = millis()) > refreshFlag){
      if(!in_tft->getBtn(EMW3_BtnL)){
        if(selected <= 1) {
          selected = numitem;
          cursor=(hlines>2?hlines-1:hlines);
          cursor2=numitem-hlines+(hlines>2?2:0);
          cursor_changed = 3;
        }
        else {
          selected --;
          if(cursor>(hlines>2?2:1)) {
            cursor--;
            cursor_changed=1;
          }
          else cursor2--;
        }
        refreshFlag=0;
      }
      else if(!in_tft->getBtn(EMW3_BtnR)){
        if(selected >= numitem) {
          selected = 1;
          cursor_changed = 4;
          cursor = (hlines>2?2:1);
          cursor2=0;
        }
        else {
          selected ++;
          if(cursor<(hlines>2?hlines-1:hlines)) {
            cursor++;
            cursor_changed=2;
          }
          else cursor2++;
        }
        refreshFlag=0;
      }
    }
    while(in_tft->getBtn(EMW3_BtnM)==0) {
      if(!beginPress) {
        beginPress=millis();
        if(interactions != nullptr && interactions[selected] != nullptr && useDrawMulti){
          char readFrom;
          if(settings&1) readFrom = pgm_read_byte(text[selected]);
          else readFrom = text[selected][0];
          if(readFrom=='\x11') //如果text首字节为0x11, 则interaction为true时此选项不可选
            long_pressed = !(*(const bool*)(interactions[selected]));//long_pressed == 1表示可被选择
          else if(readFrom=='\x12') //如果text首字节为0x12, 则interaction为false时此选项不可选
            long_pressed = (*(const bool*)(interactions[selected]));//long_pressed == 1表示可被选择
          else if(readFrom=='\x10'){ //toggle按钮 此功能需要保证 interactions[selected] 可写入
            (*(bool*)(interactions[selected])) = !(*(bool*)(interactions[selected])); //按下时候取反
            long_pressed = 0;
            refreshFlag = 0;
            cursor_changed = 5;//下一帧更新画面
          }
          else long_pressed = 1; //当 long_pressed == 1或2时退出循环,则 ListMenu 将退出并返回选定的值
        }
        else long_pressed = 1;
        delay(DEBOUNCE_DELAY_MS);
      }
      if(millis()-beginPress>LONGPRESS_DELAY_MS){
        drawBackturn(x+width_max/2,y+h/2);
        while(in_tft->getBtn(EMW3_BtnM)==0) yield();
        delay(DEBOUNCE_DELAY_MS);
        long_pressed = 2;
        break;
      }
      yield();
    }
    if(long_pressed) break;
    else if(beginPress){
        beginPress=0;
        delay(DEBOUNCE_DELAY_MS);
        continue;
    }
    if((!refreshFlag) || initial){
      char itemsFlag[16];
      sprintf_P(itemsFlag,PSTR("\x1f%d个选项"),numitem);
#ifdef LMV2_USE_SD_TXT_SUPPORT
      if(settings&8){
        int nl=0;
        txtf.seek(0);
        while(nl<cursor2-(hlines>2)){
          int rd = txtf.read();
          if(rd == EOF) break;
          nl += (rd=='\n');
        }
      }
#endif
      for(int i=0;i<hlines;i++){
        int prindex = cursor2 + i + (hlines<=2);
        if(prindex>0 && prindex<=numitem){
          if(settings&1) {
            dispspecial[i+1] = FPSTR(text[prindex]);
            disp[i+1] = dispspecial[i+1].c_str();
          }
#ifdef LMV2_USE_SD_TXT_SUPPORT
          else if(settings&8){
            dispspecial[i+1] = txtf.readStringUntil('\n');
            disp[i+1] = dispspecial[i+1].c_str();
          }
#endif
          else disp[i+1] = text[prindex];
          if(interactions != nullptr) intera[i+1] = interactions[prindex];
        }
        else {
          disp[i+1]=nullptr;
          intera[i+1] = nullptr;
        }
      }
      if(disp[1] == nullptr) disp[1] = itemsFlag;
      else if(disp[hlines] == nullptr) disp[hlines] = itemsFlag;
      if(initial) cursor_changed+=8;
      switch(cursor_changed) {
        case 1:  themeRect(themeNormal, x,y+height_max*cursor+title_height_max,width_max,height_max,0xffff); break;
        case 2:  themeRect(themeNormal, x,y+height_max*(cursor-2)+title_height_max,width_max,height_max,0xffff); break;
        default: listMenuGUI(x,y,width_max,hlines,settings,disp,intera);
        if(settings&4){
          int barLength = hlines*(height_max*hlines+title_height_max-4)/(numitem+2);
          if(barLength<1) barLength=1;
          int barwhere = cursor2*(height_max*hlines+title_height_max-4-barLength)/(numitem+2-hlines);
          in_tft->fillRect(x+width_max+2,y+1,(settings&16)?2:6,height_max*hlines+title_height_max-2,0xffff);
          if(numitem+2>hlines) in_tft->fillRect(x+width_max+((settings&16)?2:3),y+2+barwhere,
            (settings&16)?2:4,barLength,0x0);
        }
      }
      themeRect(themeNormal, x,y+height_max*(cursor-1)+title_height_max,width_max,height_max,0);
      refreshFlag = pressMillis+250;
      while(in_tft->epdBusy()) yield();
      in_tft->display(1);
      cursor_changed=0;
      if(initial){
        settings |= 2; //之后不再绘制标题栏
        initial=0;
      }
    }
    yield();
  }
  if(long_pressed==2) selected = 0;
  delay(DEBOUNCE_DELAY_MS);
#ifdef LMV2_USE_SD_TXT_SUPPORT
  if(settings & 8) txtf.close();
#endif
  return selected;
}

void listMenuV2::listMenuGUI(int16_t x,int16_t y, int16_t w, uint8_t hlines,uint8_t settings,
  const char **text, void ** interactions ){
    Serial.println("listMenuGUI fx");
  //themeRect(x,y+20*(hlines/2+1),w,20,0);
  LGFX_Sprite printspr(in_tft);
  printspr.setColorDepth(1);
  printspr.setFont(&cityFont);
  int16_t real_spr_width = spr_width,real_spr_height;
  if(real_spr_width<0) real_spr_width+=w;
  if(real_spr_width>w) real_spr_width=w;
  if(!(settings&2) && title_height_max>0) {
    themeRect(themeTitle,x,y,w,title_height_max-(hlines <= 2),0);
    real_spr_height=title_spr_height;
    if(real_spr_height<0) real_spr_height+=title_height_max;
    if(real_spr_height>title_height_max) real_spr_height=title_height_max;
    real_spr_height-=(hlines <= 2);
    //绘制标题
    printspr.createSprite(real_spr_width,real_spr_height);
    if(text[0] != nullptr) {
#ifdef LMV2_USE_DRAWMULYI_CANVAS_SUPPORT
      if(useDrawMulti) 
        drawMulti(printspr,text[0],(interactions ==nullptr)?nullptr:interactions[0]);
      else 
#endif
        drawPlain(printspr,text[0],(interactions ==nullptr)?nullptr:interactions[0]);
      //printspr.pushSprite(x+spr_offset_x,y+spr_offset_y);
      in_tft->drawBitmap(x+spr_offset_x,y+spr_offset_y,
      (const uint8_t *)printspr.getBuffer(),printspr.width(),printspr.height(),
      title_Invert?(uint16_t)0:(uint16_t)65535,title_Invert?(uint16_t)65535:(uint16_t)0);
    }
    printspr.deleteSprite();
  }
  //绘制标题以外的东西
  real_spr_height = spr_height;
  if(real_spr_height<0) real_spr_height+=height_max;
  if(real_spr_height>height_max) real_spr_height=height_max;
  in_tft->fillRect(x,y+title_height_max,w,height_max*hlines,0xffff);
  printspr.createSprite(real_spr_width,real_spr_height);
  for(int i=1;i<=hlines;i++){
    if(text[i] != nullptr) {
      if(text[i][0] == '\x1f'){
        printspr.setTextDatum(middle_left);
        printspr.setPaletteColor(0,0x0);
        printspr.setPaletteColor(1,0xffff);
        printspr.fillSprite(1);
        uint16_t twidth = printspr.textWidth(text[i]+1);
        printspr.fillRect(w/2-twidth/2-8,0,twidth+8,printspr.height(),0);
        printspr.setTextColor(1);
        printspr.drawString(text[i]+1,w/2-twidth/2-4,(printspr.height()-1)/2);
      }
      else {
#ifdef LMV2_USE_DRAWMULYI_CANVAS_SUPPORT
        if(useDrawMulti) 
          drawMulti(printspr,text[i],(interactions ==nullptr)?nullptr:interactions[i]);
        else 
#endif
          drawPlain(printspr,text[i],(interactions ==nullptr)?nullptr:interactions[i]);
      }
      printspr.pushSprite(x+spr_offset_x,y+(i-1)*height_max+title_height_max+spr_offset_y);
      //in_tft->drawBitmap(x+spr_offset_x,y+(i-1)*height_max+title_height_max+spr_offset_y,
      //(const uint8_t *)printspr.getBuffer(),printspr.width(),printspr.height(),(uint16_t)0,(uint16_t)65535);
    }
  }
  if(settings&4) in_tft->drawRect(x+w+1,y,(settings&16)?4:8,height_max*hlines+title_height_max,0);
}

void listMenuV2::themeRect(themeselection themesel, int16_t x,int16_t y,int16_t w,int16_t h,uint16_t color){
#ifdef LMV2_USE_THEME_SUPPORT
  int16_t ix ,iy ;
  LGFX_Sprite ** ui = (themesel == themeTitle?titleStyleData:styleData);
  if(ui[0] != nullptr){
    for(int i=0;i<8;i++) {
      uint8_t idx;
      if(ui[i] != nullptr) idx = i;
      else idx = (i==4)?2:(i==6);

      ix = x; iy = y;
      switch(i){
        case 1: ix += spr_offset_x; break;
        case 2: iy += (themesel == themeTitle ? title_spr_offset_y : spr_offset_y); break;
        case 3: ix += spr_offset_x+(spr_width>=0?spr_width:width_max+spr_width); break;
        case 4: ix += spr_offset_x+(spr_width>=0?spr_width:width_max+spr_width); 
        iy += (themesel == themeTitle ? title_spr_offset_y : spr_offset_y); break;
        case 5: iy += (themesel == themeTitle ? title_spr_offset_y : spr_offset_y)
         + ((themesel == themeTitle ? title_spr_height : spr_height)>=0
         ?(themesel == themeTitle ? title_spr_height : spr_height)
         :(themesel == themeTitle ? title_height_max : height_max)
         +(themesel == themeTitle ? title_spr_height : spr_height)); 
         break;
        case 6: ix += spr_offset_x; 
        iy += (themesel == themeTitle ? title_spr_offset_y : spr_offset_y)
        + ((themesel == themeTitle ? title_spr_height : spr_height)>=0
        ?(themesel == themeTitle ? title_spr_height : spr_height)
        :(themesel == themeTitle ? title_height_max : height_max)
        +(themesel == themeTitle ? title_spr_height : spr_height)); break;
        case 7: ix += spr_offset_x+(spr_width>=0?spr_width:width_max+spr_width); 
        iy += (themesel == themeTitle ? title_spr_offset_y : spr_offset_y)
        + ((themesel == themeTitle ? title_spr_height : spr_height)>=0
        ?(themesel == themeTitle ? title_spr_height : spr_height)
        :(themesel == themeTitle ? title_height_max : height_max)
        +(themesel == themeTitle ? title_spr_height : spr_height)); break;
      }
      if(idx != i){
        /*
        if(i == 3 || i == 4) ui[idx]->setRotation(6);// 3 4
        else if(i == 5 || i == 6)ui[idx]->setRotation(4); // 5 6
        else if(i == 7) ui[idx]->setRotation(2);// 7
        else ui[idx]->setRotation(0);// 0 1 2
        */
        LGFX_Sprite impl(in_tft);
        impl.setPaletteColor(0,color);
        impl.createSprite(ui[idx]->width(),ui[idx]->height());
        impl.setRotation((i == 3 || i == 4)?6:((i == 5 || i == 6)?4:((i == 7)?2:0)));
        ui[idx]->pushSprite(&impl,0,0);
        impl.pushSprite(ix,iy);
      }
      else{
        ui[idx]->setPaletteColor(0,color);
        ui[idx]->pushSprite(ix,iy);
      }
    }
  }
  else{
#endif // LMV2_USE_THEME_SUPPORT
    switch(themesel){
      case themeTitle:
    in_tft->fillRect(x+2,y,w-4,h,color);
   in_tft->drawFastVLine(x+1,y+1,h-2,color);
  in_tft->drawFastVLine(x+w-2,y+1,h-2,color);
   in_tft->drawFastVLine(x,y+2,h-4,color);
    in_tft->drawFastVLine(x+w-1,y+2,h-4,color);
    break;
      case themeNormal:
      case themeNull:
    in_tft->drawFastHLine(x+2,y,w-4,color);
    in_tft->drawFastHLine(x+2,y+h-1,w-4,color);
    in_tft->drawFastVLine(x,y+2,h-4,color);
    in_tft->drawFastVLine(x+w-1,y+2,h-4,color);
    in_tft->drawPixel(x+1,y+1,color);
    in_tft->drawPixel(x+w-2,y+1,color);
    in_tft->drawPixel(x+1,y+h-2,color);
    in_tft->drawPixel(x+w-2,y+h-2,color);
    break;
    }
#ifdef LMV2_USE_THEME_SUPPORT
  }
#endif
}
void listMenuV2::drawBackturn(int16_t x,int16_t y){
  const int16_t b_x = 40;
  const int16_t b_y = 24;
  LGFX_Sprite printspr(in_tft);
  printspr.setColorDepth(1);
  printspr.setFont(&cityFont);
  printspr.createSprite(b_x,b_y);
  printspr.setTextDatum(middle_center);
  printspr.fillSprite(1); //白色对话框背景
    printspr.drawFastHLine(2,0,b_x-4,0);
    printspr.drawFastHLine(2,b_y-1,b_x-4,0);
    printspr.drawFastVLine(0,2,b_y-4,0);
    printspr.drawFastVLine(b_x-1,2,b_y-4,0);
    printspr.drawPixel(1,1,0);
    printspr.drawPixel(b_x-2,1,0);
    printspr.drawPixel(1,b_y-2,0);
    printspr.drawPixel(b_x-2,b_y-2,0);
  printspr.setTextColor(0);
  printspr.drawString(F("返回"),b_x/2,b_y/2);
  
  printspr.pushSprite(x-b_x/2,y-b_y/2);
  in_tft->display(3);
}
#ifdef LMV2_USE_DRAWMULYI_CANVAS_SUPPORT
void listMenuV2::drawMulti(lgfx::LGFX_Sprite &sprbase,const char * cmds,void * intera){
        int offset = 0;
        bool inv = 0;   //第0行为标题行,可选择的反色
        //bool endCmd = 0;     //命令结束标志
        uint8_t printOption = 0;
        uint8_t interadataType = 0;
        int16_t cx=(useGlobalIcon
#ifdef LMV2_USE_SD_BMP_SUPPORT
        +useGlobalSDIcon
#endif
)?18:0,cy=(sprbase.height()-1)/2;
        sprbase.setTextColor(0);
        sprbase.setTextDatum(middle_left);
        sprbase.fillSprite(1);
        while(cmds[offset]){
          switch(cmds[offset]){
            case 0x1: inv=!inv;
              offset++;
              break;
            case 0x2: { //后面是一个位图数据, 紧接着是位图x显示位置(1 byte), y显示位置(1 byte), 宽度(1 byte), 
                        //高度(1 byte), 然后是位图数据(高度一定小于等于16), 数据段以连续两个\xff结尾.
              int x=cmds[offset+1],y=cmds[offset+2],w=cmds[offset+3],h=cmds[offset+4];
                if(x>=128) x+=sprbase.width()-256;
                if(y>=128) y+=sprbase.height()-256;
              offset +=5;
              sprbase.drawXBitmap(x,y,(const uint8_t *)&(cmds[offset]),w,h,0,1);
              offset += (w+7)/8*h;
              if(x>=0 && x<=8 && y==0) cx+=x+w+2;
              //Serial.printf("listMenu V2 cmd 0x2: end data: 0x%02x \n",cmds[offset]);
              offset ++;
            }
            break;
            case 0x3: {
              int x=cmds[offset+1],y=cmds[offset+2],w=cmds[offset+3],h=cmds[offset+4];
                if(x>=128) x+=sprbase.width()-256;
                if(y>=128) y+=sprbase.height()-256;
              offset+=5;
              String path="";
              if(cmds[offset]!='/'){
                path += F(_EMW3_ICON_PATH);
              }
              while(cmds[offset] != 0xff) {
                path += cmds[offset];
                offset++;
              }
              //Serial.print("listMenu V2 cmd 0x3: file path: ");
              //Serial.print(path);
              //Serial.print(",ending:0x");
              //Serial.println(cmds[offset],HEX);
#ifdef LMV2_USE_SD_BMP_SUPPORT
              sprbase.drawBmpFile(SDFS,path,x,y,w,h);
              //下面两行放在函数里: if(x>=256-w) x-=256;
              //                  if(y>=256-h) y-=256;
              if(x<=8 && y==0) cx+=x+w+2;
#endif
              offset++;
            }
            break;
            case 0x4: //点
              sprbase.drawPixel(cmds[offset+1],cmds[offset+2],0);
              offset+=3;
            break;
            case 0x5: //直线段
              sprbase.drawLine(cmds[offset+1],cmds[offset+2],cmds[offset+3],cmds[offset+4],0);
              offset+=5;
            break;
            case 0x6: //矩形
              sprbase.drawRect(cmds[offset+1],cmds[offset+2],cmds[offset+3],cmds[offset+4],0);
              offset+=5;
            break;
            case 0x7: //填充矩形
              sprbase.fillRect(cmds[offset+1],cmds[offset+2],cmds[offset+3],cmds[offset+4],0);
              offset+=5;
            break;
            case 0x8: //三角形
              sprbase.drawTriangle(cmds[offset+1],cmds[offset+2],cmds[offset+3],
                cmds[offset+4],cmds[offset+5],cmds[offset+6],0);
              offset+=7;
            break;
            case 0x9: //填充三角形
              sprbase.fillTriangle(cmds[offset+1],cmds[offset+2],cmds[offset+3],
                cmds[offset+4],cmds[offset+5],cmds[offset+6],0);
              offset+=7;
            break;
            case 0xa: //椭圆
              sprbase.drawEllipse(cmds[offset+1],cmds[offset+2],cmds[offset+3],cmds[offset+4],0);
              offset+=5;
            break;
            case 0xb: //填充椭圆
              sprbase.fillEllipse(cmds[offset+1],cmds[offset+2],cmds[offset+3],cmds[offset+4],0);
              offset+=5;
            break;
            case 0xc: //文字加粗/斜体/加下划线/加删除线
              printOption = (uint8_t)cmds[offset+1];
              offset +=2;
            break;
            case 0xd: //文本对齐方式
              sprbase.setTextDatum((lgfx::v1::textdatum::textdatum_t)cmds[offset+1]);
              offset +=2;
            break;
            case 0xe:{ //文本显示位置设置
              uint8_t cmode = (uint8_t)cmds[offset+1];
              if((uint8_t)cmds[offset+2]!=255 || (cmode&0x40)) cx = (uint8_t)cmds[offset+2];
              if((uint8_t)cmds[offset+3]!=255 || (cmode&0x40)) cy = (uint8_t)cmds[offset+3];
              if(cmode&1) cx = -cx;
              if(cmode&2) cy = -cy;
              if(cmode&4) cx = sprbase.width()-cx-1;
              if(cmode&8) cy = sprbase.height()-cx-1;
              if(cmode&0x40) { //相对位置
                cx=(cx*sprbase.width())/256;
                cy=(cy*sprbase.height())/256;
              }
              if(cmode&0x10) cx = sprbase.width ()>>1; //居中
              if(cmode&0x20) cy = sprbase.height()>>1; //居中
              offset+=4;
            }
            break;
            case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: 
            case 0x16: case 0x17: case 0x1a: case 0x1b: case 0x1c:
              interadataType = cmds[offset];
              offset++;
            break;
            case 0x10: //bool switch
              if(intera != nullptr){
                int16_t swx =sprbase.width()-17, swy=sprbase.height()/2-6;
                if(*(const bool *)intera){
                  sprbase.drawFastVLine(swx,swy+3,5,0);
                  sprbase.drawFastVLine(swx+16,swy+3,5,0);
                  sprbase.drawFastVLine(swx+1,swy+2,7,0);
                  sprbase.drawFastVLine(swx+15,swy+2,7,0);
                  sprbase.drawFastVLine(swx+2,swy+1,9,0);
                  sprbase.drawFastVLine(swx+14,swy+1,9,0);
                  sprbase.fillRect(swx+3,swy,11,11,0);
                  sprbase.fillCircle(swx+11,swy+5,3,1);
                }
                else{
                  sprbase.drawFastHLine(swx+3,swy,11,0);
                  sprbase.drawFastHLine(swx+3,swy+10,11,0);
                  sprbase.drawFastVLine(swx,swy+3,5,0);
                  sprbase.drawFastVLine(swx+16,swy+3,5,0);
                  sprbase.drawPixel(swx+1,swy+2,0);
                  sprbase.drawPixel(swx+2,swy+1,0);
                  sprbase.drawPixel(swx+1,swy+8,0);
                  sprbase.drawPixel(swx+2,swy+9,0);
                  sprbase.drawPixel(swx+15,swy+2,0);
                  sprbase.drawPixel(swx+14,swy+1,0);
                  sprbase.drawPixel(swx+15,swy+8,0);
                  sprbase.drawPixel(swx+14,swy+9,0);
                  sprbase.fillCircle(swx+5,swy+5,3,0);
                }
              }
              offset++;
            break;
            case 0x18: //int32 bar
              if(intera != nullptr){
                int32_t val = *(const int32_t *)intera;
                if(val>10000) val=10000;
                if(val<-10000) val=-10000;
                int16_t swx =sprbase.width()-cmds[offset+1], swy=sprbase.height()/2-4;
                sprbase.drawRect(swx,swy,cmds[offset+1],8,0);
                if(val>=0) sprbase.fillRect(swx+2,swy+2,val*(cmds[offset+1]-4)/10000,4,0);
                else sprbase.drawRect(sprbase.width()+val*cmds[offset+1]/10000,
                  swy+2,-val*cmds[offset+1]/10000,4,0);  //负数需要倒退绘制
              }
              offset+=2;
            break;
            case 0x19: //double bar
              if(intera != nullptr){
                double val = *(const double *)intera;
                if(val>1.0) val=1.0;
                if(val<-1.0) val=-1.0;
                int16_t swx =sprbase.width()-cmds[offset+1], swy=sprbase.height()/2-4;
                sprbase.drawRect(swx,swy,cmds[offset+1],8,0);
                if(val>=0) sprbase.fillRect(swx+2,swy+2,int32_t(val*(cmds[offset+1]-4)),4,0);
                else sprbase.drawRect(sprbase.width()+int32_t(val*cmds[offset+1]),
                  swy+2,-int32_t(val*cmds[offset+1]),4,0);  //负数需要倒退绘制
              }
              offset+=2;
            break;
            case 0x1d: //位图文件
              if(intera != nullptr){
                int x = cmds[offset+1], y = cmds[offset+2];
                int w = cmds[offset+3], h = cmds[offset+4];
                if(x>=128) x+=sprbase.width()-256;
                if(y>=128) y+=sprbase.height()-256;
                String path((const char *)intera);
#ifdef LMV2_USE_SD_BMP_SUPPORT
                if(path[0] == '/') sprbase.drawBmpFile(SDFS,path,x,y,w,h);
                else sprbase.drawBmpFile(SDFS,String(F(_EMW3_ICON_PATH)) + path,x,y,w,h);
                if(x>=0 && x<=8 && y==0) cx+=x+w+2;
#endif
              }
              offset+=5;
            break;
            case 0x1e: //位图数组-任意位置
              if(intera != nullptr){
                int x = cmds[offset+1], y = cmds[offset+2];
                int w = cmds[offset+3], h = cmds[offset+4];
                if(x>=128) x+=sprbase.width()-256;
                if(y>=128) y+=sprbase.height()-256;
                sprbase.drawXBitmap(x,y,(const uint8_t*)intera,w,h,0,1);
                if(x>=0 && x<=8 && y==0) cx+=x+w+2;
              }
              offset+=5;
            break;

            default: //包含0x0f
            //if(cmds[offset] != 0x0f) 
              //endCmd = 1;
              while(cmds[offset] && cmds[offset]<0x20) offset++;
              if(!interadataType || intera == nullptr) {
                sprbase.drawString(cmds+offset,cx,cy);
                if(printOption&1) sprbase.drawString(cmds+offset,cx+1,cy);
                offset += strlen(cmds+offset);
              }
              else{
                if(printOption&1) {
                  sprbase.setCursor(cx+1,cy);
                  sprbase.print(cmds+offset);
                }
                sprbase.setCursor(cx,cy);
                offset += sprbase.print(cmds+offset);
                switch(interadataType){
                  case 0x11: 
                  if(*(const bool *)intera) {
                    for(int i=0;i<sprbase.width();i++)
                      for(int j=0;j<sprbase.height();j++)
                        if((i+j)&1) sprbase.drawPixel(i,j,1);
                  }
                  break;
                  case 0x12: 
                  if(!(*(const bool *)intera)) {
                    for(int i=0;i<sprbase.width();i++)
                      for(int j=0;j<sprbase.height();j++)
                        if((i+j)&1) sprbase.drawPixel(i,j,1);
                  }
                  break;
                  case 0x13: 
                    if(*(const bool *)intera) sprbase.print("True"); 
                    else sprbase.print("False"); 
                    break;
                  case 0x14: sprbase.print(*(const int32_t *)intera ); break;
                  case 0x15: sprbase.print(*(const uint32_t *)intera ); break;
                  case 0x16: sprbase.print(*(const float *)intera ); break;
                  case 0x17: sprbase.print(*(const double *)intera ); break;
                  case 0x1a: sprbase.print(*(const char *)intera ); break;
                  case 0x1b: sprbase.print( (const char *)intera ); break;
                  case 0x1c: sprbase.print(*(const String *)intera ); break;
                }
              }
          }
          //if(endcmd) break;
        }
        //if(externalInvert) inv=!inv;
        //sprbase.setPaletteColor(0,inv?0xffff:0x0);
        //sprbase.setPaletteColor(1,inv?0x0:0xffff);
}
#endif
void listMenuV2::drawPlain(lgfx::LGFX_Sprite &sprbase,const char * cmds,void * intera){
  int16_t cx=(useGlobalIcon
#ifdef LMV2_USE_SD_BMP_SUPPORT
  +useGlobalSDIcon
#endif
  )?18:0,cy=(sprbase.height()-1)/2;
  sprbase.setTextColor(0);
  sprbase.setTextDatum(middle_left);
  sprbase.fillSprite(1);
  sprbase.setCursor(cx,cy);
#ifdef LMV2_USE_ADVANCED_PLAIN_SUPPORT
  int cmdshift = 0;
              if(intera != nullptr && cmds[0] == 0x10){
                int16_t swx =sprbase.width()-17, swy=sprbase.height()/2-6;
                if(*(const bool *)intera){
                  sprbase.drawFastVLine(swx,swy+3,5,0);
                  sprbase.drawFastVLine(swx+16,swy+3,5,0);
                  sprbase.drawFastVLine(swx+1,swy+2,7,0);
                  sprbase.drawFastVLine(swx+15,swy+2,7,0);
                  sprbase.drawFastVLine(swx+2,swy+1,9,0);
                  sprbase.drawFastVLine(swx+14,swy+1,9,0);
                  sprbase.fillRect(swx+3,swy,11,11,0);
                  sprbase.fillCircle(swx+11,swy+5,3,1);
                }
                else{
                  sprbase.drawFastHLine(swx+3,swy,11,0);
                  sprbase.drawFastHLine(swx+3,swy+10,11,0);
                  sprbase.drawFastVLine(swx,swy+3,5,0);
                  sprbase.drawFastVLine(swx+16,swy+3,5,0);
                  sprbase.drawPixel(swx+1,swy+2,0);
                  sprbase.drawPixel(swx+2,swy+1,0);
                  sprbase.drawPixel(swx+1,swy+8,0);
                  sprbase.drawPixel(swx+2,swy+9,0);
                  sprbase.drawPixel(swx+15,swy+2,0);
                  sprbase.drawPixel(swx+14,swy+1,0);
                  sprbase.drawPixel(swx+15,swy+8,0);
                  sprbase.drawPixel(swx+14,swy+9,0);
                  sprbase.fillCircle(swx+5,swy+5,3,0);
                }
                cmdshift=1;
              }
              else if(intera != nullptr && cmds[0] == 0x11) cmdshift=2;
              else if(intera != nullptr && cmds[0] == 0x12) cmdshift=3;
              
  sprbase.print(cmds+(cmdshift>0));
                if(intera != nullptr){
                  if(*(const bool *)intera && cmdshift==2) {
                    for(int i=0;i<sprbase.width();i++)
                      for(int j=0;j<sprbase.height();j++)
                        if((i+j)&1) sprbase.drawPixel(i,j,1);
                  }
                  if(!(*(const bool *)intera) && cmdshift==3) {
                    for(int i=0;i<sprbase.width();i++)
                      for(int j=0;j<sprbase.height();j++)
                        if((i+j)&1) sprbase.drawPixel(i,j,1);
                  }
                }
#else
  sprbase.print(cmds);
#endif
  //sprbase.setPaletteColor(0,externalInvert?0xffff:0x0);
  //sprbase.setPaletteColor(1,externalInvert?0x0:0xffff);
}

void listMenuV2::setSize(const int16_t * sizedata){
  if(sizedata != nullptr){
    if(sizedata[0]) width_max = sizedata[0];
    if(sizedata[1]>=0) height_max = sizedata[1];
    else height_max=0;
    if(sizedata[2]) spr_width = sizedata[2];
    if(sizedata[3]) spr_height = sizedata[3];
    if(sizedata[4]>=0) spr_offset_x = sizedata[4];
    if(sizedata[5]>=0) spr_offset_y = sizedata[5];
    if(sizedata[6]>=0) border_w = sizedata[6];
    if(sizedata[7]>=0) border_h = sizedata[7];
    if(sizedata[8]) title_height_max = sizedata[8];
    if(title_height_max<0) title_height_max=0;
    if(sizedata[9]) title_spr_height = sizedata[9];
    if(sizedata[10]>=0) title_spr_offset_y = sizedata[10];
    if(sizedata[11]>=0) title_Invert = sizedata[11];
  }
}
void listMenuV2::setSize(sizeOption_t option, int16_t sizedata){
  switch (option)
  {
  case widthMax: width_max = sizedata;
    break;
  case heightMax: height_max = sizedata;
    if(height_max<0) height_max=0;
    break;
  case sprWidth:spr_width = sizedata;
    break;
  case sprHeight:spr_height = sizedata;
    break;
  case sprOffsetX:spr_offset_x = sizedata;
    break;
  case sprOffsetY:spr_offset_y = sizedata;
    break;
  case borderWidth:border_w = sizedata;
    break;
  case borderHeight:border_h = sizedata;
    break;
  case titleHeightMax:title_height_max = sizedata;
    break;
  case titleSpriteHeight:title_spr_height = sizedata;
    break;
  case titleSprOffsetY:title_spr_offset_y = sizedata;
    break;
  case titleInvert:
    if(sizedata>=0) title_Invert = sizedata;
  }
}
void listMenuV2::setIcon(uint16_t num,const uint8_t **bmps){
  useGlobalIcon = num;
  globalIcon = bmps;
#ifdef LMV2_USE_SD_BMP_SUPPORT
  useGlobalSDIcon = 0;
  globalSDIcon = nullptr;
}
void listMenuV2::setSDIcon(uint16_t num,const char **path){
  useGlobalIcon = 0;
  globalIcon = nullptr;
  useGlobalSDIcon = num;
  globalSDIcon = path;
}
#else 
}
#endif

#ifdef LMV2_USE_THEME_SUPPORT
#ifdef LMV2_USE_SD_BMP_SUPPORT

uint8_t listMenuV2::setThemeFromFile(themeselection themesel, int16_t styleBmps,const char **bmpFilePath,fs::FS Fs){
  if(styleBmps<3 || themesel == themeNull) return 1; //数据错误
  LGFX_Sprite ** tsd; //Theme Setting Data
  switch(themesel){
    case themeTitle: tsd = titleStyleData; break;
    case themeNormal: tsd = styleData; break;
    default: tsd = nullptr; break;
    //case 2: tsd = titleStyleData; break;
  }
  for(int i = 0;i < styleBmps;i++){
    LGFX_Sprite spr;//创建一个基于图像文件的sprite, 因为要知道图像的宽和高, LGFX 的 API 函数不提供这些东西
    spr.setColorDepth(1); //设置颜色深度
    uint8_t idx;
    if(i < styleBmps) idx = i;
    else idx = (i==4)?2:(i==6);
    Serial.print("Index config:");
    Serial.print(i);
    Serial.println(idx);
    String path = _EMW3_THEME_PATH;
    if(*bmpFilePath[idx] == '/') path = bmpFilePath[idx];
    else path += bmpFilePath[idx];
    Serial.print("Open File: ");
    Serial.println(path);
    Serial.print("Exists: ");
    Serial.println(SD.exists(path));
    spr.createFromBmpFile(Fs,path.c_str());  //从主题图片文件打开, 当styleBmps为3时,打开现有的图像
    Serial.println("MidSprite create Done");
    if(spr.getBuffer() == nullptr) return 2; //创建失败, 文件打开失败
    /*
    switch(i){
      case 3: if(styleBmps<4) spr.setRotation(6); //右上角, 左右翻转
      case 4: if(styleBmps<5) spr.setRotation(6); //对应2,  左右翻转
      case 5: if(styleBmps<6) spr.setRotation(4); //右上角, 上下翻转
      case 6: if(styleBmps<7) spr.setRotation(4); //对应1   上下翻转
      case 7: if(styleBmps<8) spr.setRotation(2); //右上角, 180°旋转
    } */
    tsd[i] = new LGFX_Sprite(in_tft); //映射到类内对象
    tsd[i]-> setColorDepth(1);//设置颜色深度
    Serial.println("Sprite allocated Done");

    int16_t iw,ih; //宽度标签
    Serial.printf("%d %d\n",spr_offset_x,spr_offset_y);
    switch(i){
    case 0: //左上
      iw = spr_offset_x; //确定真是的sprite 宽度高度
      ih = (themesel == themeTitle?title_spr_offset_y:spr_offset_y); 
    break;
    case 1: //正上
      iw = spr_width; //确定真是的sprite 宽度高度
      if(iw<0) iw+=width_max;
      if(iw>width_max - spr_offset_x) iw=width_max - spr_offset_x;
      ih = (themesel == themeTitle?title_spr_offset_y:spr_offset_y); //高度设置为spr的y轴偏移
    break;
    case 2: //正左
      iw = spr_offset_x; //高度设置为spr的 x 轴偏移
      ih=(themesel == themeTitle?title_spr_height:spr_height);
      if(ih<0) ih+=(themesel == themeTitle?title_height_max:height_max);
      if(ih>(themesel == themeTitle?title_height_max:height_max)-(themesel == themeTitle?title_spr_offset_y:spr_offset_y)) ih=(themesel == themeTitle?title_height_max:height_max)-(themesel == themeTitle?title_spr_offset_y:spr_offset_y);
    break;
    case 3: //右上
      iw = spr_width; //确定真是的sprite 宽度高度
      if(iw<0) iw+=width_max;
      if(iw>width_max - spr_offset_x) iw=0;
      iw = width_max - spr_offset_x - iw; //确定真是的sprite 宽度高度
      ih = (themesel == themeTitle?title_spr_offset_y:spr_offset_y); 
      //tsd[i]->setRotation(6);
    break;
    case 4: //正右
      iw = spr_width; //确定真是的sprite 宽度高度
      if(iw<0) iw+=width_max;
      if(iw>width_max - spr_offset_x) iw=0;
      iw = width_max - spr_offset_x - iw; //确定真是的sprite 宽度高度
      ih=(themesel == themeTitle?title_spr_height:spr_height);
      if(ih<0) ih+=(themesel == themeTitle?title_height_max:height_max);
      if(ih>(themesel == themeTitle?title_height_max:height_max)
      -(themesel == themeTitle?title_spr_offset_y:spr_offset_y)) 
        ih=(themesel == themeTitle?title_height_max:height_max)
        -(themesel == themeTitle?title_spr_offset_y:spr_offset_y);
      //tsd[i]->setRotation(6);
    break;
    case 5: //左下
      iw = spr_offset_x; //高度设置为spr的 x 轴偏移
      ih=(themesel == themeTitle?title_spr_height:spr_height);
      if(ih<0) ih+=(themesel == themeTitle?title_height_max:height_max);
      if(ih>(themesel == themeTitle?title_height_max:height_max)
      -(themesel == themeTitle?title_spr_offset_y:spr_offset_y)) 
        ih=(themesel == themeTitle?title_height_max:height_max)
        -(themesel == themeTitle?title_spr_offset_y:spr_offset_y);
      ih = (themesel == themeTitle?title_height_max:height_max)
       - (themesel == themeTitle?title_spr_offset_y:spr_offset_y) - ih; 
      //tsd[i]->setRotation(4);
    break;
    case 6: //正下
      iw = spr_width; //确定真是的sprite 宽度高度
      if(iw<0) iw+=width_max;
      if(iw>width_max - spr_offset_x) iw=width_max - spr_offset_x;
      ih=(themesel == themeTitle?title_spr_height:spr_height);
      if(ih<0) ih+=(themesel == themeTitle?title_height_max:height_max);
      if(ih>(themesel == themeTitle?title_height_max:height_max)
      -(themesel == themeTitle?title_spr_offset_y:spr_offset_y)) 
        ih=(themesel == themeTitle?title_height_max:height_max)
        -(themesel == themeTitle?title_spr_offset_y:spr_offset_y);
      ih = (themesel == themeTitle?title_height_max:height_max)
       - (themesel == themeTitle?title_spr_offset_y:spr_offset_y) - ih; 
      //tsd[i]->setRotation(4);
    break;
    /*case 7*/ default: //右下
      iw = spr_width; //确定真是的sprite 宽度高度
      if(iw<0) iw+=width_max;
      if(iw>width_max - spr_offset_x) iw=0;
      iw = width_max - spr_offset_x - iw; //确定真是的sprite 宽度高度
      ih=(themesel == themeTitle?title_spr_height:spr_height);
      if(ih<0) ih+=(themesel == themeTitle?title_height_max:height_max);
      if(ih>(themesel == themeTitle?title_height_max:height_max)
      -(themesel == themeTitle?title_spr_offset_y:spr_offset_y))
        ih=(themesel == themeTitle?title_height_max:height_max)
        -(themesel == themeTitle?title_spr_offset_y:spr_offset_y);
      ih = (themesel == themeTitle?title_height_max:height_max)
       - (themesel == themeTitle?title_spr_offset_y:spr_offset_y) - ih; 
      //tsd[i]->setRotation(2);
    }
    Serial.printf("Sprite W: %d(%d) / H: %d(%d) set Done\n",iw,spr_offset_x,ih,spr_offset_y);
    //if(i<styleBmps){
      if(tsd[i] != nullptr) tsd[i]->deleteSprite();
      tsd[i]->createSprite(iw,ih);
      if(tsd[i]->getBuffer() == nullptr){ //设置失败了... 内存不足
        for(int j = 0; j<=i; j++) {
          tsd[j] ->deleteSprite();
          delete tsd[j];
          tsd[j] = nullptr;
        }
        return 3; //内存不足
      }
      Serial.println("Sprite created Done");
      //初始化完成了, 开始绘制
      int32_t mwidth =0;
      int32_t mheight=0;
      while(mwidth < tsd[i]->width() && mheight < tsd[i]->height()){
        tsd[i]->drawBitmap(mwidth,mheight,(const uint8_t *)spr.getBuffer(),spr.width(),spr.height(),1);
        if(i==1 || i==6) mwidth += spr.width();
        else if(i==2 || i==4) mheight += spr.height();
        else break;
      }
    //}
    // spr.deleteSprite(); 将会自动在函数退出时调用
  }
  return 0;
}
#endif
#endif