//****************************************************************

#ifndef MenuH  // sentry
#define MenuH

//****************************************************************
// Includes
//****************************************************************

#include <GnEsp8266Basics.h>

//****************************************************************
// Sw Configuration
//****************************************************************

#define Sc_WmenuLines     3
#define MaxWmenuLines     3     // # of Lines that fits onto screen in Text WrapMenu

//****************************************************************
// TBaseMenu
//****************************************************************

class TBaseMenu
 {
 protected:
                      TBaseMenu(void) {};
                      //~TBaseMenu(void) {}

  virtual void        Perform_(void) {} // Drawing of Menu-Content

 protected:
  uint_t              LineY( int8_t idx ) { return 9 + idx * 11; }    // Assume 3 Lines on Display

 public:
  void                Perform(void);  // Actions before & after drawing (clearBuffer, Perform_, sendBuffer)

  virtual void        Enter(void) {}
  virtual void        Exit(void) {}
  
  virtual bool        HandleKey( int8_t /*key*/ ) { return false; }
 };

//****************************************************************
// TBaseWrapMenu
//****************************************************************

class TBaseWrapMenu : public TBaseMenu
 {
  typedef TBaseMenu Base;
 protected:
                      TBaseWrapMenu( int8_t lines, int8_t idx );
                      //~TBaseWrapMenu(void) {}

 protected:
  int8_t              FMIdx;    // Index of current Menu-Entry (absolut)
  int8_t              FTop;     // Index of first Menu-Entry (absolut)

  int8_t              FLines;
  int8_t              FIdx;

 private:
  void                DrawBar(void);

 protected:
  virtual void        Perform_(void);    // Actions before & after Wrap-Menu-Drawing (adjust positions, Draw Bar)

  uint_t              LineY( int8_t idx ) { return idx * 11; }
  virtual void        Line( int8_t /*idx*/, int8_t /*mIdx*/, bool /*isCursorLine*/ ) {} // Prints given line

 public:
  //virtual void      Enter(void) {}
  //virtual void      Exit(void) {}
  
  virtual bool        HandleKey( int8_t key );
 };


//****************************************************************
#endif // sentry
