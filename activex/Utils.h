// Cardinal Health AIS Canada (c) 2003
// (c) 2003 Marc-Antoine Ruel
// Common utilities working in VC6, VC7 and VC7.1
// An assert much faster than the one of CRT and that breaks
// on the correct line.
// Also everything is designed to work in UNICODE and ANSI.
//
// What's inside:
// - intrinsics (if supported) for AddressOfReturnAddress, ReturnAddress and InterlockedXXX.
// - __Txxx__ for TCHAR support of __FILE__, __FUNCSIG__, __FUNCTION__ and __FUNCDNAME__.
// - __Wxxx__ for WCHAR support of (idem)....
// - dbgSetThreadName and SetThreadName to set the thread name while in VC6/7.
// - FUNC_ENTERx(x) for automatic context printout, useful in multiprocess development.
// - DbgPrintf(( x )) for debug-only printout.
// - PrintBuffer() for debug output of buffers
// - ASSERT for right on-place breakpoint.
// - STATIC_ASSERT for compile-time assert.
// - Break() for debug-only hard breakpoint.
// - super define for VC7 __super support.
// - C compilable.
// - lengthof() to know the length of arrays.
// - dbgAllocConsole/dbgFreeConsole to alloc and free a console in debug build.
// - no_unused_variable_warning removes a warning about an unused variable.
// - Use MAKE_SCOPE_GUARD for anonymous auto-destroying objects.
//   or ScopeGuard Var = MakeScopeGuard( bind(Fn, arg0) ); to be able to dismis them.
//

#pragma once

// C Support
#ifdef  __cplusplus
#define EXT_C extern "C"
#else
#define EXT_C
typedef enum
{
    false,
    true
} bool;
#endif

// Check if inside a DDK compilation
#if defined(_NTIFS_) || defined(_NTDDK_)
#define __UTIL_IN_DDK
#endif

// Asserts support
#undef assert
#undef ASSERT
#undef ATLASSERT
#define assert ASSERT
#define ATLASSERT ASSERT
/// \Usage STATIC_ASSERT( CompileTimeExpression );
#define STATIC_ASSERT( _X_ )  extern StaticAssertVariable[ (_X_)?1:0 ]

// Static string conversion. Ex: TOWSTRING( "allo" ) or TOSTRING( L"allo" )
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define TOSTRING( x ) #x
#define TOWSTRING( x ) WIDEN( #x )
// __Wxxx__ support
#define __WFILE__           WIDEN(__FILE__)
#define __WFUNCSIG__        WIDEN(__FUNCSIG__)
#define __WFUNCTION__       WIDEN(__FUNCTION__)
#define __WFUNCDNAME__      WIDEN(__FUNCDNAME__)
#define __WTIME__           WIDEN(__TIME__)
#define __WDATE__           WIDEN(__DATE__)

// __Txxx__ support in KERNEL
#if defined(__UTIL_IN_DDK)
    #define _T(_X_) _X_
    #define _TEXT(_X_) _X_
    #undef TCHAR
    #define TCHAR char
    #define _TCHAR char
#else
    // We need those headers.
    #include <tchar.h>
#endif
#if defined(_UNICODE) && !defined(__UTIL_IN_DDK)
    #define __TFILE__       __WFILE__
    #define __TFUNCSIG__    __WFUNCSIG__
    #define __TFUNCTION__   __WFUNCTION__
    #define __TFUNCDNAME__  __WFUNCDNAME__
    #define __TTIME__       __WTIME__
    #define __TDATE__       __WDATE__
#else
    #define __TFILE__       __FILE__
    #define __TFUNCSIG__    __FUNCSIG__
    #define __TFUNCTION__   __FUNCTION__
    #define __TFUNCDNAME__  __FUNCDNAME__
    #define __TTIME__       __TIME__
    #define __TDATE__       __DATE__
#endif

// Support for VC6
#if _MSC_VER < 1300
    /// Function Name
    #define __FUNCTION__    ""
    /// Function Signature (with parameters and return type)
    #define __FUNCSIG__     ""
    /// Function Decorated Name
    #define __FUNCDNAME__   ""
    #define __noop          ((void)0)
    #define __debugbreak()  __asm { __asm int 3 }	// TBM I don't know why I have a problem with this one
    //#define __debugbreak()  DebugBreak()
#endif

/// Only available in VC7 or later
#if _MSC_VER >= 1300
    // Definitions of intrinsic functions
    EXT_C void * _AddressOfReturnAddress(void);
    EXT_C void * _ReturnAddress(void);
    EXT_C long  __cdecl _InterlockedIncrement(long volatile *Addend);
    EXT_C long  __cdecl _InterlockedDecrement(long volatile *Addend);
    EXT_C long  __cdecl _InterlockedCompareExchange(volatile long * Dest, long Exchange, long Comp);
    EXT_C long  __cdecl _InterlockedExchange(volatile long * Target, long Value);
    EXT_C long  __cdecl _InterlockedExchangeAdd(volatile long * Addend, long Value);

    // Pragma'ed intrinsic functions
    #pragma intrinsic (_AddressOfReturnAddress, _ReturnAddress, _InterlockedCompareExchange, _InterlockedExchange, _InterlockedExchangeAdd, _InterlockedIncrement, _InterlockedDecrement)

    // Useful defines
    #define AddressOfReturnAddress      _AddressOfReturnAddress
    #define ReturnAddress               _ReturnAddress
    #define InterlockedCompareExchange  (LONG)_InterlockedCompareExchange
    #define InterlockedExchange         _InterlockedExchange 
    #define InterlockedExchangeAdd      _InterlockedExchangeAdd
    #define InterlockedIncrement        _InterlockedIncrement
    #define InterlockedDecrement        _InterlockedDecrement

    /// \note super is really super.. humm I mean great!
    #define super __super
#endif

// For every version
#if defined(_MSC_VER) && !defined(_M_IA64) && !defined(_M_AMD64)
    #pragma warning(push,1)
    // The first two DWORD or smaller arguments are passed in ECX and EDX registers; all other arguments are passed right to left.
    __declspec(naked) __inline __int64 __fastcall _InterlockedCompareExchange64(__int64 * Destination, const __int64 * Exchange, const __int64 * Comparand)
    {
        //arg_0 = dword ptr  0Ch
        __asm
        {
            // The compiler generates prolog and epilog code to save and restore the ESI, EDI, EBX, and EBP registers, if they are used in the function.
            // C'est pour ça qu'on est baisé un peu et qu'on est obligé de pusher EBP...
            push    ebx
                push    ebp
                mov     ebp, ecx            // ECX = Destination
                mov     ebx, [edx]          // EDX = Exchange
                mov     ecx, [edx+4]
                mov     edx, [esp+0xC]      // arg_0 = Comparand
                mov     eax, [edx]
                mov     edx, [edx+4]

                // Maintenant,
                //   EBX = Exchange0
                //   ECX = Exchange1
                //   EAX = Comparand0
                //   EDX = Comparand1
                cmpxchg8b qword ptr [ebp+0] // Use lock prefix?
                pop     ebp
                    pop     ebx
                    retn    4
        }
    }
    #pragma warning(pop)
#endif

// Only available in VC7 or later and in C++
#if _MSC_VER >= 1300 && defined(__cplusplus)
    // Should be size_t but I never saw +4gig static arrays and it sucks when compiling 32 bit code.
//    template<typename T, unsigned int N>
//    unsigned int lengthof(T (&)[N])
//    {
//        return N;
//    }
    // Not a function which is much better. Not from me but from Zhanyong Wan who didn't invent it neither but got it from someone else :)
    template <typename T, size_t N>
    char ( &_ArraySizeHelper( T (&array)[N] ))[N];
    
    #define lengthof( _X_ ) (sizeof( _ArraySizeHelper(_X_) ))

#else
    // That sucks much more..
    #define lengthof( _X_ )  (sizeof(_X_) / sizeof( (_X_)[0] ))
#endif


// Kernel mode support. (It doesn't define _DEBUG)
#if defined(DBG) && DBG
#define _DEBUG 1
#endif


#if defined(_DEBUG)
    // The macros break when returns true
    EXT_C int __stdcall MyAssert(const TCHAR * String, const TCHAR * Function, const TCHAR * File, const int Line);

    #if defined(__UTIL_IN_DDK)
        // Kernel-mode
        #define _INTERNAL_DEBUG_PRINT	DbgPrint
        #ifndef _UTILS_BASE
        #define _UTILS_BASE              __TFUNCTION__
        #endif
        // ProcessId/ThreadId is usually a non-sense in kernel mode
        #ifndef _UTILS_LOCINFO
        #define _UTILS_LOCINFO
        #endif

        __inline int __stdcall MyAssert(const char * String, const char * Function, const char * File, const int Line)
        {
            static bool DoBreak = true;

            DbgPrint("ASSERT(%s) \n", String);
            DbgPrint("%s\n"
                "%s(%d)\n", Function, File, Line);

            // If user don't want to be bored w/ it, just quit
            return DoBreak;
        }

    #else
        // User-mode
        #define _INTERNAL_DEBUG_PRINT	OutputDebugOutputf
        #ifndef _UTILS_BASE
        #define _UTILS_BASE             _T("%4d:%4d ") __TFUNCTION__
        #endif
        #ifndef _UTILS_LOCINFO
        #define _UTILS_LOCINFO          , GetCurrentProcessId(), GetCurrentThreadId()
        #endif

        #if defined(_WINBASE_) || defined(_INC_STDIO)
            // We need it.
            #include <stdarg.h>
            EXT_C __inline void OutputDebugOutputf(const TCHAR * fmt, ...)
            {
                //static std::ofstream Output("Log5250.txt");
                TCHAR Buffer[1024];
                va_list marker;
                va_start( marker, fmt );
                #ifndef _INC_STDIO
                wvsprintf(Buffer, fmt, marker);
                #else
                // Permet les floating points!
			    _vstprintf(Buffer, fmt, marker);
                #endif
                OutputDebugString(Buffer);
                //Output << Buffer;
                va_end( marker );
            }
        #endif

        #define dbgSetThreadNameOneTime(_X_)    SetThreadNameOneTime((DWORD)-1, _X_)
        #define dbgSetThreadName(_X_)           SetThreadName((DWORD)-1, _X_)
        
        #ifdef _INC_STDIO
        EXT_C __inline void dbgAllocConsole()
        {
            AllocConsole();
            freopen("CON", "w", stdout);
            freopen("CON", "w", stderr);
            freopen("CON", "r", stdin);
            #ifdef _IOS_
            std::ios::sync_with_stdio();
            #endif
        }
        #endif
        #define dbgFreeConsole()                FreeConsole()

        #if defined(_WINBASE_)
            EXT_C __inline int __stdcall MyAssert(const TCHAR * String, const TCHAR * Function, const TCHAR * File, const int Line)
            {
                static bool NeverBreak = false;
                unsigned long RetVal;
                TCHAR Buffer1[1028] = { 0 };
                wsprintf(Buffer1,	_T("ASSERT(%s) Pid:%d Tid:%d\n")
                    _T("%s\n")
                    _T("%s(%d)\n")
                    _T("Break? (Cancel to never break again)"), 
                    String, GetCurrentProcessId(), GetCurrentThreadId(), Function, File, Line);

                // Still, do an OutputDebugString...
                OutputDebugString(Buffer1);

                // If user don't want to be bored w/ it, just quit
                if ( NeverBreak )
                    return false;

                // If a debugger is present, just break.
                if (IsDebuggerPresent())
                    return true;

                RetVal = MessageBox(NULL, Buffer1, _T("ASSERTION!"), MB_YESNOCANCEL|MB_ICONSTOP|MB_TASKMODAL);
                if ( RetVal == IDYES )
                    return true;
                if ( RetVal == IDCANCEL )
                    NeverBreak = true;
                return false;
            }
        #endif
    #endif

    #define ASSERT( _X_ )                   (void) ((_X_) || !MyAssert( _T(#_X_), __TFUNCSIG__, __TFILE__, __LINE__) || (__debugbreak(), 0))
    #define HARD_ASSERT( _X_ )              (void) ((_X_) || (__debugbreak(), 0))
    #define Break()                         __debugbreak()
    #define TryBreak                        __try { Break(); } __except(EXCEPTION_EXECUTE_HANDLER) { }
    #define TryBreakCPP                     try { Break(); } catch(...) { }
    #define DbgPrintf( _X_ )                _INTERNAL_DEBUG_PRINT _X_
    #define FUNCTION_LOG()                  DbgPrintf( (_T("%s\n"), __TFUNCTION__) )
    #ifndef VERIFY
    #define VERIFY( _X_ )                   ASSERT( _X_ )
    #endif

    #if defined(_WINBASE_) || defined(_INC_STDIO) || defined(__UTIL_IN_DDK)
    // Print a buffer 16 bytes per line
    EXT_C __inline void PrintHexDump(size_t length, const void * Buffer)
    {
        static const _TCHAR s_rgbDigits[] = _TEXT("0123456789abcdef");
        const unsigned char * buffer = (const unsigned char *)Buffer;
        TCHAR rgbLine[100];
        while ( length )
        {
            size_t i;
            const size_t count = (length > 16) ? 16:length;
            for (i = 0; i < count; ++i) 
            {
                rgbLine[3*i  ] = s_rgbDigits[buffer[i] >> 4 ];
                rgbLine[3*i+1] = s_rgbDigits[buffer[i] & 0xf];
                rgbLine[3*i+2] = (i == 7) ? _TEXT(':') : _TEXT(' ');
            }
            for (; i < 16; ++i) 
            {
                rgbLine[3*i  ] = _TEXT(' ');
                rgbLine[3*i+1] = _TEXT(' ');
                rgbLine[3*i+2] = _TEXT(' ');
            }
            rgbLine[3*16] = _TEXT(' ');

            for (i = 0; i < count; i++) 
            {
                rgbLine[i+3*16+1] = (buffer[i]<32 || buffer[i]>126) ? _TEXT('.') : buffer[i];
            }
            rgbLine[i+3*16+1] = _TEXT('\n');
            rgbLine[i+3*16+2] = 0;
            DbgPrintf((rgbLine));
            length -= count;
            buffer += count;
        }
    }
    #endif

#else

    #if !defined(__UTIL_IN_DDK)
        #define dbgFreeConsole()                __noop
        #define dbgAllocConsole()               __noop
        #define dbgSetThreadNameOneTime(_X_)    __noop
        #define dbgSetThreadName(_X_)           __noop
    #endif
    #define ASSERT( _X_ )                   __noop
    #define HARD_ASSERT( _X_ )              __noop
    #define Break()                         __noop
    #define TryBreak                        __noop();
    #define TryBreakCPP                     __noop();
    #define DbgPrintf( _X_ )                __noop
    #define PrintHexDump(_X_,_Y_)           __noop
    #define FUNCTION_LOG()                  __noop
    #ifndef VERIFY
    #define VERIFY( _X_ )                   __noop
    #endif

    #pragma warning(push)
    #pragma warning(disable:4164) // '' : intrinsic function not declared
    #pragma warning(disable:4995) // '': name was marked as #pragma deprecated
    // _disable _enable _outp _outpw _inp _inpw
    // Instrisics
    #pragma push_macro("strcpy")
    #pragma push_macro("strcat")
    #undef strcpy
    #undef strcat
    #pragma intrinsic( fabs, strcmp, labs, strcpy, _rotl, memcmp, strlen, _rotr, memcpy, _lrotl, _strset, memset, _lrotr, abs, strcat )
    #pragma pop_macro("strcpy")
    #pragma pop_macro("strcat")
    // The floating-point functions listed below have true intrinsic forms when you specify both the /Oi and /Og compiler options (or any option that includes /Og: /Ox, /O1, and /O2):
    #pragma intrinsic( atan, exp, log10, sqrt, atan2, log, sin, tan, cos )
    // Fastcall types
    #pragma intrinsic( acos, cosh, pow, tanh, asin, fmod, sinh )
    #pragma warning(pop)
#endif


#define FUNC_ENTER() DbgPrintf( (_UTILS_BASE       _T("\n") _UTILS_LOCINFO) )
#define FUNC_ENTER0( Z) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO) )
#define FUNC_ENTER1( Z, A) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A) )
#define FUNC_ENTER2( Z, A, B) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B) )
#define FUNC_ENTER3( Z, A, B, C) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C) )
#define FUNC_ENTER4( Z, A, B, C, D)  DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D) )
#define FUNC_ENTER5( Z, A, B, C, D, E) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E) )
#define FUNC_ENTER6( Z, A, B, C, D, E, F) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F) )
#define FUNC_ENTER7( Z, A, B, C, D, E, F, G) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F, G) )
#define FUNC_ENTER8( Z, A, B, C, D, E, F, G, H) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F, G, H) )
#define FUNC_ENTER9( Z, A, B, C, D, E, F, G, H, I) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F, G, H, I) )
#define FUNC_ENTER10(Z, A, B, C, D, E, F, G, H, I, J) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F, G, H, I, J) )
#define FUNC_ENTER11(Z, A, B, C, D, E, F, G, H, I, J, K) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F, G, H, I, J, K) )
#define FUNC_ENTER12(Z, A, B, C, D, E, F, G, H, I, J, K, L) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F, G, H, I, J, K, L) )
#define FUNC_ENTER13(Z, A, B, C, D, E, F, G, H, I, J, K, L, M) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F, G, H, I, J, K, L, M) )
#define FUNC_ENTER14(Z, A, B, C, D, E, F, G, H, I, J, K, L, M, N) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F, G, H, I, J, K, L, M, N) )
#define FUNC_ENTER15(Z, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O) )
#define FUNC_ENTER16(Z, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P) )
#define FUNC_ENTER17(Z, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q) )
#define FUNC_ENTER18(Z, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R) )
#define FUNC_ENTER19(Z, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S) )
#define FUNC_ENTER20(Z, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T) DbgPrintf( (_UTILS_BASE _T(Z) _T("\n") _UTILS_LOCINFO, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T) )


#if !defined(__UTIL_IN_DDK)
    #ifdef _WINBASE_
    // Usage: SetThreadName (-1, "MainThread"); Should not be called too often.
    EXT_C __inline void __stdcall SetThreadName( unsigned long dwThreadID, const char * szThreadName)
    {
        typedef struct tagTHREADNAME_INFO
        {
            unsigned long dwType;       // must be 0x1000
            const char *  szName;      // pointer to name (in user addr space)
            unsigned long dwThreadID;   // thread ID (-1=caller thread)
            unsigned long dwFlags;      // reserved for future use, must be zero
        } THREADNAME_INFO;

        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = szThreadName;
        info.dwThreadID = dwThreadID;
        info.dwFlags = 0;

        __try
        {
            RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(unsigned long), (unsigned long*)&info );
        }
        __except(EXCEPTION_CONTINUE_EXECUTION)
        {
        }
    }

    /// \note This function ASSUMES static const buffers. So don't use auto variables. There's no problem with SetThreadName
    EXT_C __inline void __stdcall SetThreadNameOneTime( unsigned long dwThreadID, const char * szThreadName)
    {
        // __declspec( thread ) is not recommendable in DLLs... Especially if
        // we are delay-loaded but there's no way to know.
        const char * l_Shutup;
        static unsigned long s_Index = TLS_OUT_OF_INDEXES;
        if ( s_Index == TLS_OUT_OF_INDEXES)
        {
            unsigned long l_newIndex = TlsAlloc();
            if ( InterlockedCompareExchange((long*)&s_Index, l_newIndex, TLS_OUT_OF_INDEXES) != TLS_OUT_OF_INDEXES )
                TlsFree(l_newIndex);
        }

        l_Shutup = (const char *)TlsGetValue(s_Index);
        if ( l_Shutup != szThreadName)
        {
            SetThreadName(dwThreadID, szThreadName);
            TlsSetValue(s_Index, (void *)szThreadName);
        }
        // Thread index is never freed... Shame on me
    }
    #endif

#else
    #undef PAGED_CODE
    #if DBG
        // Almost the same as ntddk.h except that it uses Break() instead of ASSERT(FALSE)
        #define PAGED_CODE() \
            if (KeGetCurrentIrql() > APC_LEVEL) { \
            KdPrint(( "EX: Pageable code called at IRQL %d\n", KeGetCurrentIrql() )); \
                Break(); \
                }
    #else
        #define PAGED_CODE()
    #endif

    // Only in XP
    #ifndef ExFreePoolWithTag
    #define ExFreePoolWithTag(_X_, _Y_) ExFreePool(_X_)
    #endif

    #ifndef DEFAULT_TAG
    #define DEFAULT_TAG 'HC'
    #endif

    #undef TCHAR
    #ifdef  __cplusplus
        // Placement new/delete
        #ifndef __PLACEMENT_NEW_INLINE
        #define __PLACEMENT_NEW_INLINE
        __inline void *__cdecl operator new(size_t, void *_P) {return (_P); }
        #if _MSC_VER >= 1200
        __inline void __cdecl operator delete(void *, void *) {return; }
        #endif
        #endif

        #undef new
        #undef delete

        #if !defined(_DEBUG)
            // Use the operator new in kernel mode
            __inline void * __cdecl operator new(size_t nSize, POOL_TYPE pt, ULONG Tag = DEFAULT_TAG)
            {
                void * ptr = ExAllocatePoolWithTag(pt, nSize, Tag);
                if (!ptr)
                {
                    // Should be trapped.
                    ExRaiseStatus(STATUS_INSUFFICIENT_RESOURCES);
                    KeBugCheckEx((ULONG)STATUS_INSUFFICIENT_RESOURCES, 0, nSize, pt, Tag);
                }
                return ptr;
            }
            __declspec(deprecated) __inline void * __cdecl operator new(size_t nSize)
            {
                return ::operator new(nSize, NonPagedPool);
            }
            __inline void * __cdecl operator new[](size_t nSize, POOL_TYPE pt, ULONG Tag = DEFAULT_TAG)
            {
                return ::operator new(nSize, pt, Tag);
            }
            __declspec(deprecated) __inline void * __cdecl operator new[](size_t nSize)
            {
                return ::operator new(nSize, NonPagedPool);
            }
            __inline void __cdecl operator delete(void * p)
            {
                if ( !p )
                    return;
                ExFreePoolWithTag(p, DEFAULT_TAG);
            }
            __inline void __cdecl operator delete[](void * p)
            {
                return ::operator delete(p);
            }
        #else
            // Set this to the number of DWORD at each side of each memory allocation.
            extern const int s_NbULONGBuffer;
            extern const ULONG s_Guard;
            __declspec(selectany) const int s_NbULONGBuffer = 2;
            __declspec(selectany) const ULONG s_Guard = 0xFDFDFDFD;

            __inline void * __cdecl operator new(size_t nSize, POOL_TYPE pt, ULONG Tag, void * Caller)
            {
                // Dumb head?
                ASSERT(nSize<=10*1024*1024);
                ASSERT((KeGetCurrentIrql()< DISPATCH_LEVEL && (pt==PagedPool||pt==PagedPoolCacheAligned))
                    || (KeGetCurrentIrql()<=DISPATCH_LEVEL && (pt==NonPagedPool||pt==NonPagedPoolCacheAligned)));

                // Fix nSize to mod 4.
                nSize = (nSize+3)&(~3);
                size_t NewSize = nSize + sizeof(ULONG)*(2+2*s_NbULONGBuffer);
                ULONG * Ptr = (ULONG *)ExAllocatePoolWithTag(pt, NewSize, Tag);
                if ( !Ptr )
                {
                    // Should be trapped.
                    ExRaiseStatus(STATUS_INSUFFICIENT_RESOURCES);
                    KeBugCheckEx((ULONG)STATUS_INSUFFICIENT_RESOURCES, 0, nSize, pt, Tag);
                }
                *Ptr++ = (ULONG)Caller;
                *Ptr++ = (ULONG)nSize;
                const ULONG Tmp = nSize/sizeof(ULONG) + s_NbULONGBuffer;
                for ( int i = 0; i < s_NbULONGBuffer; ++i )
                {
                    Ptr[i] = s_Guard;
                    Ptr[i+Tmp] = s_Guard;
                }
                memset(Ptr+s_NbULONGBuffer, 0xCD, nSize);
                //KdPrint(("[%4d]%*c" __FUNCTION__ " 0x%08X for %d bytes\n", PsGetCurrentProcessId(), g_nbspace, ' ', Ptr+s_NbULONGBuffer, nSize));
                return Ptr+s_NbULONGBuffer;
            }
            __inline void * __cdecl operator new(size_t nSize, POOL_TYPE pt, ULONG Tag = DEFAULT_TAG)
            {
                return operator new(nSize, pt, Tag, ReturnAddress());
            }
            __inline __declspec(deprecated) void * __cdecl operator new(size_t nSize)
            {
                return operator new(nSize, NonPagedPool, DEFAULT_TAG, ReturnAddress());
            }
            __inline void * __cdecl operator new[](size_t nSize, POOL_TYPE pt, ULONG Tag = DEFAULT_TAG)
            {
                return operator new(nSize, pt, Tag, ReturnAddress());
            }
            __inline __declspec(deprecated) void * __cdecl operator new[](size_t nSize)
            {
                return operator new(nSize, NonPagedPool, DEFAULT_TAG, ReturnAddress());
            }
            __inline int _CrtIsValidHeapPointer(const void * p)
            {
                __try
                {
                    ULONG * pBase = (ULONG*)p - (2+s_NbULONGBuffer);
                    // Would fail sometime without reason
                    // if (!MmIsAddressValid((PVOID)p))
                    //    return 0;

                    ULONG nSize = pBase[1];
                    // Dumb head?
                    if (nSize > 10*1024*1024)
                        return 0;
                    ULONG Tmp = nSize/sizeof(ULONG) + s_NbULONGBuffer;
                    ULONG * Ptr = pBase + 2;
                    for ( int i = 0; i < s_NbULONGBuffer; ++i )
                    {
                        if ( Ptr[i] != s_Guard || Ptr[i+Tmp] != s_Guard )
                        {
                            ULONG OldCaller = pBase[0];
                            // ULONG ImageBase = (ULONG)__ImageBase;
                            // TBA ULONG OldCallerBase = OldCaller - ImageBase;
                            KdPrint(("Buffer overflow at 0x%p Allocated from 0x%p", p, OldCaller));
                            return 0;
                        }
                    }
                    // TBA Check for 0xDDDDDD at least for 1st ulong ??
                }
                __except(EXCEPTION_EXECUTE_HANDLER)
                {
                    KdPrint(("Error verifying 0x%08X\n", p));
                    return 0;
                }
                return 1;
            }
            __inline void __cdecl operator delete(void * p, ULONG Caller)
            {
                if ( !p )
                    return;
                ASSERT( _CrtIsValidHeapPointer(p) );
                //KdPrint(("[%4d]%*c" __FUNCTION__ " 0x%08X for %d bytes\n", PsGetCurrentProcessId(), g_nbspace, ' ', p, *((ULONG*)p-(1+s_NbULONGBuffer)) ));
                memset(p, 0xDD, *((ULONG*)p - (1+s_NbULONGBuffer)));
                ExFreePoolWithTag((ULONG*)p - (2+s_NbULONGBuffer), DEFAULT_TAG);
            }
            __inline void __cdecl operator delete(void * p)
            {
                return operator delete(p, (ULONG)ReturnAddress());
            }
            __inline void __cdecl operator delete[](void * p)
            {
                return operator delete(p, (ULONG)ReturnAddress());
            }
        #endif
    #endif
#endif


#ifdef __cplusplus
    /// ATL::CString support
    #if defined(__ATLSTR_H__) && !defined(__AFX_H__)
    using ATL::CStringT;
    using ATL::CStringA;
    using ATL::CStringW;
    using ATL::CString;
    #endif


    // Ajout de TCHAR style pour cin/cout/cerr/clog/string/stringstream
    namespace std
    {
        #if defined(UNICODE)
            #ifdef _STRING_
            typedef wstring tstring;
            typedef wstringstream tstringstream;
            #endif
            #ifdef _IOSTREAM_
            typedef wistream tistream;
            typedef wostream tostream;
            typedef wiostream tiostream;
            wistream & tcin  = wcin;
            wostream & tcout = wcout;
            wostream & tcerr = wcerr;
            wostream & tclog = wclog;
            #endif
        #else
            #ifdef _STRING_
            typedef string tstring;
            typedef stringstream tstringstream;
            #endif
            #ifdef _IOSTREAM_
            typedef istream tistream;
            typedef ostream tostream;
            typedef iostream tiostream;
            istream & tcin  = cin;
            ostream & tcout = cout;
            ostream & tcerr = cerr;
            ostream & tclog = clog;
            #endif
        #endif
    }

    // Fonction pratique
    template <class T> __inline void no_unused_variable_warning(const T&) {}

    // Only usable with boost and C++.
    // Assumes :
    // #include <boost/lambda/lambda.hpp>
    // #include <boost/lambda/bind.hpp>
    // or
    // #include <boost/bind.hpp>
    #ifdef BOOST_CONFIG_SUFFIX_HPP

    // Credits to Daniel Wallin, Andrei Alexandrescu and Petru Marginean. But I had the basic idea by myself! ;-P
    // Thanks to Paul A. Bristow
    namespace DetailsScopeGuard
    {
        struct ScopeGuardBase
        {
            ScopeGuardBase() : m_dismissed(false)   {}
            ScopeGuardBase(const ScopeGuardBase& x) : m_dismissed(x.m_dismissed) { x.Dismiss(); }
            void Dismiss() const throw() { m_dismissed = true; }
        protected:
            mutable bool m_dismissed;
        };

        template<class T>
        struct ScopeGuardImpl : public ScopeGuardBase
        {
            explicit ScopeGuardImpl(const T& x) : m_action(x) {}
            ~ScopeGuardImpl() { if (!m_dismissed) m_action(); }
        private:
            T m_action;
        };

        struct ScopeGuardAnonBase
        {
        };

        template<class T>
        struct ScopeGuardAnonImpl : public ScopeGuardAnonBase
        {
            explicit ScopeGuardAnonImpl(const T& x) : m_action(x) {}
            ~ScopeGuardAnonImpl() { m_action(); }
        private:
            T m_action;
        };
    }

    template<class T>
    DetailsScopeGuard::ScopeGuardAnonImpl<T> MakeScopeGuardAnon(T x)
    {
        return DetailsScopeGuard::ScopeGuardAnonImpl<T>(x);
    }

    template<class T>
    DetailsScopeGuard::ScopeGuardImpl<T> MakeScopeGuard(T x)
    {
        return DetailsScopeGuard::ScopeGuardImpl<T>(x);
    }

    typedef const DetailsScopeGuard::ScopeGuardAnonBase & ScopeGuardAnon;
    typedef const DetailsScopeGuard::ScopeGuardBase & ScopeGuard;

    #define ANONYMOUS_VARIABLE(str) BOOST_JOIN(str, __LINE__)

    // Make a ScopeGuardAnon reference object which points to a DetailsScopeGuard::ScopeGuardImpl<T> object.
    #define MAKE_SCOPE_GUARD( _X_ ) ScopeGuardAnon ANONYMOUS_VARIABLE(ScopeGuard) = MakeScopeGuardAnon( _X_ ); no_unused_variable_warning(ANONYMOUS_VARIABLE(ScopeGuard))
    #endif

#endif  // __cplusplus
