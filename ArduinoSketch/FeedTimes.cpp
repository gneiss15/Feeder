//****************************************************************
// Includes
//****************************************************************

//#include <stdio.h>
//#include <string.h>
#include "FeedTimes.h"
#include <LittleFS.h>
#include "FeederWifi.h"
#include "FeederServo.h"

//****************************************************************
// TFeedTimes
//****************************************************************

TFeedTimes::TFeedTimes(void)
 {
  Load();
 }

void TFeedTimes::SetTimeString( uint8_t r, String argVal )
 {
  argVal.trim();
  if( argVal.length() != 5 )
    return;
  TFeedRecord & fr = FNvData.FeedRecords[ r ];
  fr.h = argVal.substring( 0, 2 ).toInt();
  fr.m = argVal.substring( 3, 5 ).toInt();
 }

int8_t TFeedTimes::CheckTime( struct tm const & tm )
 {
  for( int r = 0;  r < MaxFeedRecords;  ++r )
   {
    TFeedRecord & fr = FNvData.FeedRecords[ r ];
    if( fr.h == tm.tm_hour && fr.m  == tm.tm_min && ( fr.wday.mask & ( 1 << tm.tm_wday ) ) )
      return fr.portions;
   }
  return -1;
 }
 
bool TFeedTimes::Load( TJsonDoc & doc )
 {
  Debug( "TFeedTimes::Load\n" );
  JsonArray arr = doc.as<JsonArray>();
  uint8_t r = 0;
  for( JsonVariant value : arr )
   {
    JsonArray row = value.as<JsonArray>();
    TFeedRecord & fr = FNvData.FeedRecords[ r ];

    Debug( "r: %d, Time: %s, wday.mask: %d, portions: %d\n", r, row[ 0 ].as<String>(), row[ 1 ].as<uint8_t>(), row[ 2 ].as<uint8_t>() );

    SetTimeString( r, row[ 0 ].as<String>() ); // TimeString
    fr.wday.mask = row[ 1 ].as<uint8_t>();
    fr.portions = row[ 2 ].as<uint8_t>();
    r++;
   }
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
    int8_t portions = CheckTime( tm );
    if( portions > 0 )
      FeederServo.Start( ServoDefault_MinPulseWidthUs, ServoDefault_MaxPulseWidthUs, portions );
   }
  tmLast = tm;
 }

bool TFeedTimes::Load(void)
 {
  ClrVar( FNvData.FeedRecords );

  File f = LittleFS.open( "/Static/FeedTimes.json", "r" );
  if( !f ) 
   {
    Debug( "Can't open file: /Static/FeedTimes.json\n" );
    return false;
   }

  TJsonDoc doc;
  DeserializationError error = deserializeJson( doc, f );
  f.close();
  if( error )
   {
    Debug( "File: '/Static/FeedTimes.json' deserializeJson() failed: %s\n", error.f_str() );
    return false;
   }

  Load( doc );
  return true;
 }

void TFeedTimes::Save(void)
 {
  TJsonDoc doc;
  JsonArray arr = doc.to<JsonArray>();;
  for( uint8_t r = 0;  r < MaxFeedRecords;  ++r )
   {
    JsonArray row = arr.createNestedArray();
    TFeedRecord & fr = FNvData.FeedRecords[ r ];
    row.add( TimeString( fr.h, fr.m ) );
    row.add( fr.wday.mask );
    row.add( fr.portions );
   }

  String msg;
  serializeJson( doc, msg );
  Debug( "TFeederConfig::Save: %s\n", msg );

  #if 1
  File f = LittleFS.open( "/Static/FeedTimes.json", "w" );
  if( !f ) 
   {
    Debug( "Can't open file for writing: /Static/FeedTimes.json\n" );
    return;
   }
  f.print( msg );
  //d serializeJson( doc, f );
  f.close();
  #endif
 }

bool TFeedTimes::Set( String const & jdata )
 {
  Debug( "FeedTimes: Set: %s\n", jdata );
  TJsonDoc doc;
  DeserializationError error = deserializeJson( doc, jdata );
  if( error )
   {
    Debug( "TFeedTimes::Set: deserializeJson() failed: %s\n", error.f_str() );
    return false;
   }
  if( !Load( doc ) )
    return false;
  Save();
  return true;
 }
