// CD: Config Definition
// CD( htmlType, typ, name, defValue [, extraHtmlText] )
// CD_Div( htmlType ) just for HTML to make an optical diversition betwwen parameters

// htmlType: (see although Index.js, function CD)
// t: text, creates a input type=text
// T: text, creates a input type=password
// n: int,  creates a input type=number
// p: int,  creates a input type=number		Pin-Number	0..15
// P: int,  creates a input type=number		Percent-Number	0..100
// u: uint, creates a input type=number		0..10000
// w: enum, creates a select for WiFiMode_t
// d: pseudo type, creates a padding line	(CD_Div)

// WiFiMode_t: WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3
// see: https://siytek.com/esp8266-ap-and-station-mode/
//#define WlanApChannel 1   // Don't fix the Channel
//#define WlanApHidden  true
//#define WlanApMaxCon  1

#ifndef CD
  #error CD must be defined before including FeederConfig.inc.h
#endif
#ifndef CD_Div
  #define CD_Div
#endif

CD( w, WiFiMode_t, WifiMode,     WIFI_AP )
CD_Div

CD( t, String,     WlanSsid,     "" )
CD( T, String,     WlanPw,       "" )
CD( t, String,     NtpServerAdr, "de.pool.ntp.org" )
CD( t, String,     TZ,           "CET-1CEST,M3.5.0,M10.5.0/3" )
CD_Div

// Ap: Acess-Point
CD( t, String,     ApSsid,       "Feeder1" )
CD( T, String,     ApPw,         "Feeder1" )
CD_Div

// FTP: pw for FTP
CD( T, String,     FtpPw,        "Feeder1" )
CD_Div

// Hostname: Used for Wlan, Ap & FTP
CD( t, String,     Hostname,     "Feeder1" )
CD_Div

// Hardware-Config
CD( p, int,        ServoPin,      0, "0-15" )
CD( p, int,        DirPin,       13, "0-15" )
CD( p, int,        ClkPin,       12, "0-15" )
CD( p, int,        BtnPin,       14, "0-15" )

#undef CD
#undef CD_Div

