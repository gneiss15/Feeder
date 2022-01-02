//****************************************************************

#ifndef GnCoopTimerH  // sentry
#define GnCoopTimerH

//****************************************************************
// Description
//
// Classes to implement Coop(erative)Timers
//****************************************************************

//****************************************************************
// Usage:
//
// Instantiate an TGnCoopTimerUs/Ms os S with the needed interval
// (in µs, ms or s) and supply a callback, that should be called
// when the timer elapses.
//
// Callbacks:
// - void Cb( TGnCoopTimerBase * tmr, uint16_t intervalsElapsed )
//   Will be called whenever the Timer elapses.
//   tmr:              the timer that calls the callback
//   intervalsElapsed: normaly 1, but may be greater if Loop
//                     (see below) was not called in time or if
//                     timer was resumed when more than one
//                     interval has elapsed
//
// Methods:
// - Set/GetCb       change/query the aktual callback at any time
// - Set/GetInterval change/query the aktual interval at any time
// - Enabled         query the state of the timer
// - Start           Timer will fire at now + interval
// - Stop            Stop is the same as Pause
// - Resume          Timer will fire (even when prev. stopped) at
//                   the same time as if it wasn't Stopped/Paused 
// - ReStart         Timer will fire after the time remaining when
//                   he was paused/stopped
//
// Remarks:
//
// All TGnCoopTimer... classes are derived from TLoopInstance,
// which implies, that inside "loop" there is a call to:
// LoopInstances.Loop();
// This enshures, that all instances of TGnCoopTimer... are
// regulary called.
//****************************************************************

//****************************************************************
// Includes
//****************************************************************

#include <GnBasics.h>
#include <Closures.h>

//****************************************************************
// Sw Configuration
//****************************************************************

//****************************************************************
// Public Vars & Methods of GnOled
//****************************************************************

//****************************************************************
// TGnCoopTimerBase (Base class)
//****************************************************************

class TGnCoopTimerBase;
typedef TClosure<void( TGnCoopTimerBase * tmr, uint16_t intervalsElapsed )> TGnCoopTimerCb;

typedef unsigned long TGnCoopTimer_t;

class TGnCoopTimerBase : TLoopInstance
 {
 protected:
  // implemented in derived classes to define the Unit for one Tick (us, ms, s...)
  virtual 
   TGnCoopTimer_t     GetAkt(void)                                                    = 0;
  virtual void        Loop(void);
  
 protected:
  TGnCoopTimerCb      FTimerCb;
  TGnCoopTimer_t      FInterval;
  TGnCoopTimer_t      FStop;      // GetAkt() at Stop called
  TGnCoopTimer_t      FNextEvent; // Fire Time
  bool                FEnabled;

 protected:
                      TGnCoopTimerBase(void)                                          {}
                      ~TGnCoopTimerBase(void)                                         {}
  void                Init( TGnCoopTimerCb timerCb,
                            TGnCoopTimer_t interval,
                            bool enabled );
 public:
  void                SetCb( TGnCoopTimerCb timerCb )                                 { FTimerCb = timerCb; }
  TGnCoopTimerCb      GetCb(void)                                                     { return FTimerCb; }

                      // Timer will fire at last now + new interval
  void                SetInterval( TGnCoopTimer_t interval );
  uint16_t            GetInterval(void)                                               { return FInterval; }
  
  bool                Enabled(void)                                                   { return FEnabled; }

                      // Timer will fire at now + interval
  void                Start(void);
                      // Stop is the same as Pause
  void                Stop(void);
                      // Timer will fire (even when prev. stopped) at the same time as if it wasn't Stopped/Paused 
  void                Resume(void)                                                    { FEnabled = true; }
                      // Timer will fire after the remaining time when he was paused/stopped
  void                ReStart(void);
 };

//****************************************************************
// TGnCoopTimerUs, TGnCoopTimerMs & TGnCoopTimerS
//****************************************************************

// Timer-Unit = us
class TGnCoopTimerUs : public TGnCoopTimerBase
 {
 protected:
  virtual 
   TGnCoopTimer_t     GetAkt(void)                                                    { return micros(); }

 public:
                      TGnCoopTimerUs( TGnCoopTimerCb timerCb,
                                      TGnCoopTimer_t interval, 
                                      bool enabled = true )                           { Init( timerCb, interval, enabled ); }
                      ~TGnCoopTimerUs(void)                                           = default;
 };

// Timer-Unit = ms
class TGnCoopTimerMs : public TGnCoopTimerBase
 {
 protected:
  virtual 
   TGnCoopTimer_t     GetAkt(void)                                                    { return millis(); }

 public:
                      TGnCoopTimerMs( TGnCoopTimerCb timerCb,
                                      TGnCoopTimer_t interval,
                                      bool enabled = true )                           { Init( timerCb, interval, enabled ); }
                      ~TGnCoopTimerMs(void)                                           = default;
 };

// Timer-Unit = s
class TGnCoopTimerS : public TGnCoopTimerBase
 {
 protected:
  virtual 
   TGnCoopTimer_t     GetAkt(void)                                                    { return time( NULL ); }

 public:
                      TGnCoopTimerS( TGnCoopTimerCb timerCb,
                                     TGnCoopTimer_t interval,
                                     bool enabled = true )                            { Init( timerCb, interval, enabled ); }
                      ~TGnCoopTimerS(void)                                            = default;
 };

//****************************************************************
#endif // sentry

