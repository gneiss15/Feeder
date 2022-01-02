//****************************************************************

#ifndef GnEsp8266BasicsH  // sentry
#define GnEsp8266BasicsH

//****************************************************************
// Sw Configuration
//****************************************************************

#include <GnBasics.h>

//****************************************************************
// Includes
//****************************************************************

#include <ets_sys.h>

//****************************************************************
// Global Types
//****************************************************************

//****************************************************************
// Global Macros
//****************************************************************

//****************************************************************
// Functions (GPIO)
//****************************************************************

// Only Pins 0..15 !
static inline void    SetPinM( uint16_t pinMask )                                     { GPOS = pinMask; }
static inline void    SetPin( uint8_t pinNr )                                         { assert( pinNr <= 15 ); SetPinM( Mask( pinNr ) ); }
static inline void    ClrPinM( uint16_t pinMask )                                     { GPOC = pinMask; }
static inline void    ClrPin( uint8_t pinNr )                                         { assert( pinNr <= 15 ); ClrPinM( Mask( pinNr ) ); }
static inline void    TogglePinM( uint16_t pinMask )                                  { GPO ^= pinMask; }
static inline void    TogglePin( uint8_t pinNr )                                      { assert( pinNr <= 15 ); TogglePinM( Mask( pinNr ) ); }
static inline bool    GetOutPinM( uint16_t pinMask )                                  { return GPO & pinMask; }
static inline bool    GetOutPin( uint8_t pinNr )                                      { assert( pinNr <= 15 ); return GetOutPinM( Mask( pinNr ) ); }
static inline bool    GetInPinM( uint16_t pinMask )                                   { return GPI & pinMask; }
static inline bool    GetInPin( uint8_t pinNr )                                       { assert( pinNr <= 15 ); return GetInPinM( Mask( pinNr ) ); }

//        Func1       Func2   Func2 Beschriftung  Nutzbar
// GPIO00 PRG-Button  SPI_CS2       D3            x(select booting mode)
// GPIO01 USB-Chip    SPI_CS1 UTXD  TX
// GPIO02 SDA         TXD1          SDA           x(select booting mode)
// GPIO03 USB-Chip            URXD  RX  
// GPIO04 OLED_SDA    SDA           D2
// GPIO05 OLED_SCL    SCL           D1
// GPIO06 SDIO_CLK
// GPIO07 SDIO_DATA_0
// GPIO08 SDIO_DATA_1
// GPIO09 SDIO_DATA_2
// GPIO10 SDIO_DATA_3
// GPIO11 SDIO_CMD
// GPIO12 HSPIQ               MISO  D6            x
// GPIO13 HSPID       CTS0    MOSI  D7            x
// GPIO14 HSPI                CLK   SCL           x
// GPIO15 HSPICS      RTS0    CS    D8            x(select booting mode)
// GPIO16 OLED_RST    WAKE          D0

static inline void PinMode_Output( uint8_t pinNr )
 {
  assert( pinNr == 0 || pinNr == 2 || ( pinNr >= 12 && pinNr <= 15 ) );
  GPF( pinNr ) = GPFFS( GPFFS_GPIO( pinNr ) ); // Set mode to GPIO
  GPC( pinNr ) = GPC( pinNr ) & ~( ( 1 << GPCS /* 0: SOURCE(GPIO)*/ ) | ( 1 << GPCD /* 0: DRIVER(PushPull) */ ) ); 
  GPES = ( 1 << pinNr ); // Output
 }
static inline void PinMode_OutputOpenDrain( uint8_t pinNr )
 {
  assert( pinNr == 0 || pinNr == 2 || ( pinNr >= 12 && pinNr <= 15 ) );
  GPF( pinNr ) = GPFFS( GPFFS_GPIO( pinNr ) ); // Set mode to GPIO
  GPC( pinNr ) =  ( GPC( pinNr ) & ~( 1 << GPCS /* 0: SOURCE(GPIO) */ )  ) | ( 1 << GPCD /* 1: DRIVER(OpenDrain) */ ) ; 
  GPES = ( 1 << pinNr ); // Output
 }
static inline void PinMode_Input( uint8_t pinNr )
 {
  assert( pinNr == 0 || pinNr == 2 || ( pinNr >= 12 && pinNr <= 15 ) );
  GPF( pinNr ) = GPFFS( GPFFS_GPIO( pinNr ) ); // Set mode to GPIO
  GPEC = ( 1 << pinNr ); // Input
  GPC( pinNr ) = ( GPC( pinNr ) & ~( 1 << GPCS /* 0: SOURCE(GPIO) */ ) ) | ( 1 << GPCD /* 1: DRIVER(OpenDrain) */ );
 }
static inline void PinMode_InputPullUp( uint8_t pinNr )
 {
  assert( pinNr == 0 || pinNr == 2 || ( pinNr >= 12 && pinNr <= 15 ) );
  GPF( pinNr ) = GPFFS( GPFFS_GPIO( pinNr ) ) | ( 1 << GPFPU ) ; // Set mode to GPIO & enable PullUp
  GPEC = ( 1 << pinNr ); // Input
  GPC( pinNr ) = ( GPC( pinNr ) & ~( 1 << GPCS /* 0: SOURCE(GPIO) */ ) ) | ( 1 << GPCD /* 1: DRIVER(OpenDrain) */ );
 }
  
// For GPIO-Irqs see: core_esp8266_wiring_digital.cpp or core_esp8266_wiring_digital.c
  
//****************************************************************
// Functions (others)
//****************************************************************

static inline IRAM_ATTR uint32_t GetCycleCount(void)
 {
  uint32_t ccount;
  __asm__ __volatile__( "esync; rsr %0,ccount":"=a" ( ccount ) );
  return ccount;
 }

//****************************************************************
#endif // sentry
