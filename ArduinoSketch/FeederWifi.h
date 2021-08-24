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

 private: // HttpServer & FwUpdater
  static void         SHandleNotFound(void);

  static void         SSendPostResponce( bool ok, bool reboot );

  static bool         SChkPost( String postName );

  static void         SHandleSetFeedTimes(void);
  static void         SHandleSetConfig(void);
  static void         SHandleFwUpdate(void);
  static void         SFwUploader(void);
  static void         SHandleIndex(void);

  ESP8266WebServer    FHttpServer;
  bool                SetupHttpServer(void);

  static void         SetErrorFromUpdater();
  static String       FUpdaterError;
  static String       FUploadName;
 };

#define FeederWifi    TFeederWifi::Instance()

//****************************************************************
#endif // sentry
