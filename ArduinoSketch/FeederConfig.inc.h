// CD: Config Definition
// CD( htmlType, typ, name, defValue )
// CD_Div( htmlType ) just for HTML to make an optical diversition betwwen parameters

#ifndef CD
  #error CD must be defined before including FeederCong.inc.h
#endif
#ifndef CD_Div
  #define CD_Div( htmlType )  
#endif

// Wlan: Station-Mode
CD( c, bool,   WlanEnabled,  false                        ) //! not used
CD( t, String, WlanSsid,     ""                           ) // OK, used
CD( p, String, WlanPw,       ""                           ) // OK, used
CD( t, String, NtpServerAdr, "de.pool.ntp.org"            ) // OK, used
CD( t, String, TZ,           "CET-1CEST,M3.5.0,M10.5.0/3" ) // OK, used
CD_Div( d )

// Ap: Acess-Point
CD( c, bool,   ApEnabled,    true                         ) //! not used
CD( t, String, ApSsid,       "Futterautomat_1"            ) // OK, used
CD( p, String, ApPw,         "Futterautomat_1"            ) // OK, used
CD_Div( d )

// Hostname: Used for Wlan & Ap
CD( t, String, Hostname,     "Feeder"                     ) // OK, used
CD_Div( d )

// Hardware-Config
CD( n, int,    ServoPin,     0  )
CD( n, int,    DirPin,       13 )
CD( n, int,    ClkPin,       12 )
CD( n, int,    BtnPin,       14 )

#undef CD
#undef CD_Div
#undef CD_String
#undef CD_int
#undef CD_bool

// see: https://siytek.com/esp8266-ap-and-station-mode/
//#define WlanApChannel 1   // Don't fix the Channel
//#define WlanApHidden  true
//#define WlanApMaxCon  1
