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

typedef struct
 {
  #define CD( htmlType, typ, name, defValue ) typ name;
  #include "FeederConfig.inc.h"
  WiFiMode_t          WifiMode;
 }
 TConfig;

//****************************************************************
// TFeederConfig
//****************************************************************

class TFeederConfig : public TSingleton<TFeederConfig>
 {
  friend class TSingleton<TFeederConfig>;
  typedef StaticJsonDocument< 1000 > TJsonDoc;
 private:
                      TFeederConfig(void);
 private:
  void                SetWifiMode(void);
  bool                Load( TJsonDoc & doc );

 public:
  TConfig             FConfig;

 public:
  bool                Load(void);
  void                Save(void);
  bool                Set( String const & jdata );
 };
#define FeederConfig   TFeederConfig::Instance().FConfig
#define FeederConfigInstance   TFeederConfig::Instance()

//****************************************************************
#endif // sentry
