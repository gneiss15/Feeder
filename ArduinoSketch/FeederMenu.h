//****************************************************************

#ifndef FeederMenuH  // sentry
#define FeederMenuH

//****************************************************************
// Includes
//****************************************************************

#include <GnEsp8266Basics.h>
#include "Menu.h"

//****************************************************************
// TFeederStatusMenu
//****************************************************************

class TFeederStatusMenu : public TBaseMenu
 {
  typedef TBaseMenu Base;
  friend class TFeederMenu;
 protected:
                      TFeederStatusMenu(void);
                      ~TFeederStatusMenu(void) {}

 private:
  bool                FInModeChg;
  uint8_t             FMode;
  unsigned long       FBlinkTicker;
  uint8_t             FBlinkColor;  // 1: Blink Off, 0: Blink On

 private:
  void                ModeChgEnter(void);
  void                ModeChgExit(void);
  void                ModeChgPerform(void);

 protected:
  virtual void        Perform_(void);

 public:
  virtual void        Enter(void);
  virtual void        Exit(void);
  
  virtual bool        HandleKey( int8_t key );
 };

//****************************************************************
// TFeederTimeMenu
//****************************************************************

class TFeederTimeMenu : public TBaseMenu
 {
  typedef TBaseMenu Base;
  friend class TFeederMenu;
 protected:
                      TFeederTimeMenu(void) {};
                      ~TFeederTimeMenu(void) {}

  virtual void        Perform_(void);

 public:
  //virtual void      Enter(void) {}
  //virtual void      Exit(void) {}
  
  //virtual bool      HandleKey( int8_t key );
 };

//****************************************************************
// TFeederMFeedMenu
//****************************************************************

class TFeederMFeedMenu : public TBaseMenu
 {
  typedef TBaseMenu Base;
  friend class TFeederMenu;
 protected:
                      TFeederMFeedMenu(void) {};
                      ~TFeederMFeedMenu(void) {}

  virtual void        Perform_(void);

 public:
  //virtual void      Enter(void) {}
  //virtual void      Exit(void) {}
  
  virtual bool        HandleKey( int8_t key );
 };

//****************************************************************
// TFeederMenu
//****************************************************************

class TFeederMenu : public TLoopInstance
 {
 private:
                      TFeederMenu(void);
                      ~TFeederMenu(void) {}
  static TFeederMenu  Instance;                        

 private:
  static TBaseMenu *  FMenus[];
  uint8_t             FMenuIdx;

 private:
  void                RotCb( int16_t & absCnt, int16_t relCnt );
  void                BtnCb( bool up );

  void                HandleKey( int8_t key );

 protected:

 public:
  void                Loop(void);

  void                SelectMenu( uint8_t menuIdx );
  void                NextMenu(void);
  void                PrevMenu(void);
 };


//****************************************************************
#endif // sentry
