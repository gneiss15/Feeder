//****************************************************************
// Includes
//****************************************************************

#include "FeederWifi.h"
#include <WifiKit8Oled.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <coredecls.h>                  // settimeofday_cb()

#include "FeederParas.h"
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
  ArduinoOTA.handle();
 }

bool TFeederWifi::Setup(void) 
 { 
  //d Debug( "TFeederWifi::Setup\n" );
  
  return SetupWifi() && SetupNTP() && SetupHttpServer() && SetupOTA();
 }

//****************************************************************
// TFeederWifi (Wifi)
//****************************************************************

bool TFeederWifi::SetupWifi(void)
 {
  //d Debug( "SetupWifi\n" );

  // Set WiFi to station + ap mode and disconnect from an AP if it was previously connected
  WiFi.disconnect( true );
  delay( 100 );
  WiFi.mode( WIFI_AP_STA ); // WIFI_STA
  WiFi.setAutoConnect( true );
  WiFi.hostname( FeederConfig.Hostname );
  WiFi.begin( FeederConfig.WlanSsid, FeederConfig.WlanPw );

  //Debug( "Setting soft-AP configuration: %s\n", WiFi.softAPConfig( IPAddress( 192,168,4,1 ), IPAddress( 192,168,4,2 ), IPAddress( 255,255,255,0 ) ) ? "Ready" : "Failed!" );
  Debug( "Starting soft-AP (Pw): %s\n", WiFi.softAP( FeederConfig.ApSsid, FeederConfig.ApPw ) ? "Ready" : "Failed!" );
  Debug( "Soft-AP IP address: %s\n", WiFi.softAPIP().toString().c_str() );
  
  delay( 100 );
  Oled.clearDisplay();
  
  #if !defined(DISABLE_WIFI)
    byte count = 0;
    String connectingStr = "Connecting";
  
    while( WiFi.status() != WL_CONNECTED && count < 100 )
     {
      //d Debug( "SetupWifi: Count: %u\n", count );
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
    Oled.sendBuffer();
  #else
    Oled.drawStr( 0, 10, "Wifi disbled" );
    Oled.sendBuffer();
    delay( 5000 );
  #endif

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
  //d Debug( "SetupNTP\n" );

  #if !defined(DISABLE_WIFI)
    // Setp Ntp
    // install callback - called when settimeofday is called (by SNTP or us)
    // once enabled (by DHCP), SNTP is updated every hour
    settimeofday_cb( STimeIsSet_cb );
  
    configTime( FeederConfig.TZ.c_str(), FeederConfig.NtpServerAdr.c_str() );
  
    Oled.drawStr( 0, 30, "Wait for Time valid" );
   #else
    Oled.drawStr( 0, 30, "No Wait for Time valid (Wifi disbled)" );
    STimeIsValid = true;
  #endif
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
// TFeederWifi (HttpServer)
//****************************************************************

#define HttpServer FeederWifi.FHttpServer

void TFeederWifi::SHandleNotFound()
 {
  String msg = "File Not Found\n\n";
  msg += "URI: ";
  msg += HttpServer.uri();
  msg += "\nMethod: ";
  msg += (HttpServer.method() == HTTP_GET) ? "GET" : "POST";
  msg += "\nArguments: ";
  msg += HttpServer.args();
  msg += "\n";
  for( uint8_t i = 0;  i < HttpServer.args();  ++i )
    msg += " " + HttpServer.argName( i ) + ": " + HttpServer.arg( i )  + "\n";
  HttpServer.send( 404, "text/plain", msg );
 }

void TFeederWifi::SHandleRoot(void)
 {
  // Create HTML content
  HttpServer.send( 200, "text/html", FeederParas.GetHtmlPage() );
 }

void TFeederWifi::SHandleData(void)
 {
  // Create DATA content
  HttpServer.send( 200, "text/html", FeederParas.GetJson() );
 }

void TFeederWifi::SHandleSet(void)
 {
  if( HttpServer.method() != HTTP_POST )
   {
    HttpServer.send( 405, "text/plain", "Method Not Allowed" );
    return;
   }

  #if 0 //d Debug
    String msg = "POST form was:\n";
    for( uint8_t i = 0;  i < HttpServer.args();  i++ )
      msg += " " + HttpServer.argName( i ) + ": " + HttpServer.arg( i ) + "\n";
    HttpServer.send( 200, "text/plain", msg );
  #endif

  // Alle HttpServer.args() durchgehen, wenn argName != plain C/R auswerten
  for( uint8_t i = 0;  i < HttpServer.args();  i++ )
   {
    String argName( HttpServer.argName( i ) );
    if( argName != "plain" )
      FeederParas.Set
       ( 
        argName.substring(1,2).toInt(), // r
        argName.substring(3,4).toInt(), //c 
        HttpServer.arg( i ) //argVal
       );
   } 

  // Alles OK -> Update FeederParas
  FeederParas.Save();
  
  //?HttpServer.send( 200, "text/html", "<a href='/'> Go Back </a>" ); // Send web page
  HttpServer.sendHeader( "Location", "/" );
  HttpServer.send( 302, "text/plain", "Updated– Press Back Button" );
 }

bool TFeederWifi::SetupHttpServer(void)
 {
  //d Debug( "SetupHttpServer\n" );

  #if !defined(DISABLE_WIFI)
    FHttpServer.onNotFound( SHandleNotFound );
    FHttpServer.on( "/", SHandleRoot ); 
    FHttpServer.on( "/data.json", SHandleData ); 
    FHttpServer.on( "/Set/", SHandleSet ); 
  
    FHttpServer.begin();
  
    if( MDNS.begin( FeederConfig.Hostname ) )
     {
      //d Debug( "MDNS responder started" );
     }
  #endif

  return true;
 }

//****************************************************************
// TFeederWifi (OTA)
//****************************************************************

void OnOtaStart(void)
 {
  String type( ArduinoOTA.getCommand() == U_FLASH ? "sketch" : "fs" );
  // NOTE: if updating FS this would be the place to unmount FS using FS.end()
  Debug( "OTA Start: %s\n", type );
 }

void OnOtaEnd(void)
 {
  Debug( "\nOTA End\n" );
 }

void OnOtaProgress( uint_t progress, uint_t total )
 {
  Debug( " Progress: %u %%\n", progress / total / 100 );
 }

void OnOtaError( ota_error_t error )
 {
  String errorTxt;
  switch( error )
   {
    case OTA_AUTH_ERROR:
      errorTxt = "Auth Failed";
      break;
    case OTA_BEGIN_ERROR:
      errorTxt = "Begin Failed";
      break;
    case OTA_CONNECT_ERROR:
      errorTxt = "Connect Failed";
      break;
    case OTA_RECEIVE_ERROR:
      errorTxt = "Receive Failed";
      break;
    case OTA_END_ERROR:
      errorTxt = "End Failed";
      break;
    default:
      errorTxt = "???";
      break;
   }
  
  Debug( "OTA Error: %u %s\n", error, errorTxt );
 }

void TFeederWifi::SetupHttpUpdater(void)
 {
  FHttpUpdater.setup( &FHttpServer );
  
  MDNS.addService( "http", "tcp", 80 );

  Debug( "HTTPUpdateServer ready! Open http://%s/update in your browser\n", WiFi.softAPIP().toString().c_str() );
 }

void TFeederWifi::SetupArduinoOTA(void)
 {
  // Port defaults to 8266
  // ArduinoOTA.setPort( 8266 );

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname( FeederConfig.Hostname.c_str() );

  // No authentication by default
  // ArduinoOTA.setPassword( "pw" );
  // Password can be set with it's md5 value as well
  // create hash with:
  // echo -n "<pw>" | openssl md5
  // Hint: Same as on PC
  ArduinoOTA.setPasswordHash( "39f586066e368d03d436d65930709c88" );

  ArduinoOTA.onStart( OnOtaStart );
  ArduinoOTA.onEnd( OnOtaEnd );
  ArduinoOTA.onProgress( OnOtaProgress );
  ArduinoOTA.onError( OnOtaError );
  ArduinoOTA.begin();

  Debug( "ArduinoOTA ready!\n" );
 }

bool TFeederWifi::SetupOTA(void)
 {
  SetupHttpUpdater();
  SetupArduinoOTA();
  return true;
 }
 
#undef HttpServer
