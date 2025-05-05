//****************************************************************

#ifndef FeederWifiH  // sentry
#define FeederWifiH

//****************************************************************
// Includes
//****************************************************************

#include <GnBasics.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <WebServer.h>
  #include <Update.h>
 #else
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266HTTPUpdateServer.h>
#endif
#include <LittleFS.h>
#include <ArduinoJson.h>


//****************************************************************
// Sw Configuration
//****************************************************************

//****************************************************************
// TFeederWifi
//****************************************************************

class TFeederWifi : public TSingleton<TFeederWifi>, TLoopInstance
 {
  friend class TSingleton<TFeederWifi>;
 private:
                      TFeederWifi(void);
 protected:
  virtual void        Loop(void);

 public:
  bool                Setup(void);
  bool                TimeIsValid(void)                                               { return STimeIsValid; }

 private: // Wifi
  bool                SetupWifi(void);

 private: // NTP
  static bool         STimeIsValid;
  static void         STimeIsSet_cb(void);

  bool                SetupNTP(void);

 private: // HttpServer & FwUpdater
  static void         SHandleNotFound(void);

  static void         SSendPostResponce( bool ok, bool reboot );

  static bool         SChkPost( String postName );

  static void         SHandleSetFeedTimes(void);
  static void         SHandleManualFeed(void);
  static void         SHandleStopFeed(void);
  static void         SHandleMFeedChk(void);
  static void         SHandleSetConfig(void);
  static void         SHandleSetServoSteps(void);
  static void         SHandleFwUpdate(void);
  static void         SFwUploader(void);

  ESP8266WebServer    FHttpServer;
  bool                SetupHttpServer(void);

  static void         SetErrorFromUpdater();
  static String       FUpdaterError;
  static String       FUploadName;
 };

#define FeederWifi    TFeederWifi::Instance()

//****************************************************************
#endif // sentry
