//****************************************************************
// Includes
//****************************************************************

//#include <stdio.h>
//#include <string.h>
#include "FeedTimes.h"
#include <LittleFS.h>
#include "FeederWifi.h"
#include "FeederServo.h"
#include <ArduinoJson.h>

//****************************************************************
// TFeedTimes
//****************************************************************

TFeedTimes::TFeedTimes(void)
 {
  ClrVar( FFeedTimesNvData.FeedRecords );
  Load( ReadFile( F( "/FeedTimes.json" ) ) );
 }

void TFeedTimes::SetTimeString( uint8_t r, String argVal )
 {
  argVal.trim();
  if( argVal.length() != 5 )
    return;
  TFeedRecord & fr = FFeedTimesNvData.FeedRecords[ r ];
  fr.h = argVal.substring( 0, 2 ).toInt();
  fr.m = argVal.substring( 3, 5 ).toInt();
  //D Debug( "TFeedTimes::SetTimeString: r:%u, argVal:%s, h-str:%s, m-str:%s, h:%u, m:%u\n", r, argVal.c_str(), argVal.substring( 0, 2 ).c_str(), argVal.substring( 3, 5 ).c_str(), fr.h, fr.m );
 }

uint8_t TFeedTimes::CheckTime( struct tm const & tm )
 {
  for( int r = 0;  r < MaxFeedRecords;  ++r )
   {
    TFeedRecord & fr = FFeedTimesNvData.FeedRecords[ r ];
    if( ( fr.wday.mask &  0x80 )                    // active ?
     && ( fr.wday.mask & ( 1 << tm.tm_wday ) )      // active for this wday ?
     && fr.h == tm.tm_hour && fr.m  == tm.tm_min )  // Time Equal ?
      return fr.portions;                           // All yes -> return portions
   }
  return 0;
 }

bool TFeedTimes::Load( String const & jsonStr )
 {
  JsonDocument doc;
  DeserializationError error = deserializeJson( doc, jsonStr );
  //D Debug( "TFeedTimes::Load: %s, bool:%s\njson: %s<<<\n", error ? "NOK" : "OK", error.c_str(), jsonStr.c_str() );
  if( error != DeserializationError::Ok ) // if( error ) ???
    return false;
  JsonArray arr = doc.as<JsonArray>();
  uint8_t r = 0;
  for( JsonVariant value : arr )
   {
    TFeedRecord & fr = FFeedTimesNvData.FeedRecords[ r ];
    JsonArray row = value;
    SetTimeString( r++, row[ 0 ].as<String>() );
    fr.wday.mask = row[ 1 ].as<uint8_t>();
    fr.portions = row[ 2 ].as<uint8_t>();
   }
  return true;
 }

bool TFeedTimes::Save(void)
 {
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();;
  for( uint8_t r = 0;  r < MaxFeedRecords;  ++r )
   {
    JsonArray row = arr.add<JsonArray>();
    TFeedRecord & fr = FFeedTimesNvData.FeedRecords[ r ];
    row.add( TimeString( fr.h, fr.m ) );
    row.add( fr.wday.mask );
    row.add( fr.portions );
   }

  File f = LittleFS.open( "/FeedTimes.json", "w" );
  if( !f ) 
   {
    Debug( "Can't open file for writing: /FeedTimes.json\n" );
    return false;
   }
  serializeJson( doc, f );
  f.close();
  return true;
 }

void TFeedTimes::Loop(void)
 {
  if( !FeederWifi.TimeIsValid() )
    return;

  static struct tm tmLast;

  time_t timeNow;
  struct tm tm;                     // the structure tm holds time information in a more convient way

  time( &timeNow );                 // read the current time
  localtime_r( &timeNow, &tm );     // split time into parts

  if( tmLast.tm_min != tm.tm_min ) // every min
   {
    Debug( "TFeedTimes::Loop every min\n" );
    uint8_t portions = CheckTime( tm );
    if( portions > 0 )
      FeederServo.Feed( portions );
   }
  tmLast = tm;
 }
