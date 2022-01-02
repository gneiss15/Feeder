//****************************************************************
// Includes
//****************************************************************

//#include <stdio.h>
//#include <string.h>
#include "FeederConfig.h"
#include <LittleFS.h>

//****************************************************************
// Default-Config
//****************************************************************

static TConfig DefaultConfig =
 {
  #define CD( htmlType, typ, name, defValue ) defValue,
  #include "FeederConfig.inc.h"
 };

//****************************************************************
// TFeederConfig
//****************************************************************

TFeederConfig::TFeederConfig(void)
 {
  FConfig = DefaultConfig;
  Load( ReadFile( F( "/Config.json" ) ) );
 }

bool TFeederConfig::Load( String const jsonStr )
 {
  DynamicJsonDocument doc( 512 );
  DeserializationError error = deserializeJson( doc, jsonStr );
  //D Debug( "TFeederConfig::Load: %s\njson: %s<<<\n", error.c_str(), jsonStr.c_str() );
  if( error != DeserializationError::Ok ) // if( error ) ???
    return false;
  #define CD( htmlType, typ, name, defValue ) FConfig.name = doc[ #name ].as<typ>();
  #include "FeederConfig.inc.h"
  return true;
 }

bool TFeederConfig::Save(void)
 {
  //StaticJsonDocument<512> doc;
  DynamicJsonDocument doc( 512 );
  #define CD( htmlType, typ, name, defValue ) doc[ #name ] = FConfig.name;
  #include "FeederConfig.inc.h"

  File f = LittleFS.open( "/Config.json", "w" );
  if( !f ) 
   {
    Debug( "Can't open file for writing: /Config.json\n" );
    return false;
   }
  serializeJson( doc, f );
  f.close();
  return true;
 }
