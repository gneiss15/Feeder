//****************************************************************
// Includes
//****************************************************************

#include <LittleFS.h>
#include "FeederConfig.h"
#include "FeederServo.h"
#include "FeederWifi.h"
#include "FeederDisplay.h"
#include "FeedTimes.h"
#include <GnRotEncoder.h>

//****************************************************************
// Sw Configuration
//****************************************************************

//#define LITTLFS_TEST

// Server-Type for Simple Ftp Server

#define DEFAULT_FTP_SERVER_NETWORK_TYPE_ESP8266   NETWORK_ESP8266
#define DEFAULT_STORAGE_TYPE_ESP8266 STORAGE_LITTLEFS
//? #include <SimpleFTPServer.h>

//****************************************************************
// Hw Configuration
//****************************************************************

/******************************************
               Wireing

Servo   RotEnc  +++-WifiKit8-+++   Farbe
                     gpio
------+-------+-----------------+-------
GND   |       | GND         GND | schwarz
5V    |       | 5V          5V  | weiß
      |  3V3  | 3V3         3V3 | rot
      |  GND  | GND         GND | braun
      |       | D2    4     CTS |
PWM   |       | D3    0     DTR | grau
      |       | D8   15  5  D1  |
      |  Dt   | D7   13  1  TX  | gelb
      |  Clk  | D6   12  3  RX  | grün
      |  Sw   | SCL  14     RST | orange
      |       | D0   16  2  SDA |
      |       | A0          EN  |
******************************************/

/* Pins are defined in FeederConfig!
ServoPin  0 // Output Pin for Servo
DirPin   13 // Direction-Input-Pin from RotEncoder
ClkPin   12 // Clock-Input-Pin from RotEncoder
BtnPin   14 // Button-Input-Pin from RotEncoder
*/

//****************************************************************
// Main
//****************************************************************

void RotCb( int16_t & absCnt, int16_t relCnt )
 {
  Debug( "Rot: PowerSave %s, RotCnt: abs: %d, rel: %d\n", Oled.InPowerSave() ? "yes" : "no ", absCnt, relCnt );
  Oled.StartScreenSaver();
 }

void BtnCb( bool up )
 {
  Debug( "Btn: PowerSave %s, Btn: %s\n", Oled.InPowerSave() ? "yes" : "no ", up ? "up" : "down" );
  Oled.StartScreenSaver();
 }

//!### LittleFs-Test ########################################################
#ifdef LITTLFS_TEST
// How large of a file to test
#define TESTSIZEKB 512

uint8_t WData[ 256 ];
uint8_t RData[ 256 ];

int TestNr = 0;
bool TestsDone = false;
long start;

#define BigFile "/Static/testwrite.bin"
#define SmallFile "/Static/test1b.bin"

String Error( String res )
 {
  TestsDone = true; 
  return res;
 }

String ShowTime( long stop, String msg )
 {
  return msg + " ==> Time: " + String( stop - start ) + " ms = " + String( TESTSIZEKB * 1024 / ( stop - start ) * 1000 ) + " bytes/s";
 }

String DoTest(void)
 {
  if( TestsDone )
    return "Done";

  start = millis();
  switch( TestNr )
   {
    case 0:
      if( !LittleFS.format() ) return Error( "Unable to format(), aborting" );
      return "Format";
    case 1:
      if( !LittleFS.begin() ) return Error( "Unable to begin(), aborting" );
      return "Begin";
    case 2:
     {
      File f = LittleFS.open( BigFile, "w" );
      if( !f ) return Error( "Unable to open file for writing, aborting" );
      for( int i = 0;  i < TESTSIZEKB;  i++ )
        for( int j = 0;  j < 4;  j++ )
          f.write( WData, 256 );
      f.close();
      return ShowTime( millis(), "Creating " + String( TESTSIZEKB ) + "KB file" );
     }
    case 3:
     {
      File f = LittleFS.open( BigFile, "r" );
      size_t fileSize = f.size();
      f.close();
      return "Created file size = " + String( fileSize );
     }
    case 4:
     {
      File f = LittleFS.open( BigFile, "r" );
      for( int i = 0;  i < TESTSIZEKB;  i++ )
        for( int j = 0;  j < 4;  j++ )
          f.read( RData, 256 );
      f.close();
      return ShowTime( millis(), "Reading " + String( TESTSIZEKB ) + "KB file sequentially in 256b chunks" );
     }
    case 5:
     {
      File f = LittleFS.open( BigFile, "r" );
      f.read();
      for( int i = 0;  i < TESTSIZEKB;  i++ )
        for( int j = 0;  j < 4;  j++ )
          f.read( RData + 1, 256 );
      f.close();
      return ShowTime( millis(), "Reading " + String( TESTSIZEKB ) + "KB file MISALIGNED in flash and RAM sequentially in 256b chunks" );
     }
    case 6:
     {
      File f = LittleFS.open( BigFile, "r" );
      for( int i = 0;  i < TESTSIZEKB;  i++ )
       {
        for( int j = 0;  j < 4;  j++ )
         {
          if( !f.seek( 256 + 256 * j * i, SeekEnd ) ) return Error( "Unable to seek to " + String( -256 - 256 * j * i ) + ", aborting" );
          if( 256 != f.read( RData, 256 ) ) return Error( "Unable to read 256 bytes, aborting" );
         }
       }
      f.close();
      return ShowTime( millis(), "Reading " + String( TESTSIZEKB ) + "KB file in reverse by 256b chunks" );
     }
    case 7:
     {
      File f = LittleFS.open( SmallFile, "w" );
      for( int i = 0;  i < 65536;  i++ )
        f.write( (uint8_t*)&i, 1 );
      f.close();
      return ShowTime( millis(), "Writing 64K file in 1-byte chunks" );
     }
    case 8:
     {
      File f = LittleFS.open( SmallFile, "r" );
      for( int i = 0;  i < 65536;  i++ )
       {
        uint8_t c;
        f.read( &c, 1 );
       }
      f.close();
      return ShowTime( millis(), "Reading 64K file in 1-byte chunks" );
     }
    default:
      TestsDone = true; 
   }
  return "Done";
 }
#endif
//!### LittleFs-Test ########################################################

//? FtpServer ftpSrv;   //set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial

void setup(void)
 {
  WiFi.persistent( false );
  Debug( "\nStart\n" );

  bool fsMounted = LittleFS.begin();
  if( !fsMounted )
    Debug( "Can't mount LittleFS\n" );
    #if 0 //d 
   else
//?     ftpSrv.begin( "esp8266","esp8266" );    //username, password for ftp.   (default 21, 50009 for PASV)
     else
     {
      struct FSInfo fs_info;
      LittleFS.info( fs_info );
      Debug
       (
        "FSInfo:\n"
        "  totalBytes: %u\n"
        "  usedBytes: %u\n"
        "  blockSize: %u\n"
        "  pageSize: %u\n"
        "  maxOpenFiles: %u\n"
        "  maxPathLength: %u\n\n",
        fs_info.totalBytes,
        fs_info.usedBytes,
        fs_info.blockSize,
        fs_info.pageSize,
        fs_info.maxOpenFiles,
        fs_info.maxPathLength
       );
     }
    #endif

  FeedTimes.Setup();
  FeederWifi.Setup();

  FeederServo.Attach( FeederConfig.ServoPin );
  GnRotEncoder.Attach( FeederConfig.ClkPin, FeederConfig.DirPin, FeederConfig.BtnPin );
  GnRotEncoder.SetRotCb( Closure( RotCb ) );
  GnRotEncoder.SetBtnCb( Closure( BtnCb ) );


  //!Oled.StartScreenSaver();
//!### LittleFs-Test ########################################################
#ifdef LITTLFS_TEST
  for( int i = 0; i < 256; i++ )
    WData[ i ] = (uint8_t)i;
  TestNr = 1;
  Log( "\n\nTest-Start\n" );
#endif  
//!### LittleFs-Test ########################################################
 }


void loop(void)
 {
  LoopInstances.Loop();
  if( FeederWifi.TimeIsValid() )
    ShowTime();

//? ftpSrv.handleFTP();        //make sure in loop you call handleFTP()!!  
    
//!### LittleFs-Test ########################################################
#ifdef LITTLFS_TEST
if( !TestsDone )
   {
    Log( "Test: %d ", TestNr );
    long startM = millis();
    String res = DoTest();
    long stopM = millis();
    Log( "(%ld ms): %s\n", stopM - startM, DoTest() );
    TestNr++;
   }
#endif
//!### LittleFs-Test ########################################################
 }
