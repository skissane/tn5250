//
//
//

// no choice :(
#include "stdafx.h"

#if defined(NT_INST)
// Kernel mode
extern "C"
{
#include <ntddk.h>
#include <tchar.h>
#include "Utils.h"

#if defined(_DEBUG)
    extern "C" int __stdcall MyAssert(const TCHAR * String, const TCHAR * Function, const TCHAR * File, const int Line)
    {
        static bool NeverBreak = false;

        DbgPrint(_T("ASSERT(%s) \n"), String);
        DbgPrint(_T("%s\n")
            _T("%s(%d)\n"), Function, File, Line);

        // If user don't want to be bored w/ it, just quit
        return !NeverBreak;
    }
#endif
}
#else

// User mode
#include <stdarg.h>
#include "Utils.h"

    #if defined(_DEBUG)
        extern "C" void OutputDebugOutputf(const TCHAR * fmt, ...)
        {
            //static std::ofstream Output("Log5250.txt");
            TCHAR Buffer[1024];
            va_list marker;
            va_start( marker, fmt );
            wvsprintf(Buffer, fmt, marker);

            OutputDebugString(Buffer);
            //Output << Buffer;

            va_end( marker );
        }

        extern "C" int __stdcall MyAssert(const TCHAR * String, const TCHAR * Function, const TCHAR * File, const int Line)
        {
            static bool NeverBreak = false;

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

            unsigned long RetVal = MessageBox(NULL, Buffer1, _T("ASSERTION!"), MB_YESNOCANCEL|MB_ICONSTOP|MB_TASKMODAL);
            if ( RetVal == IDYES )
                return true;
            if ( RetVal == IDCANCEL )
                NeverBreak = true;
            return false;
        }
    #endif  // #if defined(_DEBUG)


    //
    // Usage: SetThreadName (-1, "MainThread");
    //
    typedef struct tagTHREADNAME_INFO
    {
        DWORD dwType;       // must be 0x1000
        LPCSTR szName;      // pointer to name (in user addr space)
        DWORD dwThreadID;   // thread ID (-1=caller thread)
        DWORD dwFlags;      // reserved for future use, must be zero
    } THREADNAME_INFO;

    // Should not be called too often.
    extern "C" void __stdcall SetThreadName( unsigned long dwThreadID, const char * szThreadName)
    {
        //FUNC_ENTER2(" %d \"%s\"", dwThreadID, szThreadName);
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = szThreadName;
        info.dwThreadID = dwThreadID;
        info.dwFlags = 0;

        __try
        {
            RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
        }
        __except(EXCEPTION_CONTINUE_EXECUTION)
        {
        }
    }

    // This function ASSUMES static buffers.
    extern "C" void __stdcall SetThreadNameOneTime( unsigned long dwThreadID, const char * szThreadName)
    {
        // __declspec( thread ) is not recommendable in DLLs... Especially if
        // we are delay-loaded but there's no way to know.
        static DWORD Index = TLS_OUT_OF_INDEXES;
        if ( Index == TLS_OUT_OF_INDEXES)
        {
            DWORD newIndex = TlsAlloc();
            if ( InterlockedCompareExchange((LONG*)&Index, newIndex, TLS_OUT_OF_INDEXES) != TLS_OUT_OF_INDEXES )
                TlsFree(newIndex);
        }

        LPCSTR Shutup = (LPCSTR)TlsGetValue(Index);
        if ( Shutup != szThreadName)
        {
            SetThreadName(dwThreadID, szThreadName);
            TlsSetValue(Index, (LPVOID)szThreadName);
        }
        // Thread index is never freed... Shame on me
    }



    #if 0 
    // Untested and bugged

    // Check for Stack Overflows

    //	if ( GetExceptionCode() == EXCEPTION_STACK_OVERFLOW )
    //		CheckStackOverflow();

    // Kb Q315937
    //__declspec(naked) 
    void CheckForStackOverflow()
    {
        LPVOID lpPage;
        static DWORD dwOldProtect;
        static SYSTEM_INFO si;
        static MEMORY_BASIC_INFORMATION mi;

        // Get page size of system
        GetSystemInfo(&si);
        // Could assert that the pages are 4kb in size Anyway!

        // Find SP address
        //_asm mov lpPage, esp;	// Valid when inline
        lpPage = (LPVOID)*((DWORD*)AddressOfReturnAddress()+1);	// Should be the same!

        // Get allocation base of stack
        VirtualQuery(lpPage, &mi, sizeof(mi));

        // Go to page beyond current page
        lpPage = (LPBYTE)(mi.BaseAddress)-si.dwPageSize;

        // Free portion of stack just abandoned
        if (!VirtualFree(mi.AllocationBase, (LPBYTE)lpPage-(LPBYTE)mi.AllocationBase, MEM_DECOMMIT))
        {
            // If we get here, exit
            exit(1);
        }

        // Reintroduce the guard page
        if (!VirtualProtect(lpPage, si.dwPageSize, PAGE_GUARD|PAGE_READWRITE, &dwOldProtect))
        {
            exit(1);
        }
    }
    #endif

#endif	// #if defined(NT_INST)


/* For performance monitoring / memory verification / code analysis / code trace / etc

extern "C" void __declspec(naked) _cdecl _penter( void )
{
    _asm
    {
        push eax
        push ebx
        push ecx
        push edx
        push ebp
        push edi
        push esi
    }

printf("\nIn a function!");

    _asm
    {
        pop esi
        pop edi
        pop ebp
        pop edx
        pop ecx
        pop ebx
        pop eax
        ret
    }
}
extern "C" void __declspec(naked) _cdecl _pexit( void )
{
    _asm
    {
        push eax
        push ebx
        push ecx
        push edx
        push ebp
        push edi
        push esi
    }

printf("\nIn a function!");

    _asm
    {
        pop esi
        pop edi
        pop ebp
        pop edx
        pop ecx
        pop ebx
        pop eax
        ret
    }
}
*/

// TBA Add PDB loading + file name lookup + function name printout.
extern "C" void __declspec(naked) _cdecl _penter( void )
{
    __asm ret;
}

extern "C" void __declspec(naked) _cdecl _pexit( void )
{
    __asm ret;
}
