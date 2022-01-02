//****************************************************************
// Includes
//****************************************************************

#include <GnRotEncoder.h>

//****************************************************************
// Sw Configuration
//****************************************************************

//****************************************************************
// Variables
//****************************************************************


#if 0
int16_t               LastRotCnt;
int16_t               LastBtnCnt;


#endif

//****************************************************************
// TGnRotEncoder
//****************************************************************

TGnRotEncoder::TGnRotEncoder(void)
: RotEncTmr( Closure( this, &TGnRotEncoder::PollAndDebouncePins ), /*interval:10ms*/ 10, /*enabled*/ false )
, ClkPinMask( 0 )
, DirPinMask( 0 )
, BtnPinMask( 0 )
 { 
  Clear();
 }

void TGnRotEncoder::PollAndDebouncePins( TGnCoopTimerBase *, uint16_t )
 {
  static int8_t RotEncTab[ 16 ] = { 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0 };

  ClkInputs >>= 1;
  DirInputs >>= 1;
  BtnInputs >>= 1;
  if( GetInPinM( ClkPinMask ) )
    ClkInputs |= 0x80;
  if( GetInPinM( DirPinMask ) )
    DirInputs |= 0x80;
  if( GetInPinM( BtnPinMask ) )
    BtnInputs |= 0x80;

  static uint8_t lastAbState = 0;
  uint8_t abState = lastAbState;
  static bool btnState = true;

  if( ClkInputs == 0xFF )
    abState |= 8;
   else if( ClkInputs == 0 )
    abState &= ~8;
  if( DirInputs == 0xFF )
    abState |= 4;
   else if( DirInputs == 0 )
    abState &= ~4;
  if( BtnInputs == 0xFF )
    btnState = true;
   else if( BtnInputs == 0 )
    btnState = false;

  if( FastBtnState != btnState )
   {
    FastBtnState = btnState;
    if( FBtnCb )
      FBtnCb( btnState );
   }

  if( lastAbState == abState )
    return;

  lastAbState = abState;
  FastRotState = ( FastRotState >> 2 ) | abState;
  int dir = RotEncTab[ FastRotState ];
  if( dir == 0 )
    return;
    
  FastRotCnt += dir;
  if( ( FastRotCnt & 1 ) != 0 )
    return;

  RotCnt += dir;
  if( FRotCb )
   {
    FRotCb( RotCnt, RotCnt - LastRotCnt );
    LastRotCnt = RotCnt;
   }
 }

bool TGnRotEncoder::Attach( int clkPinNr, int dirPinNr, int btnPinNr )
 {
  Detach();

  if( clkPinNr < 0 || clkPinNr > 15 || dirPinNr < 0 || dirPinNr > 15 || btnPinNr < 0 || btnPinNr > 15 )
    return false;
  
  ClkPinMask = 1 << clkPinNr;
  DirPinMask = 1 << dirPinNr;
  BtnPinMask = 1 << btnPinNr;

  PinMode_InputPullUp( clkPinNr );
  PinMode_InputPullUp( dirPinNr );
  PinMode_InputPullUp( btnPinNr );
  
  Clear();
  
  RotEncTmr.Start();
  
  return true;
 }

void TGnRotEncoder::Detach(void)
 {
  RotEncTmr.Stop();
  ClkPinMask = DirPinMask = BtnPinMask = 0;
 }

void TGnRotEncoder::Clear(void)
 {
  ClkInputs = DirInputs = BtnInputs = FastRotState = FastRotCnt = LastRotCnt = RotCnt = 0;
  FastBtnState = true;
 }

