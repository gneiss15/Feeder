//****************************************************************
// Includes
//****************************************************************

#include "FeederWifi.h"

#include "FeedTimes.h"
#include "FeederServo.h"
#include "FeederConfig.h"

#ifdef ESP32
  #include <WiFi.h>
 #else
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
  #include <coredecls.h>                  // settimeofday_cb()
#endif
#include <LittleFS.h>
#include <WifiKit8Oled.h>

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

  if( FeederConfig.WifiMode & WIFI_STA )
   {
    WiFi.setAutoConnect( true );
    WiFi.hostname( FeederConfig.Hostname );
    WiFi.begin( FeederConfig.WlanSsid, FeederConfig.WlanPw );
   }
   else
    WiFi.setAutoConnect( false );

  if( FeederConfig.WifiMode & WIFI_AP )   
   {
    //!_WiFi.softAPConfig( /*ip*/IPAddress( 192,168,4,1 ), /*gateway*/IPAddress( 192,168,4,200 ), /*subnet*/IPAddress( 255,255,255,0 ) );
    /*bool apOk =*/ WiFi.softAP( FeederConfig.ApSsid, FeederConfig.ApPw );
    //Log( "Starting soft-AP (Pw): %s\n", apOk ? "Ready" : "Failed!" );
    //Log( "Soft-AP IP address: %s\n", WiFi.softAPIP().toString().c_str() );
   }

  return true;
 }

//****************************************************************
// TFeederWifi (NTP)
//****************************************************************

bool TFeederWifi::STimeIsValid = false;

void TFeederWifi::STimeIsSet_cb(void)
 {
  //D Debug( "TFeederWifi::STimeIsSet_cb\n" );
  if( TFeederWifi::STimeIsValid )
    return;
  TFeederWifi::STimeIsValid = true;
  Oled.StartScreenSaver();
  //settimeofday_cb( NULL );
 }

bool TFeederWifi::SetupNTP(void)
 {
  //if( FeederConfig.WifiMode & WIFI_STA )
   {
    // Setp Ntp
    // install callback - called when settimeofday is called (by SNTP or us)
    // once enabled (by DHCP), SNTP is updated every hour
    settimeofday_cb( STimeIsSet_cb );
    configTime( FeederConfig.TZ.c_str(), FeederConfig.NtpServerAdr.c_str() );
   }

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
  #if 0 //D Debug/language
    if( HttpServer.hasHeader( "accept-language" ) )
     {
      // de-DE,de;q=0.9,en-US;q=0.8,en;q=0.7
      String lang( HttpServer.header( "accept-language" ) );
      int s = lang.indexOf( ',' );
      int e = lang.indexOf( ';' );
      Debug( "SHandleRoot: accept-language: %s\n", HttpServer.header( "accept-language" ).c_str() );
     }
  #endif
  
  HttpServer.send( 200, "text/html", ReadFile( F( "/Index.html" ) ) );
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
  Log( "%s: no body\n", postName.c_str() );
  return false;
 }

void TFeederWifi::SHandleSetFeedTimes(void)
 {
  if( !SChkPost( "SetFeedTimes" ) ) return;
  //D Debug( "FeedTimes: plain: %s\n", HttpServer.arg( "plain" ).c_str() );
  SSendPostResponce( FeedTimes.Set( HttpServer.arg( "plain" ) ), false );
 }

void TFeederWifi::SHandleManualFeed(void)
 {
  if( !SChkPost( "ManualFeed" ) ) return;
  //D Debug( "ManualFeed\n" );
  SSendPostResponce( FeederServo.Feed(), false );
 }

void TFeederWifi::SHandleStopFeed(void)
 {
  if( !SChkPost( "StopFeed" ) ) return;
  //D Debug( "StopFeed\n" );
  SSendPostResponce( FeederServo.Stop(), false );
 }

void TFeederWifi::SHandleMFeedChk(void)
 {
  if( !SChkPost( "MFeedChk" ) ) return;
  //D Debug( "MFeedChk\n" );
  SSendPostResponce( !FeederServo.Running(), false );
 }

void TFeederWifi::SHandleSetConfig(void)
 {
  if( !SChkPost( "Config" ) ) return;
  //D Debug( "Config: plain: %s\n", HttpServer.arg( "plain" ).c_str() );
  SSendPostResponce( FeederConfigInstance.Set( HttpServer.arg( "plain" ) ), true );
 }

void TFeederWifi::SHandleSetServoSteps(void)
 {
  if( !SChkPost( "SetServoSteps" ) ) return;
  //D Debug( "SetServoSteps: plain: %s\n", HttpServer.arg( "plain" ).c_str() );
  SSendPostResponce( FeederServo.Set( HttpServer.arg( "plain" ) ), false );
 }

const String FwUpdatePostResponceStr = "<!DOCTYPE html>\
<html>\
<head>\
<meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\">\
<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">\
<link rel=\"icon\" type=\"image/png\" href=\"/fish.png\">\
<style>\
div {text-align: center;}\
button {font-size:1.2em; font-weight:bold; background-color:#04AA6D; border-radius:6px; padding:5px 20px;}\
</style>\
</head>\
<body>\
<div id=\"ResDiv\" style=\"display:block\">\
</div>\
</body>\
<script>\
function GetCookie(cname){\
 let name=cname+\"=\";\
 let ca=(decodeURIComponent(document.cookie)).split(';');\
 for(let i=0;i<ca.length;i++){\
  let c=ca[i];\
  while(c.charAt(0)==' '){c=c.substring(1);}\
  if(c.indexOf(name)==0)return c.substring(name.length,c.length);}\
 return \"\";}\
function GetNrCookie(cname){\
 let r=GetCookie(cname);\
 if(r==\"\")r=\"0\";\
 return parseInt(r);}\
var Translations={\
 \"Reload\":[\"Reload\",\"Neu Laden\"],\
 \"Reload Now\":[\"Reload Now\",\"Jetzt neu Laden\"],\
 \"ReloadTxt1\":[\"If You don`t have changed 'WifiMode' inside 'Configuration', You may simply Reload.\",\"Solange Du 'WifiMode' innerhalb 'Konfiguration' nicht geändert hast, kannst du einfach neu Laden.\"],\
 \"ReloadTxt2\":[\"Reloading in \",\"Seite wird in \"],\
 \"ReloadTxt3\":[\" seconds.\",\" Sekunden neu geladen.\"],\
 \"FwUpdateNok\":[\"Firmware-Update failed!\",\"Firmware-Update fehlgeschlagen!\"],\
 \"FwUpdateOk\":[\"Firmware-Update successfully, Rebooting...\",\"Firmware-Update erfolgreich, Neustart...\"]};\
var LangNr=GetNrCookie(\"LNr\");\
function Tr(name,defStr=\"\"){\
 if(!Translations.hasOwnProperty(name))\
  return defStr;\
 let res=Translations[name][LangNr];\
 if(res!==undefined)\
  return res;\
 res=Translations[name][0];\
 if(res!==undefined)\
   return res;\
 return name;}\
function ReloadCountdown(){\
 var t=10;var x=setInterval(function(){--t;document.getElementById(\"ReloadTimeOut\").innerHTML=t;if(t<=0){clearInterval(x);location.replace(\"/Index.html\");}},1000);\
 return `${Tr(\"ReloadTxt2\")}<span id=\"ReloadTimeOut\">10</span>${Tr(\"ReloadTxt3\")}<br><button onclick=location=\"/Index.html\">${Tr(\"Reload Now\")}</button>`;}\
function Init(ok){\
 const ok2Color=[\"red\",\"green\"];\
 const ok2Txt=[\"FwUpdateNok\",\"FwUpdateOk\"];\
 var e=document.getElementById(\"ResDiv\");\
 e.innerHTML=`<font color=\"${ok2Color[ok]}\" style=\"font-size:2em;\"><strong>${Tr(ok2Txt[ok])}</strong></font><br>${Tr(\"ReloadTxt1\")}<br><button onclick=location=\"/Index.html\">${Tr(\"Reload\")}</button>ErrorTxtPos`;}\
document.addEventListener(\"loaded\",Init(0));</script></html>";

void TFeederWifi::SHandleFwUpdate(void)
 {
  // handler for the update form POST (once file upload finishes)
  //!SSendPostResponce( !Update.hasError(), true );
  bool ok = !Update.hasError();
  
  String resStr( FwUpdatePostResponceStr );
  String errorStr = "";
  if( ok )
   {
    HttpServer.client().setNoDelay( true );
    resStr.replace( "Init(0)", "Init(1)" );
   }
   else
    errorStr = String( "<br>Error:" ) + FUpdaterError;
  resStr.replace( "ErrorTxtPos", errorStr );

  if( FUploadName == "filesystem" )
    resStr.replace( "Firmware", "FileSystem" );
  
  HttpServer.send( 200, "text/html", resStr );
  if( !ok )
   {
    LittleFS.begin();
    return;
   }
  delay( 500 );
  HttpServer.client().stop();
  ESP.restart();  
 }

void TFeederWifi::SFwUploader(void)
 {
  // handler for the file upload, gets the sketch bytes, and writes them through the Update object
  HTTPUpload & upload = HttpServer.upload();

  //D Debug( "SFwUploader\n" );

  if( upload.status == UPLOAD_FILE_START )
   {
    //D Debug( " UPLOAD_FILE_START\n" );
    //D Debug( " upload.name: %s\n", upload.name.c_str() );
    //D Debug( " upload.contentLength: %x\n", upload.contentLength );

    uint32_t maxFreeSpace = ( ESP.getFreeSketchSpace() - 0x1000 ) & 0xFFFFF000;
    //D Debug( " maxFreeSpace: %x\n", maxFreeSpace );
    //D Debug( " fsSize: %x\n", size_t( &_FS_end ) - size_t( &_FS_start ) );

    FUpdaterError.clear();
    WiFiUDP::stopAll();

    FUploadName = upload.name;
    if( FUploadName == "filesystem" )
     {
      close_all_fs();
      Update.begin( maxFreeSpace, U_FS ); //start with max available size
     }
     else 
     {
      Update.begin( maxFreeSpace, U_FLASH ); //start with max available size
     }
  
   }
   else if( upload.status == UPLOAD_FILE_WRITE && !FUpdaterError.length() )
   {
    //D Debug( " UPLOAD_FILE_WRITE\n" );
    if( Update.write( upload.buf, upload.currentSize ) != upload.currentSize )
      SetErrorFromUpdater();
   }
   else if( upload.status == UPLOAD_FILE_END && !FUpdaterError.length() )
   {
    //D Debug( " UPLOAD_FILE_END\n" );
    if( !Update.end( true ) ) // true to set the size to the current progress
      SetErrorFromUpdater();
   }
   else if( upload.status == UPLOAD_FILE_ABORTED )
   {
    //D Debug( " UPLOAD_FILE_ABORTED\n" );
    Update.end();
   }
  delay(0);
  //yield();
 }

#if 0
void TFeederWifi::SHandleIndex(void)
 {
  HttpServer.send( 200, "text/html", ReadFile( F( "/Index.html" ) ) );
 }
#endif
 
bool TFeederWifi::SetupHttpServer(void)
 {
  FHttpServer.onNotFound( SHandleNotFound );

  // Setup a serveStatic for / as alias for Index.html
  FHttpServer.serveStatic( "/", LittleFS, "/Index.html" );
  // Setup a serveStatic for every file inside the dir '/'
  Dir dir = LittleFS.openDir( "/" );
  while( dir.next() )
    if( !dir.isDirectory() )
     {
      String fname = dir.fileName();
      if( fname.charAt( 0 ) != '/' )
        fname = "/" + fname;
      //if( fname != "/Index.html" )
      FHttpServer.serveStatic( fname.c_str(), LittleFS, fname.c_str() );
     }

  // Setup a server.on for dynamic generated answers
  FHttpServer.on( "/SetFeedTimes/", HTTP_POST, SHandleSetFeedTimes ); 
  FHttpServer.on( "/ManualFeed/", HTTP_POST, SHandleManualFeed ); 
  FHttpServer.on( "/StopFeed/", HTTP_POST, SHandleStopFeed ); 
  FHttpServer.on( "/MFeedChk/", HTTP_POST, SHandleMFeedChk ); 
  FHttpServer.on( "/SetConfig/", HTTP_POST, SHandleSetConfig ); 
  FHttpServer.on( "/SetServoSteps/", HTTP_POST, SHandleSetServoSteps ); 
  FHttpServer.on( "/FwUpdate/", HTTP_POST, SHandleFwUpdate, SFwUploader ); 

  // For getting accept-language (only when language select by this...
  //FHttpServer.collectHeaders( "accept-language" );

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
  //D Debug( "SetErrorFromUpdater: %s\n", FUpdaterError.c_str() );
 }
String TFeederWifi::FUpdaterError;
String TFeederWifi::FUploadName;


#undef HttpServer
