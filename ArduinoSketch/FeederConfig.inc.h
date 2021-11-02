// CD: Config Definition
// CD( htmlType, typ, name, defValue )
// CD_Div( htmlType ) just for HTML to make an optical diversition betwwen parameters

// htmlType: (see although Index.js, function CD)
// c: bool, creates a checkbox
// t: text, creates a input type=text
// p: text, creates a input type=password
// n: number, creates a input type=number
// e: enum, creates a select for WifiMode
// d: pseudo type, creates a padding line

// WifiMode: from ESP8266WiFiType.h: WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3

#ifndef CD
  #error CD must be defined before including FeederCong.inc.h
#endif
#ifndef CD_Div
  #define CD_Div( htmlType )  
#endif

// Wlan: Station-Mode
CD( e, TWifiMode, WifiMode,     WIFI_AP                      )
CD_Div( d )

CD( t, String,    WlanSsid,     ""                           )
CD( p, String,    WlanPw,       ""                           )
CD( t, String,    NtpServerAdr, "de.pool.ntp.org"            )
CD( t, String,    TZ,           "CET-1CEST,M3.5.0,M10.5.0/3" )
CD_Div( d )

// Ap: Acess-Point
CD( t, String,    ApSsid,       "Futterautomat_1"            )
CD( p, String,    ApPw,         "Futterautomat_1"            )
CD_Div( d )

// Hostname: Used for Wlan & Ap
CD( t, String,    Hostname,     "Feeder"                     )
CD_Div( d )

// Hardware-Config
CD( n, int,       ServoPin,     0  )
CD( n, int,       DirPin,       13 )
CD( n, int,       ClkPin,       12 )
CD( n, int,       BtnPin,       14 )

#undef CD
#undef CD_Div
#undef CD_String
#undef CD_int
#undef CD_bool

// see: https://siytek.com/esp8266-ap-and-station-mode/
//#define WlanApChannel 1   // Don't fix the Channel
//#define WlanApHidden  true
//#define WlanApMaxCon  1
