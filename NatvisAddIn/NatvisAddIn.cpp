// NatvisAddIn.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "NatvisAddIn.h"

#pragma comment(lib, "delayimp")  


#include "../bdl/bdldfp/bdldfp_decimal.h"


namespace {

// avoid any libraries
TCHAR* PathGetFileName(TCHAR* szPath)
{
    TCHAR* result = nullptr;
    while (const TCHAR ch = *szPath++)
    {
        if (ch == _T(':') || ch == _T('\\') || ch == _T('/'))
            result = szPath;
    }
    return result ? result : (szPath - 1);
}

TCHAR savedPath[MAX_PATH];

}


BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        OutputDebugString(_T("In Natvis Addin DllMain(DLL_PROCESS_ATTACH)\n"));

        if (!GetDllDirectory(sizeof(savedPath) / sizeof(savedPath[0]), savedPath))
            savedPath[0] = _T('\0');

        TCHAR path[MAX_PATH];

        if (GetModuleFileName(hinstDLL, path, sizeof(path) / sizeof(path[0])))
        {
            *PathGetFileName(path) = _T('\0');
            SetDllDirectory(path);
        }
    }
    break;
    case DLL_PROCESS_DETACH:
    {
        OutputDebugString(_T("In Natvis Addin DllMain(DLL_PROCESS_DETACH)\n"));

        SetDllDirectory(savedPath);
    }
    break;
    }
    return TRUE;
}

// https://github.com/Microsoft/MIEngine/blob/master/src/MIDebugEngine/Natvis.Impl/natvis.xsd

/*
Natvis file locations
You can add.natvis files to your user directory or to a system directory, if you want them to apply to multiple projects.
The.natvis files are evaluated in the following order :
Any.natvis files that are embedded in a.pdb you're debugging, unless a file of the same name exists in the loaded project.
Any.natvis files that are in a loaded C++ project or top - level solution.This group includes all loaded C++ projects, including class libraries, but not projects in other languages.
The user - specific Natvis directory(for example, %USERPROFILE%\Documents\Visual Studio 2017\Visualizers).
The system - wide Natvis directory(%VSINSTALLDIR%\Common7\Packages\Debugger\Visualizers).This directory has the.natvis files that are installed with Visual Studio.If you have administrator permissions, you can add files to this directory.

https://docs.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects?view=vs-2017
*/

/*
Create a C++ library named "Library" as screenshot show
Create a natvis file named "Library.natvis"
Write codes to Library.natvis file
Add a new C++ project named "TestLibrary" to the solution
Reference the library file in the C++ project and write codes in source file
Debug the source file
We can see the Library.natvis file can be loaded successfully
*/

// https://code.msdn.microsoft.com/vstudio/Writing-debugger-type-ff6a2fa8/sourcecode?fileId=82223&pathId=1003264534

namespace BDEC = ::BloombergLP::bdldfp;

ADDIN_API HRESULT WINAPI AddIn_Decimal_Type32( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/ )
{
    try
    {
        BDEC::Decimal32 dec;
        DWORD nGot;

        // read system time from debuggee memory space 
        HRESULT hr = pHelper->ReadDebuggeeMemoryEx(pHelper, pHelper->GetRealAddress(pHelper), sizeof(dec), &dec, &nGot);
        if (hr != S_OK || nGot != sizeof(dec))
            sprintf_s(pResult, maxResult, "Error while formatting variable!");
        else
        {
            std::ostringstream s;
            s << dec;
            strcpy_s(pResult, maxResult, s.str().c_str());
        }
    }
    catch (const std::exception& ex)
    {
        sprintf_s(pResult, maxResult, "Thrown exception %s: %s", typeid(ex).name(), ex.what());
    }

    return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn_Decimal_Type64( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/ )
{
    try
    {
        BDEC::Decimal64 dec;
        DWORD nGot;

        // read system time from debuggee memory space 
        HRESULT hr = pHelper->ReadDebuggeeMemoryEx(pHelper, pHelper->GetRealAddress(pHelper), sizeof(dec), &dec, &nGot);
        if (hr != S_OK || nGot != sizeof(dec))
            sprintf_s(pResult, maxResult, "Error while formatting variable!");
        else
        {
            std::ostringstream s;
            s << dec;
            strcpy_s(pResult, maxResult, s.str().c_str());
        }
    }
    catch (const std::exception& ex)
    {
        sprintf_s(pResult, maxResult, "Thrown exception %s: %s", typeid(ex).name(), ex.what());
    }

    return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn_Decimal_Type128( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/ )
{
    try
    {
        BDEC::Decimal128 dec;
        DWORD nGot;

        // read system time from debuggee memory space 
        HRESULT hr = pHelper->ReadDebuggeeMemoryEx(pHelper, pHelper->GetRealAddress(pHelper), sizeof(dec), &dec, &nGot);
        if (hr != S_OK || nGot != sizeof(dec))
            sprintf_s(pResult, maxResult, "Error while formatting variable!");
        else
        {
            std::ostringstream s;
            s << dec;
            strcpy_s(pResult, maxResult, s.str().c_str());
        }
    }
    catch (const std::exception& ex)
    {
        sprintf_s(pResult, maxResult, "Thrown exception %s: %s", typeid(ex).name(), ex.what());
    }

    return S_OK;
}
