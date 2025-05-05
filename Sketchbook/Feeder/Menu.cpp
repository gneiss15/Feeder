//****************************************************************
// Includes
//****************************************************************

#include "Menu.h"

#include <WifiKit8Oled.h>

//****************************************************************
// Sw Configuration
//****************************************************************

#define Sc_WMenuBarX    126
#define Sc_WMenuBarT      0
#define Sc_WMenuBarB     31
#define Sc_WMenuBarH    ( Sc_WMenuBarB - Sc_WMenuBarT + 1 - 4 )

#define WMenu_ScreenLines 3  // # of Lines that fits onto screen in WrapMenu

//****************************************************************
// Macros
//****************************************************************

//****************************************************************
// TBaseMenu
//****************************************************************

void TBaseMenu::Perform(void)
 {
  Oled.clearBuffer();
  Oled.setFont( u8g2_font_6x10_mf );
  Perform_();
  Oled.sendBuffer();
 }

//****************************************************************
// TBaseWrapMenu
//****************************************************************

TBaseWrapMenu::TBaseWrapMenu( int8_t lines, int8_t idx )
: TBaseMenu()
, FMIdx( idx )
, FTop( 0 )
, FLines( lines )
, FIdx( 0 )
 {
  
 }
//----------------------------------------------------------------
void TBaseWrapMenu::DrawBar(void)
 {
  int bStart = Sc_WMenuBarH * ( FMIdx     ) / FLines + Sc_WMenuBarT + 2;
  int bEnd   = Sc_WMenuBarH * ( FMIdx + 1 ) / FLines + Sc_WMenuBarT + 1;

  Oled.drawVLine( Sc_WMenuBarX,     Sc_WMenuBarT, bStart );
  Oled.drawVLine( Sc_WMenuBarX - 1, bStart,       bEnd - bStart + 1 );
  Oled.drawVLine( Sc_WMenuBarX + 1, bStart,       bEnd - bStart + 1 );
  Oled.drawVLine( Sc_WMenuBarX,     bEnd + 1,     Sc_WMenuBarB - bEnd );
 }
//----------------------------------------------------------------
/*virtual*/ void TBaseWrapMenu::Perform_(void)
 {
  if( FIdx >= FLines )
    FIdx = 0;
   else if( FIdx < 0 )
    FIdx = FLines - 1;

  if( FLines > WMenu_ScreenLines )
   { // Multi Page Menu
    if( FTop < FMIdx - ( WMenu_ScreenLines - 1 ) )
      FTop = FMIdx - ( WMenu_ScreenLines - 1 );
    if( FTop > FMIdx )
      FTop = FMIdx;
   }
   else
    FTop = 0;

  DrawBar();

  // Print Text
  for( int8_t idx = 0, mIdx = FTop;  idx < WMenu_ScreenLines && mIdx < FLines; ++idx, ++mIdx )
    Line( idx, mIdx, idx == FMIdx - FTop );
 }
//----------------------------------------------------------------
/*virtual*/ bool TBaseWrapMenu::HandleKey( int8_t key )
 {
  switch( key )
   {
    case 'L':
     --FMIdx;
     return true;
    case 'R':
     ++FMIdx;
     return true;
    default:
     break;
   }
  return false;
 }
