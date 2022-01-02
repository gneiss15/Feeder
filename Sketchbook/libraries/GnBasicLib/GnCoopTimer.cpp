//****************************************************************
// Includes
//****************************************************************

#include <GnCoopTimer.h>

//****************************************************************
// Sw Configuration
//****************************************************************

//****************************************************************
// TGnCoopTimerBase
//****************************************************************

void TGnCoopTimerBase::Loop(void)
 {
  if( !FEnabled )
    return;

  TGnCoopTimer_t akt = GetAkt();

  // Time to do the Action ?
  if( int( akt - FNextEvent ) < 0 )
    return;
    
  // Calculate Intervals elapsed
  uint16_t intervalsElapsed = ( akt - FNextEvent ) / FInterval + 1;
  // Call the Action-Routine (if specified)
  if( FTimerCb )
    FTimerCb( this, intervalsElapsed );

  // Timer is now disabled, don't calculate next Action-Time
  if( !FEnabled )
    return;

  // Calculate next Action-Time
  FNextEvent += FInterval * intervalsElapsed;
 }

void TGnCoopTimerBase::Init( TGnCoopTimerCb timerCb, TGnCoopTimer_t interval, bool enabled )
 {
  FTimerCb = timerCb;
  FInterval = interval;
  FStop = GetAkt();
  FNextEvent = FStop + FInterval;
  FEnabled = enabled;
 }

void TGnCoopTimerBase::SetInterval( TGnCoopTimer_t interval )
 {
  FNextEvent += interval - FInterval;
  FInterval = interval;
 }

void TGnCoopTimerBase::Start(void)
 {
  FStop = GetAkt();
  FNextEvent = FStop + FInterval;
  FEnabled = true;
 }

void TGnCoopTimerBase::Stop(void)
 {
  if( !FEnabled )
    return;
  FEnabled = false;
  FStop = GetAkt();
 }

void TGnCoopTimerBase::ReStart(void)
 {
  if( FEnabled )
    return;
  FNextEvent += GetAkt() - FStop;
  FEnabled = true;
 }

