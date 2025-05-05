//****************************************************************

#ifndef GnBasicsH  // sentry
#define GnBasicsH

//****************************************************************
// Sw Configuration
//****************************************************************

#include <GnConfig.h>

//****************************************************************
// Includes
//****************************************************************

#include <Arduino.h>
#include <assert.h>

//****************************************************************
// Global Types
//****************************************************************

typedef unsigned int uint_t;

//****************************************************************
// Global Macros
//****************************************************************

  //****************************************************************
  // MakeName
  //****************************************************************

#define MakeName2( a, b )             MakeName2_( a, b )
#define MakeName2_( a, b )            a##b
#define MakeName3( a, b, c )          MakeName3_( a, b, c )
#define MakeName3_( a, b, c )         a##b##c
#define MakeName4( a, b, c, d )       MakeName4_( a, b, c, d )
#define MakeName4_( a, b, c, d )      a##b##c##d
#define MakeName5( a, b, c, d, e )    MakeName5_( a, b, c, d, e )
#define MakeName5_( a, b, c, d, e )   a##b##c##d##e

  //****************************************************************
  // MakeStr
  //****************************************************************

#define MakeStr( a )                  MakeStr_( a )
#define MakeStr_( a )                 #a

  //****************************************************************
  // MakeChar
  //****************************************************************

#define MAKE_CHAR_A     'A'
#define MAKE_CHAR_B     'B'
#define MAKE_CHAR_C     'C'
#define MAKE_CHAR_D     'D'
#define MAKE_CHAR_E     'E'
#define MAKE_CHAR_F     'F'
#define MAKE_CHAR_G     'G'
#define MAKE_CHAR_H     'H'
#define MAKE_CHAR_I     'I'
#define MAKE_CHAR_J     'J'
#define MAKE_CHAR_K     'K'
#define MAKE_CHAR_L     'L'
#define MAKE_CHAR_M     'M'
#define MAKE_CHAR_N     'N'
#define MAKE_CHAR_O     'O'
#define MAKE_CHAR_P     'P'
#define MAKE_CHAR_Q     'Q'
#define MAKE_CHAR_R     'R'
#define MAKE_CHAR_S     'S'
#define MAKE_CHAR_T     'T'
#define MAKE_CHAR_U     'U'
#define MAKE_CHAR_V     'V'
#define MAKE_CHAR_W     'W'
#define MAKE_CHAR_X     'X'
#define MAKE_CHAR_Y     'Y'
#define MAKE_CHAR_Z     'Z'

#define MAKE_CHAR_a     'a'
#define MAKE_CHAR_b     'b'
#define MAKE_CHAR_c     'c'
#define MAKE_CHAR_d     'd'
#define MAKE_CHAR_e     'e'
#define MAKE_CHAR_f     'f'
#define MAKE_CHAR_g     'g'
#define MAKE_CHAR_h     'h'
#define MAKE_CHAR_i     'i'
#define MAKE_CHAR_j     'j'
#define MAKE_CHAR_k     'k'
#define MAKE_CHAR_l     'l'
#define MAKE_CHAR_m     'm'
#define MAKE_CHAR_n     'n'
#define MAKE_CHAR_o     'o'
#define MAKE_CHAR_p     'p'
#define MAKE_CHAR_q     'q'
#define MAKE_CHAR_r     'r'
#define MAKE_CHAR_s     's'
#define MAKE_CHAR_t     't'
#define MAKE_CHAR_u     'u'
#define MAKE_CHAR_v     'v'
#define MAKE_CHAR_w     'w'
#define MAKE_CHAR_x     'x'
#define MAKE_CHAR_y     'y'
#define MAKE_CHAR_z     'z'

#define MAKE_CHAR_0     '0'
#define MAKE_CHAR_1     '1'
#define MAKE_CHAR_2     '2'
#define MAKE_CHAR_3     '3'
#define MAKE_CHAR_4     '4'
#define MAKE_CHAR_5     '5'
#define MAKE_CHAR_6     '6'
#define MAKE_CHAR_7     '7'
#define MAKE_CHAR_8     '8'
#define MAKE_CHAR_9     '9'

#define MakeChar( a )   MakeChar_( a )
#define MakeChar_( a )  MAKE_CHAR_##a

  //****************************************************************
  // Other Global Macros
  //****************************************************************

#define EmptyMacro()

#define MacroFunc( exp )      do exp while( 0 )

#define Mask( bitNr )         ( 1 << (bitNr) )

#define TableSize( tab )      ( sizeof( tab ) / sizeof( (tab)[ 0 ] ) )

#define IsPowerOf2( a )       ( ( (a) & ( (a) - 1 ) ) == 0 )

#define ClrVar( var )         memset( (var), 0, sizeof( (var) ) )

extern int CmpVar_Sizes_not_equal_(void);   // do not implement! => Linker error when sizeof(x) != sizeof(y)
#define CmpVar( x, y )        ( sizeof( x ) == sizeof( y ) ? memcmp( &(x), &(y), sizeof( x ) ) : CmpVar_Sizes_not_equal_() )

extern void * CopyVar_Sizes_not_equal_(void);   // do not implement! => Linker error when sizeof(x) != sizeof(y)
#define CopyVar( dst, src )   ( sizeof( dst ) == sizeof( src ) ? memcpy( &(dst), &(src), sizeof( dst ) ) : CopyVar_Sizes_not_equal_() )

#define GnDelete( ptr )       MacroFunc({ delete ptr; ptr = NULL; })
#define GnDeleteArray( ptr )  MacroFunc({ delete [] ptr; ptr = NULL; })

//****************************************************************
//  Function decorations
//****************************************************************

#define noreturn_                     __attribute__(( noreturn ))

#define packed_                       __attribute__((packed))
#define align_( size )                __attribute__((aligned(size)))

#define longcall_                     __attribute__((longcall))
#define pure_                         __attribute__((pure))

#define unused_( x )                  x __attribute__((unused))
#define unused_para( def, var )       def var __attribute__((unused))

//****************************************************************
// Functions (GPIO)
//****************************************************************

#ifndef DONT_USE_GpioPinMacros

#if defined( BOARD_HAS_PIN_REMAP ) && !defined( BOARD_USES_HW_GPIO_NUMBERS )
  #error "Pin remapping not supported"
#endif

#ifdef ESP32
  typedef uint32_t TGpioPinMask;
 #else
  typedef uint16_t TGpioPinMask;
#endif

/*
  ESP-Pins...

ESP8266		0,2,12-15
		? nput only
ESP32		0~39 except 24 & 28~31		32
		>= 34 are input only
ESP32S2		0~46 except 22~25
		46 is input only
ESP32S3		0~48 except 22~25
		No GPIO is input only
ESP32C3		0-21
		No GPIO is input only
*/

// Only Pins 0..15(ESP8266) or 0..32(ESP32) are supported !
#ifdef ESP32
  static inline void SetPinM( TGpioPinMask pinMask )                                 { GPIO_OUT_REG |= pinMask; }
  static inline void ClrPinM( TGpioPinMask pinMask )                                 { GPIO_OUT_REG &= ~pinMask; }
  static inline void TogglePinM( TGpioPinMask pinMask )                              { GPIO_OUT_REG ^= pinMask; }
  static inline bool GetOutPinM( TGpioPinMask pinMask )                              { return GPIO_OUT_REG & pinMask; }
  static inline bool GetInPinM( TGpioPinMask pinMask )                               { return GPIO_IN_REG & pinMask; }

  static inline void SetPin( uint8_t pinNr )                                         { SetPinM( Mask( pinNr ) ); }
  static inline void ClrPin( uint8_t pinNr )                                         { ClrPinM( Mask( pinNr ) ); }
  static inline void TogglePin( uint8_t pinNr )                                      { TogglePinM( Mask( pinNr ) ); }
  static inline bool GetOutPin( uint8_t pinNr )                                      { return GetOutPinM( Mask( pinNr ) ); }
  static inline bool GetInPin( uint8_t pinNr )                                       { return GetInPinM( Mask( pinNr ) ); }

  #if 0 // Not implemented yet 
    // -> GPIO_ENABLE_REG
    static inline void PinMode_Output( uint8_t pinNr )
     {
      GPF( pinNr ) = GPFFS( GPFFS_GPIO( pinNr ) ); // Set mode to GPIO
      GPC( pinNr ) = GPC( pinNr ) & ~( ( 1 << GPCS /* 0: SOURCE(GPIO)*/ ) | ( 1 << GPCD /* 0: DRIVER(PushPull) */ ) ); 
      GPES = ( 1 << pinNr ); // Output
     }
    static inline void PinMode_OutputOpenDrain( uint8_t pinNr )
     {
      GPF( pinNr ) = GPFFS( GPFFS_GPIO( pinNr ) ); // Set mode to GPIO
      GPC( pinNr ) =  ( GPC( pinNr ) & ~( 1 << GPCS /* 0: SOURCE(GPIO) */ )  ) | ( 1 << GPCD /* 1: DRIVER(OpenDrain) */ ) ; 
      GPES = ( 1 << pinNr ); // Output
     }
    static inline void PinMode_Input( uint8_t pinNr )
     {
      GPF( pinNr ) = GPFFS( GPFFS_GPIO( pinNr ) ); // Set mode to GPIO
      GPEC = ( 1 << pinNr ); // Input
      GPC( pinNr ) = ( GPC( pinNr ) & ~( 1 << GPCS /* 0: SOURCE(GPIO) */ ) ) | ( 1 << GPCD /* 1: DRIVER(OpenDrain) */ );
     }
    static inline void PinMode_InputPullUp( uint8_t pinNr )
     {
      GPF( pinNr ) = GPFFS( GPFFS_GPIO( pinNr ) ) | ( 1 << GPFPU ) ; // Set mode to GPIO & enable PullUp
      GPEC = ( 1 << pinNr ); // Input
      GPC( pinNr ) = ( GPC( pinNr ) & ~( 1 << GPCS /* 0: SOURCE(GPIO) */ ) ) | ( 1 << GPCD /* 1: DRIVER(OpenDrain) */ );
     }
  #endif

 #else
  static inline void SetPinM( TGpioPinMask pinMask )                                 { GPOS = pinMask; }
  static inline void ClrPinM( TGpioPinMask pinMask )                                 { GPOC = pinMask; }
  static inline void TogglePinM( TGpioPinMask pinMask )                              { GPO ^= pinMask; }
  static inline bool GetOutPinM( TGpioPinMask pinMask )                              { return GPO & pinMask; }
  static inline bool GetInPinM( TGpioPinMask pinMask )                               { return GPI & pinMask; }

  static inline void SetPin( uint8_t pinNr )                                         { SetPinM( Mask( pinNr ) ); }
  static inline void ClrPin( uint8_t pinNr )                                         { ClrPinM( Mask( pinNr ) ); }
  static inline void TogglePin( uint8_t pinNr )                                      { TogglePinM( Mask( pinNr ) ); }
  static inline bool GetOutPin( uint8_t pinNr )                                      { return GetOutPinM( Mask( pinNr ) ); }
  static inline bool GetInPin( uint8_t pinNr )                                       { return GetInPinM( Mask( pinNr ) ); }

  static inline void PinMode_Output( uint8_t pinNr )
   {
    GPF( pinNr ) = GPFFS( GPFFS_GPIO( pinNr ) ); // Set mode to GPIO
    GPC( pinNr ) = GPC( pinNr ) & ~( ( 1 << GPCS /* 0: SOURCE(GPIO)*/ ) | ( 1 << GPCD /* 0: DRIVER(PushPull) */ ) ); 
    GPES = ( 1 << pinNr ); // Output
   }
  static inline void PinMode_OutputOpenDrain( uint8_t pinNr )
   {
    GPF( pinNr ) = GPFFS( GPFFS_GPIO( pinNr ) ); // Set mode to GPIO
    GPC( pinNr ) =  ( GPC( pinNr ) & ~( 1 << GPCS /* 0: SOURCE(GPIO) */ )  ) | ( 1 << GPCD /* 1: DRIVER(OpenDrain) */ ) ; 
    GPES = ( 1 << pinNr ); // Output
   }
  static inline void PinMode_Input( uint8_t pinNr )
   {
    GPF( pinNr ) = GPFFS( GPFFS_GPIO( pinNr ) ); // Set mode to GPIO
    GPEC = ( 1 << pinNr ); // Input
    GPC( pinNr ) = ( GPC( pinNr ) & ~( 1 << GPCS /* 0: SOURCE(GPIO) */ ) ) | ( 1 << GPCD /* 1: DRIVER(OpenDrain) */ );
   }
  static inline void PinMode_InputPullUp( uint8_t pinNr )
   {
    GPF( pinNr ) = GPFFS( GPFFS_GPIO( pinNr ) ) | ( 1 << GPFPU ) ; // Set mode to GPIO & enable PullUp
    GPEC = ( 1 << pinNr ); // Input
    GPC( pinNr ) = ( GPC( pinNr ) & ~( 1 << GPCS /* 0: SOURCE(GPIO) */ ) ) | ( 1 << GPCD /* 1: DRIVER(OpenDrain) */ );
   }
#endif


//?#define GnPinMask( pin )              ( (TGpioPinMask)( 1 ) << ( pin ) )
//?#define GpioModReg            ( (volatile TGpioPinMask *)GPIO_ENABLE_REG )

  
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

#else // USE_GpioPinMacros

// ...PinM: Not Usable
#define SetPin( pinNr )                                     digitalWrite( pinNr, 1 )
#define ClrPin( pinNr )                                     digitalWrite( pinNr, 0 )
#define TogglePin( pinNr )                                  digitalWrite( pinNr, !GetOutPin( pinNr ) )
//?#define GetOutPin( pinNr )                                  digitalReadOutputPin( pinNr ) 
#define GetOutPin( pinNr )                                  digitalRead( pinNr ) 
#define GetInPin( pinNr )                                   digitalRead( pinNr ) 

#define PinMode_Output( pinNr )                             pinMode( FPinNr, OUTPUT )
#define PinMode_OutputOpenDrain( pinNr )                    pinMode( FPinNr, OUTPUT_OPEN_DRAIN )
#define PinMode_Input( pinNr )                              pinMode( FPinNr, INPUT )
#define PinMode_InputPullUp( pinNr )                        pinMode( FPinNr, INPUT_PULLUP )

#endif // USE_GpioPinMacros

//****************************************************************
// CTassert & derivates
//****************************************************************

  /*
  #define CTassert( cond, msg ) ...
    cond is a compile-time integral or pointer expression
    msg is a C++ identifier that does not need to be defined
    If expr is zero, a compile-time error is generated
    - msg will appear in the error message

  #define CTerror( msg ) ...
    always generates a compile-time error

  #define CTassertTypeSizeEQ( name, size ) ...
    Check that 'sizeof( name ) == size'
    a compile-time error is generated if condition is not fullfiled
    Depending on compiler the message may contain the size of 'name'

  #define CTassertTypeSizeGE( name, size ) ...
    Similar to CTassertTypeSizeEQ, but condition is 'sizeof( name ) >= size'

  #define CTallUnionSizeCheck( unionName ) ...
    primary used for unions with a member 'All' that should be (at least)
    as long as the union
  */

#define CTerror( msg ) \
  CTassert( false, msg )
#define CTassertTypeSizeEQ( name, size ) \
  CTassert( sizeof( name ) == size, Sizeof_##name##_not_equal_expected_size )
#define CTassertTypeSizeLE( name, maxsize ) \
  CTassert( sizeof( name ) <= maxsize, Sizeof_##name##_greater_than_max_size )
#define CTassertTypeSizeGE( name, minsize ) \
  CTassert( sizeof( name ) >= minsize, Sizeof_##name##_less_than_min_size )
#define CTallUnionSizeCheck( unionName ) \
  CTassert( sizeof( unionName ) == sizeof( ((unionName*)0)->All ), unionName##_size_invalid )

#define CTassert( cond, msg ) \
  static_assert( cond, msg )

//****************************************************************
// GnAssert
//****************************************************************

// Generate call to failed_ with: __FILE__, __LINE__, & msg
#define GnFail( msg ) \
  failed_( __FILE__, MakeStr( __LINE__ ), msg )

// Call GnFail if an assertion fails in a all builds.
#define GnCheck( cond, msg )     \
  MacroFunc \
   ({ \
    if( !(cond) ) GnFail( msg ); \
   })

// Call GnFail if an assertion fails in a debug build.
#if !defined(FINAL_VERSION)
  #define GnAssert( cond, msg ) GnCheck( cond, msg )
 #else
  #define GnAssert( cond, msg )
#endif

// Assert in debug builds, leave the code in other builds.
#if !defined(FINAL_VERSION)
  #define GnVerify( exp, msg )  GnCheck( exp, msg )
 #else
  #define GnVerify( exp, msg  ) ( exp )
#endif

extern noreturn_ void failed_
 (
  char const * file,
  char const * line,
  char const * msg
 );

//****************************************************************
// Functions (others)
//****************************************************************

template <typename t>
  void Restrict( t & var, t min, t max )
   {
    if( var < min )
      var = min;
     else if( var > max )
      var = max;
   }
  
// Convert uint8_t to 2 digit String
inline String TwoDigitStr( uint8_t const v )
 {
  if( v < 10 )
    return String( "0" ) + String( v );
  return String( v );
 }

inline String TimeString( uint8_t const h, uint8_t const m )
 {
  return TwoDigitStr( h ) + ":" + TwoDigitStr( m );
 }

inline String TimeString( uint8_t const h, uint8_t const m, uint8_t const s )
 {
  return TimeString( h, m ) + ":" + TwoDigitStr( s );
 }

// Read File from LittleFs into String
extern String ReadFile( char const * fileName );
extern String ReadFile( __FlashStringHelper const * fileName );

//****************************************************************
// Policy Classes
//****************************************************************

class no_defaultcreate
 {
 protected:
  inline              no_defaultcreate(void) = delete;
 };

class no_copy
 {
 private:
                      no_copy( no_copy const & src ) = delete;
 public:
  inline              no_copy(void) throw() = default;
 };

class no_assign
 {
 private:
  template<class T>
    no_assign &       operator=( T const & rhs ) = delete;
 public:
  inline              no_assign(void) throw() = default;
 };

template <class T>
  class TSingleton : no_copy, no_assign
   {
   protected:
    inline            TSingleton(void) throw() = default;
                      ~TSingleton(void) throw() = default;
   public:
    static inline T & Instance(void);
   };
template<class T>
 /*static*/ T &       TSingleton<T>::Instance(void)
  {
   static T SInstance;
   return SInstance;
  }

//****************************************************************
// TLoopInstances
//
// List of Instances, that needs to be called inside loop
//****************************************************************

// This (Singlton-)class maintains a list of all instances of
// TLoopInstance.
// To enshure a regulary call of all methods "Loop" of all instances
// insert a call to
// LoopInstances.Loop();
// inside loop() of the scetch

class TLoopInstances;
class TLoopInstance;

class TLoopInstances : public TSingleton<TLoopInstances>
 {
  friend class TSingleton<TLoopInstances>;
  friend class TLoopInstance;
 protected:
  TLoopInstance *     FList;
 private:
                      TLoopInstances(void)              : FList( NULL ) {}
                      ~TLoopInstances(void)             { FList = NULL; }
 protected:
  void                Add( TLoopInstance * obj );
  void                Remove( TLoopInstance * obj );
 public:
  void                Loop(void);
 };
#define LoopInstances TLoopInstances::Instance()
 
//****************************************************************
// TLoopInstance
//****************************************************************

// The method "Loop" of each class derived from TLoopInstance will be
// called by a call to
// LoopInstances.Loop();

class TLoopInstance
 {
  friend class TLoopInstances;
 private:
  virtual void        Loop(void)                        = 0;
  TLoopInstance *     FNext;
 public:
  TLoopInstance *     GetNext(void) { return FNext; }
  
 public:
                      TLoopInstance(void)               : FNext( NULL ) { LoopInstances.Add( this ); }
                      ~TLoopInstance(void)              { LoopInstances.Remove( this ); }
 };
 
//****************************************************************
// Log
//****************************************************************
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter" 

class TLog : public TSingleton<TLog>
 {
  friend class TSingleton<TLog>;
 private:
                       TLog(void);
 public:
  void                 Printf( char const * fmt, ... )  __attribute__ ((format (printf, 2, 3)));
 };
#define Log( fmt, ... )    TLog::Instance().Printf( fmt, ## __VA_ARGS__ )

//****************************************************************
// Debug
//****************************************************************

#if defined(DEBUG)
  #define Debug( fmt, ... ) TLog::Instance().Printf( fmt, ## __VA_ARGS__ )
 #else
  //**************************************************************
  // Non-Debug: Vanish All Debug-Code
  //**************************************************************
  #define Debug( ... ) 
#endif 

#pragma GCC diagnostic pop

//****************************************************************
#endif // sentry
