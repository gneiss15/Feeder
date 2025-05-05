//****************************************************************

#ifndef WifiKit8OledH  // sentry
#define WifiKit8OledH

//****************************************************************
// Includes
//****************************************************************

#include <GnBasics.h>
#include <U8g2lib.h>
#include <GnCoopTimer.h>

//****************************************************************
// Sw Configuration
//****************************************************************

/*
U8G2_R0     No rotation, landscape
U8G2_R1     90 degree clockwise rotation
U8G2_R2     180 degree clockwise rotation
U8G2_R3     270 degree clockwise rotation
U8G2_MIRROR No rotation, landscape, display content is mirrored (v2.6.x)
*/
#define DisplayDirection     U8G2_R0
#define ScreenSaverTimeoutS  30

//****************************************************************
// Public Vars & Methods of TWifiKit8Oled inherented from u8g2
//****************************************************************

#if 0
  u8g2_uint_t tx, ty; //??
  
  u8x8_t * getU8x8(void)
  u8g2_t * getU8g2(void)
    
  void sendF( char const * fmt, ... ) 

  uint32_t getBusClock(void)
  void setBusClock( uint32_t clock_speed )
  void setI2CAddress( uint8_t adr )
  
  void enableUTF8Print(void)
  void disableUTF8Print(void)
  
  uint8_t getCols(void)
  uint8_t getRows(void)
  void drawTile( uint8_t x, uint8_t y, uint8_t cnt, uint8_t * tile_ptr )  

  // set the menu pins before calling begin() or initDisplay()
  void setMenuSelectPin( uint8_t val )
  void setMenuPrevPin( uint8_t val)
  void setMenuNextPin( uint8_t val )
  void setMenuUpPin( uint8_t val )
  void setMenuDownPin( uint8_t val )
  void setMenuHomePin( uint8_t val )

  // return 0 for no event or U8X8_MSG_GPIO_MENU_SELECT, U8X8_MSG_GPIO_MENU_NEXT, U8X8_MSG_GPIO_MENU_PREV, U8X8_MSG_GPIO_MENU_HOME
  uint8_t getMenuEvent(void)
  
  void initDisplay(void)
  void clearDisplay(void)
  void setPowerSave( uint8_t is_enable )
  void setFlipMode( uint8_t mode )
  void setContrast( uint8_t value )
  void setDisplayRotation( u8g2_cb_t const * u8g2_cb )      

  bool begin(void)

  void beginSimple(void) // does not clear the display and does not wake up the display user is responsible for calling clearDisplay() and setPowerSave(0)
  bool begin( uint8_t menu_select_pin, uint8_t menu_next_pin, uint8_t menu_prev_pin, uint8_t menu_up_pin = U8X8_PIN_NONE, uint8_t menu_down_pin = U8X8_PIN_NONE, uint8_t menu_home_pin = U8X8_PIN_NONE )

  void setMaxClipWindow(void)
  void setClipWindow( u8g2_uint_t clip_x0, u8g2_uint_t clip_y0, u8g2_uint_t clip_x1, u8g2_uint_t clip_y1 )

  u8g2_uint_t getDisplayHeight(void)
  u8g2_uint_t getDisplayWidth(void)

  void sendBuffer(void)
  void clearBuffer(void)
    
  void firstPage(void)
  uint8_t nextPage(void)
    
  void setBufferPtr( uint8_t *buf )
  uint16_t getBufferSize(void)
  uint8_t *getBufferPtr(void)
  uint8_t getBufferTileHeight(void)
  uint8_t getBufferTileWidth(void)
  uint8_t getBufferCurrTileRow(void)
  void setBufferCurrTileRow( uint8_t row )
    
  void setAutoPageClear( uint8_t mode )
    
  void updateDisplayArea( uint8_t  tx, uint8_t ty, uint8_t tw, uint8_t th )
  void updateDisplay(void)
  void refreshDisplay(void)
    
  void setDrawColor( uint8_t color_index )
  uint8_t getDrawColor(void)

  void drawPixel( u8g2_uint_t x, u8g2_uint_t y )
  void drawHLine( u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w )
  void drawVLine( u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t h )
  void drawHVLine( u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len, uint8_t dir )
    
  void drawFrame( u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t h )
  void drawRFrame( u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t h, u8g2_uint_t r )
  void drawBox( u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t h )
  void drawRBox( u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t h, u8g2_uint_t r )
    
  void drawCircle( u8g2_uint_t x0, u8g2_uint_t y0, u8g2_uint_t rad, uint8_t opt = U8G2_DRAW_ALL )
  void drawDisc( u8g2_uint_t x0, u8g2_uint_t y0, u8g2_uint_t rad, uint8_t opt = U8G2_DRAW_ALL )
  void drawEllipse( u8g2_uint_t x0, u8g2_uint_t y0, u8g2_uint_t rx, u8g2_uint_t ry, uint8_t opt = U8G2_DRAW_ALL )
  void drawFilledEllipse( u8g2_uint_t x0, u8g2_uint_t y0, u8g2_uint_t rx, u8g2_uint_t ry, uint8_t opt = U8G2_DRAW_ALL )

  void drawLine( u8g2_uint_t x1, u8g2_uint_t y1, u8g2_uint_t x2, u8g2_uint_t y2 ) 

  void setBitmapMode( uint8_t is_transparent ) 
  void drawBitmap( u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t cnt, u8g2_uint_t h, uint8_t const * bitmap )
  void drawXBM( u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t h, uint8_t const * bitmap )
  void drawXBMP( u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t h, uint8_t const * bitmap )
    
  void drawTriangle( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2 ) 
      
  void drawLog( u8g2_uint_t x, u8g2_uint_t y, class U8G2LOG & u8g2log );
    
  void setFont( uint8_t const * font )
  void setFontMode( uint8_t  is_transparent )
  void setFontDirection( uint8_t dir )

  int8_t getAscent(void)
  int8_t getDescent(void)
    
  void setFontPosBaseline(void)
  void setFontPosBottom(void)
  void setFontPosTop(void)
  void setFontPosCenter(void)

  void setFontRefHeightText(void)
  void setFontRefHeightExtendedText(void)
  void setFontRefHeightAll(void)
    
  // uint8_t u8g2_IsGlyph( u8g2_t * u8g2, uint16_t requested_encoding );
  // int8_t u8g2_GetGlyphWidth( u8g2_t * u8g2, uint16_t requested_encoding );
  // u8g2_uint_t u8g2_GetStrWidth( u8g2_t * u8g2, char const * s );
  // u8g2_uint_t u8g2_GetUTF8Width( u8g2_t * u8g2, char const * str );
    
  u8g2_uint_t drawGlyph( u8g2_uint_t x, u8g2_uint_t y, uint16_t encoding )
  u8g2_uint_t drawStr( u8g2_uint_t x, u8g2_uint_t y, char const * s )
  u8g2_uint_t drawUTF8( u8g2_uint_t x, u8g2_uint_t y, char const * s )
  u8g2_uint_t drawExtUTF8( u8g2_uint_t x, u8g2_uint_t y, uint8_t to_left, uint16_t const * kerning_table, char const * s ) 

  u8g2_uint_t getStrWidth( char const * s )
  u8g2_uint_t getUTF8Width( char const * s )
    
  // screenshot functions for full buffer mode, vertical top lsb memory architecture
  void writeBufferPBM( Print & p );
  void writeBufferXBM( Print & p );
  // horizontal right lsb memory architecture: SH1122, LD7032, ST7920, ST7986, LC7981, T6963, SED1330, RA8835, MAX7219, LS0
  void writeBufferPBM2( Print & p );
  void writeBufferXBM2( Print & p );

  // virtual function for print base class
  size_t write( uint8_t v )

  // user interface
  uint8_t userInterfaceSelectionList( char const * title, uint8_t start_pos, char const * sl )
  uint8_t userInterfaceMessage( char const * title1, char const * title2, char const * title3, char const * buttons )
  uint8_t userInterfaceInputValue( char const * title, char const * pre, uint8_t * value, uint8_t lo, uint8_t hi, uint8_t digits, char const * post )
    
  // LiquidCrystal compatible functions
  void home(void)
  void clear(void)
  void noDisplay(void)
  void display(void)
  void setCursor( u8g2_uint_t x, u8g2_uint_t y )
 
  // u8glib compatible functions
  void sleepOn(void)
  void sleepOff(void)
  void setColorIndex( uint8_t color_index )
  uint8_t getColorIndex(void)
  int8_t getFontAscent(void)
  int8_t getFontDescent(void)
  int8_t getMaxCharHeight(void)
  int8_t getMaxCharWidth(void)
  u8g2_uint_t getHeight(void)
  u8g2_uint_t getWidth(void)
#endif

//****************************************************************
// TWifiKit8Oled
//****************************************************************

class TWifiKit8Oled : public U8G2, public TSingleton<TWifiKit8Oled>
 {
  friend class TSingleton<TWifiKit8Oled>;
 private:
                      TWifiKit8Oled(void);
 private:
  bool                FPowerSaveActiv;
  TGnCoopTimerS       FScreenSaverTmr;
  void                ScreenSaverCb( TGnCoopTimerBase * tmr, uint16_t intervalsElapsed );
 public:
  void                PowerSave( bool on );
  bool                InPowerSave(void)                                               { return FPowerSaveActiv; }

  bool                ScreenSaverEnabled(void)                                        { return FScreenSaverTmr.Enabled(); }
  void                RestartScreenSaver(void);
  void                StartScreenSaver( TGnCoopTimer_t interval = ScreenSaverTimeoutS );
                      // Turns Display on
  void                StopScreenSaver(void);
 public:
  void                drawStr( u8g2_uint_t x, u8g2_uint_t y, char const * str )       { setCursor( x, y ); print( str ); }

  void                vprintf( char const * fmt, va_list va );
  void                printf( char const * fmt, ... );
  void                XyPrintf( u8g2_uint_t x, u8g2_uint_t y, char const * fmt, ... ) __attribute__ ((format (printf, 4, 5)));

  void                drawCentered( uint8_t y, char const * str );

  // Has no real visible effect
  // void setContrast( uint8_t contrast, uint8_t precharge, uint8_t comdetect );
  //? void setBrightness( uint8_t brightness );
  //? void setContrast( u8g2_uint_t contrast ) { setBrightness( contrast ); }
 };
#define Oled          TWifiKit8Oled::Instance()

//****************************************************************
#endif // sentry
