//
// (c) 2003 Marc-Antoine Ruel
// Common utilities working in VC6, VC7 and VC7.1
// An assert much faster than the one of CRT and that breaks
// on the correct line.
// Also everything is designed to work in UNICODE and 8 bits.
//
// What's inside:
// - intrinsics (if supported) for AddressOfReturnAddress, ReturnAddress and InterlockedXXX.
// - __Txxx__ for TCHAR support of __FILE__, __FUNCSIG__, __FUNCTION__ and __FUNCDNAME__.
// - __Wxxx__ for WCHAR support of ....
// - dbgSetThreadName and SetThreadName to set the thread name while in VC6/7.
// - FUNC_ENTERx(x) for automatic context printout, useful in multiprocess development.
// - DbgPrintf(( x )) for debug-only printout.
// - ASSERT for right on-place breakpoint.
// - STATIC_ASSERT for compile-time assert.
// - Break() for debug-only hard breakpoint.
// - super define for VC7 __super support.
// - C compilable.
//

#pragma once

#ifdef  __cplusplus
#define EXT_C extern "C"
#else
#define EXT_C
#endif

// should include <assert.h> BEFORE is ever included in the project
#undef assert
#undef ASSERT
#define ASSERT assert
#undef ATLASSERT
#define ATLASSERT assert


#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__		WIDEN(__FILE__)
#define __WFUNCSIG__	WIDEN(__FUNCSIG__)
#define __WFUNCTION__	WIDEN(__FUNCTION__)
#define __WFUNCDNAME__	WIDEN(__FUNCDNAME__)

#if defined(_UNICODE)
	#define __TFILE__		__WFILE__
	#define __TFUNCSIG__	__WFUNCSIG__
	#define __TFUNCTION__	__WFUNCTION__
	#define __TFUNCDNAME__	__WFUNCDNAME__
#else
	#define __TFILE__		__FILE__
	#define __TFUNCSIG__	__FUNCSIG__
	#define __TFUNCTION__	__FUNCTION__
	#define __TFUNCDNAME__	__FUNCDNAME__
#endif

// Support for VC6
#if _MSC_VER < 1300
	/// Function Name
	#define __FUNCTION__	""
	/// Function Signature (with parameters and return type)
	#define __FUNCSIG__		""
	/// Function Decorated Name
	#define __FUNCDNAME__	""
	#define __noop			((void)0)
	//#define __debugbreak()  (__asm { __asm int 3 } , __noop)	// I don't know why I have a problem with this one
	#define __debugbreak()	DebugBreak()
#endif

// For compatibility w/ kernel mode style
#pragma warning(disable:4668) // 'DBG' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#if DBG
#define _DEBUG 1
#endif


#if defined(_DEBUG)
	// The macros break when returns true
	int __stdcall MyAssert(const TCHAR * String, const TCHAR * Function, const TCHAR * File, const int Line);

	// Could use #ifdef _NTDEF_ instead
	#if defined(NT_INST)
		// Kernel-mode
		#define INTERNAL_DEBUG_PRINT	DbgPrint
		#define UTILS_BASE				_T("%s")
		// ProcessId/ThreadId is usually a non-sense in kernel mode
		#define UTILS_LOCINFO				
		#define QUICKASSERT( _X_ )				( (!(_X_)) ? __debugbreak() : __noop )
	#else
		// User-mode
		#define INTERNAL_DEBUG_PRINT	OutputDebugOutputf
		#define UTILS_BASE				_T("%4d:%4d %s")
		#define UTILS_LOCINFO			, GetCurrentProcessId(), GetCurrentThreadId()
		void OutputDebugOutputf(const TCHAR * fmt, ...);
	#endif

	#define assert( _X_ )					( (!(_X_)) && MyAssert( _T(#_X_), __TFUNCSIG__, __TFILE__, __LINE__) ? __debugbreak() : __noop )
	#define DbgPrintf( _X_ )				INTERNAL_DEBUG_PRINT _X_
	#define FUNCTION_LOG()					DbgPrintf( (_T("%s\n"), __TFUNCTION__) )
	#define Break()							__debugbreak()
	#define dbgSetThreadNameOneTime(_X_)	SetThreadNameOneTime((DWORD)-1, _X_)
	#define dbgSetThreadName(_X_)			SetThreadName((DWORD)-1, _X_)
#else

	#define DbgPrintf( _X_ )				__noop
	#define assert( _X_ )					__noop
	#define FUNCTION_LOG()					__noop
	#define Break()							__noop
	#define dbgSetThreadNameOneTime(_X_)	__noop
	#define dbgSetThreadName(_X_)			__noop
#endif


#define FUNC_ENTER()					DbgPrintf( (UTILS_BASE       _T("\n") UTILS_LOCINFO, __TFUNCTION__) )
#define FUNC_ENTER0(T)					DbgPrintf( (UTILS_BASE _T(T) _T("\n") UTILS_LOCINFO, __TFUNCTION__) )
#define FUNC_ENTER1(T, A)				DbgPrintf( (UTILS_BASE _T(T) _T("\n") UTILS_LOCINFO, __TFUNCTION__, A) )
#define FUNC_ENTER2(T, A, B)			DbgPrintf( (UTILS_BASE _T(T) _T("\n") UTILS_LOCINFO, __TFUNCTION__, A, B) )
#define FUNC_ENTER3(T, A, B, C)			DbgPrintf( (UTILS_BASE _T(T) _T("\n") UTILS_LOCINFO, __TFUNCTION__, A, B, C) )
#define FUNC_ENTER4(T, A, B, C, D)		DbgPrintf( (UTILS_BASE _T(T) _T("\n") UTILS_LOCINFO, __TFUNCTION__, A, B, C, D) )
#define FUNC_ENTER5(T, A, B, C, D, E)	DbgPrintf( (UTILS_BASE _T(T) _T("\n") UTILS_LOCINFO, __TFUNCTION__, A, B, C, D, E) )
#define FUNC_ENTER6(T, A, B, C, D, E, F)	DbgPrintf( (UTILS_BASE _T(T) _T("\n") UTILS_LOCINFO, __TFUNCTION__, A, B, C, D, E, F) )
#define FUNC_ENTER7(T, A, B, C, D, E, F, G)	DbgPrintf( (UTILS_BASE _T(T) _T("\n") UTILS_LOCINFO, __TFUNCTION__, A, B, C, D, E, F, G) )


// Could use #ifdef _NTDEF_ instead
#if !defined(NT_INST)
/// \Usage SetThreadName (-1, "MainThread");
/// \note (Not unicode aware)
/// \note Should not be called too often.
void __stdcall SetThreadName( DWORD dwThreadID, LPCSTR szThreadName);
/// \note This function ASSUMES static const buffers. So don't use auto variables. There's no problem with SetThreadName
void __stdcall SetThreadNameOneTime( DWORD dwThreadID, LPCSTR szThreadName);
#endif

/// \Usage STATIC_ASSERT( CompileTimeExpression );
#define STATIC_ASSERT( _X_ )  extern StaticAssertVariable[ (_X_)?1:0 ]


/// Only available in VC7 and later
#if _MSC_VER >= 1300
	// Definitions of intrinsic functions
	EXT_C void * _AddressOfReturnAddress();
	EXT_C void * _ReturnAddress();
	EXT_C LONG  __cdecl _InterlockedIncrement(LONG volatile *Addend);
	EXT_C LONG  __cdecl _InterlockedDecrement(LONG volatile *Addend);
	EXT_C LONG  __cdecl _InterlockedCompareExchange(volatile LONG * Dest, LONG Exchange, LONG Comp);
	EXT_C LONG  __cdecl _InterlockedExchange(volatile LONG * Target, LONG Value);
	EXT_C LONG  __cdecl _InterlockedExchangeAdd(volatile LONG * Addend, LONG Value);

	// Pragma'ed intrinsic functions
	#pragma intrinsic (_AddressOfReturnAddress, _ReturnAddress, _InterlockedCompareExchange, _InterlockedExchange, _InterlockedExchangeAdd, _InterlockedIncrement, _InterlockedDecrement)

	// Useful defines
	#define AddressOfReturnAddress		_AddressOfReturnAddress
	#define ReturnAddress				_ReturnAddress
	#define InterlockedCompareExchange	_InterlockedCompareExchange
	#define InterlockedExchange			_InterlockedExchange 
	#define InterlockedExchangeAdd		_InterlockedExchangeAdd
	#define InterlockedIncrement		_InterlockedIncrement
	#define InterlockedDecrement		_InterlockedDecrement

	/// \note super is really super.. humm I mean great!
	#define super __super
#endif
