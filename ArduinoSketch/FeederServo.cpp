//****************************************************************
// Includes
//****************************************************************

#include "FeederServo.h"

//****************************************************************
// Sw Configuration
//****************************************************************

#define TimerClocksPerUs      5   // FRC1 always runs at 80MHz

//****************************************************************
// Macros
//****************************************************************

#define Us2Clks( us )   ( (us)   * TimerClocksPerUs )
#define Clks2Us( clks ) ( (clks) / TimerClocksPerUs )

//****************************************************************
// TFeederServo
//****************************************************************

TFeederServo::TFeederServo( int fullTurnMs, int holdTimeMs, int periodUs, int actPulseWidthUs, int minPulseWidthUs, int maxPulseWidthUs )
: FRunning( false )
, FPinNr( -1 )
, FPeriodClks( Us2Clks( periodUs ) )
, FChangeSpeedClks( Us2Clks( ( maxPulseWidthUs - minPulseWidthUs ) * periodUs /  ( fullTurnMs * 1000 ) ) )
, FHoldCycles( holdTimeMs * 1000. / periodUs + 0.5 )
, FActPulseWidthClks( Us2Clks( actPulseWidthUs ) ) // (angenommene) Start Servo-Stellung
, FMinPulseWidthClks( Us2Clks( minPulseWidthUs ) )
, FMaxPulseWidthClks( Us2Clks( maxPulseWidthUs ) )
 {
  Restrict( FActPulseWidthClks, FMinPulseWidthClks, FMaxPulseWidthClks );
 }

bool TFeederServo::Running(void)
 {
  return FRunning;
 }
int TFeederServo::SetPulseWidthUs(void)
 {
  return Clks2Us( FSetPulseWidthClks );
 }
int TFeederServo::ActPulseWidthUs(void)
 {
  return Clks2Us( FActPulseWidthClks );
 }

bool TFeederServo::Attach( int pinNr )
 {
  if( FPinNr < 0 )
    Detach();
    
  if( FPinNr > 15 )
    return false;
    
  FPinNr = pinNr;
  FPinMask = Mask( pinNr );

  PinMode_Output( FPinNr );
  ClrPinM( FPinMask );

  timer1_disable(); // Disconnect from "normal" library
  ETS_FRC_TIMER1_INTR_ATTACH( NULL, NULL );

  T1I = 0;  // Clear (possible) Level-IRq of Timer

  ETS_FRC_TIMER1_NMI_INTR_ATTACH( FeederServoIrq );
  return true;
 }

void TFeederServo::Detach(void)
 {
  if( FPinNr < 0 )
    return;

  T1C &= ~( 1 << TCTE );  // Timer Off
  FRunning = false;
  ETS_FRC_TIMER1_NMI_INTR_ATTACH( NULL );
  FPinNr = -1;
 }

bool TFeederServo::Start( int firstSetPulseWidthUs, int secondSetPulseWidthUs, int repeats )
 {
  if( FPinNr < 0 )
    return false;
 
  FSetPulseWidthClks = FSetPulseWidthClkArray[ 0 ] = Us2Clks( firstSetPulseWidthUs );
  FSetPulseWidthClkArray[ 1 ] = Us2Clks( secondSetPulseWidthUs );
  FRepeats = secondSetPulseWidthUs <= 0 ? 1 : repeats * 2;

  FNextLoadValueClks = FActPulseWidthClks;
  T1L = FPeriodClks - FNextLoadValueClks;

  // Config Timer
  T1C = ( 1 << TCTE )
      | ( ( TIM_DIV16 & 3 ) << TCPD )
      | ( ( TIM_EDGE & 1 ) << TCIT )
      | ( ( TIM_SINGLE & 1 ) << TCAR );
  
  FRunning = true;
  TEIE |= TEIE1; // Edge int enable
  
  return true;
 }
 
IRAM_ATTR void TFeederServo::FeederServoIrq(void)
 {
  T1L = FeederServo.FNextLoadValueClks;
  TogglePinM( FeederServo.FPinMask );

  FeederServo.FNextLoadValueClks = FeederServo.FPeriodClks - FeederServo.FNextLoadValueClks;
  if( GetOutPinM( FeederServo.FPinMask ) == 0 )
   {
    int diff = FeederServo.FSetPulseWidthClks - FeederServo.FActPulseWidthClks;
    if( diff )
     {
      FeederServo.FNextLoadValueClks = FeederServo.FActPulseWidthClks += diff > 0 
        ? diff > FeederServo.FChangeSpeedClks
          ? FeederServo.FChangeSpeedClks
          : diff
        : diff < -FeederServo.FChangeSpeedClks
          ? -FeederServo.FChangeSpeedClks
          : diff;
      FeederServo.FNextLoadValueClks = FeederServo.FActPulseWidthClks;
      if( FeederServo.FSetPulseWidthClks == FeederServo.FActPulseWidthClks )
        FeederServo.FHoldCounter = FeederServo.FHoldCycles;
     }
     else if( FeederServo.FHoldCounter > 0 && --FeederServo.FHoldCounter <= 0 )
     {
      if( --FeederServo.FRepeats <= 0 )
       {
        FeederServo.FRunning = false;
        T1C &= ~( 1 << TCTE );  // Timer Off
        return;   // Skip "Edge int enable"
       }
      FeederServo.FSetPulseWidthClks = FeederServo.FSetPulseWidthClkArray[ FeederServo.FRepeats & 1 ]; // first- or secondSetPulseWidth
     }
   }

  TEIE |= TEIE1; // Edge int enable
 }
