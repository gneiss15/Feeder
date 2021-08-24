//****************************************************************
// Includes
//****************************************************************

#include "FeederWifi.h"
#include <WifiKit8Oled.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
#include <coredecls.h>                  // settimeofday_cb()

#include "FeedTimes.h"
#include "FeederConfig.h"

//****************************************************************
// Sw Configuration
//****************************************************************

//****************************************************************
// Vars
//****************************************************************

//****************************************************************
// Functions
//****************************************************************

//****************************************************************
// TFeederWifi
//****************************************************************

TFeederWifi::TFeederWifi(void)
: FHttpServer( 80 )
 {
 }

/*virtual*/ void TFeederWifi::Loop(void)
 {
  FHttpServer.handleClient();
  MDNS.update();
 }

bool TFeederWifi::Setup(void) 
 { 
  return SetupWifi() && SetupNTP() && SetupHttpServer();
 }

//****************************************************************
// TFeederWifi (Wifi)
//****************************************************************

bool TFeederWifi::SetupWifi(void)
 {
  // Set WiFi to configured mode and disconnect from an AP if it was previously connected
  WiFi.disconnect( true );
  delay( 100 );
  WiFi.mode( FeederConfig.WifiMode );
  WiFi.setAutoConnect( true );
  WiFi.hostname( FeederConfig.Hostname );
  WiFi.begin( FeederConfig.WlanSsid, FeederConfig.WlanPw );

  delay( 100 );
  Oled.clearDisplay();
  Oled.setFont( u8g2_font_6x10_mf );

  if( FeederConfig.WlanEnabled )
   {
    byte count = 0;
    String connectingStr = "Connecting";
  
    while( WiFi.status() != WL_CONNECTED && count < 100 )
     {
      count ++;
      delay( 500 );
      Oled.drawStr( 0, 10, connectingStr );
      Oled.sendBuffer();
      connectingStr += ".";
     }
  
    Oled.clearDisplay();
    Oled.XyPrintf( 0, 10, "Connection to %s", FeederConfig.WlanSsid );
    if( WiFi.status() != WL_CONNECTED )
      Oled.drawStr( 0, 20, "Failed" );
     else
      Oled.XyPrintf( 0, 20, "IP: %s", WiFi.localIP().toString() );
   }
   else
    Oled.drawStr( 0, 10, "Station mode disabled" );

  if( FeederConfig.ApEnabled )   
   {
    //!_WiFi.softAPConfig( /*ip*/IPAddress( 192,168,4,1 ), /*gateway*/IPAddress( 192,168,4,200 ), /*subnet*/IPAddress( 255,255,255,0 ) );
    bool apOk =  WiFi.softAP( FeederConfig.ApSsid, FeederConfig.ApPw );
    Log( "Starting soft-AP (Pw): %s\n", apOk ? "Ready" : "Failed!" );
    Log( "Soft-AP IP address: %s\n", WiFi.softAPIP().toString() );
    Oled.XyPrintf( 0, 30, "AP-Enabled: %s", apOk ? "Ready" : "Failed!" );
   }
  Oled.sendBuffer();
  delay( 5000 );

  return true;
 }

//****************************************************************
// TFeederWifi (NTP)
//****************************************************************

bool TFeederWifi::STimeIsValid = false;

void TFeederWifi::STimeIsSet_cb(void)
 {
  if( TFeederWifi::STimeIsValid )
    return;
  TFeederWifi::STimeIsValid = true;
  //settimeofday_cb( NULL );
  Oled.clear();
  Oled.display();
  Oled.StartScreenSaver();
 }

bool TFeederWifi::SetupNTP(void)
 {
  // Setp Ntp
  // install callback - called when settimeofday is called (by SNTP or us)
  // once enabled (by DHCP), SNTP is updated every hour
  settimeofday_cb( STimeIsSet_cb );
  configTime( FeederConfig.TZ.c_str(), FeederConfig.NtpServerAdr.c_str() );
  //                             1         2 2
  //                    1234567890123456789012
  Oled.drawStr( 0, 20, "Wait for Time valid   " );
  Oled.sendBuffer();
  return true;
 }

#if 0
// OPTIONAL: change SNTP startup delay
// a weak function is already defined and returns 0 (RFC violation)
// it can be redefined:
//uint32_t sntp_startup_delay_MS_rfc_not_less_than_60000 ()
//{
//    //info_sntp_startup_delay_MS_rfc_not_less_than_60000_has_been_called = true;
//    return 60000; // 60s (or lwIP's original default: (random() % 5000))
//}

// OPTIONAL: change SNTP update delay
// a weak function is already defined and returns 1 hour
// it can be redefined:
uint32_t sntp_update_delay_MS_rfc_not_less_than_15000( void )
 {
  //info_sntp_update_delay_MS_rfc_not_less_than_15000_has_been_called = true;
  // return 15 * 1000; // 15s
  return 15 * 60 * 1000; // 15min
 }
#endif

//****************************************************************
// HttpServer Template-Code
//****************************************************************

#if 0
void TFeederWifi::SHandle....(void)
 {
  #if 0 //d Debug/language
    if( HttpServer.hasHeader( "accept-language" ) )
     {
      // de-DE,de;q=0.9,en-US;q=0.8,en;q=0.7
      String lang( HttpServer.header( "accept-language" ) );
      int s = lang.indexOf( ',' );
      int e = lang.indexOf( ';' );
      Debug( "SHandleRoot: accept-language: %s\n", HttpServer.header( "accept-language" ) );
     }
  #endif
  
  HttpServer.send( 200, "text/html", ReadFile( "/Static/Index.html" ) );
 }
#endif

//****************************************************************
// TFeederWifi (HttpServer & FwUpdater)
//****************************************************************

#define HttpServer FeederWifi.FHttpServer

void TFeederWifi::SHandleNotFound(void)
 {
  String msg = "File Not Found\n";
  msg += "\nURI: " + HttpServer.uri();
  msg += "\nMethod: " + ( HttpServer.method() == HTTP_GET ) ? "GET" : "POST";
  msg += "\nArguments: " + HttpServer.args();
  msg += "\n";
  for( uint8_t i = 0;  i < HttpServer.args();  ++i )
    msg += " " + HttpServer.argName( i ) + ": " + HttpServer.arg( i )  + "\n";
  HttpServer.send( 404, "text/plain", msg );
 }

void TFeederWifi::SSendPostResponce( bool ok, bool reboot )
 {
  if( reboot && ok )
    HttpServer.client().setNoDelay( true );
  HttpServer.send( 200, "text/plain", ok ? "1" : "0" );
  if( !reboot || !ok )
    return;
  delay( 500 );
  HttpServer.client().stop();
  ESP.restart();
 }

bool TFeederWifi::SChkPost( String postName )
 {
  if( HttpServer.hasArg( "plain" ) ) return true;
  Log( "%s: no body\n", postName );
  return false;
 }

void TFeederWifi::SHandleSetFeedTimes(void)
 {
  if( !SChkPost( "FeedTimes" ) ) return;
  Debug( "FeedTimes: plain: %s\n", HttpServer.arg( "plain" ) );
  SSendPostResponce( FeedTimes.Set( HttpServer.arg( "plain" ) ), false );
 }

void TFeederWifi::SHandleSetConfig(void)
 {
  if( !SChkPost( "Config" ) ) return;
  Debug( "Config: plain: %s\n", HttpServer.arg( "plain" ) );
  SSendPostResponce( FeederConfigInstance.Set( HttpServer.arg( "plain" ) ), true );
 }

void TFeederWifi::SHandleFwUpdate(void)
 {
  // handler for the update form POST (once file upload finishes)
  SSendPostResponce( !Update.hasError(), true );
 }

void TFeederWifi::SFwUploader(void)
 {
  // handler for the file upload, gets the sketch bytes, and writes them through the Update object
  HTTPUpload & upload = HttpServer.upload();

  if( upload.status == UPLOAD_FILE_START )
   {
    FUpdaterError.clear();
    WiFiUDP::stopAll();
    if( upload.name == "filesystem" )
     {
      size_t fsSize = size_t( &_FS_end ) - size_t( &_FS_start );
      close_all_fs();
      Update.begin( fsSize, U_FS ); // start with max available size
     }
     else 
     {
      uint32_t maxSketchSpace = ( ESP.getFreeSketchSpace() - 0x1000 ) & 0xFFFFF000;
      Update.begin( maxSketchSpace, U_FLASH ); //start with max available size
     }
    FUploadName = upload.name;
   }
   else if( upload.status == UPLOAD_FILE_WRITE && !FUpdaterError.length() )
   {
    if( Update.write( upload.buf, upload.currentSize ) != upload.currentSize )
      SetErrorFromUpdater();
   }
   else if( upload.status == UPLOAD_FILE_END && !FUpdaterError.length() )
   {
    if( !Update.end( true ) ) // true to set the size to the current progress
      SetErrorFromUpdater();
   }
   else if( upload.status == UPLOAD_FILE_ABORTED )
   {
    Update.end();
   }
  delay(0);
 }

void TFeederWifi::SHandleIndex(void)
 {
  HttpServer.send( 200, "text/html", ReadFile( "/Static/Index.html" ) );
 }
 
bool TFeederWifi::SetupHttpServer(void)
 {
  FHttpServer.onNotFound( SHandleNotFound );

  // Setup a serveStatic for every file inside the dir 'Static'
  // FHttpServer.serveStatic( "/", LittleFS, "/Static/Index.html" );
  FHttpServer.on( "/", SHandleIndex ); 
  FHttpServer.on( "/Index.html", SHandleIndex ); 
  Dir dir = LittleFS.openDir( "/Static/" );
  while( dir.next() )
    if( !dir.isDirectory() )
     {
      String fname = dir.fileName();
      if( fname.charAt( 0 ) != '/' )
       {
        Debug( "Dir-Listing of /Static/, found file: %s\n", fname );
        fname = "/" + fname;
       }
      if( fname != "/Index.html" )
       {
        String fsName = "/Static" + fname;
        FHttpServer.serveStatic( fname.c_str(), LittleFS, fsName.c_str() );
       }
     }

  FHttpServer.on( "/SetFeedTimes/", HTTP_POST, SHandleSetFeedTimes ); 
  FHttpServer.on( "/SetConfig/", HTTP_POST, SHandleSetConfig ); 
  FHttpServer.on( "/FwUpdate/", HTTP_POST, SHandleFwUpdate, SFwUploader ); 

  FHttpServer.collectHeaders( "accept-language" );

  FHttpServer.begin();

  MDNS.begin( FeederConfig.Hostname );
  MDNS.addService( "http", "tcp", 80 );

  return true;
 }
void TFeederWifi::SetErrorFromUpdater(void)
 {
  StreamString str;
  Update.printError(str);
  FUpdaterError = str;
 }
String TFeederWifi::FUpdaterError;
String TFeederWifi::FUploadName;


#undef HttpServer
