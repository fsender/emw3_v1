/******* FRIENDSHIPENDER *****
 * @file listMenuV2.cpp
 * @author fsender (Bilibili FriendshipEnder)
 * @brief 菜单显示程序,可以提供一个功能丰富的菜单功能,仅使用于EMW3
 * 将来会加入键盘,对话框,滑动条等更多功能
 * @version 1.2
 * @date 2022-02-13
 * 
 */
#include "listMenuV2.h"

#define LMV2_MIN(x,y) ((x)<(y)?(x):(y)) //取最小值函数
#define LMV2_MAX(x,y) ((x)>(y)?(x):(y)) //取最小值函数
   String listMenuV2::dispspecial[MAX_XV_LINE] = {};
/*
listMenuV2::listMenuV2(EMW3 *intft, const char **text, void **interaction) : in_tft(intft) {
  emw3_cnt_fs = &SDFS; 
  if(text == nullptr) return;
  //_text = text;
  //if(interaction != nullptr) _intera = interaction;
} */
listMenuV2::~listMenuV2(){
#ifdef LMV2_USE_THEME_SUPPORT
  for(int i=0;i<8;i++)
    if(styleData[i] != nullptr) {
      styleData[i]->deleteSprite();
      delete []styleData[i];
    }
  for(int i=0;i<8;i++)
    if(titleStyleData[i] != nullptr) {
      titleStyleData[i]->deleteSprite();
      delete []titleStyleData[i];
    }
#endif
}
uint16_t listMenuV2::listMenu(int16_t x,int16_t y,uint8_t hlines,int16_t numitem,uint16_t selected,
  uint8_t settings, const String *text, void ** interactions ){
  const char **text_c_str = new const char * [numitem+1];
  for(int i=0;i<=numitem;i++) text_c_str[i] = text[i].c_str();
  uint16_t willReturn = listMenu(x,y,hlines,numitem,selected,settings,text_c_str,interactions);
  delete[] text_c_str;
  return willReturn;
}

uint16_t listMenuV2::listMenu(int16_t x,int16_t y,uint8_t hlines,int16_t numitem,uint16_t selected,
  uint8_t settings, const char **text, void ** interactions ){
  //  Serial.println("LISTMENU FX");
  uint8_t long_pressed=0; //长按检测标签
  uint8_t cursor_changed=0; //标记 cursor 是否改变过, 
  // 0 无改变  1 向上   2 向下    >3: 需要刷新整个画面
  uint8_t initial = 1;  //初始化阶段此数值为1
  uint32_t refreshFlag = 0; //刷新检测
  uint32_t beginPress = 0; //标记开始按下 BtnM 的时间
  uint32_t pressMillis = 0; //标记开始按下 BtnL 或 BtnR 的时间
  int16_t cursor = hlines>2?2:1; //光标在第几个的位置上,当 hlines>=3 时,cursor只能为 2 到 hlines-1之间.
  int16_t cursor2 = 0; //当光标在最上面时,正好选择的是第一个选项,则此数值为0
  //hlines |= 1;
  int16_t h = height_max*hlines+title_height_max;
  uint8_t flipCombo = 0;  //新增自适应刷新速度功能,速度不同将使用不同的lut
  uint8_t olddepth = in_tft->getLut(true,16);
  /* uint8_t enable_pgm = 0;
  if(settings&8) enable_pgm = 8;      //SD卡
  else if(settings&1) enable_pgm = 1; //pgm
  uint8_t dispbar = settings&4;       //显示右侧进度条
  */
  const char *disp[MAX_XV_LINE] = {nullptr};//文本内容
  const char *disp_icon[MAX_XV_LINE] = {nullptr};//图标内容
  void *intera[MAX_XV_LINE] = {nullptr};
  if(selected == 0) selected = 1;
  if(selected>numitem) selected=numitem;
  if(settings&1) {  //使用了pgm的字符串
    dispspecial[0] = FPSTR(text[0]);
    disp[0] = dispspecial[0].c_str();
  }
  else disp[0] = text[0];
  in_tft->setAutoFullRefresh(0);
//准备图标数据
  if(useGlobalIcon) disp_icon[0] = (const char *)globalIcon[0];
#ifdef LMV2_USE_SD_BMP_SUPPORT
  else if(useGlobalSDIcon) disp_icon[0] = globalSDIcon[0];
#endif

#ifdef LMV2_USE_SD_TXT_SUPPORT
  if(settings&8) {
    if(settings&1) {
      dispspecial[1] = FPSTR(text[1]);
      txtf = emw3_cnt_fs->open(dispspecial[1],"r");
    }
    else txtf = emw3_cnt_fs->open(text[1],"r");
    if(!txtf) { //对应文件未打开, 文件损坏?
      end_listMenu_settings_impl(olddepth);
      return 0;
    }
    else useDrawMulti = 0;  //使用SD卡文件时, 不能使用useDrawMulti
  }
#endif
  if(interactions != nullptr) intera[0] = interactions[0];
  while(in_tft->getBtn(keyM)==0) yield();
  delay(DEBOUNCE_DELAY_MS);
  //Serial.println("INITIAL DISPLAY!!\n");
  if(numitem==0){ //无条目
    const char msg[] = "\0370个选项";
    if(cursor==2) disp[1] = msg;
    listMenu_impl_GUI(x,y,width_max,hlines,settings,disp,nullptr,disp_icon);
    themeRect(themeNormal, x,y+height_max*(cursor-1)+title_height_max,width_max,height_max,0);
#ifdef LMV2_USE_LISTMENU_DEMO_MODE_SUPPORT
    if(settings & 32){
      end_listMenu_settings_impl(olddepth);
      delay(DEBOUNCE_DELAY_MS);
      return 1;
    }
#endif
    in_tft->display(3);
    end_listMenu_settings_impl(olddepth);
    while(in_tft->getBtn(keyM)==1) yield();
    delay(DEBOUNCE_DELAY_MS);
    while(in_tft->getBtn(keyM)==0) yield();
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
    for(uint8_t i=3;i<=hlines;i++) disp[i] = nullptr;
    
    if(useGlobalIcon>=2) disp_icon[hlines<=2?1:2] = (const char *)globalIcon[1]; //准备图标数据
#ifdef LMV2_USE_SD_BMP_SUPPORT
    else if(useGlobalSDIcon>=2) disp_icon[hlines<=2?1:2] = globalSDIcon[1];
#endif
    listMenu_impl_GUI(x,y,width_max,hlines,settings,disp,intera,disp_icon);
    themeRect(themeNormal, x,y+height_max*(cursor-1)+title_height_max,width_max,height_max,0);
#ifdef LMV2_USE_LISTMENU_DEMO_MODE_SUPPORT
    if(settings & 32){
      end_listMenu_settings_impl(olddepth);
      delay(DEBOUNCE_DELAY_MS);
      return 1;
    }
#endif
    if(listMenu_cb != nullptr) listMenu_cb(selected,userdata);  //调用回调函数
    in_tft->display(3);
    while(in_tft->getBtn(keyM)==1) yield();
    delay(DEBOUNCE_DELAY_MS);
    while(in_tft->getBtn(keyM)==0) {
      if(!beginPress) {
        beginPress=millis();
        delay(DEBOUNCE_DELAY_MS);
      }
      if(millis()-beginPress>LONGPRESS_DELAY_MS){
        drawKeyText(x+width_max/2,y+h/2);
        in_tft->display(3);
        end_listMenu_settings_impl(olddepth);
        while(in_tft->getBtn(keyM)==0) yield();
        delay(DEBOUNCE_DELAY_MS);
        return 0;
      }
      yield();
    }
    end_listMenu_settings_impl(olddepth);
    delay(DEBOUNCE_DELAY_MS);
    return 1;
  }
  cursor+=selected-1;
  
  if((cursor-hlines+(hlines>2))>0){
    cursor2+=cursor-hlines+(hlines>2);
    cursor=hlines-(hlines>2);
  }
#ifdef LMV2_USE_LISTMENU_DEMO_MODE_SUPPORT
  if(settings&32) goto aaaa; //此功能必须使用
#endif
  for(;;){
    //Serial.println("MAIN LOOP GOING!!\n");
    if((pressMillis = millis()) > refreshFlag){
      if(!in_tft->getBtn(keyL)){
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
        if(flipCombo<olddepth-3) flipCombo++;  //新增自适应刷新速度功能
      }
      else if(!in_tft->getBtn(keyR)){
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
        if(flipCombo<olddepth-3) flipCombo++;  //新增自适应刷新速度功能
      }
    }
    while(in_tft->getBtn(keyM)==0) {
      if(!beginPress) {
        bool breakFlag = 0;
        beginPress=millis();
        while(in_tft->getBtn(keyM)==0) {
          if(millis()-beginPress>LONGPRESS_DELAY_MS) {
            breakFlag = 1;
            break;
          }
          yield();
        }
        
        if(interactions != nullptr && interactions[selected] != nullptr && useDrawMulti && !breakFlag){
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
        else 
          long_pressed = 1;
        delay(DEBOUNCE_DELAY_MS);
      }
      if(millis()-beginPress>LONGPRESS_DELAY_MS){
        drawKeyText(x+width_max/2,y+h/2);
        in_tft->display(3);
        while(in_tft->getBtn(keyM)==0) yield();
        delay(DEBOUNCE_DELAY_MS);
        long_pressed = 2;
        break;
      }
      yield();
    }
    if(long_pressed) { /*
      if(long_pressed == 1 && interactions != nullptr 
        && interactions[selected] != nullptr && useDrawMulti){
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
          else break; //当 long_pressed == 1或2时退出循环,则 ListMenu 将退出并返回选定的值
        }
      else */ break;
    }
    else if(beginPress){
        beginPress=0;
        delay(DEBOUNCE_DELAY_MS);
        continue;
    }
    if(in_tft->getBtn(keyL) && in_tft->getBtn(keyR)) {
      if( flipCombo>=5 ) {
        in_tft->setDepth(olddepth);
        in_tft->display(7);
        in_tft->display(3);
      }
      flipCombo=0; 
    } //新增自适应刷新速度功能
    if((!refreshFlag) || initial){
#ifdef LMV2_USE_LISTMENU_DEMO_MODE_SUPPORT
aaaa:
#endif
      char itemsFlag[16];
      sprintf_P(itemsFlag,PSTR("\037%d个选项"),numitem);
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

//准备图标数据
          if(useGlobalIcon) {
            if(useGlobalIcon>prindex) disp_icon[i+1] = (const char *)globalIcon[prindex];
            else disp_icon[i+1]=nullptr;
          }
#ifdef LMV2_USE_SD_BMP_SUPPORT
          else if(useGlobalSDIcon) {
            if(useGlobalSDIcon>prindex) disp_icon[i+1] = globalSDIcon[prindex];
            else disp_icon[i+1]=nullptr;
          }
#endif

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
        case 1:  themeRect(themeNormal, x,y+height_max*cursor+title_height_max,width_max,height_max,1); break;
        case 2:  themeRect(themeNormal, x,y+height_max*(cursor-2)+title_height_max,width_max,height_max,1); break;
        default: listMenu_impl_GUI(x,y,width_max,hlines,settings,disp,intera,disp_icon);
        if(settings&4){
          int barLength = hlines*(height_max*hlines+title_height_max-4)/(numitem+(hlines<3?0:2));
          if(barLength<1) barLength=1;
          int barwhere = cursor2*(height_max*hlines+title_height_max-4-barLength)/(numitem+(hlines<3?0:2)-hlines);
          in_tft->fillRect(x+width_max+2,y+1,(settings&16)?2:6,height_max*hlines+title_height_max-2,1);
          if(numitem+(hlines<3?0:2)>hlines) in_tft->fillRect(x+width_max+((settings&16)?2:3),y+2+barwhere,
            (settings&16)?2:4,barLength,0x0);
        }
      }
      themeRect(themeNormal, x,y+height_max*(cursor-1)+title_height_max,width_max,height_max,0);
      refreshFlag = pressMillis+(olddepth - flipCombo)*18; //此处以接近屏幕刷新时间为宜,
#ifdef LMV2_USE_LISTMENU_DEMO_MODE_SUPPORT
      if(!(settings&32)) {
#endif
        if(listMenu_cb != nullptr) listMenu_cb(selected,userdata);  //调用回调函数
        //while(in_tft->epdBusy()) yield();
        in_tft->setDepth(olddepth - flipCombo);
        in_tft->display(3);
#ifdef LMV2_USE_LISTMENU_DEMO_MODE_SUPPORT
      }
#endif
      cursor_changed=0;
      if(initial){
        settings |= 2; //之后不再绘制标题栏
        initial=0;
      }
#ifdef LMV2_USE_LISTMENU_DEMO_MODE_SUPPORT
      if(settings&32) {
        end_listMenu_settings_impl(olddepth);
        return 0;
      }
#endif
    }
    yield();
  }
  if(long_pressed==2) selected = 0;
  delay(DEBOUNCE_DELAY_MS);
#ifdef LMV2_USE_SD_TXT_SUPPORT
  if(settings & 8) txtf.close();
#endif
  end_listMenu_settings_impl(olddepth);
  return selected;
}
void listMenuV2::end_listMenu_settings_impl(uint8_t d){
  while(in_tft->epdBusy()) yield();
    setListMenuCallback(nullptr,nullptr);
    in_tft->setAutoFullRefresh(1);
    in_tft->setDepth(d);
}
void listMenuV2::listMenu_impl_GUI(int16_t x,int16_t y, int16_t w, uint8_t hlines,uint8_t settings,
  const char **text, void ** interactions, const char ** bmpsrc ){
  //  Serial.println("listMenu_impl_GUI FX\n");
  //themeRect(x,y+20*(hlines/2+1),w,20,0);
  LGFX_Sprite printspr(in_tft);
  bool leftSpace = 0;  //有无图标空位
  printspr.setColorDepth(1);
  printspr.setFont(&cn_font);
  int16_t real_spr_width = spr_width,real_spr_height;
  if(real_spr_width<0) real_spr_width+=w;
  if(real_spr_width>w) real_spr_width=w;

  //绘制标题
  if(!(settings&2) && title_height_max>0) {
    themeRect(title_Invert?themeTitle:themeNormal,x,y,w,title_height_max-(hlines <= 2),0);
    real_spr_height=title_spr_height;
    if(real_spr_height<0) real_spr_height+=title_height_max;
    if(real_spr_height>title_height_max) real_spr_height=title_height_max;
    real_spr_height-=(hlines <= 2);
    if(text[0] != nullptr) {
//-----------绘制图标-----------此部分有bug--------------
        if(bmpsrc[0]!=nullptr){
          if(useGlobalIcon) {
            in_tft->drawBitmap(x+spr_offset_x,y+spr_offset_y,(const unsigned char *)bmpsrc[0],
            16,16,title_Invert?(uint16_t)1:(uint16_t)0,title_Invert?(uint16_t)0:(uint16_t)1);
            leftSpace = 1;
            printspr.createSprite(real_spr_width-18,real_spr_height);
          }
#ifdef LMV2_USE_SD_BMP_SUPPORT
          else if(useGlobalSDIcon) {
            LGFX_Sprite bmp_spr(in_tft);
            bmp_spr.setColorDepth(1);
            if(bmpsrc[0][0]!='/') {
              char *a = new char[strlen(_EMW3_ICON_PATH)+strlen(bmpsrc[0])+1];
              strcpy(a,_EMW3_ICON_PATH);
              strcat(a,bmpsrc[0]);
              bmp_spr.createFromBmpFile(*emw3_cnt_fs,a);
              delete []a;
            }
            else bmp_spr.createFromBmpFile(*emw3_cnt_fs,bmpsrc[0]);
            in_tft->drawBitmap(x+spr_offset_x,y+spr_offset_y,
            (const uint8_t *)bmp_spr.getBuffer(),bmp_spr.width(),bmp_spr.height(),
            title_Invert?(uint16_t)0:(uint16_t)1,title_Invert?(uint16_t)1:(uint16_t)0); //绘制标题图标
            leftSpace = 1;
          }
#endif
        }
        printspr.createSprite(real_spr_width-(leftSpace?18:0),real_spr_height);
//-----------图标绘制完了--------------
#ifdef LMV2_USE_DRAWMULYI_CANVAS_SUPPORT
      if(useDrawMulti) 
        drawMulti(printspr,text[0],(interactions ==nullptr)?nullptr:interactions[0]);
      else 
#endif
        drawPlain(printspr,text[0],(interactions ==nullptr)?nullptr:interactions[0]);
      //printspr.pushSprite(x+spr_offset_x,y+spr_offset_y);
      in_tft->drawBitmap(x+spr_offset_x+(leftSpace?18:0),y+spr_offset_y,
      (const uint8_t *)printspr.getBuffer(),printspr.width(),printspr.height(),
      title_Invert?(uint16_t)0:(uint16_t)1,title_Invert?(uint16_t)1:(uint16_t)0);
    }
    printspr.deleteSprite();
  }
  
  //绘制标题以外的东西
  real_spr_height = spr_height;
  if(real_spr_height<0) real_spr_height+=height_max;
  if(real_spr_height>height_max) real_spr_height=height_max;
  in_tft->fillRect(x,y+title_height_max,w,height_max*hlines,1);
   //初始化sprite, 注意:多次初始化将删除原有的sprite
  printspr.createSprite(real_spr_width,real_spr_height);
  for(int i=1;i<=hlines;i++){
    if(text[i] != nullptr) {
      if(text[i][0] == '\x1f'){
        printspr.createSprite(real_spr_width,real_spr_height);
        printspr.setTextDatum(middle_left);
        printspr.setPaletteColor(0,0);
        printspr.setPaletteColor(1,1);
        printspr.fillSprite(1);
        uint16_t twidth = printspr.textWidth(text[i]+1);
        printspr.fillRect(w/2-twidth/2-8,0,twidth+8,printspr.height(),0);
        printspr.setTextColor(1);
        printspr.drawString(text[i]+1,w/2-twidth/2-4,(printspr.height()-1)/2);
      }
      else {
//-----------绘制图标-----------此部分有bug--------------
        int16_t drx=x+spr_offset_x, dry=y+(i-1)*height_max+title_height_max+spr_offset_y;
        leftSpace = 0; //重置 leftSpace (有无图标空位);
        if(bmpsrc[i]!=nullptr){
          if(useGlobalIcon>=i) {
            in_tft->drawBitmap(drx,dry,(const unsigned char *)bmpsrc[i],16,16,0);
            leftSpace=1;
          }
#ifdef LMV2_USE_SD_BMP_SUPPORT
          else if(useGlobalSDIcon>=i) { //绘制图标
          uint8_t cached = 0; //检测是否已缓存
          for(uint32_t j=0;j<LMV2_MIN(cacheUsage,16u);j++){
            if(iconDataCacheLabel[j]==bmpsrc[i]){
              cached = j+1;
              //Serial.printf("FOUND CACHED BITMAP DATA AT %d, I=%d\n\n",j,i);
              break;
            }
          }
          if(cached){ //绘制缓存的图标
            in_tft->drawBitmap(drx,dry,iconDataCache[cached-1],16,16,1,0);
          }
          else{ //没有图标缓存
            if(bmpsrc[i][0] != '/') {
              char *a = new char[strlen(_EMW3_ICON_PATH)+strlen(bmpsrc[i])+1];
              strcpy(a,_EMW3_ICON_PATH);
              strcat(a,bmpsrc[i]);
              in_tft->drawBmpFile(*emw3_cnt_fs,a,drx,dry);
              delete []a;
            }
            else in_tft->drawBmpFile(*emw3_cnt_fs,bmpsrc[i],drx,dry);
            //创建图标缓存
            iconDataCacheLabel[cacheUsage&15] = bmpsrc[i];
            //Serial.printf("CACHING BMP FILE DATA, I=%d, CACHEUSAGE=%d\n\n",i,cacheUsage);
            for(int16_t jj=0;jj<32;jj++) iconDataCache[cacheUsage&15][jj]=0;
            for(int16_t ji=0;ji<256;ji++){
              if(in_tft->readPixel(drx+(ji&15),dry+(ji>>4))) 
                iconDataCache[cacheUsage&15][ji>>3] |= (1<<(7-(ji&7)));
            }
            //in_tft->readPixel(drx,dry,16,16,iconDataCache[cacheUsage&15]);
            cacheUsage++; //此处最多支持4294967295步操作哈哈
          }
            leftSpace=1;
          }
#endif
        }
        printspr.createSprite(real_spr_width-(leftSpace?18:0),real_spr_height);
//-----------图标绘制完了--------------
#ifdef LMV2_USE_DRAWMULYI_CANVAS_SUPPORT
        if(useDrawMulti) 
          drawMulti(printspr,text[i],(interactions ==nullptr)?nullptr:interactions[i]);
        else 
#endif
          drawPlain(printspr,text[i],(interactions ==nullptr)?nullptr:interactions[i]);
      }
      printspr.pushSprite(x+spr_offset_x+(leftSpace?18:0),
      y+(i-1)*height_max+title_height_max+spr_offset_y);
      //in_tft->drawBitmap(x+spr_offset_x,y+(i-1)*height_max+title_height_max+spr_offset_y,
      //(const uint8_t *)printspr.getBuffer(),printspr.width(),printspr.height(),(uint16_t)0,(uint16_t)65535);
    }
  }
  if(settings&4) in_tft->drawRect(x+w+1,y,(settings&16)?4:8,height_max*hlines+title_height_max,0);
  //printspr.deleteSprite(); //因为使用了静态方式, 所以需要及时删掉sprite
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
void listMenuV2::drawKeyText(int16_t x,int16_t y,const char *_2ch,int16_t w,int16_t h){
  //const int16_t w = 40;
  //const int16_t h = 24;
  bool colorf = 0;
  LGFX_Sprite printspr(in_tft);
  printspr.setColorDepth(1);
  printspr.setFont(&cn_font);
  if(w<8){
    if(_2ch != nullptr) w = printspr.textWidth(_2ch)+4;
    else w=8;
  }
  if(h<0) {
    h=-h;
    colorf = 1;
  }
  if(h<8) h=8;
  printspr.createSprite(w,h);
  printspr.setTextDatum(middle_center);
    printspr.fillSprite(1); 
  if(colorf){
    printspr.fillRect(2,0,w-4,h,0); //黑色对话框背景
    printspr.drawFastVLine(0,2,h-4); //黑色对话框背景
    printspr.drawFastVLine(1,1,h-2); //黑色对话框背景
    printspr.drawFastVLine(w-2,1,h-2); //黑色对话框背景
    printspr.drawFastVLine(w-1,2,h-4); //黑色对话框背景
    printspr.setTextColor(1); //白色字体
  }
  else{  //白色对话框背景
    printspr.drawFastHLine(2,0,w-4,0);
    printspr.drawFastHLine(2,h-1,w-4,0);
    printspr.drawFastVLine(0,2,h-4,0);
    printspr.drawFastVLine(w-1,2,h-4,0);
    printspr.drawPixel(1,1,0);
    printspr.drawPixel(w-2,1,0);
    printspr.drawPixel(1,h-2,0);
    printspr.drawPixel(w-2,h-2,0);
    printspr.setTextColor(0);
  }
  if(_2ch == nullptr) printspr.drawString(F("返回"),w/2,h/2);
  else printspr.drawString(_2ch,w/2,h/2);
  
  printspr.pushSprite(x-w/2,y-h/2);
}
#ifdef LMV2_USE_DRAWMULYI_CANVAS_SUPPORT
void listMenuV2::drawMulti(lgfx::LGFX_Sprite &sprbase,const char * cmds,void * intera){
        int offset = 0;
        bool inv = 0;   //第0行为标题行,可选择的反色
        //bool endCmd = 0;     //命令结束标志
        uint8_t printOption = 0;
        uint8_t interadataType = 0;
        int16_t cx=0,cy=(sprbase.height()-1)/2;
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
              //Serial.printf("LISTMENU V2 CMD 0x2: END DATA: 0x%02x \n",cmds[offset]);
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
              //Serial.print("LISTMENU V2 CMD 0x3: FILE PATH: ");
              //Serial.print(path);
              //Serial.print(",ENDING:0X");
              //Serial.println(cmds[offset],HEX);
#ifdef LMV2_USE_SD_BMP_SUPPORT
              sprbase.drawBmpFile(*emw3_cnt_fs,path,x,y,w,h);
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
                if(path[0] == '/') sprbase.drawBmpFile(*emw3_cnt_fs,path,x,y,w,h);
                else sprbase.drawBmpFile(*emw3_cnt_fs,String(F(_EMW3_ICON_PATH)) + path,x,y,w,h);
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
        if(inv)
        for(uint32_t i=0;i<sprbase.bufferLength();i++)
          *((uint8_t *)sprbase.getBuffer()+i) = ~*((uint8_t *)sprbase.getBuffer()+i);
        //sprbase.setPaletteColor(0,inv?1:0x0);
        //sprbase.setPaletteColor(1,inv?0x0:1);
}
#endif
void listMenuV2::drawPlain(lgfx::LGFX_Sprite &sprbase,const char * cmds,void * intera){
  int16_t cx=0,cy=(sprbase.height()-1)/2;
  sprbase.setTextColor(0);
  sprbase.setTextDatum(middle_left);
  sprbase.fillSprite(1);
#ifdef LMV2_USE_ADVANCED_PLAIN_SUPPORT
  sprbase.setCursor(cx,cy);
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
  sprbase.drawString(cmds,cx,cy);
#endif
  //sprbase.setPaletteColor(0,externalInvert?1:0x0);
  //sprbase.setPaletteColor(1,externalInvert?0x0:1);
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
  clearSDIconCache();
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
    //Serial.print("INDEX CONFIG:");
    //Serial.print(i);
    //Serial.println(idx);
    String path = _EMW3_THEME_PATH;
    if(*bmpFilePath[idx] == '/') path = bmpFilePath[idx];
    else path += bmpFilePath[idx];
    //Serial.print("OPEN FILE: ");
    //Serial.println(path);
    //Serial.print("EXISTS: ");
    //Serial.println(SDFS.exists(path));
    spr.createFromBmpFile(Fs,path.c_str());  //从主题图片文件打开, 当styleBmps为3时,打开现有的图像
    //Serial.println("MIDSPRITE CREATE DONE\n");
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
    //Serial.println("SPRITE ALLOCATED DONE.\n");

    int16_t iw,ih; //宽度标签
    //Serial.printf("%d %d\n\n",spr_offset_x,spr_offset_y);
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
      if(ih>(themesel == themeTitle?title_height_max:height_max)
      -(themesel == themeTitle?title_spr_offset_y:spr_offset_y)) 
        ih=(themesel == themeTitle?title_height_max:height_max)-(themesel == themeTitle?title_spr_offset_y:spr_offset_y);
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
    //Serial.printf("SPRITE W: %d(%d) / H: %d(%d) SET DONE\n\n",iw,spr_offset_x,ih,spr_offset_y);
    //if(i<styleBmps){
      if(tsd[i] != nullptr) tsd[i]->deleteSprite();
      tsd[i]->createSprite(iw,ih);
      if(tsd[i]->getBuffer() == nullptr){ //设置失败了... 内存不足
        for(int j = 0; j<=i; j++) {
          tsd[j] ->deleteSprite();
          delete []tsd[j];
          tsd[j] = nullptr;
        }
        return 3; //内存不足
      }
      //Serial.println("SPRITE CREATED DONE\n");
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

uint8_t listMenuV2::drawDialog(const String *str,uint8_t dx,uint8_t dmultiw,uint8_t dmultih,int16_t x,int16_t y){
  const char *text_c_str[5];
  for(int i=0;i<5;i++) text_c_str[i] = str[i].c_str();
  return drawDialog(text_c_str,dx&63,dmultiw,dmultih,x,y); //String时不支持内嵌图标数组
}

uint8_t listMenuV2::drawDialog(const char **str,uint8_t dx,uint8_t dmultiw,uint8_t dmultih,int16_t x,int16_t y){ //x, y为左上角坐标
  LGFX_Sprite dlgspr(in_tft),logospr(in_tft);
  int16_t fw, fh=0;

  int16_t fmaxw = in_tft->width() - 32;
  int16_t gBtnWid = 32; //按钮宽度,在自定义字符串时候将被设置为64
  uint8_t retn = 0; //回车符号数
  const char *okText = CTG_CN_okText,
  *backText = CTG_CN_backText,
  *moreText = CTG_CN_moreText;

#ifndef LMV2_USE_DRAWMULYI_CANVAS_SUPPORT
  if(dmultiw) return 0; //不支持此功能
#endif
  dlgspr.setFont(&cn_font);
  logospr.setColorDepth(1);
  
  fw = dmultiw;
  if(!fw) fw = dlgspr.textWidth(str[0]);
  else fh = dmultih;
  if(!fh) fh = 12;
  for(size_t i=0;i<strlen(str[0]);i++) if(str[0][i] == '\n') retn++;
  if(retn){
    fh += 16 * retn;
    fw = 16384;
  }

  if(dx&96){
    if((dx&32) && (!(dx&64))){ //显示sd卡上的图标文件
      if(str[1][0] == '/') logospr.createFromBmpFile(*emw3_cnt_fs,str[1]);
      else if(str[1][0]) logospr.createFromBmpFile(*emw3_cnt_fs,(String(_EMW3_DIALOG_PATH)+str[1]).c_str());
      //Serial.println(String(_EMW3_DIALOG_PATH)+str[1]);
    }
    else if(dx&64){//显示数组的图标
      int16_t logosize = 16+((dx&32)>>1)+((dx&128)>>2); //图标数组大小
      logospr.createSprite(logosize,logosize);
      logospr.drawBitmap(0,0,(const uint8_t *)str[1],logosize,logosize,0,1);
    }
    fmaxw -= logospr.width()+4;
    if(fmaxw<96) fmaxw = 96; //限制最小大小

    if(fw+logospr.width()<20 && (dx&3) ==1) fw = 20 - logospr.width();
    if(fw+logospr.width()<92 && (dx&3) >=2) fw = 92 - logospr.width();
    if( logospr.height() > fh ) fh = LMV2_MIN( logospr.height(),in_tft->height() - 32);
  }
  else{
    if(fw<24 && (dx&3) ==1) fw = 24;//(dx&3) >=2 时候,总宽度最少112 像素, fh最少96 像素
    if(fw<96 && (dx&3) >=2) fw = 96;//(dx&3) ==1 时候,总宽度最少40 像素, fh最少24 像素
  }
  if(fw > fmaxw){
    fw = fmaxw;
    if(fh < 28) fh = 28;
  }
  if(!dmultiw &&(dlgspr.textWidth(str[0])>=(fmaxw*2) + 8/*此处的 8 以半个字符的宽度为宜*/ || (dx&8)))
    fh = in_tft->height() - 32;
  dlgspr.setColorDepth(1);
#ifdef LMV2_USE_DRAWMULYI_CANVAS_SUPPORT
  if(dmultiw) {
    dlgspr.createSprite(LMV2_MIN(dmultiw,fmaxw),LMV2_MIN(dmultih,in_tft->height() - 32));
    drawMulti(dlgspr,str[0],nullptr);
  }
  else{
#endif
  dlgspr.createSprite(fw,fh);
  dlgspr.fillSprite(1);   //从这里修改背景色等参数
  dlgspr.setTextColor(0);      //从这里修改前景色等参数
  dlgspr.setTextDatum(TC_DATUM);
  if(fh==16) dlgspr.drawString(str[0],fw/2,0);
  else {
    dlgspr.setTextDatum(TL_DATUM);
    dlgspr.setCursor(0,0);
    dlgspr.print(str[0]);
  }
#ifdef LMV2_USE_DRAWMULYI_CANVAS_SUPPORT
  }
#endif
  uint16_t w = fw+16;
  if(dx&96) w += logospr.width()+4;
  uint16_t h = fh+16+((dx&3)?20:0);
  if(h>in_tft->height()) h=in_tft->height();
  if(x==-32768 && y==-32768){
    x = (in_tft->width() - w)>>1;
    y = (in_tft->height() - h)>>1;
  }
  in_tft->fillRect(x>=2?x-2:0,y-2,w+4,h+4,1);
  themeRect(themeNormal,x,y,w,h);
  if(dx&96) {
    logospr.pushSprite(x+8,y+8);
    dlgspr.pushSprite(x+12+logospr.width(),y+8);
  }
  else dlgspr.pushSprite(x+8,y+8);  //显示文本信息 记得给图标留空哦

  if(dx&4){//支持自定义按钮文本, 可能导致对话框布局排版出问题
    if(str[2] != nullptr && str[2][0]) okText = str[2];
    if(str[3] != nullptr && str[3][0]) backText = str[3];
    if(str[4] != nullptr && str[4][0]) moreText = str[4];
    gBtnWid = (w-16)/3; //三个按钮宽度总和为192, 加上4像素空隙和边缘预留.
  }

  switch(dx&3){
    case 3:
    drawKeyText(x+w/2-4-gBtnWid,y+h-14,moreText,gBtnWid,20);
    case 2: //此处没有break. 因为case 3包含以下的情况.
    if((dx&3)!=3) {
      in_tft->drawFastHLine(x+w/2-36-15,y+h-5,gBtnWid-2);
      in_tft->drawFastHLine(x+w/2-36-16,y+h-6,gBtnWid);
      in_tft->drawFastHLine(x+w/2-36-15,y+h-7,gBtnWid-2);
    }
    drawKeyText(x+w/2+4+gBtnWid,y+h-14,backText,gBtnWid,20);
    case 1:
    drawKeyText(x+w/2,y+h-14,okText,gBtnWid,-20);
    break;
    default: return 0;
  }
  in_tft->display((dx&16)?2:3);
  uint8_t got = 0;
  if((dx&3)>=1){
    while(in_tft->getBtn(keyM)){
      if(in_tft->getBtn(keyL)==0 && (dx&3)==3){
        drawKeyText(x+w/2-4-gBtnWid,y+h-14,moreText,gBtnWid,-20);
        in_tft->display(3);
        while(in_tft->getBtn(keyL)==0) yield();
        got = 2; break;
      } 
      else if(in_tft->getBtn(keyR)==0 && (dx&3)>=2){
        drawKeyText(x+w/2+4+gBtnWid,y+h-14,backText,gBtnWid,-20);
        in_tft->display(3);
        while(in_tft->getBtn(keyR)==0) yield();
        got = 1; break;
      }
      yield();
    }
    if(got ==0) { drawKeyText(x+w/2,y+h-14,okText,gBtnWid,20);
        in_tft->display(3); }
    while(!in_tft->getBtn(keyM)) yield();
    delay(DEBOUNCE_DELAY_MS);
  }
  return got;
}

uint8_t listMenuV2::selectionList(uint8_t sel, const String* str, int16_t btnlen){
  const char * text_c_str[10];
  for(int i=0;i<LMV2_MIN(sel,10);i++) text_c_str[i] = str[i].c_str();
  return selectionList(sel,text_c_str,btnlen);
}
uint8_t listMenuV2::selectionList(uint8_t sel, const char ** str, int16_t btnlen){
  uint8_t csel = (sel+1)>>1; //总共条目数
  int16_t drx = in_tft->width()>>1;
  int16_t dry = in_tft->height()>>1;
  uint8_t cnt = 0; //选中的值
  while(in_tft->getBtn(keyM)==0) yield();
  delay(DEBOUNCE_DELAY_MS);
  if(csel>5) {
    setSelectionListCallback(nullptr,nullptr);
    return 0;
  }
  in_tft->setAutoFullRefresh(0);
  in_tft->setFont(&cn_font);
  if(!btnlen){
    for(uint8_t i = 0;i<sel; i++){
      if(btnlen<in_tft->textWidth(str[i])) btnlen = in_tft->textWidth(str[i]);
    }
    btnlen+=8;
    if((btnlen<<1)+18>in_tft->width()) btnlen=(in_tft->width()>>1)-9; //防止宽度太大
  }
  in_tft->fillRect(drx-btnlen-8,dry-11*csel-3,(btnlen<<1)+18,22*csel+6,1);
  themeRect(themeNormal,drx-btnlen-8,dry-11*csel-3,(btnlen<<1)+18,22*csel+6,0);
  for(;;){
    for(uint8_t i=0;i<sel;i++)
      drawKeyText(drx-btnlen/2-3+(i&1)*(btnlen+6),dry-11*(csel-(i&254))+11,str[i],btnlen,i==cnt?-20:20);
    if(selectionList_cb != nullptr) selectionList_cb(cnt,userdata);  //调用回调函数
    in_tft->display(3);
    for(;;){
    if(in_tft->getBtn(keyM)==0) {
  in_tft->setAutoFullRefresh(1);
      setSelectionListCallback(nullptr,nullptr);
      while(in_tft->getBtn(keyM)==0) yield();
      delay(DEBOUNCE_DELAY_MS);
      return cnt;
    }
    if(in_tft->getBtn(keyL)==0) {
      if(cnt == 0) cnt = sel;
      cnt--;
      break;
    }
    else if(in_tft->getBtn(keyR)==0) {
      if(cnt == sel-1) cnt = 0;
      else cnt++;
      break;
    }
    yield();
    }
  }
  in_tft->setAutoFullRefresh(1);
      setSelectionListCallback(nullptr,nullptr);
      return 0;
}
int32_t listMenuV2::slider(const char * str, int32_t minv, int32_t maxv, int32_t initialVal){
  LGFX_Sprite workspr(in_tft);
  int32_t slideSpeed = 1; //挡位,滑动速度
  int32_t iVal = initialVal; //当前值
  bool firstIn = 1; //初次进入
  int16_t sprw=0, sprh=0, pry;
  workspr.setFont(&cn_font);
  workspr.setColorDepth(1);
  in_tft->setAutoFullRefresh(0);
  while(in_tft->getBtn(keyM)==0) yield();
  delay(DEBOUNCE_DELAY_MS);
  if(str == nullptr || str[0] == 0) pry = (in_tft->height()>>1)-16;
  else if(workspr.textWidth(str)<=in_tft->width()-32) {
    pry = in_tft->height()>>1;
    sprw = workspr.textWidth(str);
    sprh = 20;
  }
  else {
    pry = (in_tft->height()>>1)+16;
    sprw = in_tft->width()-32;
    sprh = 40;
  }
  if(str != nullptr && str[0]){
    workspr.createSprite(sprw,sprh);
    workspr.fillScreen(1);
    workspr.setTextColor(0);
    workspr.setCursor(0,0);
    workspr.print(str);
    themeRect(themeNormal,(in_tft->width() - sprw-12)>>1,(in_tft->height()>>1)-32,
    workspr.width()+12,sprh+16); //先绘制框架
    workspr.pushSprite((in_tft->width() - sprw)>>1,(in_tft->height()>>1)-26);
    //in_tft->display(3);
    workspr.deleteSprite();
    workspr.createSprite(LMV2_SLIDER_D_WIDTH,16);
    for(;;){// slider的控制代码
      int32_t iVal2 =iVal; //备份原始数据, 用于比较
      if(in_tft->getBtn(keyL) == 0) {
        if(in_tft->getBtn(keyR) == 0) {
          drawKeyText(in_tft->width()>>1,in_tft->height()>>1,"取销操作",72,24);
          in_tft->display(3);
          in_tft->setAutoFullRefresh(1);
          setSliderCallback(nullptr,nullptr);
          while(in_tft->getBtn(keyL) == 0 || in_tft->getBtn(keyR) == 0) yield();
          delay(DEBOUNCE_DELAY_MS);
          return initialVal;
        }
        else iVal -= slideSpeed;
      }
      else if(in_tft->getBtn(keyR) == 0) iVal += slideSpeed;
      //Serial.printf("%d %d\n",iVal, iVal2);

      if(iVal<minv) iVal = minv;
      if(iVal>maxv) iVal = maxv;

      if(iVal != iVal2 || firstIn) {
        int32_t barW = map(iVal,minv,maxv,0,in_tft->width()-2);
        /*if(iVal >= iVal2)      */in_tft->fillRect(1,pry+1,barW,30,0);
        /*else if(iVal <= iVal2) */in_tft->fillRect(barW+1,pry+1,in_tft->width()-barW-2,30,1);

        if(in_tft->width() - barW>LMV2_SLIDER_D_WIDTH+4) {
          workspr.setTextDatum(middle_left);
          workspr.fillScreen(1);
          workspr.drawString(String(iVal),0,7);
          workspr.pushSprite(barW+2,pry+8);
        }
        else {
          workspr.setTextDatum(middle_right);
          workspr.fillScreen(0);
          workspr.setTextColor(1);
          workspr.drawString(String(iVal),LMV2_SLIDER_D_WIDTH-1,7);
          workspr.pushSprite(barW-LMV2_SLIDER_D_WIDTH,pry+8);
          workspr.setTextColor(0);
        }
        themeRect(themeNormal,0,pry,in_tft->width(),32,0);

        if(slider_cb != nullptr) slider_cb(iVal,userdata);  //调用回调函数
        in_tft->display(3);
        if(slideSpeed>=4) slideSpeed+=slideSpeed>>1;
        else slideSpeed++;
        firstIn = 0;
      }
      else slideSpeed = 1;
      if(in_tft->getBtn(keyM) == 0){
        in_tft->setAutoFullRefresh(1);
        setSliderCallback(nullptr,nullptr);
        while(in_tft->getBtn(keyM) == 0) yield();
        delay(DEBOUNCE_DELAY_MS);
        return iVal;
      }

      yield();
    }
  }
  in_tft->setAutoFullRefresh(1);
  setSliderCallback(nullptr,nullptr);
  return 0;
}