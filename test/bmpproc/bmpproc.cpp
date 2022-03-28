#include "bmpproc.h"
bmpproc:: bmpproc(fs::FS &bmpfs, const char *path){
  goError = 0; //错误原因
  bmpf = bmpfs.open(path,"r");
  if(bmpf){
    uint8_t cfg[10];
    if(bmpf.read(cfg,10)!=10) goError = 2; //文件损坏
    bmpf.seek(0x0a);
    offset = bmpf.read();
    bmpf.seek(0x1c);
    bpp = bmpf.read();
    bmpf.seek(0x1e);
    uint8_t compress = bmpf.read();
    if(compress || cfg[0]!='B' || cfg[1]!='M' || cfg[6] || cfg[7] || cfg[8] || cfg[9]) goError = 4;//不符合bmp标准, 
    else {
      if(bpp & 7) goError = 8; // 仅支持16,24,32位位图, 返回此值则说明不支持的位数.
    }
    bmpf.seek(0x12); //计算宽度高度
    w = (uint32_t)bmpf.read() | (uint32_t(bmpf.read())<<8) | (uint32_t(bmpf.read())<<16) | (uint32_t(bmpf.read())<<24);
    h = (uint32_t)bmpf.read() | (uint32_t(bmpf.read())<<8) | (uint32_t(bmpf.read())<<16) | (uint32_t(bmpf.read())<<24);
    w3 = uint32_t(w*(bpp>>3)+3) & ~3u;//一行的像素数
  }
  else goError = 1;
}
uint32_t bmpproc::get(uint32_t x, uint32_t y){
  if(goError || x>=w || y>=h || (bpp>>3) <= 2) return 0x7fffffffu; //防止错读
  bmpf.seek(offset+w3*(h-y-1)+x*(bpp>>3));
  return uint32_t(bmpf.read()) | (uint32_t(bmpf.read())<<8) | (uint32_t(bmpf.read())<<16);
}
uint8_t bmpproc::getGray(uint32_t x, uint32_t y, bool gamma_on){
  uint32_t got = get(x,y);
  // 开启gamma校正, 视觉颜色比例为1:1.5:0.6
  if(gamma_on) return (uint8_t)(powf(powf(((got&0xff0000)>>16),2.2f)+powf(((got&0xff00)>>8)*1.5f,2.2f)+powf((got&0xff)*0.6f,2.2f),1/2.2f)*0.547373141f);
  return (((got&0xff0000)>>15)+((got&0xff00)>>6)+(got&0xff))/7; //关闭gamma校正, 红:绿:蓝 明度比约为2:4:1, 
}

// 以下函数为EMW3的专有函数, 不可以移植到其他平台
#ifdef BAYER_GRAYSCALE_ALG
const uint8_t bmpproc::bayer_tab [64]={
   0, 32,  8, 40,  2, 34, 10, 42,
  48, 16, 56, 24, 50, 18, 58, 26,
  12, 44,  4, 36, 14, 46,  6, 38,
  60, 28, 52, 20, 62, 30, 54, 22,
   3, 35, 11, 43,  1, 33,  9, 41,
  51, 19, 59, 27, 49, 17, 57, 25,
  15, 47,  7, 39, 13, 45,  5, 37,
  63, 31, 55, 23, 61, 29, 53, 21
};
#endif
void bmpproc::drawBW(EMW3 &emw3, int x, int y, int w, int h){
  LGFX_Sprite spr(&emw3);
  spr.setColorDepth(1);
  if(!w) w = width();
  if(!h) h = height();
  if(x+w<=0 || y+h<=0) return;
  int sprw = x<0?min(emw3.width(),w+x):min(w,emw3.width()-x);
  int sprh = y<0?min(emw3.height(),w+y):min(h,emw3.height()-y);
  int minusx = x<0?map(0,x,x+w,0,width()):0;
  int minusy = y<0?map(0,y,y+h,0,height()):0;
  spr.createSprite(sprw,sprh);
#ifdef FLOYD_STEINBERG_GRAYSCALE_ALG
    int16_t *floyd_tab[2];
    floyd_tab[0] = new int16_t [sprw];
    floyd_tab[1] = new int16_t [sprw];
    for(int j=0;j<sprw;j++){ floyd_tab[0][j] = 0; floyd_tab[1][j] = 0; }
#endif
    for(int i=0;i<sprh;i++){
      for(int j=0;j<sprw;j++){
        //Serial.printf("%02x ",getGray(map(j,0,emw3.width()-1,0,width()-1),map(i,0,emw3.height()-1,0,height()-1)));
#ifdef FLOYD_STEINBERG_GRAYSCALE_ALG
        int16_t flodelta = floyd_tab[i&1][j]+(int16_t)(getGray(map(j,0,w+(x<0?x:0)-1,
          minusx,width()-1),map(i,0,h+(y<0?y:0)-1,minusy,height()-1))<<4);//此步骤将会计算出当前像素到底显示什么颜色
          
        if(flodelta>=2048) { spr.drawPixel(j,i,1); flodelta = - 4095 + flodelta; }
        //计算出读取到的颜色, 然后与128比较, 如果小于128, 显示黑色,否则显示白色
        else              { spr.drawPixel(j,i,0); }
        if(j!=sprw-1) { floyd_tab[i&1]   [j+1] += (flodelta*7)>>4; }
        if(j)         { floyd_tab[!(i&1)][j-1] += (flodelta*3)>>4; }
                      { floyd_tab[!(i&1)][j  ] += (flodelta*5)>>4; }
        if(j!=sprw-1) { floyd_tab[!(i&1)][j+1] += (flodelta  )>>4; }
#endif
#ifdef BAYER_GRAYSCALE_ALG
        spr.drawPixel(j,i,bayer_tab[((j&7)<<3)|(i&7)]<(getGray(map(j,0,w+(x<0?x:0)-1,minusx,width()-1),map(i,0,h+(y<0?y:0)-1,minusy,height()-1))>>2));
#endif
        yield();
      }
#ifdef FLOYD_STEINBERG_GRAYSCALE_ALG
      for(int floi=0;floi<sprw;floi++) floyd_tab[i&1][floi]={0};
#endif
    }
#ifdef FLOYD_STEINBERG_GRAYSCALE_ALG
    delete[] floyd_tab[0];
    delete[] floyd_tab[1];
#endif
  emw3.fillRect(x>0?x:0,y>0?y:0,sprw,sprh,1);
  spr.pushSprite((x>0?x:0),(y>0?y:0));
  emw3.display(7); //先全白刷屏
  emw3.display(3);
}
void bmpproc::draw16Gray(EMW3 &emw3, int x, int y, int w, int h, bool gamma_on){
  LGFX_Sprite spr(&emw3);
  spr.setColorDepth(4);
  spr.setPaletteGrayscale();
  if(!w) w = width();
  if(!h) h = height();
  if(x+w<=0 || y+h<=0) return;
  int sprw = x<0?min(emw3.width(),w+x):min(w,emw3.width()-x);
  int sprh = y<0?min(emw3.height(),w+y):min(h,emw3.height()-y);
  int minusx = x<0?map(0,x,x+w,0,width()):0;
  int minusy = y<0?map(0,y,y+h,0,height()):0;
  spr.createSprite(sprw,sprh);
    for(int i=0;i<sprh;i++){
      for(int j=0;j<sprw;j++){
        //Serial.printf("%02x ",getGray(map(j,0,emw3.width()-1,0,width()-1),map(i,0,emw3.height()-1,0,height()-1)));
        spr.drawPixel(j,i,getGray(map(j,0,w+(x<0?x:0)-1,minusx,width()-1),map(i,0,h+(y<0?y:0)-1,minusy,height()-1),gamma_on)>>4);
        yield();
      }
    }
  emw3.push16bitSprite(spr,(x>0?x:0),(y>0?y:0));
}