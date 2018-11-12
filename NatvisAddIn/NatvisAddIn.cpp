// NatvisAddIn.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "NatvisAddIn.h"

#include "../bdl/bdldfp/bdldfp_decimal.h"

/*
Create a C++ library named "Library" as screenshot show
Create a natvis file named "Library.natvis"
Write codes to Library.natvis file
Add a new C++ project named "TestLibrary" to the solution
Reference the library file in the C++ project and write codes in source file
Debug the source file
We can see the Library.natvis file can be loaded successfully
*/

namespace BDEC = ::BloombergLP::bdldfp;

ADDIN_API HRESULT WINAPI AddIn_Decimal_Type32( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/ )
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
        sprintf_s(pResult, maxResult, s.str().c_str());
    }

    return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn_Decimal_Type64( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/ )
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
        sprintf_s(pResult, maxResult, s.str().c_str());
    }

    return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn_Decimal_Type128( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/ )
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
        sprintf_s(pResult, maxResult, s.str().c_str());
    }

    return S_OK;
}
