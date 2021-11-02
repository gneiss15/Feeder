//****************************************************************

#ifndef FeedTimesH  // sentry
#define FeedTimesH

//****************************************************************
// Includes
//****************************************************************

#include <GnEsp8266Basics.h>

//****************************************************************
// Sw Configuration
//****************************************************************

#define MaxFeedRecords  10

//****************************************************************
// typedefs
//****************************************************************

typedef struct
 {
  uint8_t             So : 1;
  uint8_t             Mo : 1;
  uint8_t             Di : 1;
  uint8_t             Mi : 1;
  uint8_t             Do : 1;
  uint8_t             Fr : 1;
  uint8_t             Sa : 1;
  uint8_t             Active : 1;
 }
 TWdayMask;

typedef union
 {
  uint8_t             mask;
  TWdayMask           bits;
 }
 TWdayUnion;

typedef struct
 {
  uint8_t             h;
  uint8_t             m;
  TWdayUnion          wday;
  uint8_t             portions;
 }
 TFeedRecord;

typedef union
 {
  TFeedRecord         FeedRecords[ MaxFeedRecords ];
 }
 TNvData;

//****************************************************************
// TFeedTimes
//****************************************************************

class TFeedTimes : public TSingleton<TFeedTimes>, TLoopInstance
 {
  friend class TSingleton<TFeedTimes>;
 private:
                      TFeedTimes(void);
 private:
  TNvData             FNvData;

 private:
                      // Set FeedTime( r ) .h & .m to argVal ("hh:mm")
  void                SetTimeString( uint8_t r, String argVal );
                      // if tm is a active FeedRecord, return portions for that time, else 0
  uint8_t             CheckTime( struct tm const & tm );

  bool                Load( String const & jsonStr );
  bool                Save(void);

 protected:
                      // Perform "CheckTime" every Minute
  virtual void        Loop(void);

 public:
  bool                Set( String const & jsonStr )               { return Load( jsonStr ) && Save(); }
 
 public:
                      // setup & load data
  void                Setup(void)                                 {}
 };
#define FeedTimes   TFeedTimes::Instance()

//****************************************************************
#endif // sentry
