//****************************************************************

#ifndef FeederWifiH  // sentry
#define FeederWifiH

//****************************************************************
// Includes
//****************************************************************

#include <GnEsp8266Basics.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

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

 private: // HttpServer
  static void         SHandleNotFound(void);
  static void         SHandleRoot(void);
  static void         SHandleData(void);
  static void         SHandleSet(void);

  ESP8266WebServer    FHttpServer;
  bool                SetupHttpServer(void);

 private: // OTA
  void                SetupHttpUpdater(void);
  void                SetupArduinoOTA(void);
  bool                SetupOTA(void);

  ESP8266HTTPUpdateServer FHttpUpdater;
 };

#define FeederWifi    TFeederWifi::Instance()

//****************************************************************
#endif // sentry
