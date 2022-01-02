//****************************************************************
// Includes
//****************************************************************

#include <WifiKit8Oled.h>
#include <stdio.h>
#include <stdarg.h>

//****************************************************************
// Sw Configuration
//****************************************************************

//****************************************************************
// TWifiKit8Oled
//****************************************************************


TWifiKit8Oled::TWifiKit8Oled(void)
: U8G2()
, FPowerSaveActiv( false )
, FScreenSaverTmr( /*timerCb*/ Closure( this, &TWifiKit8Oled::ScreenSaverCb ),
                     /*interval[s]*/ ScreenSaverTimeoutS, /*enabled*/ false )
 {
  //d Debug( "TWifiKit8Oled::TWifiKit8Oled: Start\n" );
  u8g2_Setup_ssd1306_i2c_128x32_univision_f( &u8g2, DisplayDirection, u8x8_byte_arduino_hw_i2c, u8x8_gpio_and_delay_arduino );
  u8x8_SetPin_HW_I2C( getU8x8(), /* reset=*/ 16, /* clock=*/ 5, /* data=*/ 4 ); // Wifi Kit 8
  //d Debug( "TWifiKit8Oled::TWifiKit8Oled: Done\n" );
  begin();
  setFont( u8g2_font_6x12_mr );
  setContrast( 0 );
  //d Debug( "TWifiKit8Oled::TWifiKit8Oled: Print Done\n" );
 }

void TWifiKit8Oled::ScreenSaverCb( TGnCoopTimerBase * tmr, uint16_t /*intervalsElapsed*/ )
 {
  //d Debug( "TWifiKit8Oled::TWifiKit8Oled: ScreenSaverCb\n" );
  PowerSave( true );
 }

void TWifiKit8Oled::PowerSave( bool on )
 {
  //d Debug( "TWifiKit8Oled::TWifiKit8Oled: PowerSave: %s\n", on ? "ON": "Off" );
  FPowerSaveActiv = on;
  setPowerSave( on );
  FScreenSaverTmr.Stop();
  if( !on )
    FScreenSaverTmr.Start();
 }

void TWifiKit8Oled::RestartScreenSaver(void)
 {
  PowerSave( false );
 }

void TWifiKit8Oled::StartScreenSaver( TGnCoopTimer_t interval )
 {
  //d Debug( "TWifiKit8Oled::TWifiKit8Oled: StartScreenSaver\n" );
  FScreenSaverTmr.SetInterval( interval );
  RestartScreenSaver();
 }

void TWifiKit8Oled::StopScreenSaver(void)
 {
  PowerSave( false );
  FScreenSaverTmr.Stop();
 }

void TWifiKit8Oled::vprintf( char const * fmt, va_list va )
 {
  char * str = NULL;

  if( vasprintf( &str, fmt, va ) > 0 )
    print( str );

  free( str );
 }

void TWifiKit8Oled::printf( char const * fmt, ... )
 {
  va_list va;
  va_start( va, fmt );
  vprintf( fmt, va );
  va_end( va );
 }

void TWifiKit8Oled::XyPrintf( u8g2_uint_t x, u8g2_uint_t y, char const * fmt, ... )
 {
  setCursor( x, y ); 

  va_list va;
  va_start( va, fmt );
  vprintf( fmt, va );
  va_end( va );
 }
 
void TWifiKit8Oled::drawCentered( uint8_t y, char const * str )
 {
  u8g2_uint_t sx = getStrWidth( str );
  int x = ( 128 - sx ) / 2;
  if( x < 0 )
    x = 0;
  drawStr( uint8_t( x ), y, str );
 }
 
#if 0 //? 
void TWifiKit8Oled::setContrast( uint8_t contrast, uint8_t precharge, uint8_t comdetect )
 {
  //#define u8g2_GetU8x8(u8g2) ((u8x8_t *)(u8g2))
  // u8x8_t * u8x8 = reinterpret_cast<u8x8_t*>( &u8g2 );
  u8x8_t * u8x8 = u8g2_GetU8x8( &u8g2 );
  
  u8x8_cad_StartTransfer( u8x8 );
    u8x8_cad_SendCmd( u8x8, 0xD9 );       // SETPRECHARGE
    u8x8_cad_SendArg( u8x8, precharge );  // 0xF1 default, to lower the contrast, put 1-1F
    u8x8_cad_SendCmd( u8x8, 0x81 );       // SETCONTRAST
    u8x8_cad_SendArg( u8x8, contrast );
    u8x8_cad_SendCmd( u8x8, 0xDB );       // SETVCOMDETECT
    u8x8_cad_SendArg( u8x8, comdetect );  // 0x40 default, to lower the contrast, put 0
    u8x8_cad_SendCmd( u8x8, 0xA4 );       // DISPLAYALLON_RESUME
    u8x8_cad_SendCmd( u8x8, 0xA6 );       // NORMALDISPLAY
    u8x8_cad_SendCmd( u8x8, 0xAF );       // DISPLAYON
  u8x8_cad_EndTransfer( u8x8 );
 }

void TWifiKit8Oled::setBrightness( uint8_t brightness )
 {
  // Magic values to get a smooth/ step-free transition
  uint8_t contrast = brightness * 1.171 + ( brightness < 128 ? 0 : -43 );
  setContrast( contrast, brightness == 0 ? 0 : 241, brightness / 8 );
 }
#endif
