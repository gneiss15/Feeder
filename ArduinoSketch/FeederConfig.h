//****************************************************************

#ifndef FeederConfigH  // sentry
#define FeederConfigH

//****************************************************************
// Includes
//****************************************************************

#include <GnEsp8266Basics.h>

//****************************************************************
// Sw Configuration
//****************************************************************

//****************************************************************
// typedefs
//****************************************************************

typedef struct
 {
  #define CD( typ, name, defValue ) typ name;
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
  bool                FirstLoadOk;   //Debug

 private:
  String              ReadFile( char const * fileName );

 public:
  bool                Load(void);
  void                Save(void);
  void                DebugShow(void);

                      // Get Html-Page
  String              GetHtmlPage(void) { return ReadFile( "/Config.html" ); }
                      // Get Paras as JSON object (for Html-Page)
  String              GetJson(void) { return ReadFile( "/Config.json" ); };
 };
#define FeederConfig   TFeederConfig::Instance().FConfig

//****************************************************************
#endif // sentry
