// CD: Config Definition
// CD( typ, name, defValue )

#ifndef CD
  #error CD must be defined before including FeederCong.inc.h
#endif

// Wlan: Station-Mode
CD( bool,   WlanEnabled,  false )                         //! not used
CD( String, WlanSsid,     "" )                            // OK, used
CD( String, WlanPw,       "" )                            // OK, used
CD( String, NtpServerAdr, "de.pool.ntp.org")              // OK, used
CD( String, TZ,           "CET-1CEST,M3.5.0,M10.5.0/3" )  // OK, used

// Ap: Acess-Point
CD( bool,   ApEnabled,    true )                          //! not used
CD( String, ApSsid,       "Futterautomat_1" )             // OK, used
CD( String, ApPw,         "Futterautomat_1" )             // OK, used

// Hostname: Used for Wlan & Ap
CD( String, Hostname,     "Feeder" )                      // OK, used

// Hardware-Config
CD( int,    ServoPin,     0 )
CD( int,    DirPin,       13 )
CD( int,    ClkPin,       12 )
CD( int,    BtnPin,       14 )

#undef CD
#undef CD_String
#undef CD_int
#undef CD_bool

// see: https://siytek.com/esp8266-ap-and-station-mode/
//#define WlanApChannel 1   // Don't fix the Channel
//#define WlanApHidden  true
//#define WlanApMaxCon  1
