//****************************************************************
// Includes
//****************************************************************

//#include <stdio.h>
//#include <string.h>
#include "FeederConfig.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

//****************************************************************
// Default-Config
//****************************************************************

static TConfig DefaultConfig =
 {
  #define CD( typ, name, defValue ) defValue,
  #include "FeederConfig.inc.h"
 };

//****************************************************************
// TFeederConfig
//****************************************************************

TFeederConfig::TFeederConfig(void)
: FirstLoadOk( false )   //Debug
 {
  //d Debug( "TFeederConfig::TFeederConfig\n" );
  FirstLoadOk = Load(); //Debug
 }

String TFeederConfig::ReadFile( char const * fileName )
 {
  File f = LittleFS.open( fileName, "r" );
  if( !f ) 
   {
    Debug( "Can't open file: %s\n", fileName );
    return "";
   }
  String res = f.readString();
  f.close();
  return res;
 }

void TFeederConfig::Save(void)
 {
  File f = LittleFS.open( "/Config.json", "w" );
  if( !f ) 
   {
    Debug( "Can't open file for writing: /Config.json\n" );
    return;
   }

  StaticJsonDocument< 1000 > doc;

  #define CD( typ, name, defValue ) doc[ #name ] = FConfig.name;
  #include "FeederConfig.inc.h"

  serializeJson( doc, f );

  f.close();
 }

bool TFeederConfig::Load(void)
 {
  FConfig = DefaultConfig;

  File f = LittleFS.open( "/Config.json", "r" );
  if( !f ) 
   {
    Debug( "Can't open file: /Config.json\n" );
    return false;
   }

  StaticJsonDocument< 1000 > doc;

  DeserializationError error = deserializeJson( doc, f );
  f.close();

  if( error )
   {
    Debug( "File: '/Config.json' deserializeJson() failed: %s\n", error.f_str() );
    return false;
   }

  // Load
  #define CD( typ, name, defValue ) FConfig.name = doc[ #name ].as<typ>();
  #include "FeederConfig.inc.h"

  return true;
 }

void TFeederConfig::DebugShow(void)
 {
  Debug( "\nFeederConfig: FirstLoadOk: %s\n", FirstLoadOk ? "OK" : "Fail" );

  // Debug (show all values)
  #define CD( typ, name, defValue ) CD_##typ( name )
  #define CD_bool( name ) Debug( #name ": %s\n", FConfig.name ? "true" : "false" );
  #define CD_int( name ) Debug( #name ": %d\n", FConfig.name );
  #define CD_String( name ) Debug( #name ": %s\n", FConfig.name.c_str() );
  #include "FeederConfig.inc.h"

  Debug( "\n" );
 }
