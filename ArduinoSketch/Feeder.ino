//****************************************************************
// Includes
//****************************************************************

#include <LittleFS.h>
#include "FeederConfig.h"
#include "FeederServo.h"
#include "FeederWifi.h"
#include "FeederDisplay.h"
#include "FeederParas.h"
#include <GnRotEncoder.h>

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

void setup(void)
 {
  Debug( "LittleFS.begin\n" );
  bool fsMounted = LittleFS.begin();
  if( !fsMounted )
   {
    Debug( "Can't mount LittleFS\n" );
   }
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

  TFeederConfig::Instance().DebugShow();

  FeederServo.Attach( FeederConfig.ServoPin );
  FeederWifi.Setup();
  GnRotEncoder.Attach( FeederConfig.ClkPin, FeederConfig.DirPin, FeederConfig.BtnPin );
  GnRotEncoder.SetRotCb( Closure( RotCb ) );
  GnRotEncoder.SetBtnCb( Closure( BtnCb ) );

  Debug( "Start\n" );
  Oled.StartScreenSaver(); //!
 }

void PrintTest(void)
 {
  static uint8_t y = 0;
  //d Debug( "PrintTest\n" );

  Oled.clearBuffer();
  Oled.drawFrame( 0, y++, 128, 4 );
  if( y > 30 )
    y = 0;
  Oled.setFont( u8g2_font_6x12_mr );
  Oled.drawStr( 0, 0 , "ABC" );
  Oled.drawStr( 0, 11, "defA" );
  Oled.drawStr( 0, 22, "ghi" );

  Oled.setFont( u8g2_font_inb30_mr );
  Oled.drawStr( 40, 0, "BIG" );

  Oled.sendBuffer();
 }

void loop(void)
 {
  LoopInstances.Loop();
  //PrintTest();
  //d Debug( "loop\n" );

  if( FeederWifi.TimeIsValid() )
    ShowTime();
 }

void OnOtaBegin(void)
 {
  LittleFS.end();
 }
