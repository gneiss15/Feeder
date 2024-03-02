//****************************************************************
// Includes
//****************************************************************

#include <LittleFS.h>

#include "FeederConfig.h"
#include "FeederServo.h"
#include "FeederWifi.h"
#include "FeedTimes.h"

//****************************************************************
// Sw Configuration
//****************************************************************

//? #include <GnFTPServer.h>
//? #define FtpServer GnFtpServer

// Server-Type for Simple Ftp Server
#include <SimpleFTPServer.h>

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

// #define FTP_CMD_PORT 21
// #define FTP_DATA_PORT_PASV 50009
// FtpServer( uint16_t _cmdPort = FTP_CMD_PORT, uint16_t _pasvPort = FTP_DATA_PORT_PASV );
// set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial
FtpServer ftpSrv;   

void setup(void)
 {
  WiFi.persistent( false );
  Debug( "\nStart\n" );

  bool fsMounted = LittleFS.begin();
  if( !fsMounted )
    Debug( "Can't mount LittleFS\n" );
   else
    ftpSrv.begin( FeederConfig.Hostname.c_str(), FeederConfig.FtpPw.c_str() ); // Set username, password for ftp.

  #if 0 //d 
  if( fsMounted )
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
 }

void loop(void)
 {
  LoopInstances.Loop();
  ftpSrv.handleFTP();        //make sure in loop you call handleFTP()!!  
 }
