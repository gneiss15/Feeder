//****************************************************************
// Includes
//****************************************************************

//#include <stdio.h>
//#include <string.h>
#include "FeederParas.h"
#include <LittleFS.h>
#include "FeederWifi.h"
#include "FeederServo.h"

//****************************************************************
// TFeederParas
//****************************************************************

TFeederParas::TFeederParas(void)
: FNvData()
 {
  //d Debug( "TFeederParas::TFeederParas\n" );
  Load();
 }

void TFeederParas::Loop(void)
 {
  if( !FeederWifi.TimeIsValid() )
    return;

  static struct tm tmLast;

  time_t timeNow;
  struct tm tm;                     // the structure tm holds time information in a more convient way

  time( &timeNow );                 // read the current time
  localtime_r( &timeNow, &tm );     // split time into parts

  //d Debug( "TFeederParas::Loop\n" );

  #if 0 //d 
  {
   static bool done = false;
   if( !done )
    {
      done = true;
      Debug( "sizeof( TWdayMask ): %u\n", sizeof( TWdayMask ) );
      Debug( "sizeof( TWdayUnion ): %u\n", sizeof( TWdayUnion ) );
      Debug( "sizeof( TFeedRecord ): %u\n", sizeof( TFeedRecord ) );
      Debug( "sizeof( TNvData ): %u\n", sizeof( TNvData ) );
    }
  }
  #endif

  if( tmLast.tm_min != tm.tm_min ) // every min
   {
    Debug( "TFeederParas::Loop every min\n" );

    uint8_t portions = CheckTime( tm );
    if( portions > 0 )
      FeederServo.Start( ServoDefault_MinPulseWidthUs, ServoDefault_MaxPulseWidthUs, portions );
   }
  tmLast = tm;
 }

void TFeederParas::Clear(void)
 {
  ClrVar( FNvData.FeedRecords );
 }

String TFeederParas::ReadFile( char const * fileName )
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

void TFeederParas::Setup(void)
 {
  Load();
 }

String TFeederParas::GetJson(void)
 {
  // Create DATA content
  String data;
  for( int i = 0;  i < MaxFeedRecords;  ++i )
   {
    TFeedRecord & fr = FNvData.FeedRecords[ i ];
    data += "[\"" + TimeString( fr.h, fr.m ) + "\"," + fr.wday.mask + ",\"" + fr.portions + "\"],";
   }
  data.remove( data.length() - 1 );
  return "[" + data + "]";    
 }

void TFeederParas::Set( uint8_t r, uint8_t c, String argVal )
 {
  TFeedRecord & fr = FNvData.FeedRecords[ r ];
  if( c == 9 ) // Portionen
    fr.portions = argVal.toInt() - 1;
   else if( c == 8 ) // Time
   {
    fr.h = argVal.substring( 0, 2 ).toInt();
    fr.m = argVal.substring( 3, 5 ).toInt();
   }
   else
    fr.wday.mask |= ( 1 << c );
 }

void TFeederParas::Save(void)
 {
  File f = LittleFS.open( "/FeedRecords.bin", "wb" );
  if( !f ) 
   {
    Debug( "Can't open file for writing:/FeedRecords.bin\n" );
    return;
   }

  size_t bytes = f.write( FNvData.AsChars, sizeof( FNvData )  );
  f.close();

  if( bytes == sizeof( FNvData ) )
    return;
  Debug( "Write failed (wrong # of bytes), file: /FeedRecords.bin\n" );
 }

bool TFeederParas::Load(void)
 {
  File f = LittleFS.open( "/FeedRecords.bin", "rb" );
  if( !f ) 
   {
    Debug( "Can't open file: /FeedRecords.bin\n" );
    Clear();
    return false;
   }

  size_t bytes = f.readBytes( FNvData.AsChars, sizeof( FNvData ) );
  f.close();
  
  if( bytes == sizeof( FNvData ) )
    return true;

  Debug( "Read failed (wrong # of bytes), file: /FeedRecords.bin\n" );
  Clear();
  return false;
 }

uint8_t TFeederParas::CheckTime( struct tm const & tm )
 {
  for( int i = 0;  i < MaxFeedRecords;  ++i )
   {
    TFeedRecord & fr = FNvData.FeedRecords[ i ];
    if( fr.h == tm.tm_hour && fr.m  == tm.tm_min && ( fr.wday.mask & ( 1 << tm.tm_wday ) ) )
      return fr.portions + 1;
   }
  return 0;
 }
