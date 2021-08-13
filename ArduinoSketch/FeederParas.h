//****************************************************************

#ifndef FeederParasH  // sentry
#define FeederParasH

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
  char                AsChars[ sizeof( TFeedRecord ) * MaxFeedRecords ];
  TFeedRecord         FeedRecords[ MaxFeedRecords ];
 }
 TNvData;

//****************************************************************
// TFeederParas
//****************************************************************

class TFeederParas : public TSingleton<TFeederParas>, TLoopInstance
 {
  friend class TSingleton<TFeederParas>;
 private:
                      TFeederParas(void);
 protected:
                      // Perform "CheckTime" every Minute
  virtual void        Loop(void);

 private:
  TNvData             FNvData;

  void                Clear(void);
  String              ReadFile( char const * fileName );

 public:
                      // setup & load data
  void                Setup(void);

                      // Get Html-Page
  String              GetHtmlPage(void) { return ReadFile( "/Index.html" ); }
                      // Get Paras as JSON object (for Html-Page)
  String              GetJson(void);
                      // Set Para( r, c ) to argVal
  void                Set( uint8_t r, uint8_t c, String argVal );

                      // Save / Load Paras to / from file
  void                Save(void);
  bool                Load(void);  // true if file was OK

                      // if tm is a active FeedRecord, return portions for that time, else 0
  uint8_t             CheckTime( struct tm const & tm );
 };
#define FeederParas   TFeederParas::Instance()

//****************************************************************
#endif // sentry
