//****************************************************************
// Includes
//****************************************************************

#include "FeederDisplay.h"
#include <WifiKit8Oled.h>

//****************************************************************
// Sw Configuration
//****************************************************************

//****************************************************************
// Vars
//****************************************************************

char const * WdayStrs[ 7 ] = { "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa" };

//****************************************************************
// Functions
//****************************************************************
void DrawStringCentered( uint8_t y, String str )
 {
  u8g2_uint_t sx = Oled.getStrWidth( str );
  int x = ( 128 - sx ) / 2;
  if( x < 0 )
    x = 0;
  Oled.drawStr( uint8_t( x ), y, str );
 }

// Font Names:
// u8g2_xxx_<Type>x   -> Type:
//  t Transparent gylphs with variable width
//  m Monospace/fixed width glyphs
//  h Glyphs with variable width and common height
//  8 Monospace/fixed width glyphs in a 8x8 box

// u8g2_xxx_x<Char Set>x   -> Char Set:
//  e Extended: Glyphs with unicode 32 to 701 are included in the font (v2.16.x will also include big ß)
//  f Full: Glyphs with unicode 32 to 255 are included in the font
//  r Restricted: Only characters from 32 to 127 are included
//  u Uppercase: Numbers and uppercase letters
//  n Numbers and some extra glyphs for date and time printing are included
//  _something  Special selection of glyphs. See the font picture for details

// 5x8_mf         OK
// 6x10_mf
// profont10_mf
// crox1cb_mf
void DrawDate( struct tm const & tm )
 {
  // Assemble date string.
  String dateStr = String( WdayStrs[ tm.tm_wday ] ) + ", " + TwoDigitStr( tm.tm_mday ) + "." + TwoDigitStr( tm.tm_mon ) + "." + String( tm.tm_year + 1900 );
  if( tm.tm_isdst > 0 )
    dateStr += " DST";
  Oled.setFont( u8g2_font_6x10_mf );
  DrawStringCentered( 7, dateStr );
 }

// 10x20_mf                             OK, etwas zu klein
// t0_22_mf t0_22b_mf
// profont22_mf profont29_mf
// crox1cb_mf crox1c_mf 1,2,3
// inr21_mf inr24_mf inr27_mf inr30_mf
void DrawTime( struct tm const & tm )
 {
  Oled.setFont( u8g2_font_profont29_mf );
  DrawStringCentered( 32, TimeString( tm.tm_hour, tm.tm_min, tm.tm_sec ) );
 }

void DrawTimePage( struct tm const & tm )
 {
  Oled.clearBuffer();
  DrawDate( tm );
  DrawTime( tm );
  //Oled.drawFrame( 0, 0, 128, 32 ); //!
  Oled.sendBuffer();
 }

void ShowTime( bool force )
 {
  static struct tm tmLast;

  time_t timeNow;
  struct tm tm;                     // the structure tm holds time information in a more convient way

  time( &timeNow );                 // read the current time
  localtime_r( &timeNow, &tm );     // split time into parts

  if( force || memcmp( &tmLast, &tm, sizeof( struct tm ) ) != 0 )
    DrawTimePage( tm );
  tmLast = tm;
 }
