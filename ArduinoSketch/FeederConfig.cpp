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
  WIFI_AP
 };

//****************************************************************
// TFeederConfig
//****************************************************************

TFeederConfig::TFeederConfig(void)
 {
  Load();
 }

void TFeederConfig::SetWifiMode(void)
 {
  FConfig.WifiMode = FConfig.WlanEnabled ? FConfig.ApEnabled ? WIFI_AP_STA : WIFI_STA : WIFI_AP;
 }


bool TFeederConfig::Load( TJsonDoc & doc )
 {
  #define CD( htmlType, typ, name, defValue ) FConfig.name = doc[ #name ].as<typ>();
  #include "FeederConfig.inc.h"
  SetWifiMode();
  return true;
 }

bool TFeederConfig::Load(void)
 {
  FConfig = DefaultConfig;
  SetWifiMode();

  File f = LittleFS.open( "/Static/Config.json", "r" );
  if( !f ) 
   {
    Debug( "Can't open file: /Static/Config.json\n" );
    return false;
   }

  TJsonDoc doc;
  DeserializationError error = deserializeJson( doc, f );
  f.close();
  if( error )
   {
    Debug( "File: '/Static/Config.json' deserializeJson() failed: %s\n", error.f_str() );
    return false;
   }

  Load( doc );
  return true;
 }

void TFeederConfig::Save(void)
 {
  File f = LittleFS.open( "/Static/Config.json", "w" );
  if( !f ) 
   {
    Debug( "Can't open file for writing: /Static/Config.json\n" );
    return;
   }

  TJsonDoc doc;
  #define CD( htmlType, typ, name, defValue ) doc[ #name ] = FConfig.name;
  #include "FeederConfig.inc.h"
  SetWifiMode();

  serializeJson( doc, f );

  f.close();
 }
 
bool TFeederConfig::Set( String const & jdata )
 {
  TJsonDoc doc;
  DeserializationError error = deserializeJson( doc, jdata );
  if( error )
   {
    Debug( "TFeederConfig::Set: deserializeJson() failed: %s\n", error.f_str() );
    return false;
   }
  if( !Load( doc ) )
    return false;
  Save();
  return true;
 }
