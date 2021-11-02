//****************************************************************

#ifndef FeederConfigH  // sentry
#define FeederConfigH

//****************************************************************
// Includes
//****************************************************************

#include <GnEsp8266Basics.h>
//#include <ESP8266WiFiType.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

//****************************************************************
// Sw Configuration
//****************************************************************

//****************************************************************
// typedefs
//****************************************************************

// from ESP8266WiFiType.h: WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3
typedef enum WiFiMode TWifiMode;	

typedef struct
 {
  #define CD( htmlType, typ, name, defValue ) typ name;
  #include "FeederConfig.inc.h"
 }
 TConfig;

//****************************************************************
// TFeederConfig
//****************************************************************

class TFeederConfig : public TSingleton<TFeederConfig>
 {
  friend class TSingleton<TFeederConfig>;
 private:
                      TFeederConfig(void);
 public:
  TConfig             FConfig;

 private:
  void                SetWifiMode(void);
  bool                Load( String const jsonStr );

 public:
  bool                Save(void);
  bool                Set( String const jsonStr )                 { return Load( jsonStr ) && Save(); }
 };
#define FeederConfig   TFeederConfig::Instance().FConfig
#define FeederConfigInstance   TFeederConfig::Instance()

//****************************************************************
#endif // sentry
