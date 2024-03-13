//****************************************************************

#ifndef FeederServoH  // sentry
#define FeederServoH

//****************************************************************
// Includes
//****************************************************************

#include <GnEsp8266Basics.h>

//****************************************************************
// typedefs
//****************************************************************

typedef struct _TServoStep
 {
  int32_t  soll_Clks;
  int32_t  speed_Clks;
  int32_t  hold_Counts;
 }
 TServoStep;
 
typedef struct _TJsonServoStep
 {
  double    position_Grad;
  uint32_t  speed_ms;
  uint32_t  hold_ms;
 }
 TJsonServoStep;

//****************************************************************
// Sw Configuration
//****************************************************************

#define Servo_MinPw_us             500   //  500µs = linker  Anschlag = 0°
#define Servo_MaxPw_us            2500   // 2500µs = rechter Anschlag = 180°
#define Servo_Period_us          20000   // 20000us

#define Servo_FullTurn_Grad        180.

//****************************************************************
// TFeederServo
//****************************************************************

class TFeederServo : public TSingleton<TFeederServo>
 {
  friend class TSingleton<TFeederServo>;
 private:
                      TFeederServo
                       (
                        int32_t periodUs = Servo_Period_us,         // Periodendauer des Servo-Signals (20ms)
                        int32_t minPulseWidthUs = Servo_MinPw_us, 
                        int32_t maxPulseWidthUs = Servo_MaxPw_us
                       );
                      ~TFeederServo(void);

 private:
  int32_t             Position_Grad2Soll_Clks( double position_Grad );
  int32_t             Speed_ms2Speed_Clks( uint32_t speed_ms );
  int32_t             Hold_ms2Hold_Counts( uint32_t hold_ms );

  double              Soll_Clks2Position_Grad( int32_t soll_Clks );
  uint32_t            Speed_Clks2Speed_ms( int32_t speed_Clks );
  uint32_t            Hold_Counts2Hold_ms( int32_t hold_Counts );

  bool                Save(void);
  bool                Load( String const & jsonStr );

 private: // initialised in constructor
  bool                FRunning;
  int                 FPinNr;

  int32_t             FPeriodClks;
  int32_t             FMinPulseWidthClks;
  int32_t             FMaxPulseWidthClks;
  int32_t             FActPulseWidthClks;
                      // helper vars for faster calculation
  int32_t             FdPulseWidthClks;
  int32_t             FdPulseWidthClks_Mul_PeriodClks;

  TServoStep *        FServoSteps;

 private: // internal use only
  int16_t             FPinMask;
  TServoStep const *  FActStep;
  int32_t             FNextLoadValueClks;
  int32_t             FHoldCounter;
  int32_t             FRepeats;

  int32_t             FPwmCycles;

 public:
  bool                Running(void);
  bool                Set( String const & jsonStr )               { return Load( jsonStr ) && Save(); }
                      // setup & load data
  void                Setup(void);
  bool                Attach( int pinNr );
  void                Detach(void);
  bool                Feed( int32_t repeats = 1 );
  bool                Stop(void);

 private:
  static IRAM_ATTR
    void              FeederServoIrq(void);
 };

#define FeederServo TFeederServo::Instance()

//****************************************************************
#endif // sentry
