#ifndef ClosuresH    // sentry
#define ClosuresH
#pragma once

#include <GnBasics.h>
#include <memory.h> // to allow <,> comparisons

// Shortcuts used
// Gen:   Generic
// MF:    Member Function
// SF:    Static Function ("normal Function)
// GpMF:  Generic Pointer to Member Function 
// GC:    GenericClass

/*
 * __cplusplus:
 * 199711L for the 1998 C++ standard
 * 201103L for the 2011 C++ standard
 * 201402L for the 2014 C++ standard
 * 201703L for the 2017 C++ standard
 * > 201703L for the experimental languages enabled by -std=c++2a and -std=gnu++2a.
*/

#if __cplusplus < 201103L
  #error > C++11
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#pragma GCC diagnostic ignored "-Wunused-parameter"

//****************************************************************
// Helper templates
//****************************************************************

namespace Detail { // hide the implementation details in a nested namespace.

// implicit_cast< > .Usage is identical to static_cast<>
template<class O, class I>
  inline O            implicit_cast( I in )
   {
    return in;
   }

// union_cast< >. Usage is identical to reinterpret_cast<>.
template<class O, class I>
  inline O            union_cast( I const in )
   {
    union { O out; I in; } u;
    CTassert( sizeof( I ) == sizeof( u ) && sizeof( I ) == sizeof( O ), "compiler has peculiar unions" );
    u.in = in;
    return u.out;
   }

//****************************************************************
// Conversion of member function pointer to a standard form
//****************************************************************

// GC is a fake class, ONLY used to provide a type.
// It is vitally important that it is never defined, so that the compiler doesn't
// think it can optimize the invocation.
class GC;
typedef void ( GC::* TpSMF )(void);
typedef GC * TpGC;

// SimplifyMF< >::Convert()
//
// A template function that converts an arbitrary member function pointer into the 
// simplest possible form of member function pointer, using a supplied 'this' pointer.
// According to the standard, this can be done legally with reinterpret_cast<>.
// For (non-standard) compilers which use member function pointers which vary in size 
// depending on the class, we need to use knowledge of the internal structure of a 
// member function pointer, as used by the compiler. Template specialization is used
// to distinguish between the sizes. Because some compilers don't support partial 
// template specialisation, I use full specialisation of a wrapper struct.

// general case -- don't know how to convert it. Force a compile failure
template<int N>
  struct SimplifyMF
   {
    template<class X, class TXFunc, class TMFP>
      inline static GC * Convert( X * pthis, TXFunc func, TMFP & boundFunc )
       {
        // Unsupported member function type -- force a compile failure.
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wunused-local-typedefs"
          typedef char ERROR_Unsupported_member_function_pointer_on_this_compiler[ N - 100 ];
        #pragma GCC diagnostic push
        return 0;
       }
     };

// For compilers where all member func ptrs are the same size, everything goes here.
// For non-standard compilers, only single_inheritance classes go here.
template<>
  struct SimplifyMF<sizeof( TpSMF )> // The size of a single inheritance member function pointer.
   {
    template<class X, class TXFunc, class TMFP>
      inline static TpGC Convert( X * pthis, TXFunc func, TMFP & boundFunc )
       {
        boundFunc = reinterpret_cast<TMFP>( func );
        return reinterpret_cast<TpGC>( pthis );
       }
   };

//****************************************************************
// Define the TClosure storage, and cope with static functions
//****************************************************************

// TClosure_<>
//
// It's the class that does most of the actual work.
// The signatures are specified by:
// GpMF: must be a type of GC member function pointer. 
// SpF: must be a type of function pointer with the same signature as GpMF.

// An outer class, TClosure<>, handles the invoking and creates the necessary typedefs.
// This class does everything else.

// For compilers where data pointers are at least as big as code pointers, it is 
// possible to store the function pointer in the this pointer, using a cast.
// +-FThis -+- FFunc-+-- Meaning----------------------+
// |   0    |    0   | Empty                          |
// |  !=0   |   !=0  | Static function or method call |
// +--------+--------+--------------------------------+
template<class GpMF, class SpF>
  class TClosure_
   {
   private: 
    TpGC              FThis;
    TpSMF             FFunc;

   public:
                      TClosure_(void)                                                 : FThis( NULL ), FFunc( 0 ) {};
    void              Clear(void)                                                     { FThis = NULL; FFunc = 0;  }

   public:
    inline bool       IsEqual( TClosure_ const & o ) const                            { return FThis == o.FThis && FFunc == o.FFunc; }
    inline bool       IsLess( TClosure_ const & o ) const                             { return FThis != o.FThis ? FThis < o.FThis : uint_t( FFunc ) < uint_t( o.FFunc ); } //##
    inline bool       operator!() const                                               { return FThis == NULL && FFunc == 0; }
    inline bool       empty(void) const                                               { return FThis == NULL && FFunc == 0; }
   public:
    TClosure_ &       operator=( TClosure_ const & o )                                { Assign( o ); return *this; }
    inline bool       operator<( TClosure_ const & o )                                { return IsLess( o ); }
    inline bool       operator>( TClosure_ const & o )                                { return o.IsLess( *this ); }
                      TClosure_( TClosure_ const & o )                                : FThis( o.FThis ), FFunc( o.FFunc ) {}
   protected:
    void              Assign( TClosure_ const & o )                                   { FFunc = o.FFunc; FThis = o.FThis; }
  
   public:
    // Convert any MF into a standard form.
    // TXMF should be a member function of class X, but this can't be
    // enforce that here. It needs to be enforced by the wrapper class.
    template<class X, class TXMF>
      inline void     bindMF( X * pthis, TXMF func )                                  { FThis = SimplifyMF<sizeof( func )>::Convert( pthis, func, FFunc ); }
    // For const member functions, we only need a const class pointer.
    // Since we know that the member function is const, it's safe to 
    // remove the const qualifier from the 'this' pointer with a const_cast.
    template<class X, class TXMF>
      inline void     bindMF( X const * pthis, TXMF func )                            { FThis = SimplifyMF<sizeof( func )>::Convert( const_cast<X*>( pthis ), func, FFunc ); }

    // These functions are required for invoking the stored function
    inline GC *       GetThis(void) const                                             { return FThis; }
    inline GpMF       GetMFP(void) const                                              { return reinterpret_cast<GpMF>( FFunc ); }

    template<class DerivedClass>
      inline void     Assign( DerivedClass * pParent, TClosure_ const & o )           { Assign( o ); }

    // For static functions, the 'static_function_invoker' class in the parent 
    // will be called. The parent then needs to call GetpSF() to find out 
    // the actual function to invoke.
    template<class DerivedClass, class ParentInvokerSig>
      inline void bindSF( DerivedClass * pParent, ParentInvokerSig sfInvoker, SpF func )
       {
        if( func == NULL )
          FFunc = NULL;
         else
          // We'll be ignoring the 'this' pointer, but we need to make sure we pass
          // a valid value to bindMF().
          bindMF( pParent, sfInvoker );

        CTassert( sizeof( GC * ) == sizeof( func ), 
                  "function and data pointers have different sizes: Does not work on this compiler!" );
        FThis = union_cast<GC *>( func );
       }

    // This function will be called with an invalid 'this' pointer!!
    // We're just returning the 'this' pointer, converted into a function pointer!
    inline SpF GetpSF(void) const                                
     {
      CTassert( sizeof( SpF * ) == sizeof( this ), 
                "function and data pointers have different sizes: Does not work on this compiler!" );
      return union_cast<SpF>( this );
     }
  
    // Does the closure contain this static function?
    inline bool       IsEqualToSpF( SpF funcptr )                                     { return funcptr == NULL ? empty() : funcptr == reinterpret_cast<SpF>( GetpSF() ); }
   };

} // namespace Detail

//****************************************************************
// Wrapper classes to ensure type safety
//****************************************************************

// TClosure<ret_t, ...parameter_list>
// All this class does is to enforce type safety, and invoke TClosure_ with the correct list of parameters.

// Because of the weird rule about the class of derived member function pointers,
// you sometimes need to apply a downcast to the 'this' pointer.
// This is the reason for the use of "implicit_cast<X*>(pthis)" in the code below. 
// If CDerivedClass is derived from CBaseClass, but doesn't override SimpleVirtualFunction,
// without this trick you'd need to write:
//    MyDelegate(static_cast<CBaseClass *>(&d), &CDerivedClass::SimpleVirtualFunction);
// but with the trick you can write
//    MyDelegate(&d, &CDerivedClass::SimpleVirtualFunction);

template<class R, typename ...P>
  class TClosure 
   {
   private:
    typedef R ( * TpSF )( P... );
    typedef R ( Detail::GC::* TpMF )( P... );
    typedef Detail::TClosure_<TpMF, TpSF> TClosureType;

    TClosureType      FClosure;

  public:
    // Typedefs to aid generic programming
    typedef TClosure type;
  
    // Construction and comparison functions
                      TClosure(void)                                                  { Clear(); }
                      TClosure( TClosure const & o )                                  { FClosure.Assign( this, o.FClosure ); }
    void              operator=( TClosure const & o )                                 { FClosure.Assign( this, o.FClosure ); }
    bool              operator==( TClosure const & o ) const                          { return FClosure.IsEqual( o.FClosure );  }
    bool              operator!=( TClosure const & o ) const                          { return !FClosure.IsEqual( o.FClosure ); }
    bool              operator<( TClosure const & o ) const                           { return FClosure.IsLess( o.FClosure );  }
    bool              operator>( TClosure const & o ) const                           { return o.FClosure.IsLess( FClosure );  }

    // Binding to non-const member functions
    template<class X, class Y>
      TClosure( Y * pthis, R( X::* func )( P... ) )                                   { FClosure.bindMF( Detail::implicit_cast<X*>( pthis ), func ); }
    template<class X, class Y>
      inline void bind( Y * pthis, R( X::* func )( P... ) )                           { FClosure.bindMF( Detail::implicit_cast<X*>( pthis ), func ); }
    // Binding to const member functions.
    template<class X, class Y>                                                        
      TClosure( Y const * pthis, R( X::* func )( P... ) const )                       { FClosure.bindMF( Detail::implicit_cast<const X *>( pthis ), func ); }
    template<class X, class Y>
      inline void bind( Y const * pthis, R( X::* func )( P... ) const )               { FClosure.bindMF( Detail::implicit_cast<const X *>( pthis ), func ); }

    // Static functions. We convert them into a member function call.
    // This constructor also provides implicit conversion
                      TClosure( R( *func )( P... ) )                                  { bind( func ); }
    void              operator=( R( *func )( P... ) )                                 { bind( func ); }
    inline void       bind( R( *func )( P... ) )                                      { FClosure.bindSF( this, &TClosure::InvokeSF, func ); }
  
    // Invoke it
    R                 operator()( P... p ) const                                      { return ( FClosure.GetThis()->*( FClosure.GetMFP() ) )( p... ); }

    // Convert to bool & Comparsion with pSF
                      operator bool() const                                           { return !FClosure ? false : true; }
    inline bool       operator==( TpSF funcptr )                                      { return FClosure.IsEqualToSpF( funcptr ); }
    inline bool       operator!=( TpSF funcptr )                                      { return !FClosure.IsEqualToSpF( funcptr ); }
    inline bool       operator!() const                                               { return !FClosure; }

    inline bool       empty(void) const                                               { return !FClosure; }
    void              Clear(void)                                                     { FClosure.Clear();}
  
  private:        // Invoker for static functions
    R                 InvokeSF( P... p ) const                                        { return ( *( FClosure.GetpSF() ) )( p... ); }
  };

//****************************************************************
// TClosure<ret_t(...parameter_list)>
// Allows boost::function style syntax
//****************************************************************

template<typename R, typename ...P>
  class TClosure<R( P... )> : public TClosure<R, P...>
   {
   public:
    typedef TClosure<R, P...> BaseType;
    typedef TClosure SelfType;
   
                      TClosure(void)                                                  : BaseType() {}
                      TClosure( R( *func )( P... ) )                                  : BaseType( func ) {}
    void              operator=( const BaseType & o )                                 { *static_cast<BaseType*>( this ) = o; }

    template<class X, class Y>
      TClosure( Y * pthis, R( X::* func )( P... ) )                                   : BaseType( pthis, func ) {}
    template < class X, class Y >
      TClosure( const Y * pthis, R( X::* func )( P... ) const )                       : BaseType( pthis, func ) {}
 };


//****************************************************************
// Closure() helper function
//
// Closure( &x, &X::func ) returns a TClosure of the type necessary for
// calling x.func() with the correct number of arguments.
// -> X must be implicitly castable to x!
// Closure( func ) returns a TClosure of the type necessary for
// calling func() with the correct number of arguments.
// This makes it possible to eliminate many typedefs from user code.
//****************************************************************

template<class X, class Y, typename R, typename ...P>
  TClosure<R( P... )> Closure( Y * x, R( X::* func )( P... ) )                        { return TClosure<R( P... )>( x, func ); }

template<typename R, typename ...P>
  TClosure<R( P... )> Closure( R( *func )( P... ) )                                   { return TClosure<R( P... )>( func ); }
  
#pragma GCC diagnostic pop

#endif    // sentry
