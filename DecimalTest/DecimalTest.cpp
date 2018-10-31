// DecimalTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#pragma warning(disable: 4146)

#include <iostream>
#include <sstream>

#include <streambuf>

#include <string_view>

#include <bdldfp_decimal.h>

//#include <bid_conf.h>
//#include <bid_functions.h>

namespace BDEC = ::BloombergLP::bdldfp;

namespace {

// https://stackoverflow.com/questions/13059091/creating-an-input-stream-from-constant-memory
struct membuf: std::streambuf {
    membuf(char const* base, size_t size) {
        char* p(const_cast<char*>(base));
        this->setg(p, p, p + size);
    }
};
struct imemstream: virtual membuf, std::istream {
    imemstream(char const* base, size_t size)
        : membuf(base, size)
        , std::istream(static_cast<std::streambuf*>(this)) {
    }
};

BDEC::Decimal128 GetDecimal128(const std::string_view& v)
{
    imemstream s(v.data(), v.size());
    BDEC::Decimal128 result;
    s >> result;
    return result;
}

} // namespace


int main()
{
    BDEC::Decimal128 dec1;

    std::istringstream("123.45") >> dec1;

    BDEC::Decimal128 dec2 = GetDecimal128("67.89");

    std::cout << dec1 << ", " << dec2 << '\n';

    std::cout << dec1 * dec2 << '\n';
    std::cout << (dec1 * dec2) / 5 << '\n';
    std::cout << (dec1 + dec2) << '\n';
}
