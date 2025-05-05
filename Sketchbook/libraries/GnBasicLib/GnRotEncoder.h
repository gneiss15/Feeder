//****************************************************************

#ifndef GnRotEncoderH  // sentry
#define GnRotEncoderH

//****************************************************************
// Description
//
// (Singelton-)Class to handle a Rotary Encoder
//****************************************************************

//****************************************************************
// Usage:
//
// 1. Add a call to setup():
//    GnRotEncoder.Attach( ClkPin, DirPin, BtnPin );
//    That will define the pins the Encoder is conneted to
// 2. Add (at least) one of the following calls to  setup():
//    GnRotEncoder.SetRotCb( Closure( RotCb ) );
//    GnRotEncoder.SetBtnCb( Closure( BtnCb ) );
//    Witch each call you suply a Callback, that gets called when
//    the event in question ocours.
// 3. Add a call to loop():
//    LoopInstances.Loop();
//    see Remarks for Reason
//
// Callbacks:
// - void RotCb( int16_t & absCnt, int16_t relCnt )
//   Will be called whenever the RotCnt changes.
//   absCnt: the number of steps the Rotary has done since reseted
//           Callback may reset/change this value
//   relCnt: the relative steps done since last call of Callback
//           It will be usually only 1 or -1, but may be bigger
//           if the Callback was disabled (set to nothing) while
//           the knob was turned.
// - void BtnCb( bool up )
//   Will be called whenever the Button was pressed or released
//   up: indicates the state of the button
//
// Remarks:
//
// Each Callback may be disabled (set to nothing) by calling
// SetRotCb / SetBtnCb with no argument.
//
// This Module uses TGnCoopTimer which is a class derived from
// TLoopInstance, which in turn implies, that inside "loop" there
// is a call to: LoopInstances.Loop();
// This enshures, that all instances of TLoopInstance are
// regulary called.
//****************************************************************

//****************************************************************
// Implementation Detail:
//
// The polling aproach was choosen, because the used Rotary Encoder
// (a KY-040 type) produces very noisy outputs.
// Each Input-Pin is sampled every 10ms.
// Only if the input has the same value for 8 samples, the internal
// state of this input is set to this value.
// Only this internal state is then used for further calculations.
// This implements a digital low pass filter.
// Because the encoder has mechanical steps that produce 2 signal
// steps, these signal steps are counted by an internal "FastRotCnt".
// The callback is only called when the knob has reached one of his
// "stable" positions (every second internal count).
//****************************************************************

//****************************************************************
// Includes
//****************************************************************

#include <GnBasics.h>
#include <GnCoopTimer.h>
//#include <Closures.h>	included by GnCoopTimer.h

//****************************************************************
// Sw Configuration
//****************************************************************

//****************************************************************
// TGnRotEncoder
//****************************************************************

typedef TClosure<void( int16_t & absCnt, int16_t relCnt )> TGnRotEncoderRotCb;
typedef TClosure<void( bool up )> TGnRotEncoderBtnCb;

class TGnRotEncoder : public TSingleton<TGnRotEncoder>
 {
  friend class TSingleton<TGnRotEncoder>;
 private:
                      TGnRotEncoder(void);
                      ~TGnRotEncoder(void)                                            { Detach(); }
 private:
  TGnCoopTimerMs      RotEncTmr;

  int16_t             ClkPinMask;
  int16_t             DirPinMask;
  int16_t             BtnPinMask;

  uint8_t             ClkInputs;
  uint8_t             DirInputs;
  uint8_t             BtnInputs;

  uint8_t             FastRotState;
  int16_t             FastRotCnt;
  bool                FastBtnState;

  int16_t             LastRotCnt;

  int16_t             RotCnt;

  void                PollAndDebouncePins( TGnCoopTimerBase *, uint16_t );

  TGnRotEncoderRotCb  FRotCb;
  TGnRotEncoderBtnCb  FBtnCb;
                      
 public:
  bool                Attach( int clkPinNr, int dirPinNr, int btnPinNr );
  void                Detach(void);

  void                Clear(void);

											// SetRotCb(); or  SetBtnCb(); will clear the Callback
  void                SetRotCb( TGnRotEncoderRotCb rotCb = NULL )                     { FRotCb = rotCb; }
  TGnRotEncoderRotCb  GetRotCb(void)                                                  { return FRotCb; }
  void                SetBtnCb( TGnRotEncoderBtnCb btnCb = NULL )                     { FBtnCb = btnCb; }
  TGnRotEncoderBtnCb  GetBtnCb(void)                                                  { return FBtnCb; }
 };

#define GnRotEncoder TGnRotEncoder::Instance()

//****************************************************************
#endif // sentry

