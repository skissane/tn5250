#pragma once

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#define CASE_STRING(_X_) case _X_: return _T(#_X_)
inline LPCTSTR ToString(TYPEKIND a) { switch (a) {
    case TKIND_ENUM: return _T("Enumeration");
    case TKIND_RECORD: return _T("Record");
    case TKIND_MODULE: return _T("Module");
    case TKIND_INTERFACE: return _T("Interface");
    case TKIND_DISPATCH: return _T("Dispatch");
    case TKIND_COCLASS: return _T("CoClass");
    case TKIND_ALIAS: return _T("Alias");
    case TKIND_UNION: return _T("Union");
    case TKIND_MAX: default: return _T("<unknown>");
    }
}
inline LPCTSTR ToString(INVOKEKIND a) { switch (a) {
    case INVOKE_FUNC: return _T("function");
    case INVOKE_PROPERTYGET: return _T("propget");
    case INVOKE_PROPERTYPUT: return _T("propput");
    case INVOKE_PROPERTYPUTREF: return _T("propputref");
    default: return _T("<unknown>");
    }
}
inline LPCTSTR ToString2(INVOKEKIND a) { switch (a) {
    case INVOKE_FUNC: return _T("");
    case INVOKE_PROPERTYGET: return _T("get_");
    case INVOKE_PROPERTYPUT: return _T("put_");
    case INVOKE_PROPERTYPUTREF: return _T("putref_");
    default: return _T("???_");
    }
}inline LPCTSTR ToString(FUNCKIND a) { switch (a) {
    case FUNC_VIRTUAL: return _T("virtual");
    case FUNC_PUREVIRTUAL: return _T("pure virtual");
    case FUNC_NONVIRTUAL: return _T("non virtual");
    case FUNC_STATIC: return _T("static");
    case FUNC_DISPATCH: return _T("dispatch");
    default: return _T("<unknown>");
    }
}
inline LPCTSTR ToString(VARKIND a) { switch (a) {
    CASE_STRING(VAR_PERINSTANCE);
	CASE_STRING(VAR_STATIC);
	CASE_STRING(VAR_CONST);
	CASE_STRING(VAR_DISPATCH);
    default: return _T("<unknown>");
    }
}
inline LPCTSTR ToString(CALLCONV a) { switch (a) {
    case CC_FASTCALL: return _T("fastcall");
	case CC_CDECL: return _T("cdecl");
	case CC_PASCAL: return _T("pascal");
	CASE_STRING(CC_MACPASCAL);
	case CC_STDCALL: return _T("stdcall");
	CASE_STRING(CC_FPFASTCALL);
	case CC_SYSCALL: return _T("syscall");
	CASE_STRING(CC_MPWCDECL);
	CASE_STRING(CC_MPWPASCAL);
    case CC_MAX: default: return _T("<unknown>");
    }
}
inline LPCTSTR ToString(DESCKIND a) { switch (a) {
    CASE_STRING(DESCKIND_NONE);
	CASE_STRING(DESCKIND_FUNCDESC);
	CASE_STRING(DESCKIND_VARDESC);
	CASE_STRING(DESCKIND_TYPECOMP);
	CASE_STRING(DESCKIND_IMPLICITAPPOBJ);
    case DESCKIND_MAX: default: return _T("<unknown>");
    }
}
inline LPCTSTR ToString(SYSKIND a) { switch (a) {
    CASE_STRING(SYS_WIN16);
	CASE_STRING(SYS_WIN32);
	CASE_STRING(SYS_MAC);
	CASE_STRING(SYS_WIN64);
    default: return _T("<unknown>");
    }
}
inline LPCTSTR ToString(CHANGEKIND a) { switch (a) {
    CASE_STRING(CHANGEKIND_ADDMEMBER);
	CASE_STRING(CHANGEKIND_DELETEMEMBER);
	CASE_STRING(CHANGEKIND_SETNAMES);
	CASE_STRING(CHANGEKIND_SETDOCUMENTATION);
	CASE_STRING(CHANGEKIND_GENERAL);
	CASE_STRING(CHANGEKIND_INVALIDATE);
	CASE_STRING(CHANGEKIND_CHANGEFAILED);
    case CHANGEKIND_MAX: default: return _T("<unknown>");
    }
}
#undef CASE_STRING
#define CASE_STRING(_X_) if (a&_X_) s += _T(#_X_) _T(" ")
inline ATL::CString TYPEFLAGSToString(DWORD a) {
    ATL::CString s;
    CASE_STRING(TYPEFLAG_FAPPOBJECT);
    CASE_STRING(TYPEFLAG_FCANCREATE);
	CASE_STRING(TYPEFLAG_FLICENSED);
	CASE_STRING(TYPEFLAG_FPREDECLID);
	CASE_STRING(TYPEFLAG_FHIDDEN);
	CASE_STRING(TYPEFLAG_FCONTROL);
	CASE_STRING(TYPEFLAG_FDUAL);
	CASE_STRING(TYPEFLAG_FNONEXTENSIBLE);
	CASE_STRING(TYPEFLAG_FOLEAUTOMATION);
	CASE_STRING(TYPEFLAG_FRESTRICTED);
	CASE_STRING(TYPEFLAG_FAGGREGATABLE);
	CASE_STRING(TYPEFLAG_FREPLACEABLE);
	CASE_STRING(TYPEFLAG_FDISPATCHABLE);
	CASE_STRING(TYPEFLAG_FREVERSEBIND);
	CASE_STRING(TYPEFLAG_FPROXY);
    return s;
}
inline ATL::CString FUNCFLAGSToString(DWORD a) {
    ATL::CString s;
    CASE_STRING(FUNCFLAG_FRESTRICTED);
	CASE_STRING(FUNCFLAG_FSOURCE);
	CASE_STRING(FUNCFLAG_FBINDABLE);
	CASE_STRING(FUNCFLAG_FREQUESTEDIT);
	CASE_STRING(FUNCFLAG_FDISPLAYBIND);
	CASE_STRING(FUNCFLAG_FDEFAULTBIND);
	CASE_STRING(FUNCFLAG_FHIDDEN);
	CASE_STRING(FUNCFLAG_FUSESGETLASTERROR);
	CASE_STRING(FUNCFLAG_FDEFAULTCOLLELEM);
	CASE_STRING(FUNCFLAG_FUIDEFAULT);
	CASE_STRING(FUNCFLAG_FNONBROWSABLE);
	CASE_STRING(FUNCFLAG_FREPLACEABLE);
	CASE_STRING(FUNCFLAG_FIMMEDIATEBIND);
    return s;
}
inline ATL::CString VARFLAGSToString(DWORD a) {
    ATL::CString s;
    CASE_STRING(VARFLAG_FREADONLY);
	CASE_STRING(VARFLAG_FSOURCE);
	CASE_STRING(VARFLAG_FBINDABLE);
	CASE_STRING(VARFLAG_FREQUESTEDIT);
	CASE_STRING(VARFLAG_FDISPLAYBIND);
	CASE_STRING(VARFLAG_FDEFAULTBIND);
	CASE_STRING(VARFLAG_FHIDDEN);
	CASE_STRING(VARFLAG_FRESTRICTED);
	CASE_STRING(VARFLAG_FDEFAULTCOLLELEM);
	CASE_STRING(VARFLAG_FUIDEFAULT);
	CASE_STRING(VARFLAG_FNONBROWSABLE);
	CASE_STRING(VARFLAG_FREPLACEABLE);
	CASE_STRING(VARFLAG_FIMMEDIATEBIND);
    return s;
}
inline ATL::CString LIBFLAGSToString(DWORD a) {
    ATL::CString s;
    CASE_STRING(LIBFLAG_FRESTRICTED);
	CASE_STRING(LIBFLAG_FCONTROL);
	CASE_STRING(LIBFLAG_FHIDDEN);
	CASE_STRING(LIBFLAG_FHASDISKIMAGE);
    return s;
}
#undef CASE_STRING

struct TypeInfo : public ATL::CComPtr<ITypeInfo>
{
    HRESULT DisplayTypeInfo()
    {
        ATL::CComBSTR pszTypeInfoName;
        HRESULT hr = p->GetDocumentation(MEMBERID_NIL, &pszTypeInfoName, 0, 0, 0);
        ASSERT(SUCCEEDED(hr));
        if (FAILED(hr))
            return hr;
        TYPEATTR * pTypeAttr;
        hr = p->GetTypeAttr(&pTypeAttr);
        ASSERT(SUCCEEDED(hr));
        if (FAILED(hr))
            return hr;

        _tprintf(_T("%ls - %s\n"), pszTypeInfoName, ToString(pTypeAttr->typekind));
        hr = EnumTypeInfoMembers(pTypeAttr);
        _tprintf(_T("\n"));
        p->ReleaseTypeAttr(pTypeAttr);
        return hr;
    }

    HRESULT EnumTypeInfoMembers(TYPEATTR * pTypeAttr)
    {
        HRESULT hr = S_FALSE;
        if (pTypeAttr->cFuncs)
        {
            _tprintf( _T("  Functions:\n") );
            for (WORD i = 0; i < pTypeAttr->cFuncs; ++i)
            {
                FUNCDESC * pDesc;
                hr = p->GetFuncDesc(i, &pDesc);
                ASSERT(SUCCEEDED(hr));
                if (FAILED(hr))
                    return hr;
                ATL::CComBSTR pszFuncName;
                hr = p->GetDocumentation(pDesc->memid, &pszFuncName, 0, 0, 0);
                if (pDesc->memid & 0x40000000)
                    _tprintf(_T("    N/A  %s %s%ls(...)\n"), ToString(pDesc->callconv), ToString2(pDesc->invkind), pszFuncName);
                else
                    _tprintf(_T("    %-4d %s %s%ls(...)\n"), pDesc->memid, ToString(pDesc->callconv), ToString2(pDesc->invkind), pszFuncName);
                p->ReleaseFuncDesc(pDesc);
                ASSERT(SUCCEEDED(hr));
                if (FAILED(hr))
                    return hr;
            }
        }
        if (pTypeAttr->cVars)
        {
            _tprintf(_T("  Variables:\n"));
            for (WORD i = 0; i < pTypeAttr->cVars; ++i)
            {
                VARDESC * pDesc;
                hr = p->GetVarDesc(i, &pDesc);
                ASSERT(SUCCEEDED(hr));
                if (FAILED(hr))
                    return hr;
                ATL::CComBSTR pszVarName;
                
                hr = p->GetDocumentation(pDesc->memid, &pszVarName,0,0,0);
                if (pDesc->memid & 0x40000000)
                    _tprintf(_T("    N/A  %-32ls"), pszVarName);
                else
                    _tprintf(_T("    %-4d %-32ls"), pDesc->memid, pszVarName);
                ATL::CComVariant V;
                if (SUCCEEDED(V.ChangeType(VT_BSTR, pDesc->lpvarValue)))
                    _tprintf(_T(" %ls\n"), V.bstrVal);
                else
                    _tprintf(_T("\n"));
                p->ReleaseVarDesc(pDesc);
                ASSERT(SUCCEEDED(hr));
                if (FAILED(hr))
                    return hr;
            }
        }
        return hr;
    }
};

// Used to load a type library
struct TypeLib : public ATL::CComPtr<ITypeLib2>
{
    typedef ATL::CComPtr<ITypeLib2> Parent;
    TypeLib()
    {
        // Never unloaded
        static ITypeLib * pITypeLib;
        HRESULT hr;
        if (!pITypeLib)
        {
            wchar_t FileName[MAX_PATH];
            if (!GetModuleFileNameW(HINST_THISCOMPONENT, FileName, MAX_PATH))
                __debugbreak();
            hr = LoadTypeLibEx(FileName, REGKIND_NONE, &pITypeLib);
            ASSERT(SUCCEEDED(hr));
        }
        hr = pITypeLib->QueryInterface(&(Parent&)*this);
        ASSERT(SUCCEEDED(hr));
    }

    TypeLib(const IID & T)
    {
        ATL::CComPtr<ITypeLib> pITypeLib;
        HRESULT hr = LoadRegTypeLib(T, 1, 0, 0, &pITypeLib);
        ASSERT(SUCCEEDED(hr));
        UNREFERENCED_PARAMETER(hr);
        (Parent&)*this = pITypeLib;
    }

    TypeLib(LPCWSTR pszFileName, REGKIND RegKind = REGKIND_NONE)
    {
        ATL::CComPtr<ITypeLib> pITypeLib;
        HRESULT hr = LoadTypeLibEx(pszFileName, RegKind, &pITypeLib);
        ASSERT(SUCCEEDED(hr));
        UNREFERENCED_PARAMETER(hr);
        (Parent&)*this = pITypeLib;
    }

    HRESULT EnumTypeLib()
    {
        if (!p)
            return E_INVALIDARG; 

        ULONG UniqueNames = 0;
        ULONG chUniqueNames = 0;
        HRESULT hr = p->GetLibStatistics(&UniqueNames, &chUniqueNames);
        ASSERT(SUCCEEDED(hr));
        if (FAILED(hr))
            return hr;
        _tprintf(_T(" %d unique names\n"), UniqueNames);
        UINT tiCount = p->GetTypeInfoCount();
        for (UINT i = 0; i < tiCount; ++i)
        {
            TypeInfo pITypeInfo;
            hr = p->GetTypeInfo(i, &pITypeInfo);
            ASSERT(SUCCEEDED(hr));
            if (FAILED(hr))
                return hr;

            hr = pITypeInfo.DisplayTypeInfo();
            ASSERT(SUCCEEDED(hr));
            if (FAILED(hr))
                return hr;
        }
        return hr;
    }

    ATL::CString GetEnumName(LPCTSTR EnumName, int Value)
    {
        ATL::CString RetVal(_T("<unknown>"));
        if (!p)
            return RetVal; 

        WORD Found = 256;
        ITypeInfo * pITypeInfos[256];
        MEMBERID MemberIds[256];
        ATL::CComBSTR sEnumName(EnumName);
        HRESULT hr = p->FindName(sEnumName, 0, pITypeInfos, MemberIds, &Found);
        ASSERT(SUCCEEDED(hr));
        if (FAILED(hr))
            return RetVal;

        for (UINT i = 0; i < Found; ++i)
        {
            ITypeInfo * pITypeInfo = pITypeInfos[i];
            TYPEATTR * pTypeAttr;
            hr = pITypeInfo->GetTypeAttr(&pTypeAttr);
            ASSERT(SUCCEEDED(hr));
            if (FAILED(hr))
                return RetVal;
            if (pTypeAttr->typekind == TKIND_ENUM)
            {
                for (WORD j = 0; j < pTypeAttr->cVars; ++j)
                {
                    VARDESC * pDesc;
                    hr = pITypeInfo->GetVarDesc(j, &pDesc);
                    ASSERT(SUCCEEDED(hr));
                    if (FAILED(hr))
                        return RetVal;
                    if (pDesc->lpvarValue->iVal == Value)
                    {
                        ATL::CComBSTR pszVarName;
                        hr = pITypeInfo->GetDocumentation(pDesc->memid, &pszVarName, 0, 0, 0);
                        ASSERT(SUCCEEDED(hr));
                        if (SUCCEEDED(hr))
                            RetVal = pszVarName;
                        // Break
                        i = Found;
                        j = pTypeAttr->cVars;
                    }
                    pITypeInfo->ReleaseVarDesc(pDesc);
                }
            }
            pITypeInfo->ReleaseTypeAttr(pTypeAttr);
            pITypeInfo->Release();
        }
        return RetVal;
    }
};
// Used to preload enum values for fast access
class EnumCache
{
    typedef std::map<int, ATL::CString> Map;
    Map m_Cache;
public:
    EnumCache(ITypeLib * pTypeLib, LPCTSTR EnumName)
    {
        if (!pTypeLib)
            return; 

        WORD Found = 4;
        ITypeInfo * pITypeInfos[4];
        MEMBERID MemberIds[4];
        ATL::CComBSTR sEnumName(EnumName);
        HRESULT hr = pTypeLib->FindName(sEnumName, 0, pITypeInfos, MemberIds, &Found);
        ASSERT(SUCCEEDED(hr));
        if (FAILED(hr))
            return;
        ASSERT(Found == 1);
        for (UINT i = 0; i < Found; ++i)
        {
            ITypeInfo * pITypeInfo = pITypeInfos[i];
            TYPEATTR * pTypeAttr;
            hr = pITypeInfo->GetTypeAttr(&pTypeAttr);
            ASSERT(SUCCEEDED(hr));
            if (FAILED(hr))
                return;

            ASSERT(pTypeAttr->typekind == TKIND_ENUM);
            if (pTypeAttr->typekind != TKIND_ENUM)
                return;

            for (WORD j = 0; j < pTypeAttr->cVars; ++j)
            {
                VARDESC * pDesc;
                hr = pITypeInfo->GetVarDesc(j, &pDesc);
                ASSERT(SUCCEEDED(hr));
                if (FAILED(hr))
                    return;

                ATL::CComBSTR pVarName;
                hr = pITypeInfo->GetDocumentation(pDesc->memid, &pVarName, 0, 0, 0);
                if (SUCCEEDED(hr))
                    m_Cache[pDesc->lpvarValue->iVal] = pVarName;
                pITypeInfo->ReleaseVarDesc(pDesc);
                ASSERT(SUCCEEDED(hr));
                if (FAILED(hr))
                    return;
            }
            pITypeInfo->ReleaseTypeAttr(pTypeAttr);
            pITypeInfo->Release();
        }
    }
    LPCTSTR GetName(int Value)
    {
        Map::const_iterator itr = m_Cache.find(Value);
        if (itr == m_Cache.end())
            return _T("<unknown>");
        return itr->second;
    }
};
