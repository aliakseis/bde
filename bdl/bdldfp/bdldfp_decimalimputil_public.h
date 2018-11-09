#pragma once

#ifndef INCLUDED_BDLDFP_DECIMALPLATFORM
#include <bdldfp_decimalplatform.h>
#endif

#include <stdint.h>

#ifdef BDLDFP_DECIMALPLATFORM_SOFTWARE

// DECIMAL FLOATING-POINT LITERAL EMULATION


#define BDLDFP_DECIMALIMPUTIL_DF(lit)                                        \
    BloombergLP::bdldfp::DecimalImpUtil::parse32(                            \
        (BloombergLP::bdldfp::DecimalImpUtil::checkLiteral(lit), #lit))

#define BDLDFP_DECIMALIMPUTIL_DD(lit)                                        \
    BloombergLP::bdldfp::DecimalImpUtil::parse64(                            \
        (BloombergLP::bdldfp::DecimalImpUtil::checkLiteral(lit), #lit))

#define BDLDFP_DECIMALIMPUTIL_DL(lit)                                        \
    BloombergLP::bdldfp::DecimalImpUtil::parse128(                           \
        (BloombergLP::bdldfp::DecimalImpUtil::checkLiteral(lit), #lit))

#elif defined(BDLDFP_DECIMALPLATFORM_C99_TR) || defined( __IBM_DFP__ )

#define BDLDFP_DECIMALIMPUTIL_JOIN_(a,b) a##b

// Portable decimal floating-point literal support

#define BDLDFP_DECIMALIMPUTIL_DF(lit) BDLDFP_DECIMALIMPUTIL_JOIN_(lit,df)

#define BDLDFP_DECIMALIMPUTIL_DD(lit) BDLDFP_DECIMALIMPUTIL_JOIN_(lit,dd)

#define BDLDFP_DECIMALIMPUTIL_DL(lit) BDLDFP_DECIMALIMPUTIL_JOIN_(lit,dl)

#endif


namespace BloombergLP {
namespace bdldfp {

namespace DecimalImpUtil_IntelDfp {

struct ValueType32;
struct ValueType64;
struct ValueType128;

}
                        // ====================
                        // namespace DecimalImpUtil
                        // ====================

namespace DecimalImpUtil {

struct ValueType32 
{ 
    uint32_t  d_raw; 

    ValueType32() = default;
    ValueType32(const DecimalImpUtil_IntelDfp::ValueType32& other);
    operator DecimalImpUtil_IntelDfp::ValueType32() const;
};

struct ValueType64 
{ 
    uint64_t  d_raw; 

    ValueType64() = default;
    ValueType64(const DecimalImpUtil_IntelDfp::ValueType64& other);
    operator DecimalImpUtil_IntelDfp::ValueType64() const;
};

struct ValueType128 
{ 
    struct Type128 { 
        Type128() = default;
        template<typename T> Type128(const T& other) : w{ other.w[0], other.w[1] } {}
        template<typename T> operator T() const { return { { w[0], w[1] } }; }
        uint64_t w[2]; 
    } d_raw;

    ValueType128() = default;
    ValueType128(const DecimalImpUtil_IntelDfp::ValueType128& other);
    operator DecimalImpUtil_IntelDfp::ValueType128() const;
};

// Parsing functions

ValueType32 parse32(const char *input);
// Parse the specified 'input' string as a 32 bit decimal floating-
// point value and return the result.  The parsing is as specified for
// the 'strtod32' function in section 9.6 of the ISO/EIC TR 24732 C
// Decimal Floating-Point Technical Report, except that it is
// unspecified whether the NaNs returned are quiet or signaling.  If
// 'input' does not represent a valid 32 bit decimal floating-point
// number, then return NaN.  Note that this method does not guarantee
// the behavior of ISO/EIC TR 24732 C when parsing NaN because the AIX
// compiler intrinsics return a signaling NaN.

ValueType64 parse64(const char *input);
// Parse the specified 'input' string as a 64 bit decimal floating-
// point value and return the result.  The parsing is as specified for
// the 'strtod64' function in section 9.6 of the ISO/EIC TR 24732 C
// Decimal Floating-Point Technical Report, except that it is
// unspecified whether the NaNs returned are quiet or signaling.  If
// 'input' does not represent a valid 64 bit decimal floating-point
// number, then return NaN.  Note that this method does not guarantee
// the behavior of ISO/EIC TR 24732 C when parsing NaN because the AIX
// compiler intrinsics return a signaling NaN.

ValueType128 parse128(const char *input);
// Parse the specified 'input' string as a 128 bit decimal floating-
// point value and return the result.  The parsing is as specified for
// the 'strtod128' function in section 9.6 of the ISO/EIC TR 24732 C
// Decimal Floating-Point Technical Report, except that it is
// unspecified whether the NaNs returned are quiet or signaling.  If
// 'input' does not represent a valid 128 bit decimal floating-point
// number, then return NaN.  Note that this method does not guarantee
// the behavior of ISO/EIC TR 24732 C when parsing NaN because the AIX
// compiler intrinsics return a signaling NaN.

#ifdef BDLDFP_DECIMALPLATFORM_SOFTWARE

                        // Literal Checking Functions

struct This_is_not_a_floating_point_literal {};
// This 'struct' is a helper type used to generate error messages for
// bad literals.


template <class TYPE>
inline
void checkLiteral(const TYPE& t)
{
    (void)static_cast<This_is_not_a_floating_point_literal>(t);
}
// Generate an error if the specified 't' is bad decimal
// floating-point.  Note that this function is intended for use with
// literals

inline
void checkLiteral(double)
{
}
// Overload to avoid an error when the decimal floating-point literal
// (without the suffix) can be interpreted as a 'double' literal.

#elif defined(BDLDFP_DECIMALPLATFORM_HARDWARE)

#else

#error Improperly configured decimal floating point platform settings

#endif

                        // makeDecimalRaw functions

ValueType32 makeDecimalRaw32(int significand, int exponent);
// Create a 'ValueType32' object representing a decimal floating point
// number consisting of the specified 'significand' and 'exponent',
// with the sign given by 'significand'.  The behavior is undefined
// unless 'abs(significand) <= 9,999,999' and '-101 <= exponent <= 90'.

ValueType64 makeDecimalRaw64(unsigned long long int significand,
    int exponent);
ValueType64 makeDecimalRaw64(long long int significand,
    int exponent);
ValueType64 makeDecimalRaw64(unsigned           int significand,
    int exponent);
ValueType64 makeDecimalRaw64(int significand,
    int exponent);
// Create a 'ValueType64' object representing a decimal floating point
// number consisting of the specified 'significand' and 'exponent',
// with the sign given by 'significand'.  The behavior is undefined
// unless 'abs(significand) <= 9,999,999,999,999,999' and
// '-398 <= exponent <= 369'.

ValueType128 makeDecimalRaw128(unsigned long long int significand,
    int exponent);
ValueType128 makeDecimalRaw128(long long int significand,
    int exponent);
ValueType128 makeDecimalRaw128(unsigned           int significand,
    int exponent);
ValueType128 makeDecimalRaw128(int significand,
    int exponent);
// Create a 'ValueType128' object representing a decimal floating point
// number consisting of the specified 'significand' and 'exponent',
// with the sign given by 'significand'.  The behavior is undefined
// unless '-6176 <= exponent <= 6111'.


                          // normalize

ValueType32 normalize(ValueType32 original);
ValueType64 normalize(ValueType64 original);
ValueType128 normalize(ValueType128 original);
// Return a 'ValueTypeXX' number having the value as the specified
// 'original, but with the significand, that can not be divided by ten,
// and appropriate exponent.
//
//: o Any representations of zero value (either positive or negative)
//:   are normalized to positive zero having null significand and
//:   exponent.
//:
//: o Any NaN values (either signaling or quiet) are normalized to
//:   quiet NaN.
//:
//: o Normalized non-zero value has the same sign as the original one.


}

}

}