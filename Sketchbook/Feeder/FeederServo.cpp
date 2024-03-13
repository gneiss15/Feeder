//****************************************************************
// Includes
//****************************************************************

#include "FeederServo.h"
#include <LittleFS.h>
#include "FeederConfig.h"
#include <ArduinoJson.h>

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

TFeederServo::TFeederServo( int32_t periodUs, int32_t minPulseWidthUs, int32_t maxPulseWidthUs )
: FRunning( false )
, FPinNr( -1 )
, FPeriodClks( Us2Clks( periodUs ) )
, FMinPulseWidthClks( Us2Clks( minPulseWidthUs ) )
, FMaxPulseWidthClks( Us2Clks( maxPulseWidthUs ) )
, FServoSteps( NULL )
 {
  FActPulseWidthClks = FMaxPulseWidthClks;      // (angenommene) Start Servo-Stellung

  FdPulseWidthClks = FMaxPulseWidthClks - FMinPulseWidthClks;
  FdPulseWidthClks_Mul_PeriodClks = FdPulseWidthClks * FPeriodClks;
  Load( ReadFile( F( "/ServoSteps.json" ) ) );
 }
TFeederServo::~TFeederServo(void)
 {
  Detach();
  delete [] FServoSteps; FServoSteps = NULL;
 }

int32_t TFeederServo::Position_Grad2Soll_Clks( double position_Grad )
 {
  return FdPulseWidthClks * position_Grad / Servo_FullTurn_Grad + FMinPulseWidthClks;
 }
int32_t TFeederServo::Speed_ms2Speed_Clks( uint32_t speed_ms )
 {
  if( speed_ms < 20 )
    speed_ms = 20;
  if( speed_ms > 3000 )
    speed_ms = 3000;

  return Clks2Us( FdPulseWidthClks_Mul_PeriodClks / Us2Clks( speed_ms * 1000 ) );
 }
int32_t TFeederServo::Hold_ms2Hold_Counts( uint32_t hold_ms )
 {
  return hold_ms / ( Servo_Period_us / 1000 ) + 1;
 }

double TFeederServo::Soll_Clks2Position_Grad( int32_t soll_Clks )
 {
  return Servo_FullTurn_Grad * ( soll_Clks - FMinPulseWidthClks ) / FdPulseWidthClks;
 }
uint32_t TFeederServo::Speed_Clks2Speed_ms( int32_t speed_Clks )
 {
  return Clks2Us( FdPulseWidthClks_Mul_PeriodClks / Us2Clks( speed_Clks ) / 1000 );
 }
uint32_t TFeederServo::Hold_Counts2Hold_ms( int32_t hold_Counts )
 {
  return ( hold_Counts - 1 ) * ( Servo_Period_us / 1000 );
 }

bool TFeederServo::Save(void)
 {
  if( !FServoSteps )
    return false;

  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();;

  for( TServoStep * pSservoStep = FServoSteps;  pSservoStep->speed_Clks > 0;  pSservoStep++ )
   {
    JsonArray row = arr.add<JsonArray>();
    row.add( Soll_Clks2Position_Grad( pSservoStep->soll_Clks ) );
    row.add( Speed_Clks2Speed_ms( pSservoStep->speed_Clks ) );
    row.add( Hold_Counts2Hold_ms( pSservoStep->hold_Counts ) );
   }

  {
   // Block, so jsonStrOld & jsonStrNew may be dealocated when block is left
   String jsonStrOld = ReadFile( F( "/ServoSteps.json" ) );
   String jsonStrNew;
   serializeJson( doc, jsonStrNew );

   if( jsonStrOld == jsonStrNew )
     return true;
  }

  File f = LittleFS.open( "/ServoSteps.json", "w" );
  if( !f ) 
   {
    Debug( "Can't open file for writing: /ServoSteps.json\n" );
    return false;
   }
  serializeJson( doc, f );
  f.close();
  return true;
 }

bool TFeederServo::Load( String const & jsonStr )
 {
  JsonDocument doc;
  DeserializationError error = deserializeJson( doc, jsonStr );
  //D Debug( "TFeederServo::Load: %s, bool:%s\njson: %s\n", error ? "NOK" : "OK", error.c_str(), jsonStr.c_str() ); //D 
  if( error != DeserializationError::Ok ) // if( error ) ???
    return false;
  JsonArray arr = doc.as<JsonArray>();

  // Calculate nrOfSteps (Entries with speed >= 20)
  size_t nrOfSteps = 0;
  for( JsonVariant value : arr )
   {
    JsonArray row = value;
    if( row[ 1 ].as<int32_t>() < 20 )
      break;
    ++nrOfSteps;
   }

  if( nrOfSteps < 2 )
    return false;

  TServoStep * newServoSteps = new TServoStep[ nrOfSteps + 1 ];
  TServoStep & lastStep = newServoSteps[ nrOfSteps ];
  lastStep.soll_Clks = lastStep.speed_Clks = lastStep.hold_Counts = 0;

  // Get & convert the values
  uint8_t r = 0;
  for( JsonVariant value : arr )
   {
    JsonArray row = value;
    if( row[ 1 ].as<int32_t>() < 20 )
      break;

    //D Debug( "TFeederServo::Load: Step-Nr:%d\n", r );
    //D Debug( "  position:%f, speed_ms:%u, hold_ms:%u\n", row[ 0 ].as<double>(), row[ 1 ].as<uint32_t>(), row[ 2 ].as<uint32_t>() ); //D 
   
    TServoStep & outStep = newServoSteps[ r++ ];
    outStep.soll_Clks = Position_Grad2Soll_Clks( row[ 0 ].as<double>() );
    outStep.speed_Clks = Speed_ms2Speed_Clks( row[ 1 ].as<uint32_t>() );
    outStep.hold_Counts = Hold_ms2Hold_Counts( row[ 2 ].as<uint32_t>() );
    //D Debug( "  soll_Clks:%d, speed_Clks:%d, hold_Counts:%d\n", outStep.soll_Clks, outStep.speed_Clks, outStep.hold_Counts ); //D 
   }

  // Allocate new Array & Set last entry to [0,0,0]
  delete [] FServoSteps;
  FServoSteps = newServoSteps;

  return true;
 }

bool TFeederServo::Running(void)
 {
  //D Debug( "TFeederServo::Running(): Running: %s, FActPulseWidthClks: %d, FHoldCounter: %d, FRepeats: %d, FPwmCycles:%d\n", FRunning ? "true" : "false", FActPulseWidthClks, FHoldCounter, FRepeats, FPwmCycles );
  FPwmCycles = 0;

  return FRunning;
 }

void TFeederServo::Setup(void)
 {
  Attach( FeederConfig.ServoPin );
 }

bool TFeederServo::Attach( int pinNr )
 {
  if( pinNr < 0 || pinNr > 15 )
    return false;

  if( FPinNr == pinNr )
    return true;

  if( FPinNr >= 0 )
    Detach();
    
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

bool TFeederServo::Feed( int32_t repeats )
 {
  if( !FServoSteps || FPinNr < 0 || FRunning )
    return false;

  FActStep = FServoSteps;
  FNextLoadValueClks = FActPulseWidthClks;
  FHoldCounter = 0;
  FRepeats = repeats;

  T1L = FPeriodClks - FNextLoadValueClks;

  // Config Timer
  /*
  T1C = ( 1 << TCTE )
      | ( ( TIM_DIV16 & 3 ) << TCPD )
      | ( ( TIM_EDGE & 1 ) << TCIT )
      | ( ( TIM_SINGLE & 1 ) << TCAR );
  */
  timer1_enable( TIM_DIV16, TIM_EDGE, TIM_SINGLE );
  
  FRunning = true;
  TEIE |= TEIE1; // Edge int enable

  return true;
 }

bool TFeederServo::Stop(void)
 {
  if( FRunning )
   {
    T1C &= ~( 1 << TCTE );  // Timer Off
    FRunning = false;
   }
  return true;
 }

IRAM_ATTR void TFeederServo::FeederServoIrq(void)
 {
  T1L = FeederServo.FNextLoadValueClks;
  TogglePinM( FeederServo.FPinMask );

  FeederServo.FNextLoadValueClks = FeederServo.FPeriodClks - FeederServo.FNextLoadValueClks;
  if( GetOutPinM( FeederServo.FPinMask ) == 0 )
   {
    FeederServo.FPwmCycles++;
    if( FeederServo.FHoldCounter > 0 )
     {
      if( --FeederServo.FHoldCounter <= 0 )
       {
        FeederServo.FActStep++;
        if( FeederServo.FActStep->speed_Clks <= 0 )
         {
          if( --FeederServo.FRepeats <= 0 )
           {
            FeederServo.FRunning = false;
            T1C &= ~( 1 << TCTE );  // Timer Off
            return;   // Skip "Edge int enable"
           }
          FeederServo.FActStep = FeederServo.FServoSteps;
         }
       }
     }
     else
     {
      int32_t diff = FeederServo.FActStep->soll_Clks - FeederServo.FActPulseWidthClks;
      if( diff )
       {
        FeederServo.FNextLoadValueClks = FeederServo.FActPulseWidthClks += diff > 0 
          ? diff > FeederServo.FActStep->speed_Clks
            ? FeederServo.FActStep->speed_Clks
            : diff
          : diff < -FeederServo.FActStep->speed_Clks
            ? -FeederServo.FActStep->speed_Clks
            : diff;
        if( FeederServo.FActStep->soll_Clks == FeederServo.FActPulseWidthClks )
          FeederServo.FHoldCounter = FeederServo.FActStep->hold_Counts;
       }
     }
   }

  TEIE |= TEIE1; // Edge int enable
 }
