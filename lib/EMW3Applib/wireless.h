/******* FRIENDSHIPENDER *****
 * @file wireless.h
 * @author FriendshipEnder
 * @brief 连接到WiFi
 * 
   --------JSON编码示例程序--------
  DynamicJsonDocument wifiList(512);
  wifiList["n"] = 4;
  JsonArray dat = wifiList.createNestedArray("w");
  const char *s[8] = {
    "我的WiFi SSID","12345678",
    "可莉和绫人","87654321",
    "神工天巧","10086abcdef",
    "TP_LINK_0000","13578642"
  };
  for(int i=0;i<4;i++){
    JsonObject wifiinfo = dat.createNestedObject();
    wifiinfo["s"] = s[i*2];
    wifiinfo["p"] = s[i*2+1];
  }
  File wificfg = SDFS.open(WIRELESS_WIFI_JSON_PATH,"w");
  serializeJsonPretty(wifiList,wificfg);
  wificfg.close();
  
  --------JSON解码示例程序--------
  SDFS.begin(EMW3_SD_CS_PIN);
  File jsonFile = SDFS.open(WIRELESS_WIFI_JSON_PATH,"r");
  if(!jsonFile) for(;;) yield();
  DynamicJsonDocument wifiList(512);
  DeserializationError error = deserializeJson(wifiList,jsonFile);
    if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    for(;;) yield();
  }
  Serial.println(wifiList["w"][0]["s"].as<const char *>());
  Serial.println(wifiList["w"][0]["p"].as<const char *>());


 * @version 0.1
 * Update:  2022-03-13
 * 初次创建
 */
#ifndef _WIRELESS_H_FILE
#define _WIRELESS_H_FILE
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <emw3.h>
#include <ESP_EEPROM.h>
#include <listMenuV2.h>
#include <emw3keyboard.h>
#include <TimeLib.h>
#include <WiFiUDP.h>
//#include "EMW3App.h"

#define WIRELESS_WIFI_JSON_PATH   "/EMW3/wifi.json"
#define WIFI_CONNECT_MAX_TIME_MS  10000
#define _NTP_SERVERS              4                         //NTP 服务器数
//#define WIRELESS_DEBUG
class wireless{
public:
  wireless(EMW3 *emw3base, listMenuV2 *in_menu) : tft(emw3base), menu(in_menu), 
    wireless_fs(&SDFS), wjsonDoc(nullptr){
    menu->setFS(SDFS);
  }
  inline void setFS(fs::FS &ing_FS){ wireless_fs = &ing_FS; menu->setFS(ing_FS); }
  uint8_t connectToWiFi_direct(bool allowSearch = 1, const String &ssid = emptyString, const String &pswd = emptyString);
  uint8_t connectToWiFi_search();
  uint8_t connectToWiFi_scan();
  /// @return 0 正常选择
  uint8_t connectToWiFi_man();
  uint8_t connectToWiFi_dis();
  //void autoSyncTimeNtp();
  //bool syncTimeNtp();
  
    /*----------------- NTP code ------------------*/
    #define NTP_PACKET_SIZE 48 // NTP time is in the first 48 bytes of message
    //static uint8_t packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
    //time_t get_ntp_time_impl(uint8_t _server);
    //void sendNTPpacket(IPAddress &address);
    /// @brief 获取网络时间
    time_t getNTPTime();
    /*----------------- NTP code end ------------------*/
private:
  /// @brief 扫描WiFi, 需要手动删除WiFI扫描结果
  int selectWiFi();
  /// @brief 向json加数据 
  void addWiFiToJson(const char *, const char *);
  /** @brief 输入的SSID,从保存的JSON列表里找出密码
   *  @param s SSID
   *  @param wifiList JSON列表
   *  @return String 密码, 未发现则返回空串
   */ 
  String selectWiFiFromJson(const char *s, DynamicJsonDocument &wifiList);
  friend void show_psk(uint16_t stage, void *jsonDoc);
  friend class EMW3App; //此声明意味着 EMW3App是自己人, 资源随便访问
  EMW3 * tft;
  listMenuV2 * menu;
  fs::FS * wireless_fs;
  DynamicJsonDocument *wjsonDoc;
  static String wl_ssid;
  static String wl_psk;
    /// @brief 屏幕内容缓存, 用于截屏,只支持单色截屏
  
    /*----------------- NTP code ------------------*/
    WiFiUDP udp; //用于校对时间
    //#define NTP_PACKET_SIZE 48 // NTP time is in the first 48 bytes of message
    static uint8_t packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
    time_t get_ntp_time_impl(uint8_t _server);
    //void sendNTPpacket(IPAddress &address);
    //time_t getNTPTime();
    
static const char * ntpServerName[_NTP_SERVERS];

static const int timeZone; //BeiJing in China
static unsigned int localPort;  // local port to listen for UDP packets
    /*----------------- NTP code end ------------------*/
};
#endif