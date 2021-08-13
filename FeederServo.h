//****************************************************************

#ifndef FeederServoH  // sentry
#define FeederServoH

//****************************************************************
// Includes
//****************************************************************

#include <GnEsp8266Basics.h>

//****************************************************************
// Sw Configuration
//****************************************************************

        // All Values in µs
#define ServoDefault_MinPulseWidthUs     500   //  500µs = linker  Anschlag
#define ServoDefault_MaxPulseWidthUs    2500   // 2500µs = rechter Anschlag
#define ServoDefault_MidPulseWidthUs    1500   // 1500µs = Mittelstellung
#define ServoDefault_PeriodUs          20000   // 20000us

#define ServoDefault_FullTurnMs         5000   // Angleichung von Soll- und Ist-Wert, so das eine volle Drehbewegung 8s dauert
#define ServoDefault_HoldTimeMs         1000   // Nach Angleichung noch 1s lang den Soll-Puls ausgeben

//****************************************************************
// TFeederServo
//****************************************************************

class TFeederServo : public TSingleton<TFeederServo>
 {
  friend class TSingleton<TFeederServo>;
 private:
                      TFeederServo
                       (
                        int fullTurnMs = ServoDefault_FullTurnMs,             // ms für eine volle Drehbewegung
                        int holdTimeMs = ServoDefault_HoldTimeMs,             // ms die das Soll-Signal weiter ausgegeben wird, wenn der Sollwert erreicht wurde
                        int periodUs = ServoDefault_PeriodUs,                 // Periodendauer des Servo-Signals (20ms)
                        int actPulseWidthUs = ServoDefault_MaxPulseWidthUs,   // (angenommene) Start Servo-Stellung
                        int minPulseWidthUs = ServoDefault_MinPulseWidthUs, 
                        int maxPulseWidthUs = ServoDefault_MaxPulseWidthUs
                       );
                      ~TFeederServo(void) { Detach(); }

 private: // internal use only
  bool                FRunning;
  int16_t             FPinMask;
  int                 FHoldCounter;
  int                 FNextLoadValueClks;

 private: // Used in ISR, set via methods
  int                 FSetPulseWidthClks;

  int                 FPinNr;
  int                 FPeriodClks;
  int                 FChangeSpeedClks;
  int                 FHoldCycles;
  int                 FActPulseWidthClks;
  int                 FMinPulseWidthClks;
  int                 FMaxPulseWidthClks;

  int                 FSetPulseWidthClkArray[ 2 ];
  int                 FRepeats;

  static IRAM_ATTR
    void              FeederServoIrq(void);
  
 public:
  bool                Running(void);
  int                 SetPulseWidthUs(void);
  int                 ActPulseWidthUs(void);

  bool                Attach( int pinNr );
  void                Detach(void);
  
  bool                Start( int firstSetPulseWidthUs, int secondSetPulseWidthUs = -1, int repeats = 0 );
 };

#define FeederServo TFeederServo::Instance()

//****************************************************************
#endif // sentry
