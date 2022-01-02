//****************************************************************
// Includes
//****************************************************************

#include <GnBasics.h>
#include <LittleFS.h>

//****************************************************************
// Configuration
//****************************************************************

//****************************************************************
// typedefs
//****************************************************************

//****************************************************************
// Global Vars
//****************************************************************

//****************************************************************
// GnAssert
//****************************************************************

noreturn_ void failed_
 (
  char const * file,
  char const * line,
  char const * msg
 )
 {
  static char const * file_ __attribute__(( unused )) = NULL;
  static char const * line_ __attribute__(( unused )) = NULL;
  static char const * msg_ __attribute__(( unused )) = NULL;

  file_ = file;
  line_ = line;
  msg_ = msg;

  Serial.begin( 115200 );
  Serial.println( "" );
  Serial.print( "Assertion failed: File: " );
  Serial.print( file );
  Serial.print( ", Line: " );
  Serial.print( line );
  Serial.print( ", Msg: " );
  Serial.println( msg );

  while( 1 );
 }

//****************************************************************
// Functions (others)
//****************************************************************

// Read File from LittleFs into String
String ReadFile( char const * fileName )
 {
  LittleFS.begin();
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
String ReadFile( __FlashStringHelper const * fileName )
 {
  LittleFS.begin();
  File f = LittleFS.open( fileName, "r" );
  if( !f ) 
   {
    Debug( "Can't open file: %s\n", String( fileName ).c_str() );
    return "";
   }
  String res = f.readString();
  f.close();
  return res;
 }

//****************************************************************
// TLoopInstances List of Instances, that needs to be called inside loop
//****************************************************************

void TLoopInstances::Add( TLoopInstance * obj )
 {
  // Put the new LoopInstance at the beginning of the list
  obj->FNext = FList;
  FList = obj;
 }

void TLoopInstances::Remove( TLoopInstance * obj )
 {
  for( TLoopInstance ** p = &FList;  *p;  p = &( (*p)->FNext ) )
    if( *p == obj )
     {
      *p = obj->GetNext();
      return;
     }
 }

void TLoopInstances::Loop(void)
 {
  for( TLoopInstance * p = FList;  p;  p = p->FNext )
    p->Loop();
 }

//****************************************************************
// TLog
//****************************************************************
TLog::TLog(void)
 {
  Serial.begin( 115200 );
  while (!Serial);
  printf( "\nLog-Start:\n" );
  delay( 2 );
 }

void TLog::Printf( char const * fmt, ... )
 {
  va_list va;
  va_start( va, fmt );
  vprintf( fmt, va );
  va_end( va );
 }

