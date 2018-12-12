#include "pch.h"

#include "../bdldfp/bdldfp_decimal.h"

#include <random>

//TEST(TestCaseName, TestName) {
//  EXPECT_EQ(1, 1);
//  EXPECT_TRUE(true);
//}

using namespace BloombergLP;

namespace BDEC = BloombergLP::bdldfp;

enum {
    verbose = true,
    veryVerbose = true,
    veryVeryVerbose = true,
    veryVeryVeryVerbose = true,
};

#define Q(X)                                                   \
    std::cout << "<| " #X " |>" << std::endl;
// Quote identifier literally.

#define P(X)                                                   \
    std::cout << #X " = " << (X) << std::endl;
    // Print identifier and its value.

#define P_(X)                                                  \
    std::cout << #X " = " << (X) << ", " << std::flush;
    // 'P(X)' without '\n'

#define L_                                                     \
    __LINE__
    // current Line number

#define T_                                                     \
    std::cout << "\t" << std::flush;
    // Print tab (w/o newline).


#define LOOP_ASSERT(I,X) EXPECT_TRUE(X)

namespace {

//=============================================================================
//              GLOBAL HELPER FUNCTIONS AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                 // stringstream helpers - not thread safe!

void getStringFromStream(std::ostringstream &o, std::string  *out)
{
    //bslma::TestAllocator osa("osstream");
    //bslma::DefaultAllocatorGuard g(&osa);
    *out = o.str();
}

void getStringFromStream(std::wostringstream &o, std::wstring *out)
{
    //bslma::TestAllocator osa("osstream");
    //bslma::DefaultAllocatorGuard g(&osa);
    *out = o.str();
}

 // String compare for decimal floating point numbers needs 'e'/'E' conversion

std::string& decLower(std::string& s)
{
    for (size_t i = 0; i < s.length(); ++i) if ('E' == s[i]) s[i] = 'e';
    return s;
}

std::wstring& decLower(std::wstring& s)
{
    for (size_t i = 0; i < s.length(); ++i) if (L'E' == s[i]) s[i] = L'e';
    return s;
}

//-----------------------------------------------------------------------------


template <class EXPECT, class RECEIVED>
void checkType(const RECEIVED&)
{
    EXPECT_TRUE(typeid(EXPECT) == typeid(RECEIVED));
}

                          // Stream buffer helpers

template <int SIZE>
struct BufferBuf : std::streambuf {
    BufferBuf() { reset(); }
    const char *str() { *this->pptr() =0; return this->pbase(); }
    void reset() { this->setp(this->d_buf, this->d_buf + SIZE); }
    char d_buf[SIZE + 1];
};

struct PtrInputBuf : std::streambuf {
    PtrInputBuf(const char *s) {
        char *x = const_cast<char *>(s);
        this->setg(x, x, x + strlen(x));
    }
};

struct NulBuf : std::streambuf {
    char d_dummy[64];
    virtual int overflow(int c) {
        setp( d_dummy, d_dummy + sizeof(d_dummy));
        return traits_type::not_eof(c);
    }
};



std::default_random_engine dre;

bdldfp::Decimal32 randomDecimal32()
    // Return randomly generated Decimal32 object having finite value on
    // success and infinity value otherwise.
{
    int significand = 0;
    int exponent = 0;

    //int rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
    //                            reinterpret_cast<unsigned char*>(&significand),
    //                            sizeof(int));
    //ASSERTV(rc, 0 == rc);

    //significand = static_cast<int>(clipValue( significand,
    //                                         -9999999,
    //                                          9999999));

    //rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
    //                               reinterpret_cast<unsigned char*>(&exponent),
    //                               sizeof(int));
    //ASSERTV(rc, 0 == rc);

    //exponent = static_cast<int>(clipValue(exponent, -101, 90));

    {
        std::uniform_int_distribution<int> di(-9999999, 9999999);
        significand = di(dre);
    }

    {
        std::uniform_int_distribution<int> di(-101, 90);
        exponent = di(dre);
    }

    return BDEC::DecimalImpUtil::makeDecimalRaw32(significand, exponent);
}

bdldfp::Decimal64 randomDecimal64()
    // Return randomly generated Decimal64 object having finite value on
    // success and infinity value otherwise.
{
    long long int significand = 0;
    int           exponent = 0;

    //int rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
    //                            reinterpret_cast<unsigned char*>(&significand),
    //                            sizeof(long long int));
    //ASSERTV(rc, 0 == rc);

    //significand = clipValue( significand,
    //                        -9999999999999999LL,
    //                         9999999999999999LL);

    //rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
    //                               reinterpret_cast<unsigned char*>(&exponent),
    //                               sizeof(int));
    //ASSERTV(rc, 0 == rc);

    //exponent = static_cast<int>(clipValue(exponent, -398, 369));

    {
        std::uniform_int_distribution<long long int> di(-9999999999999999LL, 9999999999999999LL);
        significand = di(dre);
    }

    {
        std::uniform_int_distribution<int> di(-398, 369);
        exponent = di(dre);
    }

    return BDEC::DecimalImpUtil::makeDecimalRaw64(significand, exponent);
}

bdldfp::Decimal128 randomDecimal128()
    // Return randomly generated Decimal128 object having finite value on
    // success and infinity value otherwise.
{
    long long int significand = 0;
    int           exponent = 0;

    //int rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
    //                            reinterpret_cast<unsigned char*>(&significand),
    //                            sizeof(long long int));
    //ASSERTV(rc, 0 == rc);

    //rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
    //                               reinterpret_cast<unsigned char*>(&exponent),
    //                               sizeof(int));
    //ASSERTV(rc, 0 == rc);

    //exponent = static_cast<int>(clipValue(exponent, -6176, 6111));

    {
        std::uniform_int_distribution<unsigned long long int> di;
        significand = di(dre);
    }

    {
        std::uniform_int_distribution<int> di (-6176, 6111);
        exponent = di(dre);
    }

    return BDEC::DecimalImpUtil::makeDecimalRaw128(significand, exponent);
}


} // namespace



TEST(Decimal, testCase8)
{
    // ------------------------------------------------------------------------
    // USAGE EXAMPLE
    //   Extracted from component header file.
    //
    // Concerns:
    //: 1 The usage example provided in the component header file compiles,
    //:   links, and runs as shown.
    //
    // Plan:
    //: 1 Incorporate usage example from header into test driver, remove
    //:   leading comment characters, and replace 'assert' with 'EXPECT_TRUE'.
    //:   (C-1)
    //
    // Testing:
    //   USAGE EXAMPLE
    // ------------------------------------------------------------------------

    if (verbose) std::cout << std::endl
        << "Testing Usage Example" << std::endl
        << "=====================" << std::endl;

    if (veryVerbose) std::cout << std::endl
        << "Portable initialization of "
        << "non-integer, constant values"
        << std::endl;
    {
        // If your compiler does not support the C Decimal TR, it does not
        // support decimal floating point literals, only binary floating
        // point literals.  The problem with binary floating point literals
        // is the same as with binary floating point numbers in general:
        // they cannot represent the decimal numbers we care about.  To
        // solve this problem there are 3 macros provided by this component
        // that can be used to initialize decimal floating point types with
        // non-integer values, precisely.  These macros will evaluate to
        // real, C language literals where those are supported and to a
        // runtime-parsed solution otherwise.  The following code
        // demonstrates the use of these macros as well as mixed-type
        // arithmetics and comparisons:
        //..
        bdldfp::Decimal32  d32(BDLDFP_DECIMAL_DF(0.1));
        bdldfp::Decimal64  d64(BDLDFP_DECIMAL_DD(0.2));
        bdldfp::Decimal128 d128(BDLDFP_DECIMAL_DL(0.3));

        EXPECT_TRUE(d32 + d64 == d128);
        EXPECT_TRUE(bdldfp::Decimal64(d32) * 10 == bdldfp::Decimal64(1));
        EXPECT_TRUE(d64 * 10 == bdldfp::Decimal64(2));
        EXPECT_TRUE(d128 * 10 == bdldfp::Decimal128(3));
    }

    if (veryVerbose) std::cout << std::endl
        << "Precise calculations with decimal "
        << "values" << std::endl;
    {
        //..
        // Suppose we need to add two (decimal) numbers and then tell if
        // the result is a particular decimal number or not.  That can get
        // difficult with binary floating-point, but easy with decimal:
        //..
        if (std::numeric_limits<double>::radix == 2) {
            EXPECT_TRUE(.1 + .2 != .3);
        }
        EXPECT_TRUE(BDLDFP_DECIMAL_DD(0.1) + BDLDFP_DECIMAL_DD(0.2) ==
            BDLDFP_DECIMAL_DD(0.3));
        //..
    }
}

TEST(Decimal, testCase7)
{
    // ------------------------------------------------------------------------
    // TESTING 'hashAppend'
    //
    // Concerns:
    //: 1 The 'hashAppend' function hashes objects with different values
    //:   differently.
    //:
    //: 2 The 'hashAppend' function hashes objects with the same values
    //:   identically irregardless their representations.
    //:
    //: 3 The 'hashAppend' function hashes const and non-const objects.
    //
    // Plan:
    //: 1 Brute force test of a several hand picked and randomly generated
    //:   values, ensuring that hashes of equivalent objects match and hashes
    //:   of unequal objects do not.  (C-1..3)
    //
    // Testing:
    //   void hashAppend(HASHALG& hashAlg, const Decimal32& object);
    //   void hashAppend(HASHALG& hashAlg, const Decimal64& object);
    //   void hashAppend(HASHALG& hashAlg, const Decimal128& object);
    // ------------------------------------------------------------------------

    if (verbose) std::cout << std::endl
        << "Testing 'hashAppend'" << std::endl
        << "====================" << std::endl;

    typedef ::BloombergLP::bslh::Hash<> Hasher;
    typedef Hasher::result_type         HashType;

    Hasher hasher;

    //bslma::TestAllocator va("vector", veryVeryVeryVerbose);

    if (verbose) std::cout << "\tTesting Decimal32" << std::endl;
    {
        typedef BDEC::Decimal32                      Obj;
        typedef std::numeric_limits<BDEC::Decimal32> d32_limits;
        typedef std::pair<Obj, HashType>             TestDataPair;
        typedef std::vector<TestDataPair>            TestDataVector;

        TestDataVector testData;// (&va);

        // Adding boundary and special values.

        const Obj ZERO_P = BDLDFP_DECIMAL_DF(0.00);
        const Obj MIN_P = d32_limits::min();
        const Obj MAX_P = d32_limits::max();
        const Obj EPSILON_P = d32_limits::epsilon();
        const Obj ROUND_ERROR_P = d32_limits::round_error();
        const Obj INFINITE_P = d32_limits::infinity();
        const Obj DENORM_MIN_P = d32_limits::denorm_min();
        const Obj ZERO_N = -ZERO_P;
        const Obj MIN_N = -MIN_P;
        const Obj MAX_N = -MAX_P;
        const Obj EPSILON_N = -EPSILON_P;
        const Obj ROUND_ERROR_N = -ROUND_ERROR_P;
        const Obj INFINITE_N = -INFINITE_P;
        const Obj DENORM_MIN_N = -DENORM_MIN_P;

        const Obj Q_NAN = d32_limits::quiet_NaN();
        HashType  Q_NAN_HASH = hasher(Q_NAN);
        const Obj S_NAN = d32_limits::signaling_NaN();
        HashType  S_NAN_HASH = hasher(S_NAN);

        testData.push_back(TestDataPair(ZERO_P, hasher(ZERO_P)));
        testData.push_back(TestDataPair(MIN_P, hasher(MIN_P)));
        testData.push_back(TestDataPair(MAX_P, hasher(MAX_P)));
        testData.push_back(TestDataPair(EPSILON_P, hasher(EPSILON_P)));
        testData.push_back(TestDataPair(ROUND_ERROR_P, hasher(ROUND_ERROR_P)));
        testData.push_back(TestDataPair(INFINITE_P, hasher(INFINITE_P)));
        testData.push_back(TestDataPair(DENORM_MIN_P, hasher(DENORM_MIN_P)));
        testData.push_back(TestDataPair(ZERO_N, hasher(ZERO_N)));
        testData.push_back(TestDataPair(MIN_N, hasher(MIN_N)));
        testData.push_back(TestDataPair(MAX_N, hasher(MAX_N)));
        testData.push_back(TestDataPair(EPSILON_N, hasher(EPSILON_N)));
        testData.push_back(TestDataPair(ROUND_ERROR_N, hasher(ROUND_ERROR_N)));
        testData.push_back(TestDataPair(INFINITE_N, hasher(INFINITE_N)));
        testData.push_back(TestDataPair(DENORM_MIN_N, hasher(DENORM_MIN_N)));

        // Adding identical values, having different representations:
        // 1e+3
        // 10e+2
        // ...
        // 100000e-2
        // 1000000e-3

        int       cloneSignificand = 1;
        int       cloneExponent = 3;
        const Obj ORIGINAL = BDEC::DecimalImpUtil::makeDecimalRaw32(
            cloneSignificand,
            cloneExponent);
        testData.push_back(TestDataPair(ORIGINAL, hasher(ORIGINAL)));

        for (int i = 0; i < 6; ++i) {
            --cloneExponent;
            cloneSignificand *= 10;
            const Obj CLONE = BDEC::DecimalImpUtil::makeDecimalRaw32(
                cloneSignificand,
                cloneExponent);
            EXPECT_TRUE(ORIGINAL == CLONE);
            testData.push_back(TestDataPair(CLONE, hasher(CLONE)));
        }

        // Adding zero values, having different representations.

        const Obj ZERO1 = BDLDFP_DECIMAL_DF(-0.00);
        const Obj ZERO2 = BDEC::DecimalImpUtil::makeDecimalRaw32(0, 1);
        const Obj ZERO3 = BDEC::DecimalImpUtil::makeDecimalRaw32(0, 10);
        const Obj ZERO4 = BDEC::DecimalImpUtil::makeDecimalRaw32(0, 90);

        testData.push_back(TestDataPair(ZERO1, hasher(ZERO1)));
        testData.push_back(TestDataPair(ZERO2, hasher(ZERO2)));
        testData.push_back(TestDataPair(ZERO3, hasher(ZERO3)));
        testData.push_back(TestDataPair(ZERO4, hasher(ZERO4)));

        // Adding random values.

        const int RANDOM_VALUES_NUM = 100;
        for (int i = 0; i < RANDOM_VALUES_NUM; ++i) {
            Obj randomValue = randomDecimal32();
            testData.push_back(TestDataPair(randomValue,
                hasher(randomValue)));
        }

        if (veryVerbose) {
            T_ T_ P(testData.size());
        }

        // Testing 'hashAppend' function.

        TestDataVector::iterator iter1 = testData.begin();

        while (iter1 != testData.end()) {
            const Obj&               VALUE1 = iter1->first;
            const HashType&          HASH1 = iter1->second;
            TestDataVector::iterator iter2 = testData.begin();

            if (veryVerbose) {
                T_ T_ P_(VALUE1) P(HASH1);
            }

            while (iter2 != testData.end()) {
                const Obj&      VALUE2 = iter2->first;
                const HashType& HASH2 = iter2->second;
                if (VALUE1 == VALUE2) {
                    EXPECT_TRUE(HASH1 == HASH2);
                }
                else {
                    EXPECT_TRUE(HASH1 != HASH2);
                }
                ++iter2;
            }
            EXPECT_TRUE(Q_NAN_HASH != HASH1);
            EXPECT_TRUE(S_NAN_HASH != HASH1);
            ++iter1;
        }
    }

    if (verbose) std::cout << "\tTesting Decimal64" << std::endl;
    {
        typedef BDEC::Decimal64                      Obj;
        typedef std::numeric_limits<BDEC::Decimal32> d64_limits;
        typedef std::pair<Obj, HashType>             TestDataPair;
        typedef std::vector<TestDataPair>            TestDataVector;

        TestDataVector       testData;// (&va);

        // Adding boundary and special values.

        const Obj ZERO_P = BDLDFP_DECIMAL_DD(0.00);
        const Obj MIN_P = d64_limits::min();
        const Obj MAX_P = d64_limits::max();
        const Obj EPSILON_P = d64_limits::epsilon();
        const Obj ROUND_ERROR_P = d64_limits::round_error();
        const Obj INFINITE_P = d64_limits::infinity();
        const Obj DENORM_MIN_P = d64_limits::denorm_min();
        const Obj ZERO_N = -ZERO_P;
        const Obj MIN_N = -MIN_P;
        const Obj MAX_N = -MAX_P;
        const Obj EPSILON_N = -EPSILON_P;
        const Obj ROUND_ERROR_N = -ROUND_ERROR_P;
        const Obj INFINITE_N = -INFINITE_P;
        const Obj DENORM_MIN_N = -DENORM_MIN_P;

        const Obj Q_NAN = d64_limits::quiet_NaN();
        HashType  Q_NAN_HASH = hasher(Q_NAN);
        const Obj S_NAN = d64_limits::signaling_NaN();
        HashType  S_NAN_HASH = hasher(S_NAN);

        testData.push_back(TestDataPair(ZERO_P, hasher(ZERO_P)));
        testData.push_back(TestDataPair(MIN_P, hasher(MIN_P)));
        testData.push_back(TestDataPair(MAX_P, hasher(MAX_P)));
        testData.push_back(TestDataPair(EPSILON_P, hasher(EPSILON_P)));
        testData.push_back(TestDataPair(ROUND_ERROR_P, hasher(ROUND_ERROR_P)));
        testData.push_back(TestDataPair(INFINITE_P, hasher(INFINITE_P)));
        testData.push_back(TestDataPair(DENORM_MIN_P, hasher(DENORM_MIN_P)));
        testData.push_back(TestDataPair(ZERO_N, hasher(ZERO_N)));
        testData.push_back(TestDataPair(MIN_N, hasher(MIN_N)));
        testData.push_back(TestDataPair(MAX_N, hasher(MAX_N)));
        testData.push_back(TestDataPair(EPSILON_N, hasher(EPSILON_N)));
        testData.push_back(TestDataPair(ROUND_ERROR_N, hasher(ROUND_ERROR_N)));
        testData.push_back(TestDataPair(INFINITE_N, hasher(INFINITE_N)));
        testData.push_back(TestDataPair(DENORM_MIN_N, hasher(DENORM_MIN_N)));

        // Adding identical values, having different representations:
        // 1e+7
        // 10e+6
        // ...
        // 100000000000000e-7
        // 1000000000000000e-8

        long long cloneSignificand = 1;
        int       cloneExponent = 7;
        const Obj ORIGINAL = BDEC::DecimalImpUtil::makeDecimalRaw64(
            cloneSignificand,
            cloneExponent);
        testData.push_back(TestDataPair(ORIGINAL, hasher(ORIGINAL)));

        for (int i = 0; i < 15; ++i) {
            --cloneExponent;
            cloneSignificand *= 10;
            const Obj CLONE = BDEC::DecimalImpUtil::makeDecimalRaw64(
                cloneSignificand,
                cloneExponent);
            EXPECT_TRUE(ORIGINAL == CLONE);
            testData.push_back(TestDataPair(CLONE, hasher(CLONE)));
        }

        // Adding zero values, having different representations.

        const Obj ZERO1 = BDLDFP_DECIMAL_DD(-0.00);
        const Obj ZERO2 = BDEC::DecimalImpUtil::makeDecimalRaw64(0, 1);
        const Obj ZERO3 = BDEC::DecimalImpUtil::makeDecimalRaw64(0, 10);
        const Obj ZERO4 = BDEC::DecimalImpUtil::makeDecimalRaw64(0, 100);
        const Obj ZERO5 = BDEC::DecimalImpUtil::makeDecimalRaw64(0, 369);

        testData.push_back(TestDataPair(ZERO1, hasher(ZERO1)));
        testData.push_back(TestDataPair(ZERO2, hasher(ZERO2)));
        testData.push_back(TestDataPair(ZERO3, hasher(ZERO3)));
        testData.push_back(TestDataPair(ZERO4, hasher(ZERO4)));
        testData.push_back(TestDataPair(ZERO5, hasher(ZERO5)));

        // Adding random values.

        const int RANDOM_VALUES_NUM = 100;
        for (int i = 0; i < RANDOM_VALUES_NUM; ++i) {
            const Obj randomValue = randomDecimal64();
            testData.push_back(TestDataPair(randomValue,
                hasher(randomValue)));
        }

        if (veryVerbose) {
            T_ T_ P(testData.size());
        }

        // Testing 'hashAppend' function.

        TestDataVector::iterator iter1 = testData.begin();

        while (iter1 != testData.end()) {
            const Obj&               VALUE1 = iter1->first;
            const HashType&          HASH1 = iter1->second;
            TestDataVector::iterator iter2 = testData.begin();

            if (veryVerbose) {
                T_ T_ P_(VALUE1) P(HASH1);
            }

            while (iter2 != testData.end()) {
                const Obj&      VALUE2 = iter2->first;
                const HashType& HASH2 = iter2->second;
                if (VALUE1 == VALUE2) {
                    EXPECT_TRUE(HASH1 == HASH2);
                }
                else {
                    EXPECT_TRUE(HASH1 != HASH2);
                }
                ++iter2;
            }
            EXPECT_TRUE(Q_NAN_HASH != HASH1);
            EXPECT_TRUE(S_NAN_HASH != HASH1);
            ++iter1;
        }
    }

    if (verbose) std::cout << "\tTesting Decimal128" << std::endl;
    {
        typedef BDEC::Decimal128                     Obj;
        typedef std::numeric_limits<BDEC::Decimal32> d128_limits;
        typedef std::pair<Obj, HashType>             TestDataPair;
        typedef std::vector<TestDataPair>            TestDataVector;

        TestDataVector       testData;// (&va);

        // Adding boundary and special values.

        const Obj ZERO_P = BDLDFP_DECIMAL_DL(0.00);
        const Obj MIN_P = d128_limits::min();
        const Obj MAX_P = d128_limits::max();
        const Obj EPSILON_P = d128_limits::epsilon();
        const Obj ROUND_ERROR_P = d128_limits::round_error();
        const Obj INFINITE_P = d128_limits::infinity();
        const Obj DENORM_MIN_P = d128_limits::denorm_min();
        const Obj ZERO_N = -ZERO_P;
        const Obj MIN_N = -MIN_P;
        const Obj MAX_N = -MAX_P;
        const Obj EPSILON_N = -EPSILON_P;
        const Obj ROUND_ERROR_N = -ROUND_ERROR_P;
        const Obj INFINITE_N = -INFINITE_P;
        const Obj DENORM_MIN_N = -DENORM_MIN_P;

        const Obj Q_NAN = d128_limits::quiet_NaN();
        HashType  Q_NAN_HASH = hasher(Q_NAN);
        const Obj S_NAN = d128_limits::signaling_NaN();
        HashType  S_NAN_HASH = hasher(S_NAN);

        testData.push_back(TestDataPair(ZERO_P, hasher(ZERO_P)));
        testData.push_back(TestDataPair(MIN_P, hasher(MIN_P)));
        testData.push_back(TestDataPair(MAX_P, hasher(MAX_P)));
        testData.push_back(TestDataPair(EPSILON_P, hasher(EPSILON_P)));
        testData.push_back(TestDataPair(ROUND_ERROR_P, hasher(ROUND_ERROR_P)));
        testData.push_back(TestDataPair(INFINITE_P, hasher(INFINITE_P)));
        testData.push_back(TestDataPair(DENORM_MIN_P, hasher(DENORM_MIN_P)));
        testData.push_back(TestDataPair(ZERO_N, hasher(ZERO_N)));
        testData.push_back(TestDataPair(MIN_N, hasher(MIN_N)));
        testData.push_back(TestDataPair(MAX_N, hasher(MAX_N)));
        testData.push_back(TestDataPair(EPSILON_N, hasher(EPSILON_N)));
        testData.push_back(TestDataPair(ROUND_ERROR_N, hasher(ROUND_ERROR_N)));
        testData.push_back(TestDataPair(INFINITE_N, hasher(INFINITE_N)));
        testData.push_back(TestDataPair(DENORM_MIN_N, hasher(DENORM_MIN_N)));

        // Adding identical values, having different representations:
        // 1e+9
        // 10e+8
        // ...
        // 100000000000000000e-8
        // 1000000000000000000e-9

        long long cloneSignificand = 1;
        int       cloneExponent = 9;
        const Obj ORIGINAL = BDEC::DecimalImpUtil::makeDecimalRaw128(
            cloneSignificand,
            cloneExponent);
        testData.push_back(TestDataPair(ORIGINAL, hasher(ORIGINAL)));

        for (int i = 0; i < 18; ++i) {
            --cloneExponent;
            cloneSignificand *= 10;
            const Obj CLONE = BDEC::DecimalImpUtil::makeDecimalRaw128(
                cloneSignificand,
                cloneExponent);
            EXPECT_TRUE(ORIGINAL == CLONE);
            testData.push_back(TestDataPair(CLONE, hasher(CLONE)));
        }

        // Adding zero values, having different representations.

        const Obj ZERO1 = BDLDFP_DECIMAL_DL(-0.00);
        const Obj ZERO2 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 1);
        const Obj ZERO3 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 10);
        const Obj ZERO4 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 100);
        const Obj ZERO5 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 1000);
        const Obj ZERO6 = BDEC::DecimalImpUtil::makeDecimalRaw128(0, 6111);

        testData.push_back(TestDataPair(ZERO1, hasher(ZERO1)));
        testData.push_back(TestDataPair(ZERO2, hasher(ZERO2)));
        testData.push_back(TestDataPair(ZERO3, hasher(ZERO3)));
        testData.push_back(TestDataPair(ZERO4, hasher(ZERO4)));
        testData.push_back(TestDataPair(ZERO5, hasher(ZERO5)));
        testData.push_back(TestDataPair(ZERO6, hasher(ZERO6)));

        // Adding random values.

        const int RANDOM_VALUES_NUM = 100;
        for (int i = 0; i < RANDOM_VALUES_NUM; ++i) {
            const Obj randomValue = randomDecimal128();
            testData.push_back(TestDataPair(randomValue,
                hasher(randomValue)));
        }

        if (veryVerbose) {
            T_ T_ P(testData.size());
        }

        // Testing 'hashAppend' function.

        TestDataVector::iterator iter1 = testData.begin();

        while (iter1 != testData.end()) {
            const Obj&               VALUE1 = iter1->first;
            const HashType&          HASH1 = iter1->second;
            TestDataVector::iterator iter2 = testData.begin();

            if (veryVerbose) {
                T_ T_ P_(VALUE1) P(HASH1);
            }

            while (iter2 != testData.end()) {
                const Obj&      VALUE2 = iter2->first;
                const HashType& HASH2 = iter2->second;
                if (VALUE1 == VALUE2) {
                    EXPECT_TRUE(HASH1 == HASH2);
                }
                else {
                    EXPECT_TRUE(HASH1 != HASH2);
                }
                ++iter2;
            }
            EXPECT_TRUE(Q_NAN_HASH != HASH1);
            EXPECT_TRUE(S_NAN_HASH != HASH1);
            ++iter1;
        }
    }
}

#if 0
TEST(Decimal, testCase6)
{
    // ------------------------------------------------------------------------
    // TESTING BDEX STREAMING
    //   Verify the BDEX streaming implementation works correctly.  Specific
    //   concerns include wire format, handling of stream states (valid, empty,
    //   invalid, incomplete, and corrupted), and exception neutrality.
    //
    // Concerns:
    //: 1 The class method 'maxSupportedBdexVersion' returns the correct
    //:   version to be used for the specified 'versionSelector'.
    //:
    //: 2 The 'bdexStreamOut' method is callable on a reference providing only
    //:   non-modifiable access.
    //:
    //: 3 For valid streams, externalization and unexternalization are inverse
    //:   operations.
    //:
    //: 4 For invalid streams, externalization leaves the stream invalid and
    //:   unexternalization does not alter the value of the object and leaves
    //:   the stream invalid.
    //:
    //: 5 Unexternalizing of incomplete, invalid, or corrupted data results in
    //:   a valid object of unspecified value and an invalidated stream.
    //:
    //: 6 The wire format of the object is as expected.
    //:
    //: 7 All methods are exception neutral.
    //:
    //: 8 The 'bdexStreamIn' and 'bdexStreamOut' methods return a reference to
    //:   the provided stream in all situations.
    //:
    //: 9 The initial value of the object has no affect on unexternalization.
    //:
    //:10 Streaming version 1 provides the expected compatibility between
    //:   the two calendar modes.
    //
    // Testing:
    //   static int maxSupportedBdexVersion(int versionSelector);
    //   static int maxSupportedBdexVersion();
    //   STREAM& bdexStreamIn(STREAM& stream, int version);
    //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
    // ------------------------------------------------------------------------

    if (verbose) std::cout << std::endl
        << "TESTING BDEX STREAMING" << std::endl
        << "======================" << std::endl;

    // Allocator to use instead of the default allocator.
    //bslma::TestAllocator allocator("bslx", veryVeryVeryVerbose);

    // Scalar obj
    // ect values used in various stream tests.

    typedef BDEC::Decimal64 Obj;
    //typedef bslx::TestInStream In;
    //typedef bslx::TestOutStream Out;
    typedef std::istringstream In;
    typedef std::ostringstream Out;

    const Obj VA(BDLDFP_DECIMAL_DD(1.12));
    const Obj VB(BDLDFP_DECIMAL_DD(2.55));
    const Obj VC(BDLDFP_DECIMAL_DD(3.67));
    const Obj VD(BDLDFP_DECIMAL_DD(5.17));
    const Obj VE(BDEC::DecimalImpUtil::parse64("Inf"));
    const Obj VF(BDEC::DecimalImpUtil::parse64("-Inf"));
    const Obj VG(BDLDFP_DECIMAL_DD(-7.64));

    // Array object used in various stream tests.
    const Obj VALUES[] = { VA, VB, VC, VD, VE, VF, VG };
    const int NUM_VALUES =
        static_cast<int>(sizeof VALUES / sizeof *VALUES);
    const int VERSION_SELECTOR = 20151026;

    // Testing maxSupportedBDexVersion
    {
        EXPECT_TRUE(1 == Obj::maxSupportedBdexVersion());
        EXPECT_TRUE(1 == Obj::maxSupportedBdexVersion(0));
        EXPECT_TRUE(1 == Obj::maxSupportedBdexVersion(
            VERSION_SELECTOR));

        //using bslx::VersionFunctions::maxSupportedBdexVersion;
        //EXPECT_TRUE(1 == maxSupportedBdexVersion(
        //    reinterpret_cast<Obj *>(0), 0));
        //EXPECT_TRUE(1 == maxSupportedBdexVersion(
        //    reinterpret_cast<Obj *>(0), VERSION_SELECTOR));
    }

    // Direct initial trial of 'bdexStreamOut' and (valid) 'bdexStreamIn'
    // functionality.
    const int VERSION = Obj::maxSupportedBdexVersion(0);
    {
        const Obj X(VC);
        Out out;// (VERSION_SELECTOR, &allocator);

        Out& rvOut = X.bdexStreamOut(out, VERSION);
        EXPECT_TRUE(&out == &rvOut);

        //const char *const OD = out.data();
        //const int         LOD = static_cast<int>(out.length());

        In in(out.str()); //OD, LOD);
        EXPECT_TRUE(in);
        //EXPECT_TRUE(!in.isEmpty());

        Obj mT(VA);  const Obj& T = mT;
        EXPECT_TRUE(X != T);

        In& rvIn = mT.bdexStreamIn(in, VERSION);
        EXPECT_TRUE(&in == &rvIn);
        EXPECT_TRUE(X == T);
        EXPECT_TRUE(in);
        //EXPECT_TRUE(in.isEmpty());
    }

#if 0
    // Testing valid streams
    {
        for (int i = 0; i < NUM_VALUES; ++i) {
            const Obj X(VALUES[i]);

            Out out(VERSION_SELECTOR, &allocator);

            using bslx::OutStreamFunctions::bdexStreamOut;
            using bslx::InStreamFunctions::bdexStreamIn;

            Out& rvOut = bdexStreamOut(out, X, VERSION);
            LOOP_ASSERT(i, &out == &rvOut);
            const char *const OD = out.data();
            const int         LOD = static_cast<int>(out.length());

            // Verify that each new value overwrites every old value and
            // that the input stream is emptied, but remains valid.

            for (int j = 0; j < NUM_VALUES; ++j) {
                In in(OD, LOD);
                LOOP2_ASSERT(i, j, in);
                LOOP2_ASSERT(i, j, !in.isEmpty());

                Obj mT(VALUES[j]);  const Obj& T = mT;
                LOOP2_ASSERT(i, j, (X == T) == (i == j));

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    In& rvIn = bdexStreamIn(in, mT, VERSION);
                    LOOP2_ASSERT(i, j, &in == &rvIn);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;

                LOOP2_ASSERT(i, j, X == T);
                LOOP2_ASSERT(i, j, in);
                LOOP2_ASSERT(i, j, in.isEmpty());
            }
        }
    }

    // Testing empty streams (valid and invalid).
    {
        Out               out(VERSION_SELECTOR, &allocator);
        const char *const OD = out.data();
        const int         LOD = static_cast<int>(out.length());
        EXPECT_TRUE(0 == LOD);

        for (int i = 0; i < NUM_VALUES; ++i) {
            In in(OD, LOD);
            LOOP_ASSERT(i, in);
            LOOP_ASSERT(i, in.isEmpty());

            // Ensure that reading from an empty or invalid input stream
            // leaves the stream invalid and the target object unchanged.

            using bslx::InStreamFunctions::bdexStreamIn;

            const Obj  X(VALUES[i]);
            Obj        mT(X);
            const Obj& T = mT;
            LOOP_ASSERT(i, X == T);

            BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                in.reset();

                // Stream is valid.
                In& rvIn1 = bdexStreamIn(in, mT, VERSION);
                LOOP_ASSERT(i, &in == &rvIn1);
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, X == T);

                // Stream is invalid.
                In& rvIn2 = bdexStreamIn(in, mT, VERSION);
                LOOP_ASSERT(i, &in == &rvIn2);
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, X == T);
            } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;
        }
    }

    // Testing non-empty, invalid streams.

    {
        Out out(VERSION_SELECTOR, &allocator);

        using bslx::OutStreamFunctions::bdexStreamOut;
        Out& rvOut = bdexStreamOut(out, Obj(), VERSION);
        EXPECT_TRUE(&out == &rvOut);

        const char *const OD = out.data();
        const int         LOD = static_cast<int>(out.length());
        EXPECT_TRUE(0 < LOD);

        for (int i = 0; i < NUM_VALUES; ++i) {
            In in(OD, LOD);
            in.invalidate();
            LOOP_ASSERT(i, !in);
            LOOP_ASSERT(i, !in.isEmpty());

            // Ensure that reading from a non-empty, invalid input stream
            // leaves the stream invalid and the target object unchanged.

            using bslx::InStreamFunctions::bdexStreamIn;

            const Obj  X(VALUES[i]);
            Obj        mT(X);
            const Obj& T = mT;
            LOOP_ASSERT(i, X == T);

            BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                in.reset();
                in.invalidate();
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, !in.isEmpty());

                In& rvIn = bdexStreamIn(in, mT, VERSION);
                LOOP_ASSERT(i, &in == &rvIn);
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, X == T);
            } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;
        }
    }

    // Test wire-format
    {
        for (int i = 0; i < NUM_VALUES; ++i) {
            const Obj X(VALUES[i]);
            bslx::ByteOutStream out(VERSION_SELECTOR, &allocator);

            using bslx::OutStreamFunctions::bdexStreamOut;
            using bslx::InStreamFunctions::bdexStreamIn;

            bdexStreamOut(out, X, VERSION);
            const char *const OD = out.data();
            const int         LOD = static_cast<int>(out.length());

            bdldfp::BinaryIntegralDecimalImpUtil::StorageType64 bidVal =
                bdldfp::DecimalImpUtil::convertToBID(VALUES[i].value());
            bsls::Types::Uint64 expectedValue = BSLS_BYTEORDER_HTONLL(
                bidVal.d_raw);

            std::cout << LOD << std::endl;
            ASSERTV(i, X, LOD == 8);
            EXPECT_TRUE(memcmp(OD, &expectedValue, 8) == 0);
        }
    }
#endif
}
#endif

TEST(Decimal, testCase5)
{
    // ------------------------------------------------------------------------
    // TESTING PRINT METHOD
    //
    // Concerns:
    //: 1 The 'print' method writes the value to the specified 'ostream'.
    //:
    //: 2 The 'print' method write the value in the intended format.
    //:
    //: 3 The 'print' method uses the same underlying formatting function as
    //:   'operator<<'.
    //:
    //: 4 The 'print' method's signature and return type are standard.
    //:
    //: 5 The 'print' method returns the supplied 'ostream'.
    //:
    //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
    //:   correct default values (0 and 4, respectively).
    //
    // Testing:
    //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
    // ------------------------------------------------------------------------

    if (verbose) std::cout << "\nTESTING PRINT METHOD"
        << "\n===================="
        << std::endl;

    // Verify that the signatures and return types are standard.
    {
        typedef std::ostream& (BDEC::Decimal64::*funcPtr)(std::ostream&,
            int, int) const;

        funcPtr     printMember = &BDEC::Decimal64::print;

        (void)printMember;
    }

#define DFP(X) BDLDFP_DECIMAL_DD(X)
    static const struct {
        int              d_line;
        BDEC::Decimal64  d_decimalValue;
        int              d_level;
        int              d_spacesPerLevel;
        const char      *d_expected_p;
    } DATA[] = {

        // -8 implies using default values

        //LINE  NUMBER     LEVEL  SPL  EXPECTED
        //----  ---------  -----  ---  --------
        { L_,  DFP(4.25),     0,   0,  "4.25\n" },
        { L_,  DFP(4.25),     0,   1,  "4.25\n" },
        { L_,  DFP(4.25),     0,  -1,  "4.25"   },
        { L_,  DFP(4.25),     0,  -8,  "4.25\n" },

        { L_,  DFP(4.25),     3,   0,  "4.25\n" },
        { L_,  DFP(4.25),     3,   2,  "      4.25\n" },
        { L_,  DFP(4.25),     3,  -2,  "      4.25" },
        { L_,  DFP(4.25),     3,  -8,  "            4.25\n" },
        { L_,  DFP(4.25),    -3,   0,  "4.25\n" },
        { L_,  DFP(4.25),    -3,   2,  "4.25\n" },
        { L_,  DFP(4.25),    -3,  -2,  "4.25" },
        { L_,  DFP(4.25),    -3,  -8,  "4.25\n" },

        { L_,  DFP(4.25),    -8,  -8,  "4.25\n" },
    };

#undef DFP

    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int             LINE = DATA[ti].d_line;
        const BDEC::Decimal64 VALUE = DATA[ti].d_decimalValue;
        const int             LEVEL = DATA[ti].d_level;
        const int             SPL = DATA[ti].d_spacesPerLevel;
        const char*           EXPECTED = DATA[ti].d_expected_p;

        std::stringstream outdec;
        if (-8 != SPL) {
            VALUE.print(outdec, LEVEL, SPL);
        }
        else if (-8 != LEVEL) {
            VALUE.print(outdec, LEVEL);
        }
        else {
            VALUE.print(outdec);
        }
        std::string ACTUAL = outdec.str();

        EXPECT_TRUE(ACTUAL == EXPECTED);
    }
}

TEST(Decimal, testCase4)
{
    // ------------------------------------------------------------------------
    // TESTING IOSTREAM OPERATORS
    //
    // Concerns:
    //: 1 Calling 'operator<<' on a 'Decimal32', 'Decimal64', 'Decimal128' type
    //:   renders the decimals value to the appropriate stream.
    //:
    //: 2 That 'operator<<' renders a simple decimal value in a fixed point
    //:   format using the correct digits of precisions.
    //:
    //: 3 That 'operator<<' correctly renders infinity, and negative infinity.
    //:
    //: 4 That 'operator<<' correctly renders NaN.
    //:
    //: 5 That 'operator<<' correctly handles a set width.
    //:
    //: 6 That 'operator<<' correctly handles a set width with either a left,
    //:   internal, or right justification.
    //:
    //: 7 That 'operator<<' correctly handles 'std::uppercase'.
    //:
    //: 8 That the 'print' method writes the value to the specified 'ostream'.
    //:
    //: 9 That the 'print' method write the vlaue in the intended format.
    //:
    //:10 That the output using 's << obj' is the same as
    //:   'obj.print(s, 0, -1)'.
    //:
    //:11 That 'operator<<' sets the fail and errors bit if the memory buffer
    //:   in the supplied output stream is not large enough.
    //
    // Note that this is not (yet) a complete set of concerns (or test) for
    // this function.
    //
    // Plan:
    //  1 Create a test table, where each element contains a decimal value, a
    //    set of formatting flags, and an expected output.  Iterate over the
    //    test table for 'Decimal32', 'Decimal64', and 'Decimal128' types, and
    //    ensure the streamed output matches the expected value.
    //
    //  2 Stream out a value to an output stream with a fixed memory buffer
    //    that is not large enough.  Make sure that the bad and fail bits are
    //    set in the output stream.
    //
    // Testing:
    //   std::basic_ostream& operator<<(std::basic_ostream& , Decimal32 );
    //   std::basic_ostream& operator<<(std::basic_ostream& , Decimal64 );
    //   std::basic_ostream& operator<<(std::basic_ostream& , Decimal64 );
    // ------------------------------------------------------------------------

    if (verbose) std::cout << "\nTESTING IOSTREAM OPERATORS"
        << "\n=========================="
        << std::endl;

    // Note that this test is not yet complete.  Possible improvements:
    //
    //: o Providing expected values more precisely (significant, exponent)
    //:   rather than using 'BDLDFP_DECIMAL_DF'
    //:
    //: o Testing a wider array of numbers, including values rendered in
    //:   scientific notation.

#define DFP(X) BDLDFP_DECIMAL_DF(X)

    BDEC::Decimal32 INF_P = BDEC::Decimal32(
        std::numeric_limits<double>::infinity());
    BDEC::Decimal32 INF_N = BDEC::Decimal32(
        -std::numeric_limits<double>::infinity());
    BDEC::Decimal32 NAN_Q = BDEC::Decimal32(
        std::numeric_limits<double>::quiet_NaN());
    static const struct {
        int              d_line;
        BDEC::Decimal32  d_decimalValue;
        int              d_width;
        char             d_justification;
        bool             d_capital;
        const char      *d_expected;
    } DATA[] = {
        // L   NUMBER    WIDTH   JUST  CAPITAL      EXPECTED
        // --- ------    -----   ----  -------      --------
        {  L_, DFP(4.25),  0,     'l', false,         "4.25" },
        {  L_, DFP(4.25),  1,     'l', false,         "4.25" },
        {  L_, DFP(4.25),  2,     'l', false,         "4.25" },
        {  L_, DFP(4.25),  3,     'l', false,         "4.25" },
        {  L_, DFP(4.25),  4,     'l', false,         "4.25" },
        {  L_, DFP(4.25),  5,     'l', false,        "4.25 " },
        {  L_, DFP(4.25),  6,     'l', false,       "4.25  " },
        {  L_, DFP(4.25),  7,     'l', false,      "4.25   " },
        {  L_, DFP(4.25),  8,     'l', false,     "4.25    " },
        {  L_, DFP(4.25),  9,     'l', false,    "4.25     " },

        {  L_, DFP(4.25),  0,     'i', false,         "4.25" },
        {  L_, DFP(4.25),  1,     'i', false,         "4.25" },
        {  L_, DFP(4.25),  2,     'i', false,         "4.25" },
        {  L_, DFP(4.25),  3,     'i', false,         "4.25" },
        {  L_, DFP(4.25),  4,     'i', false,         "4.25" },
        {  L_, DFP(4.25),  5,     'i', false,        " 4.25" },
        {  L_, DFP(4.25),  6,     'i', false,       "  4.25" },
        {  L_, DFP(4.25),  7,     'i', false,      "   4.25" },
        {  L_, DFP(4.25),  8,     'i', false,     "    4.25" },
        {  L_, DFP(4.25),  9,     'i', false,    "     4.25" },

        {  L_, DFP(4.25),  0,     'r', false,         "4.25" },
        {  L_, DFP(4.25),  1,     'r', false,         "4.25" },
        {  L_, DFP(4.25),  2,     'r', false,         "4.25" },
        {  L_, DFP(4.25),  3,     'r', false,         "4.25" },
        {  L_, DFP(4.25),  4,     'r', false,         "4.25" },
        {  L_, DFP(4.25),  5,     'r', false,        " 4.25" },
        {  L_, DFP(4.25),  6,     'r', false,       "  4.25" },
        {  L_, DFP(4.25),  7,     'r', false,      "   4.25" },
        {  L_, DFP(4.25),  8,     'r', false,     "    4.25" },
        {  L_, DFP(4.25),  9,     'r', false,    "     4.25" },

        {  L_, DFP(-4.25), 0,     'l', false,        "-4.25" },
        {  L_, DFP(-4.25), 1,     'l', false,        "-4.25" },
        {  L_, DFP(-4.25), 2,     'l', false,        "-4.25" },
        {  L_, DFP(-4.25), 3,     'l', false,        "-4.25" },
        {  L_, DFP(-4.25), 4,     'l', false,        "-4.25" },
        {  L_, DFP(-4.25), 5,     'l', false,        "-4.25" },
        {  L_, DFP(-4.25), 6,     'l', false,       "-4.25 " },
        {  L_, DFP(-4.25), 7,     'l', false,      "-4.25  " },
        {  L_, DFP(-4.25), 8,     'l', false,     "-4.25   " },
        {  L_, DFP(-4.25), 9,     'l', false,    "-4.25    " },

        {  L_, DFP(-4.25), 0,     'i', false,        "-4.25" },
        {  L_, DFP(-4.25), 1,     'i', false,        "-4.25" },
        {  L_, DFP(-4.25), 2,     'i', false,        "-4.25" },
        {  L_, DFP(-4.25), 3,     'i', false,        "-4.25" },
        {  L_, DFP(-4.25), 4,     'i', false,        "-4.25" },
        {  L_, DFP(-4.25), 5,     'i', false,        "-4.25" },
        {  L_, DFP(-4.25), 6,     'i', false,       "- 4.25" },
        {  L_, DFP(-4.25), 7,     'i', false,      "-  4.25" },
        {  L_, DFP(-4.25), 8,     'i', false,     "-   4.25" },
        {  L_, DFP(-4.25), 9,     'i', false,    "-    4.25" },

        {  L_, DFP(-4.25), 0,     'r', false,        "-4.25" },
        {  L_, DFP(-4.25), 1,     'r', false,        "-4.25" },
        {  L_, DFP(-4.25), 2,     'r', false,        "-4.25" },
        {  L_, DFP(-4.25), 3,     'r', false,        "-4.25" },
        {  L_, DFP(-4.25), 4,     'r', false,        "-4.25" },
        {  L_, DFP(-4.25), 5,     'r', false,        "-4.25" },
        {  L_, DFP(-4.25), 6,     'r', false,       " -4.25" },
        {  L_, DFP(-4.25), 7,     'r', false,      "  -4.25" },
        {  L_, DFP(-4.25), 8,     'r', false,     "   -4.25" },
        {  L_, DFP(-4.25), 9,     'r', false,    "    -4.25" },

        {  L_, INF_P,      0,     'l', false,     "infinity" },
        {  L_, INF_P,      1,     'l', false,     "infinity" },
        {  L_, INF_P,      2,     'l', false,     "infinity" },
        {  L_, INF_P,      3,     'l', false,     "infinity" },
        {  L_, INF_P,      4,     'l', false,     "infinity" },
        {  L_, INF_P,      5,     'l', false,     "infinity" },
        {  L_, INF_P,      6,     'l', false,     "infinity" },
        {  L_, INF_P,      7,     'l', false,     "infinity" },
        {  L_, INF_P,      8,     'l', false,     "infinity" },
        {  L_, INF_P,      9,     'l', false,    "infinity " },
        {  L_, INF_P,     10,     'l', false,   "infinity  " },
        {  L_, INF_P,      0,     'l', true,      "INFINITY" },

        {  L_, INF_N,      0,     'l', false,    "-infinity" },
        {  L_, INF_N,      1,     'l', false,    "-infinity" },
        {  L_, INF_N,      2,     'l', false,    "-infinity" },
        {  L_, INF_N,      3,     'l', false,    "-infinity" },
        {  L_, INF_N,      4,     'l', false,    "-infinity" },
        {  L_, INF_N,      5,     'l', false,    "-infinity" },
        {  L_, INF_N,      6,     'l', false,    "-infinity" },
        {  L_, INF_N,      7,     'l', false,    "-infinity" },
        {  L_, INF_N,      8,     'l', false,    "-infinity" },
        {  L_, INF_N,      9,     'l', false,    "-infinity" },
        {  L_, INF_N,     10,     'l', false,   "-infinity " },
        {  L_, INF_N,      0,     'l', true,     "-INFINITY" },


        {  L_, INF_N,      0,     'i', false,    "-infinity" },
        {  L_, INF_N,      1,     'i', false,    "-infinity" },
        {  L_, INF_N,      2,     'i', false,    "-infinity" },
        {  L_, INF_N,      3,     'i', false,    "-infinity" },
        {  L_, INF_N,      4,     'i', false,    "-infinity" },
        {  L_, INF_N,      5,     'i', false,    "-infinity" },
        {  L_, INF_N,      6,     'i', false,    "-infinity" },
        {  L_, INF_N,      7,     'i', false,    "-infinity" },
        {  L_, INF_N,      8,     'i', false,    "-infinity" },
        {  L_, INF_N,      9,     'i', false,    "-infinity" },
        {  L_, INF_N,     10,     'i', false,   "- infinity" },

        {  L_, INF_N,      0,     'r', false,    "-infinity" },
        {  L_, INF_N,      1,     'r', false,    "-infinity" },
        {  L_, INF_N,      2,     'r', false,    "-infinity" },
        {  L_, INF_N,      3,     'r', false,    "-infinity" },
        {  L_, INF_N,      4,     'r', false,    "-infinity" },
        {  L_, INF_N,      5,     'r', false,    "-infinity" },
        {  L_, INF_N,      6,     'r', false,    "-infinity" },
        {  L_, INF_N,      7,     'r', false,    "-infinity" },
        {  L_, INF_N,      8,     'r', false,    "-infinity" },
        {  L_, INF_N,      9,     'r', false,    "-infinity" },
        {  L_, INF_N,     10,     'r', false,   " -infinity" },

        {  L_, NAN_Q,      0,     'l', false,         "nan" },
        {  L_, NAN_Q,      1,     'l', false,         "nan" },
        {  L_, NAN_Q,      2,     'l', false,         "nan" },
        {  L_, NAN_Q,      3,     'l', false,         "nan" },
        {  L_, NAN_Q,      4,     'l', false,         "nan " },

        {  L_, NAN_Q,      0,     'i', false,         "nan" },
        {  L_, NAN_Q,      1,     'i', false,         "nan" },
        {  L_, NAN_Q,      2,     'i', false,         "nan" },
        {  L_, NAN_Q,      3,     'i', false,         "nan" },
        {  L_, NAN_Q,      4,     'i', false,         " nan" },

        {  L_, NAN_Q,      0,     'r', false,         "nan" },
        {  L_, NAN_Q,      1,     'r', false,         "nan" },
        {  L_, NAN_Q,      2,     'r', false,         "nan" },
        {  L_, NAN_Q,      3,     'r', false,         "nan" },
        {  L_, NAN_Q,      4,     'r', false,         " nan" },

        {  L_, NAN_Q,      0,     'i', true,          "NAN" },
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int             LINE = DATA[ti].d_line;
        const BDEC::Decimal32 DECIMAL32 = DATA[ti].d_decimalValue;
        const int             WIDTH = DATA[ti].d_width;
        const bool            LEFT = (DATA[ti].d_justification == 'l');
        const bool            INTERNAL = (DATA[ti].d_justification == 'i');
        const bool            RIGHT = (DATA[ti].d_justification == 'r');
        const bool            CAPITAL = DATA[ti].d_capital;
        const char           *EXPECTED = DATA[ti].d_expected;

        if (veryVerbose) {
            P_(LINE); P_(EXPECTED); P(DECIMAL32);
        }

        // Test with Decimal32.
        {
            const BDEC::Decimal32 VALUE(DECIMAL32);

            std::stringstream outdec;

            outdec.width(WIDTH);
            if (LEFT) { outdec << std::left; }
            if (INTERNAL) { outdec << std::internal; }
            if (RIGHT) { outdec << std::right; }
            if (CAPITAL) { outdec << std::uppercase; }
            else { outdec << std::nouppercase; }
            outdec << VALUE;

            std::string ACTUAL = outdec.str();

            EXPECT_TRUE(ACTUAL == EXPECTED);
            EXPECT_TRUE(outdec.good());
        }

        // Test with Decimal64.
        {
            const BDEC::Decimal64 VALUE(DECIMAL32);

            std::stringstream outdec;

            outdec.width(WIDTH);
            if (LEFT) { outdec << std::left; }
            if (INTERNAL) { outdec << std::internal; }
            if (RIGHT) { outdec << std::right; }
            if (CAPITAL) { outdec << std::uppercase; }
            else { outdec << std::nouppercase; }
            outdec << VALUE;

            std::string ACTUAL = outdec.str();

            EXPECT_TRUE(ACTUAL == EXPECTED);
            EXPECT_TRUE(outdec.good());
        }

        // Test with print
        {
            const BDEC::Decimal64 VALUE(DECIMAL32);

            std::stringstream outdec;

            outdec.width(WIDTH);
            if (LEFT) { outdec << std::left; }
            if (INTERNAL) { outdec << std::internal; }
            if (RIGHT) { outdec << std::right; }
            if (CAPITAL) { outdec << std::uppercase; }
            else { outdec << std::nouppercase; }
            VALUE.print(outdec, 0, -1);

            std::string ACTUAL = outdec.str();

            EXPECT_TRUE(ACTUAL == EXPECTED);
        }

        // Test with Decimal128.
        {
            const BDEC::Decimal128 VALUE(DECIMAL32);

            std::stringstream outdec;

            outdec.width(WIDTH);
            if (LEFT) { outdec << std::left; }
            if (INTERNAL) { outdec << std::internal; }
            if (RIGHT) { outdec << std::right; }
            if (CAPITAL) { outdec << std::uppercase; }
            else { outdec << std::nouppercase; }
            outdec << VALUE;

            std::string ACTUAL = outdec.str();

            EXPECT_TRUE(ACTUAL == EXPECTED);
            EXPECT_TRUE(outdec.good());
        }
    }

    // Bug in Studio Studio's C++ standard library: 'ostreambuf_iterator'
    // doesn't set the 'failed' attribute when the iterator reaches the end of
    // EOF of the 'streambuf'.  Therefore, 'operator<<' for the decimal types
    // does not set the 'fail' and 'bad' bits when streaming to an 'ostream'
    // with a 'streambuf' that is not large enough.  This is consistent with
    // the behavior for 'int' and 'double'.  Note that the bug does not exist
    // when using stlport.

#if 0

#if (!defined(BSLS_PLATFORM_OS_SUNOS) &&                                      \
     !defined(BSLS_PLATFORM_OS_SOLARIS)) ||                                   \
    defined(BDE_BUILD_TARGET_STLPORT)

    {
        {
            char buffer[4];
            bdlsb::FixedMemOutStreamBuf obuf(buffer, 4);
            std::ostream out(&obuf);
            BDEC::Decimal64 value = DFP(-1.0);
            out << value;
            EXPECT_TRUE(!out.fail());
            EXPECT_TRUE(!out.bad());
        }
        {
            char buffer[4];
            bdlsb::FixedMemOutStreamBuf obuf(buffer, 3);
            std::ostream out(&obuf);
            BDEC::Decimal32 value = DFP(-1.0);
            out << value;
            EXPECT_TRUE(out.fail());
            EXPECT_TRUE(out.bad());
        }
        {
            char buffer[4];
            bdlsb::FixedMemOutStreamBuf obuf(buffer, 3);
            std::ostream out(&obuf);
            BDEC::Decimal64 value = DFP(-1.0);
            out << value;
            EXPECT_TRUE(out.fail());
            EXPECT_TRUE(out.bad());
        }
        {
            char buffer[4];
            bdlsb::FixedMemOutStreamBuf obuf(buffer, 3);
            std::ostream out(&obuf);
            BDEC::Decimal128 value = DFP(-1.0);
            out << value;
            EXPECT_TRUE(out.fail());
            EXPECT_TRUE(out.bad());
        }
    }
#endif

#endif

#undef DFP
}

TEST(Decimal, testCase3)
{
    // ------------------------------------------------------------------------
    // TESTING 'Decimal128'
    //   Ensure that 'Decimal128' functions correctly
    //
    // Concerns:
    //: 1 'Decimal128' operations are properly forwarded to implementation
    //: 2 All possible operand combinations work correctly.
    //
    // Plan:
    //: 1 Expected value testing will be used.
    //:
    //: 2 All forms of LHS and RHS combinations for all free operators will
    //:   be tested.
    //:
    //: 3 Every member operator will be tested
    //
    // Testing:
    //   'class Decimal128'
    // ------------------------------------------------------------------------

    if (verbose) std::cout << "\nTesting class Decimal128"
        << "\n========================" << std::endl;

    if (veryVerbose) std::cout << "Constructors" << std::endl;

    if (veryVeryVerbose) std::cout << "Copy/convert" << std::endl;
    {
        const BDEC::Decimal32  c32 = BDEC::Decimal32(32);
        const BDEC::Decimal64  c64 = BDEC::Decimal64(64);
        const BDEC::Decimal128 c128 = BDEC::Decimal128(128);

        EXPECT_TRUE(BDLDFP_DECIMAL_DL(32.0) == BDEC::Decimal128(c32));
        EXPECT_TRUE(BDLDFP_DECIMAL_DL(64.0) == BDEC::Decimal128(c64));
        EXPECT_TRUE(BDLDFP_DECIMAL_DL(128.0) == BDEC::Decimal128(c128));
    }

    if (veryVeryVerbose) std::cout << "Integral" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(0.0) == BDEC::Decimal128());  // default
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-42.0) == BDEC::Decimal128(-42)); // int
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(42.0) == BDEC::Decimal128(42u)); // unsigned
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-42.0) == BDEC::Decimal128(-42l)); // long
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(42.0) == BDEC::Decimal128(42ul)); // ulong
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-42.0) == BDEC::Decimal128(-42ll)); //longlong
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(42.0) == BDEC::Decimal128(42ull)); // ulongl

    BDEC::Decimal128 cDefault;
    BDEC::Decimal128 cExpectedDefault = BDLDFP_DECIMAL_DL(0e-6176);
    EXPECT_TRUE(0 == memcmp(&cDefault, &cExpectedDefault, sizeof(cDefault)));

    BDEC::Decimal128 cZero(0);
    BDEC::Decimal128 cExpectedZero = BDLDFP_DECIMAL_DL(0e0);
    EXPECT_TRUE(0 == memcmp(&cZero, &cExpectedZero, sizeof(cZero)));

    EXPECT_TRUE(0 != memcmp(&cDefault, &cZero, sizeof(cDefault)));

    if (veryVeryVerbose) std::cout << "Binary FP" << std::endl;

    // Note that to test binary-float taking constructors I use numbers
    // that can be represented exactly in both binary and decimal FP.

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(4.25) == BDEC::Decimal128(4.25f)); // float
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(4.25) == BDEC::Decimal128(4.25)); // double

    if (veryVeryVerbose) std::cout << "Decimal FP" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-42.0) == BDLDFP_DECIMAL_DF(-42.0));
    // Decimal32
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(42.0) == BDLDFP_DECIMAL_DF(42.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(4.2) == BDLDFP_DECIMAL_DF(4.2));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(4.2e9) == BDLDFP_DECIMAL_DF(4.2e9));

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-42.0) == BDLDFP_DECIMAL_DD(-42.0));// Dec64
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(42.0) == BDLDFP_DECIMAL_DD(42.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(4.2) == BDLDFP_DECIMAL_DD(4.2));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(4.2e9) == BDLDFP_DECIMAL_DD(4.2e9));

    if (veryVerbose) std::cout << "Propriatery accessors" << std::endl;
    {

        BDEC::Decimal128 d128(42);
        EXPECT_TRUE((void*)d128.data() == (void*)&d128);

        // XLC versions prior to 12.0 incorrectly pass decimal128 values in
        // some contexts (0x0c00 -> 12.00)
#if defined(BSLS_PLATFORM_CMP_IBM) && (BSLS_PLATFORM_CMP_VERSION >= 0x0c00)
        ASSERTV(BDEC::Decimal128(d128.value()) == BDEC::Decimal128(42));
#endif
    }

    if (veryVerbose) std::cout << "Operator==" << std::endl;

    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DL(4.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DD(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DD(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DF(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DF(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    if (veryVerbose) std::cout << "Operator!=" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(4.0) != BDLDFP_DECIMAL_DL(5.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(7.0) != BDLDFP_DECIMAL_DL(5.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(-9.345e27) !=
        BDLDFP_DECIMAL_DL(-9.345e27)));

    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DD(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DD(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(4.0) == BDLDFP_DECIMAL_DF(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-9.345e27) == BDLDFP_DECIMAL_DF(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(4.0) == BDLDFP_DECIMAL_DL(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-9.345e27) == BDLDFP_DECIMAL_DL(-9.345e27));

    if (veryVerbose) std::cout << "Operator<" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(4.0) < BDLDFP_DECIMAL_DL(5.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(-9.345e27) <
        BDLDFP_DECIMAL_DL(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(5.0) < BDLDFP_DECIMAL_DL(4.0)));

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(4.0) < BDLDFP_DECIMAL_DD(5.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(-9.345e27) <
        BDLDFP_DECIMAL_DD(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(5.0) < BDLDFP_DECIMAL_DD(4.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(4.0) < BDLDFP_DECIMAL_DL(5.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(-9.345e27) <
        BDLDFP_DECIMAL_DL(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(5.0) < BDLDFP_DECIMAL_DL(4.0)));

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(4.0) < BDLDFP_DECIMAL_DF(5.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(-9.345e27) <
        BDLDFP_DECIMAL_DF(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(5.0) < BDLDFP_DECIMAL_DF(4.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(4.0) < BDLDFP_DECIMAL_DL(5.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(-9.345e27) <
        BDLDFP_DECIMAL_DL(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(5.0) < BDLDFP_DECIMAL_DL(4.0)));

    if (veryVerbose) std::cout << "Operator>" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(5.0) > BDLDFP_DECIMAL_DL(4.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(-9.345e27) >
        BDLDFP_DECIMAL_DL(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(4.0) > BDLDFP_DECIMAL_DL(5.0)));

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(5.0) > BDLDFP_DECIMAL_DD(4.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(-9.345e27) >
        BDLDFP_DECIMAL_DD(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(4.0) > BDLDFP_DECIMAL_DD(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(5.0) > BDLDFP_DECIMAL_DL(4.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(-9.345e27) >
        BDLDFP_DECIMAL_DL(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(4.0) > BDLDFP_DECIMAL_DL(5.0)));

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(5.0) > BDLDFP_DECIMAL_DF(4.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(-9.345e27) >
        BDLDFP_DECIMAL_DF(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(4.0) > BDLDFP_DECIMAL_DF(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(5.0) > BDLDFP_DECIMAL_DL(4.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(-9.345e27) >
        BDLDFP_DECIMAL_DL(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(4.0) > BDLDFP_DECIMAL_DL(5.0)));

    if (veryVerbose) std::cout << "Operator<=" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(4.0) <= BDLDFP_DECIMAL_DL(5.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-9.345e27) <= BDLDFP_DECIMAL_DL(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(5.0) <= BDLDFP_DECIMAL_DL(4.0)));

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(4.0) <= BDLDFP_DECIMAL_DD(5.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-9.345e27) <= BDLDFP_DECIMAL_DD(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(5.0) <= BDLDFP_DECIMAL_DD(4.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(4.0) <= BDLDFP_DECIMAL_DL(5.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-9.345e27) <= BDLDFP_DECIMAL_DL(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(5.0) <= BDLDFP_DECIMAL_DL(4.0)));

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(4.0) <= BDLDFP_DECIMAL_DF(5.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-9.345e27) <= BDLDFP_DECIMAL_DF(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(5.0) <= BDLDFP_DECIMAL_DF(4.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(4.0) <= BDLDFP_DECIMAL_DL(5.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-9.345e27) <= BDLDFP_DECIMAL_DL(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(5.0) <= BDLDFP_DECIMAL_DL(4.0)));

    if (veryVerbose) std::cout << "Operator>=" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(5.0) >= BDLDFP_DECIMAL_DL(4.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-9.345e27) >= BDLDFP_DECIMAL_DL(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(4.0) >= BDLDFP_DECIMAL_DL(5.0)));

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(5.0) >= BDLDFP_DECIMAL_DD(4.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-9.345e27) >= BDLDFP_DECIMAL_DD(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(4.0) >= BDLDFP_DECIMAL_DD(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(5.0) >= BDLDFP_DECIMAL_DL(4.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-9.345e27) >= BDLDFP_DECIMAL_DL(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(4.0) >= BDLDFP_DECIMAL_DL(5.0)));

    EXPECT_TRUE(BDLDFP_DECIMAL_DL(5.0) >= BDLDFP_DECIMAL_DF(4.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DL(-9.345e27) >= BDLDFP_DECIMAL_DF(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DL(4.0) >= BDLDFP_DECIMAL_DF(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(5.0) >= BDLDFP_DECIMAL_DL(4.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-9.345e27) >= BDLDFP_DECIMAL_DL(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(4.0) >= BDLDFP_DECIMAL_DL(5.0)));

    // TODO TBD: Note that I am testing with the maximum number of
    // significant digits and I actually expect stream output to do no
    // rounding.  That is wrong (see later why), but necessary to
    // remote-test the 'format' function and make sure it does not lose
    // digits or does some unwanted rounding.  When 'format' will be in the
    // lower utility, it will be tested directly.
    //
    // Expecting to see all digits is wrong because that is not how the
    // stream output should behave: it should print with the default
    // precision as it would for binary floating point *or* with the
    // implied precision of the cohort if that is larger.  AFAIU

    if (veryVerbose) std::cout << "Test stream out" << std::endl;
    {
        std::ostringstream  out;// (void);

        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(
            -1.234567890123456789012345678901234e-24));
        out << d1;
        std::string s;// (void);
        getStringFromStream(out, &s);
        EXPECT_TRUE(
            decLower(s) == "-1.234567890123456789012345678901234e-24");
    }

    if (veryVerbose) std::cout << "Test stream in" << std::endl;
    {
        std::istringstream  in;// (void);
        std::string ins("-1.234567890123456789012345678901234e-24");// , void);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        EXPECT_TRUE(d1 ==
            BDLDFP_DECIMAL_DL(-1.234567890123456789012345678901234e-24));
    }

    // bdldfp does not know how to parse128("NaN") etc.
    if (veryVerbose) std::cout << "Test stream in NaN" << std::endl;
    {
        std::istringstream  in;// (void);
        std::string ins("NaN");// , void);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        EXPECT_TRUE(d1 != d1);
    }
    {
        std::istringstream  in;// (void);
        std::string ins("nan");// , void);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        EXPECT_TRUE(d1 != d1);
    }

    if (veryVerbose) std::cout << "Test stream in Inf" << std::endl;
    {
        std::istringstream  in;// (void);
        std::string ins("Inf");// , void);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        EXPECT_TRUE(d1 > std::numeric_limits<BDEC::Decimal128>::max());
    }
    {
        std::istringstream  in;// (void);
        std::string ins("inf");// , void);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        EXPECT_TRUE(d1 > std::numeric_limits<BDEC::Decimal128>::max());
    }

    if (veryVerbose) std::cout << "Test stream in -Inf" << std::endl;
    {
        std::istringstream  in;// (void);
        std::string ins("-Inf");// , void);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        EXPECT_TRUE(d1 < -std::numeric_limits<BDEC::Decimal128>::max());
    }
    {
        std::istringstream  in;// (void);
        std::string ins("-inf");// , void);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        EXPECT_TRUE(d1 < -std::numeric_limits<BDEC::Decimal128>::max());
    }

    if (veryVerbose) std::cout << "Test stream in NaNa (bad)" << std::endl;
    {
        std::istringstream  in;// (void);
        std::string ins("NaNa");// , void);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        EXPECT_TRUE(in.fail() == true);
    }

    if (veryVerbose) std::cout << "Test stream in Infinity" << std::endl;
    {
        std::istringstream  in;// (void);
        std::string ins("Infinity");// , void);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        EXPECT_TRUE(d1 > std::numeric_limits<BDEC::Decimal128>::max());
    }

    if (veryVerbose) std::cout << "Test stream in -Infinity" << std::endl;
    {
        std::istringstream  in;// (void);
        std::string ins("-Infinity");// , void);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        EXPECT_TRUE(d1 < -std::numeric_limits<BDEC::Decimal128>::max());
    }

    if (veryVerbose) std::cout << "Test stream in Infin (bad)" << std::endl;
    {
        std::istringstream  in;// (void);
        std::string ins("-Infin");//, void);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        EXPECT_TRUE(in.fail() == true);
    }

    if (veryVerbose) std::cout << "Test wide stream out" << std::endl;
    {
        std::wostringstream  out;// (void);
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(
            -1.234567890123456789012345678901234e-24));
        out << d1;
        std::wstring s;// (void);
        getStringFromStream(out, &s);
        EXPECT_TRUE(decLower(s) == L"-1.234567890123456789012345678901234e-24");
    }

    if (veryVerbose) std::cout << "Test wide stream in" << std::endl;
    {
        std::wistringstream  in;// (void);
        std::wstring ins(L"-1.234567890123456789012345678901234e-24");//, void);
        in.str(ins);

        BDEC::Decimal128 d1;
        in >> d1;
        EXPECT_TRUE(d1 ==
            BDLDFP_DECIMAL_DL(-1.234567890123456789012345678901234e-24));
    }

    if (veryVerbose) std::cout << "Operator++" << std::endl;
    {
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(-5.0));
        BDEC::Decimal128 d2 = d1++;
        EXPECT_TRUE(BDLDFP_DECIMAL_DL(-5.0) == d2);
        LOOP_ASSERT(d1, BDLDFP_DECIMAL_DL(-4.0) == d1);
        BDEC::Decimal128 d3 = ++d2;
        LOOP_ASSERT(d2, BDLDFP_DECIMAL_DL(-4.0) == d2);
        LOOP_ASSERT(d3, BDLDFP_DECIMAL_DL(-4.0) == d3);
    }

    if (veryVerbose) std::cout << "Operator--" << std::endl;
    {
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(-5.0));
        BDEC::Decimal128 d2 = d1--;
        EXPECT_TRUE(BDLDFP_DECIMAL_DL(-5.0) == d2);
        LOOP_ASSERT(d1, BDLDFP_DECIMAL_DL(-6.0) == d1);
        BDEC::Decimal128 d3 = --d2;
        LOOP_ASSERT(d2, BDLDFP_DECIMAL_DL(-6.0) == d2);
        LOOP_ASSERT(d3, BDLDFP_DECIMAL_DL(-6.0) == d3);
    }

    if (veryVerbose) std::cout << "Unary-" << std::endl;
    {
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(-5.0));
        BDEC::Decimal128 d2(BDLDFP_DECIMAL_DL(5.0));
        EXPECT_TRUE(BDLDFP_DECIMAL_DL(5.0) == -d1);
        EXPECT_TRUE(BDLDFP_DECIMAL_DL(-5.0) == -d2);

        // Unary - must make +0 into -0

        std::ostringstream out;// (void);
        out << -BDLDFP_DECIMAL_DL(0.0);
        std::string s;// (void);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] == '-'); // it is negative
        EXPECT_TRUE(-BDLDFP_DECIMAL_DL(0.0) == BDEC::Decimal128(0)); // and 0
    }

    if (veryVerbose) std::cout << "Unary+" << std::endl;
    {
        BDEC::Decimal128 d1(BDLDFP_DECIMAL_DL(-5.0));
        BDEC::Decimal128 d2(BDLDFP_DECIMAL_DL(5.0));
        EXPECT_TRUE(BDLDFP_DECIMAL_DL(-5.0) == +d1);
        EXPECT_TRUE(BDLDFP_DECIMAL_DL(5.0) == +d2);

        // Unary + must make -0 into +0

        BDEC::Decimal128 negzero(-BDLDFP_DECIMAL_DL(0.0));
        std::ostringstream out;// (void);
        out << +negzero;
        std::string s;// (void);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] != '+'); // it is positive
        EXPECT_TRUE(-BDLDFP_DECIMAL_DL(0.0) == BDEC::Decimal128(0)); // and 0
    }


    if (veryVerbose) std::cout << "+=" << std::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) std::cout << "+=(int)" << std::endl;
        d += 1;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-4.0) == d);

        if (veryVeryVerbose) std::cout << "+=(unsigned int)" << std::endl;
        d += 2u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-2.0) == d);

        if (veryVeryVerbose) std::cout << "+=(long int)" << std::endl;
        d += -1l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-3.0) == d);

        if (veryVeryVerbose) std::cout << "+=(unsigned long int)"
            << std::endl;
        d += 5ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(2.0) == d);

        if (veryVeryVerbose) std::cout << "+=(long long int)" << std::endl;
        d += -2ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.0) == d);

        if (veryVeryVerbose) std::cout << "+=(unsigned long long int)"
            << std::endl;
        d += 42ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(42.0) == d);

        if (veryVeryVerbose) std::cout << "+=(Decimal32)" << std::endl;
        d += BDLDFP_DECIMAL_DF(-22.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(20.0) == d);

        if (veryVeryVerbose) std::cout << "+=(Decimal64)" << std::endl;
        d += BDLDFP_DECIMAL_DD(3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(23.0) == d);

        if (veryVeryVerbose) std::cout << "+=(Decimal128)" << std::endl;
        d += BDLDFP_DECIMAL_DL(9.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(32.0) == d);
    }

    if (veryVerbose) std::cout << "-=" << std::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) std::cout << "-=(int)" << std::endl;
        d -= 1;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-6.0) == d);

        if (veryVeryVerbose) std::cout << "-=(unsigned int)" << std::endl;
        d -= 2u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-8.0) == d);

        if (veryVeryVerbose) std::cout << "-=(long int)" << std::endl;
        d -= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(2.0) == d);

        if (veryVeryVerbose) std::cout << "-=(unsigned long int)"
            << std::endl;
        d -= 10ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-8.0) == d);

        if (veryVeryVerbose) std::cout << "-=(long long int)" << std::endl;
        d -= -8ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.0) == d);

        if (veryVeryVerbose) std::cout << "-=(unsigned long long int)"
            << std::endl;
        d -= 42ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-42.0) == d);

        if (veryVeryVerbose) std::cout << "-=(Decimal32)" << std::endl;
        d -= BDLDFP_DECIMAL_DF(-22.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-20.0) == d);

        if (veryVeryVerbose) std::cout << "-=(Decimal64)" << std::endl;
        d -= BDLDFP_DECIMAL_DD(3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-23.0) == d);

        if (veryVeryVerbose) std::cout << "-=(Decimal128)" << std::endl;
        d -= BDLDFP_DECIMAL_DL(9.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-32.0) == d);
    }

    if (veryVerbose) std::cout << "*=" << std::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) std::cout << "*=(int)" << std::endl;
        d *= -2;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(10.0) == d);

        if (veryVeryVerbose) std::cout << "*=(unsigned int)" << std::endl;
        d *= 2000u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(20000.0) == d);

        if (veryVeryVerbose) std::cout << "*=(long int)" << std::endl;
        d *= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-200000.0) == d);

        if (veryVeryVerbose) std::cout << "*=(unsigned long int)"
            << std::endl;
        d *= 3ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-600000.0) == d);

        if (veryVeryVerbose) std::cout << "*=(long long int)" << std::endl;
        d *= -1ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(600000.0) == d);

        if (veryVeryVerbose) std::cout << "*=(unsigned long long int)"
            << std::endl;
        d *= 5ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(3000000.0) == d);

        if (veryVeryVerbose) std::cout << "*=(Decimal32)" << std::endl;
        d *= BDLDFP_DECIMAL_DF(1e-5);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(30.0) == d);

        if (veryVeryVerbose) std::cout << "*=(Decimal64)" << std::endl;
        d *= BDLDFP_DECIMAL_DD(-3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-90.0) == d);

        if (veryVeryVerbose) std::cout << "*=(Decimal128)" << std::endl;
        d *= BDLDFP_DECIMAL_DL(2.4e-134);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-2.16e-132) == d);
    }

    if (veryVerbose) std::cout << "/=" << std::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) std::cout << "/=(int)" << std::endl;
        d /= -2;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(2.5) == d);

        if (veryVeryVerbose) std::cout << "/=(unsigned int)" << std::endl;
        d /= 2000u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.00125) == d);

        if (veryVeryVerbose) std::cout << "/=(long int)" << std::endl;
        d /= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-0.000125) == d);

        if (veryVeryVerbose) std::cout << "/=(unsigned long int)"
            << std::endl;
        d /= 5ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-0.000025) == d);

        if (veryVeryVerbose) std::cout << "/=(long long int)" << std::endl;
        d /= -5ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.000005) == d);

        if (veryVeryVerbose) std::cout << "/=(unsigned long long int)"
            << std::endl;
        d /= 5ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.000001) == d);

        if (veryVeryVerbose) std::cout << "/=(Decimal32)" << std::endl;
        d /= BDLDFP_DECIMAL_DF(1e-5);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(0.1) == d);

        if (veryVeryVerbose) std::cout << "/=(Decimal64)" << std::endl;
        d /= BDLDFP_DECIMAL_DD(-5.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(-0.02) == d);

        if (veryVeryVerbose) std::cout << "/=(Decimal128)" << std::endl;
        d /= BDLDFP_DECIMAL_DL(-2.5e-134);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DL(8.e131) == d);
    }

    if (veryVerbose) std::cout << "operator+" << std::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) std::cout << "dec + int" << std::endl;
        LOOP_ASSERT(d + 1, d + 1 == BDLDFP_DECIMAL_DL(-4.0));
        LOOP_ASSERT(1 + d, 1 + d == BDLDFP_DECIMAL_DL(-4.0));

        if (veryVeryVerbose) std::cout << "dec + unsigned int"
            << std::endl;
        LOOP_ASSERT(d + 10u, d + 10u == BDLDFP_DECIMAL_DL(5.0));
        LOOP_ASSERT(10u + d, 10u + d == BDLDFP_DECIMAL_DL(5.0));

        if (veryVeryVerbose) std::cout << "dec + long int"
            << std::endl;
        LOOP_ASSERT(d + 1l, d + 1l == BDLDFP_DECIMAL_DL(-4.0));
        LOOP_ASSERT(1l + d, 1l + d == BDLDFP_DECIMAL_DL(-4.0));

        if (veryVeryVerbose) std::cout << "dec + unsigned long int"
            << std::endl;
        LOOP_ASSERT(d + 10ul, d + 10ul == BDLDFP_DECIMAL_DL(5.0));
        LOOP_ASSERT(10ul + d, 10ul + d == BDLDFP_DECIMAL_DL(5.0));

        if (veryVeryVerbose) std::cout << "dec + long long int"
            << std::endl;
        LOOP_ASSERT(d + 1ll, d + 1ll == BDLDFP_DECIMAL_DL(-4.0));
        LOOP_ASSERT(1ll + d, 1ll + d == BDLDFP_DECIMAL_DL(-4.0));

        if (veryVeryVerbose) std::cout << "dec + unsigned long long int"
            << std::endl;
        LOOP_ASSERT(d + 10ull, d + 10ull == BDLDFP_DECIMAL_DL(5.0));
        LOOP_ASSERT(10ull + d, 10ull + d == BDLDFP_DECIMAL_DL(5.0));

        if (veryVeryVerbose) std::cout << "dec + dec128" << std::endl;
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DL(-3.0),
            d + BDLDFP_DECIMAL_DL(-3.0) ==
            BDLDFP_DECIMAL_DL(-8.0));

        if (veryVeryVerbose) std::cout << "dec + dec32" << std::endl;
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DF(-3.0),
            d + BDLDFP_DECIMAL_DF(-3.0) ==
            BDLDFP_DECIMAL_DL(-8.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) + d,
            BDLDFP_DECIMAL_DF(-3.0) + d ==
            BDLDFP_DECIMAL_DL(-8.0));

        if (veryVeryVerbose) std::cout << "dec + dec64" << std::endl;
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DD(-3.0),
            d + BDLDFP_DECIMAL_DD(-3.0) ==
            BDLDFP_DECIMAL_DL(-8.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DD(-3.0) + d,
            BDLDFP_DECIMAL_DD(-3.0) + d ==
            BDLDFP_DECIMAL_DL(-8.0));
    }

    if (veryVerbose) std::cout << "operator-" << std::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) std::cout << "dec - int" << std::endl;
        LOOP_ASSERT(d - 1, d - 1 == BDLDFP_DECIMAL_DL(-6.0));
        LOOP_ASSERT(1 - d, 1 - d == BDLDFP_DECIMAL_DL(6.0));

        if (veryVeryVerbose) std::cout << "dec - unsigned int"
            << std::endl;
        LOOP_ASSERT(d - 10u, d - 10u == BDLDFP_DECIMAL_DL(-15.0));
        LOOP_ASSERT(10u - d, 10u - d == BDLDFP_DECIMAL_DL(15.0));

        if (veryVeryVerbose) std::cout << "dec - long int"
            << std::endl;
        LOOP_ASSERT(d - 1l, d - 1l == BDLDFP_DECIMAL_DL(-6.0));
        LOOP_ASSERT(1l - d, 1l - d == BDLDFP_DECIMAL_DL(6.0));

        if (veryVeryVerbose) std::cout << "dec - unsigned long int"
            << std::endl;
        LOOP_ASSERT(d - 10ul, d - 10ul == BDLDFP_DECIMAL_DL(-15.0));
        LOOP_ASSERT(10ul - d, 10ul - d == BDLDFP_DECIMAL_DL(15.0));

        if (veryVeryVerbose) std::cout << "dec - long long int"
            << std::endl;
        LOOP_ASSERT(d - 1ll, d - 1ll == BDLDFP_DECIMAL_DL(-6.0));
        LOOP_ASSERT(1ll - d, 1ll - d == BDLDFP_DECIMAL_DL(6.0));

        if (veryVeryVerbose) std::cout << "dec - unsigned long long int"
            << std::endl;
        LOOP_ASSERT(d - 10ull, d - 10ull == BDLDFP_DECIMAL_DL(-15.0));
        LOOP_ASSERT(10ull - d, 10ull - d == BDLDFP_DECIMAL_DL(15.0));

        if (veryVeryVerbose) std::cout << "dec - dec128" << std::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DL(-3.0),
            d - BDLDFP_DECIMAL_DL(-3.0) ==
            BDLDFP_DECIMAL_DL(-2.0));

        if (veryVeryVerbose) std::cout << "dec - dec32" << std::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DF(-3.0),
            d - BDLDFP_DECIMAL_DF(-3.0) ==
            BDLDFP_DECIMAL_DL(-2.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) - d,
            BDLDFP_DECIMAL_DF(-3.0) - d ==
            BDLDFP_DECIMAL_DL(2.0));

        if (veryVeryVerbose) std::cout << "dec - dec64" << std::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DD(-3.0),
            d - BDLDFP_DECIMAL_DD(-3.0) ==
            BDLDFP_DECIMAL_DL(-2.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DD(-3.0) - d,
            BDLDFP_DECIMAL_DD(-3.0) - d ==
            BDLDFP_DECIMAL_DL(2.0));
    }

    if (veryVerbose) std::cout << "operator*" << std::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) std::cout << "dec * int" << std::endl;
        LOOP_ASSERT(d * -2, d * -2 == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2 * d, -2 * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) std::cout << "dec * unsigned int"
            << std::endl;
        LOOP_ASSERT(d * 10u, d * 10u == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10u * d, 10u * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) std::cout << "dec * long int" << std::endl;
        LOOP_ASSERT(d * -2l, d * -2l == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2l * d, -2l * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) std::cout << "dec * unsigned long int"
            << std::endl;
        LOOP_ASSERT(d * 10ul, d * 10ul == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10ul * d, 10ul * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) std::cout << "dec * long long int"
            << std::endl;
        LOOP_ASSERT(d * -2ll, d * -2ll == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2ll * d, -2ll * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) std::cout << "dec * unsigned long long int"
            << std::endl;
        LOOP_ASSERT(d * 10ull, d * 10ull == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10ull * d, 10ull * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) std::cout << "dec * dec64" << std::endl;
        LOOP_ASSERT(d * BDLDFP_DECIMAL_DD(-3.0),
            d * BDLDFP_DECIMAL_DD(-3.0) ==
            BDLDFP_DECIMAL_DD(15.0));

        if (veryVeryVerbose) std::cout << "dec * dec32" << std::endl;
        LOOP_ASSERT(d * BDLDFP_DECIMAL_DF(-3.0),
            d * BDLDFP_DECIMAL_DF(-3.0) ==
            BDLDFP_DECIMAL_DD(15.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) * d,
            BDLDFP_DECIMAL_DF(-3.0) * d ==
            BDLDFP_DECIMAL_DD(15.0));
    }

    if (veryVerbose) std::cout << "operator/" << std::endl;
    {
        BDEC::Decimal128 d(BDLDFP_DECIMAL_DL(-5.0));

        if (veryVeryVerbose) std::cout << "dec / int" << std::endl;
        LOOP_ASSERT(d / -2, d / -2 == BDLDFP_DECIMAL_DL(2.5));
        LOOP_ASSERT(-2 / d, -2 / d == BDLDFP_DECIMAL_DL(0.4));

        if (veryVeryVerbose) std::cout << "dec / unsigned int"
            << std::endl;
        LOOP_ASSERT(d / 100u, d / 100u == BDLDFP_DECIMAL_DL(-0.05));
        LOOP_ASSERT(100u / d, 100u / d == BDLDFP_DECIMAL_DL(-20.0));

        if (veryVeryVerbose) std::cout << "dec / long int" << std::endl;
        LOOP_ASSERT(d / -2l, d / -2l == BDLDFP_DECIMAL_DL(2.5));
        LOOP_ASSERT(-2l / d, -2l / d == BDLDFP_DECIMAL_DL(0.4));

        if (veryVeryVerbose) std::cout << "dec / unsigned long int"
            << std::endl;
        LOOP_ASSERT(d / 100ul, d / 100ul == BDLDFP_DECIMAL_DL(-0.05));
        LOOP_ASSERT(100ul / d, 100ul / d == BDLDFP_DECIMAL_DL(-20.0));

        if (veryVeryVerbose) std::cout << "dec / long long int"
            << std::endl;
        LOOP_ASSERT(d / -2ll, d / -2ll == BDLDFP_DECIMAL_DL(2.5));
        LOOP_ASSERT(-2ll / d, -2ll / d == BDLDFP_DECIMAL_DL(0.4));

        if (veryVeryVerbose) std::cout << "dec / unsigned long long int"
            << std::endl;
        LOOP_ASSERT(d / 100ull, d / 100ull == BDLDFP_DECIMAL_DL(-0.05));
        LOOP_ASSERT(100ull / d, 100ull / d == BDLDFP_DECIMAL_DL(-20.0));

        if (veryVeryVerbose) std::cout << "dec / dec128" << std::endl;
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DL(-50.0),
            d / BDLDFP_DECIMAL_DL(-50.0) ==
            BDLDFP_DECIMAL_DL(0.1));

        if (veryVeryVerbose) std::cout << "dec / dec32" << std::endl;
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DF(-50.0),
            d / BDLDFP_DECIMAL_DF(-50.0) ==
            BDLDFP_DECIMAL_DL(0.1));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-50.0) / d,
            BDLDFP_DECIMAL_DF(-50.0) / d ==
            BDLDFP_DECIMAL_DL(10.0));

        if (veryVeryVerbose) std::cout << "dec / dec64" << std::endl;
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DD(-50.0),
            d / BDLDFP_DECIMAL_DD(-50.0) ==
            BDLDFP_DECIMAL_DL(0.1));
        LOOP_ASSERT(BDLDFP_DECIMAL_DD(-50.0) / d,
            BDLDFP_DECIMAL_DD(-50.0) / d ==
            BDLDFP_DECIMAL_DL(10.0));
    }

    if (veryVerbose) std::cout << "Create test objects" << std::endl;

    BDEC::Decimal32        d32 = BDEC::Decimal32();
    BDEC::Decimal128       d128 = BDEC::Decimal128();
    const BDEC::Decimal128 c128 = BDEC::Decimal128();

    if (veryVerbose) std::cout << "Check return types" << std::endl;

    checkType<BDEC::Decimal128&>(++d128);
    checkType<BDEC::Decimal128>(d128++);
    checkType<BDEC::Decimal128&>(--d128);
    checkType<BDEC::Decimal128>(d128--);
    checkType<BDEC::Decimal128&>(d128 += static_cast<char>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<unsigned char>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<signed char>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<short>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<unsigned short>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<int>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<unsigned int>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<long>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<unsigned long>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<long long>(1));
    checkType<BDEC::Decimal128&>(d128 +=
        static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal128&>(d128 += static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<char>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<signed char>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<short>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<int>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<long>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<long long>(1));
    checkType<BDEC::Decimal128&>(d128 -=
        static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal128&>(d128 -= static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<char>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<signed char>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<short>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<int>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<long>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<long long>(1));
    checkType<BDEC::Decimal128&>(d128 *=
        static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal128&>(d128 *= static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<char>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<signed char>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<short>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<int>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<long>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<long long>(1));
    checkType<BDEC::Decimal128&>(d128 /=
        static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal128&>(d128 /= static_cast<BDEC::Decimal128>(1));

    checkType<BDEC::Decimal128>(+d128);
    checkType<BDEC::Decimal128>(-d128);

    checkType<BDEC::Decimal128>(d128 + d128);
    checkType<BDEC::Decimal128>(d128 - d128);
    checkType<BDEC::Decimal128>(d128 * d128);
    checkType<BDEC::Decimal128>(d128 / d128);
    checkType<bool>(d128 == d128);
    checkType<bool>(d32 == d128);
    checkType<bool>(d128 == d32);
    checkType<bool>(d128 != d128);
    checkType<bool>(d32 != d128);
    checkType<bool>(d128 != d32);
    checkType<bool>(d128 < d128);
    checkType<bool>(d32 < d128);
    checkType<bool>(d128 < d32);
    checkType<bool>(d128 <= d128);
    checkType<bool>(d32 <= d128);
    checkType<bool>(d128 <= d32);
    checkType<bool>(d128 > d128);
    checkType<bool>(d32 > d128);
    checkType<bool>(d128 > d32);
    checkType<bool>(d128 >= d128);
    checkType<bool>(d32 >= d128);
    checkType<bool>(d128 >= d32);

    {
        std::istringstream  in;// ;//(pa);
        std::wistringstream win;//;//(pa);
        std::ostringstream  out;//;//(pa);
        std::wostringstream wout;//;//(pa);

        checkType<std::istream&>(in >> d128);
        checkType<std::wistream&>(win >> d128);
        checkType<std::ostream&>(out << c128);
        checkType<std::wostream&>(wout << c128);
    }


    typedef std::numeric_limits<BDEC::Decimal128> d128_limits;
    checkType<bool>(d128_limits::is_specialized);
    checkType<BDEC::Decimal128>(d128_limits::min());
    checkType<BDEC::Decimal128>(d128_limits::max());
    checkType<int>(d128_limits::digits);
    checkType<int>(d128_limits::digits10);
    checkType<int>(d128_limits::max_digits10);
    checkType<bool>(d128_limits::is_signed);
    checkType<bool>(d128_limits::is_integer);
    checkType<bool>(d128_limits::is_exact);
    checkType<int>(d128_limits::radix);
    checkType<BDEC::Decimal128>(d128_limits::epsilon());
    checkType<BDEC::Decimal128>(d128_limits::round_error());
    checkType<int>(d128_limits::min_exponent);
    checkType<int>(d128_limits::min_exponent10);
    checkType<int>(d128_limits::max_exponent);
    checkType<int>(d128_limits::max_exponent10);
    checkType<bool>(d128_limits::has_infinity);
    checkType<bool>(d128_limits::has_quiet_NaN);
    checkType<bool>(d128_limits::has_signaling_NaN);
    checkType<std::float_denorm_style>(d128_limits::has_denorm);
    checkType<bool>(d128_limits::has_denorm_loss);
    checkType<BDEC::Decimal128>(d128_limits::infinity());
    checkType<BDEC::Decimal128>(d128_limits::quiet_NaN());
    checkType<BDEC::Decimal128>(d128_limits::signaling_NaN());
    checkType<BDEC::Decimal128>(d128_limits::denorm_min());
    checkType<bool>(d128_limits::is_iec559);
    checkType<bool>(d128_limits::is_bounded);
    checkType<bool>(d128_limits::is_modulo);
    checkType<bool>(d128_limits::traps);
    checkType<bool>(d128_limits::tinyness_before);
    checkType<std::float_round_style>(d128_limits::round_style);
}

TEST(Decimal, testCase2)
{
    // ------------------------------------------------------------------------
    // TESTING 'Decimal64'
    //   Ensure that 'Decimal64' functions correctly
    //
    // Concerns:
    //: 1 'Decimal64' operations are properly forwarded to implementation
    //: 2 All possible operand combinations work correctly.
    //
    // Plan:
    //: 1 Expected value testing will be used.
    //:
    //: 2 All forms of LHS and RHS combinations for all free operators will be
    //:   tested.
    //:
    //: 3 Every member operator will be tested
    //
    // Testing:
    //   'class Decimal64'
    // ------------------------------------------------------------------------

    if (verbose) std::cout << "\nTesting class Decimal64"
        << "\n=======================" << std::endl;

    if (veryVerbose) std::cout << "Constructors" << std::endl;

    if (veryVeryVerbose) std::cout << "Copy/convert" << std::endl;
    {
        const BDEC::Decimal32  c32 = BDEC::Decimal32(32);
        const BDEC::Decimal64  c64 = BDEC::Decimal64(64);
        const BDEC::Decimal128 c128 = BDEC::Decimal128(128);

        EXPECT_TRUE(BDLDFP_DECIMAL_DD(32.0) == BDEC::Decimal64(c32));
        EXPECT_TRUE(BDLDFP_DECIMAL_DD(64.0) == BDEC::Decimal64(c64));
        EXPECT_TRUE(BDLDFP_DECIMAL_DD(128.0) == BDEC::Decimal64(c128));
    }

    if (veryVeryVerbose) std::cout << "Integral" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DD(0.0) == BDEC::Decimal64());  // default
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-42.0) == BDEC::Decimal64(-42)); // int
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(42.0) == BDEC::Decimal64(42u)); // unsigned
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-42.0) == BDEC::Decimal64(-42l)); // long
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(42.0) == BDEC::Decimal64(42ul)); // ulong
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-42.0) == BDEC::Decimal64(-42ll)); // longlong
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(42.0) == BDEC::Decimal64(42ull)); // ulongl

    BDEC::Decimal64 cDefault;
    BDEC::Decimal64 cExpectedDefault = BDLDFP_DECIMAL_DD(0e-398);
    EXPECT_TRUE(0 == memcmp(&cDefault, &cExpectedDefault, sizeof(cDefault)));

    BDEC::Decimal64 cZero(0);
    BDEC::Decimal64 cExpectedZero = BDLDFP_DECIMAL_DD(0e0);
    EXPECT_TRUE(0 == memcmp(&cZero, &cExpectedZero, sizeof(cZero)));

    EXPECT_TRUE(0 != memcmp(&cDefault, &cZero, sizeof(cDefault)));

    if (veryVeryVerbose) std::cout << "Binary FP" << std::endl;

    // Note that to test binary-float taking constructors I use numbers
    // that can be represented exactly in both binary and decimal FP.

    EXPECT_TRUE(BDLDFP_DECIMAL_DD(4.25) == BDEC::Decimal64(4.25f)); // float
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(4.25) == BDEC::Decimal64(4.25)); // double

    if (veryVeryVerbose) std::cout << "Decimal FP" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-42.0) ==
        BDLDFP_DECIMAL_DF(-42.0));// Decimal342
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(42.0) == BDLDFP_DECIMAL_DF(42.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(4.2) == BDLDFP_DECIMAL_DF(4.2));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(4.2e9) == BDLDFP_DECIMAL_DF(4.2e9));

    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-42.0) == BDLDFP_DECIMAL_DL(-42.0));// Dec128
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(42.0) == BDLDFP_DECIMAL_DL(42.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(4.2) == BDLDFP_DECIMAL_DL(4.2));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(4.2e9) == BDLDFP_DECIMAL_DL(4.2e9));

    if (veryVerbose) std::cout << "Propriatery accessors" << std::endl;
    {
        BDEC::Decimal64 d64(42);
        EXPECT_TRUE((void*)d64.data() == (void*)&d64);

        const BDEC::Decimal64 cd64(42);
        EXPECT_TRUE((const void*)d64.data() == (const void*)&d64);

        EXPECT_TRUE(BDEC::Decimal64(cd64.value()) == BDEC::Decimal64(42));
    }

    if (veryVerbose) std::cout << "Operator==" << std::endl;

    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(4.0) == BDLDFP_DECIMAL_DD(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-9.345e27) == BDLDFP_DECIMAL_DD(-9.345e27));

    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(4.0) == BDLDFP_DECIMAL_DF(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-9.345e27) == BDLDFP_DECIMAL_DF(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(4.0) == BDLDFP_DECIMAL_DD(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-9.345e27) == BDLDFP_DECIMAL_DD(-9.345e27));

    if (veryVerbose) std::cout << "Operator!=" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DD(4.0) != BDLDFP_DECIMAL_DD(5.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(-9.345e27) !=
        BDLDFP_DECIMAL_DD(-9.345e27)));

    EXPECT_TRUE(BDLDFP_DECIMAL_DD(4.0) != BDLDFP_DECIMAL_DF(5.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(-9.345e27) !=
        BDLDFP_DECIMAL_DF(-9.345e27)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(4.0) != BDLDFP_DECIMAL_DD(5.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(-9.345e27) !=
        BDLDFP_DECIMAL_DD(-9.345e27)));

    if (veryVerbose) std::cout << "Operator<" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DD(4.0) < BDLDFP_DECIMAL_DD(5.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(-9.345e27) <
        BDLDFP_DECIMAL_DD(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(5.0) < BDLDFP_DECIMAL_DD(4.0)));

    EXPECT_TRUE(BDLDFP_DECIMAL_DD(4.0) < BDLDFP_DECIMAL_DF(5.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(-9.345e27) <
        BDLDFP_DECIMAL_DF(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(5.0) < BDLDFP_DECIMAL_DF(4.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(4.0) < BDLDFP_DECIMAL_DD(5.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(-9.345e27) <
        BDLDFP_DECIMAL_DD(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(5.0) < BDLDFP_DECIMAL_DD(4.0)));

    if (veryVerbose) std::cout << "Operator>" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DD(5.0) > BDLDFP_DECIMAL_DD(4.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(-9.345e27) >
        BDLDFP_DECIMAL_DD(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(4.0) > BDLDFP_DECIMAL_DD(5.0)));

    EXPECT_TRUE(BDLDFP_DECIMAL_DD(5.0) > BDLDFP_DECIMAL_DF(4.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(-9.345e27) >
        BDLDFP_DECIMAL_DF(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(4.0) > BDLDFP_DECIMAL_DF(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(5.0) > BDLDFP_DECIMAL_DD(4.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(-9.345e27) >
        BDLDFP_DECIMAL_DD(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(4.0) > BDLDFP_DECIMAL_DD(5.0)));

    if (veryVerbose) std::cout << "Operator<=" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DD(4.0) <= BDLDFP_DECIMAL_DD(5.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-9.345e27) <= BDLDFP_DECIMAL_DD(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(5.0) <= BDLDFP_DECIMAL_DD(4.0)));

    EXPECT_TRUE(BDLDFP_DECIMAL_DD(4.0) <= BDLDFP_DECIMAL_DF(5.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-9.345e27) <= BDLDFP_DECIMAL_DF(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(5.0) <= BDLDFP_DECIMAL_DF(4.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(4.0) <= BDLDFP_DECIMAL_DD(5.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-9.345e27) <= BDLDFP_DECIMAL_DD(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(5.0) <= BDLDFP_DECIMAL_DD(4.0)));

    if (veryVerbose) std::cout << "Operator>=" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DD(5.0) >= BDLDFP_DECIMAL_DD(4.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-9.345e27) >= BDLDFP_DECIMAL_DD(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(4.0) >= BDLDFP_DECIMAL_DD(5.0)));

    EXPECT_TRUE(BDLDFP_DECIMAL_DD(5.0) >= BDLDFP_DECIMAL_DF(4.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DD(-9.345e27) >= BDLDFP_DECIMAL_DF(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DD(4.0) >= BDLDFP_DECIMAL_DF(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(5.0) >= BDLDFP_DECIMAL_DD(4.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-9.345e27) >= BDLDFP_DECIMAL_DD(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(4.0) >= BDLDFP_DECIMAL_DD(5.0)));

    // TODO TBD: Note that I am testing with the maximum number of
    // significant digits and I actually expect stream output to do no
    // rounding.  That is wrong (see later why), but necessary to
    // remote-test the 'format' function and make sure it does not lose
    // digits or does some unwanted rounding.  When 'format' will be in the
    // lower utility, it will be tested directly.
    //
    // Expecting to see all digits is wrong because that is not how the
    // stream output should behave: it should print with the default
    // precision as it would for binary floating point *or* with the
    // implied precision of the cohort if that is larger.  AFAIU

    if (veryVerbose) std::cout << "Test stream out" << std::endl;
    {
        std::ostringstream  out;//(pa);

        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-1.234567890123456e-24));
        out << d1;
        std::string s;//(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, decLower(s) == "-1.234567890123456e-24");
    }

    if (veryVerbose) std::cout << "Test stream in" << std::endl;
    {
        std::istringstream  in;//(pa);
        std::string ins("-1.234567890123456e-24");// , pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        EXPECT_TRUE(d1 == BDLDFP_DECIMAL_DD(-1.234567890123456e-24));
    }

    if (veryVerbose) std::cout << "Test stream in NaN" << std::endl;
    {
        std::istringstream  in;//(pa);
        std::string ins("NaN");//, pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        EXPECT_TRUE(d1 != d1);
    }

    if (veryVerbose) std::cout << "Test stream in Infinity" << std::endl;
    {
        std::istringstream  in;//(pa);
        std::string ins("Inf");//, pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        EXPECT_TRUE(d1 > std::numeric_limits<BDEC::Decimal64>::max());
    }

    if (veryVerbose) std::cout << "Test stream in -Infinity" << std::endl;
    {
        std::istringstream  in;//(pa);
        std::string ins("-Inf");//, pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        EXPECT_TRUE(d1 < -std::numeric_limits<BDEC::Decimal64>::max());
    }

    if (veryVerbose) std::cout << "Test wide stream out" << std::endl;
    {
        std::wostringstream  out;//(pa);
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-1.234567890123456e-24));
        out << d1;
        std::wstring s;//(pa);
        getStringFromStream(out, &s);
        EXPECT_TRUE(decLower(s) == L"-1.234567890123456e-24");
    }

    if (veryVerbose) std::cout << "Test wide stream in" << std::endl;
    {
        std::wistringstream  in;//(pa);
        std::wstring ins(L"-1.234567890123456e-24");//, pa);
        in.str(ins);

        BDEC::Decimal64 d1;
        in >> d1;
        EXPECT_TRUE(d1 == BDLDFP_DECIMAL_DD(-1.234567890123456e-24));
    }

    if (veryVerbose) std::cout << "Operator++" << std::endl;
    {
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-5.0));
        BDEC::Decimal64 d2 = d1++;
        EXPECT_TRUE(BDLDFP_DECIMAL_DD(-5.0) == d2);
        LOOP_ASSERT(d1, BDLDFP_DECIMAL_DD(-4.0) == d1);
        BDEC::Decimal64 d3 = ++d2;
        LOOP_ASSERT(d2, BDLDFP_DECIMAL_DD(-4.0) == d2);
        LOOP_ASSERT(d3, BDLDFP_DECIMAL_DD(-4.0) == d3);
    }

    if (veryVerbose) std::cout << "Operator--" << std::endl;
    {
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-5.0));
        BDEC::Decimal64 d2 = d1--;
        EXPECT_TRUE(BDLDFP_DECIMAL_DD(-5.0) == d2);
        LOOP_ASSERT(d1, BDLDFP_DECIMAL_DD(-6.0) == d1);
        BDEC::Decimal64 d3 = --d2;
        LOOP_ASSERT(d2, BDLDFP_DECIMAL_DD(-6.0) == d2);
        LOOP_ASSERT(d3, BDLDFP_DECIMAL_DD(-6.0) == d3);
    }

    if (veryVerbose) std::cout << "Unary-" << std::endl;
    {
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-5.0));
        BDEC::Decimal64 d2(BDLDFP_DECIMAL_DD(5.0));
        EXPECT_TRUE(BDLDFP_DECIMAL_DD(5.0) == -d1);
        EXPECT_TRUE(BDLDFP_DECIMAL_DD(-5.0) == -d2);

        // Unary - must make +0 into -0

        std::ostringstream out;//(pa);
        out << -BDLDFP_DECIMAL_DD(0.0);
        std::string s;//(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] == '-'); // it is negative
        EXPECT_TRUE(-BDLDFP_DECIMAL_DD(0.0) == BDEC::Decimal64(0)); // and 0

            BDEC::Decimal64  dd = BDLDFP_DECIMAL_DD(0.0);
        BDEC::Decimal64 ndd = -BDLDFP_DECIMAL_DD(0.0);
        EXPECT_TRUE(std::memcmp(&ndd, &dd, sizeof(dd)));
        dd = -dd;
        EXPECT_TRUE(!std::memcmp(&ndd, &dd, sizeof(dd)));
    }

    if (veryVerbose) std::cout << "Unary+" << std::endl;
    {
        BDEC::Decimal64 d1(BDLDFP_DECIMAL_DD(-5.0));
        BDEC::Decimal64 d2(BDLDFP_DECIMAL_DD(5.0));
        EXPECT_TRUE(BDLDFP_DECIMAL_DD(-5.0) == +d1);
        EXPECT_TRUE(BDLDFP_DECIMAL_DD(5.0) == +d2);

        // Unary + must make -0 into +0

        BDEC::Decimal128 negzero(-BDLDFP_DECIMAL_DD(0.0));
        std::ostringstream out;//(pa);
        out << +negzero;
        std::string s;//(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] != '+'); // it is positive
        EXPECT_TRUE(-BDLDFP_DECIMAL_DD(0.0) == BDEC::Decimal64(0)); // and 0
    }

    if (veryVerbose) std::cout << "+=" << std::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) std::cout << "+=(int)" << std::endl;
        d += 1;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-4.0) == d);

        if (veryVeryVerbose) std::cout << "+=(unsigned int)" << std::endl;
        d += 2u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-2.0) == d);

        if (veryVeryVerbose) std::cout << "+=(long int)" << std::endl;
        d += -1l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-3.0) == d);

        if (veryVeryVerbose) std::cout << "+=(unsigned long int)"
            << std::endl;
        d += 5ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(2.0) == d);

        if (veryVeryVerbose) std::cout << "+=(long long int)" << std::endl;
        d += -2ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.0) == d);

        if (veryVeryVerbose) std::cout << "+=(unsigned long long int)"
            << std::endl;
        d += 42ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(42.0) == d);

        if (veryVeryVerbose) std::cout << "+=(Decimal32)" << std::endl;
        d += BDLDFP_DECIMAL_DF(-22.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(20.0) == d);

        if (veryVeryVerbose) std::cout << "+=(Decimal64)" << std::endl;
        d += BDLDFP_DECIMAL_DD(3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(23.0) == d);

        if (veryVeryVerbose) std::cout << "+=(Decimal128)" << std::endl;
        d += BDLDFP_DECIMAL_DL(9.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(32.0) == d);
    }

    if (veryVerbose) std::cout << "-=" << std::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) std::cout << "-=(int)" << std::endl;
        d -= 1;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-6.0) == d);

        if (veryVeryVerbose) std::cout << "-=(unsigned int)" << std::endl;
        d -= 2u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-8.0) == d);

        if (veryVeryVerbose) std::cout << "-=(long int)" << std::endl;
        d -= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(2.0) == d);

        if (veryVeryVerbose) std::cout << "-=(unsigned long int)"
            << std::endl;
        d -= 10ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-8.0) == d);

        if (veryVeryVerbose) std::cout << "-=(long long int)" << std::endl;
        d -= -8ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.0) == d);

        if (veryVeryVerbose) std::cout << "-=(unsigned long long int)"
            << std::endl;
        d -= 42ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-42.0) == d);

        if (veryVeryVerbose) std::cout << "-=(Decimal32)" << std::endl;
        d -= BDLDFP_DECIMAL_DF(-22.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-20.0) == d);

        if (veryVeryVerbose) std::cout << "-=(Decimal64)" << std::endl;
        d -= BDLDFP_DECIMAL_DD(3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-23.0) == d);

        if (veryVeryVerbose) std::cout << "-=(Decimal128)" << std::endl;
        d -= BDLDFP_DECIMAL_DL(9.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-32.0) == d);
    }

    if (veryVerbose) std::cout << "*=" << std::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) std::cout << "*=(int)" << std::endl;
        d *= -2;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(10.0) == d);

        if (veryVeryVerbose) std::cout << "*=(unsigned int)" << std::endl;
        d *= 2000u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(20000.0) == d);

        if (veryVeryVerbose) std::cout << "*=(long int)" << std::endl;
        d *= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-200000.0) == d);

        if (veryVeryVerbose) std::cout << "*=(unsigned long int)"
            << std::endl;
        d *= 3ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-600000.0) == d);

        if (veryVeryVerbose) std::cout << "*=(long long int)" << std::endl;
        d *= -1ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(600000.0) == d);

        if (veryVeryVerbose) std::cout << "*=(unsigned long long int)"
            << std::endl;
        d *= 5ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(3000000.0) == d);

        if (veryVeryVerbose) std::cout << "*=(Decimal32)" << std::endl;
        d *= BDLDFP_DECIMAL_DF(1e-5);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(30.0) == d);

        if (veryVeryVerbose) std::cout << "*=(Decimal64)" << std::endl;
        d *= BDLDFP_DECIMAL_DD(-3.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-90.0) == d);

        if (veryVeryVerbose) std::cout << "*=(Decimal128)" << std::endl;
        d *= BDLDFP_DECIMAL_DL(2.4e-134);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-2.16e-132) == d);
    }

    if (veryVerbose) std::cout << "/=" << std::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) std::cout << "/=(int)" << std::endl;
        d /= -2;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(2.5) == d);

        if (veryVeryVerbose) std::cout << "/=(unsigned int)" << std::endl;
        d /= 2000u;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.00125) == d);

        if (veryVeryVerbose) std::cout << "/=(long int)" << std::endl;
        d /= -10l;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-0.000125) == d);

        if (veryVeryVerbose) std::cout << "/=(unsigned long int)"
            << std::endl;
        d /= 5ul;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-0.000025) == d);

        if (veryVeryVerbose) std::cout << "/=(long long int)" << std::endl;
        d /= -5ll;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.000005) == d);

        if (veryVeryVerbose) std::cout << "/=(unsigned long long int)"
            << std::endl;
        d /= 5ull;
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.000001) == d);

        if (veryVeryVerbose) std::cout << "/=(Decimal32)" << std::endl;
        d /= BDLDFP_DECIMAL_DF(1e-5);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(0.1) == d);

        if (veryVeryVerbose) std::cout << "/=(Decimal64)" << std::endl;
        d /= BDLDFP_DECIMAL_DD(-5.0);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(-0.02) == d);

        if (veryVeryVerbose) std::cout << "/=(Decimal128)" << std::endl;
        d /= BDLDFP_DECIMAL_DL(-2.5e-134);
        LOOP_ASSERT(d, BDLDFP_DECIMAL_DD(8.e131) == d);
    }

    if (veryVerbose) std::cout << "operator+" << std::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) std::cout << "dec + int" << std::endl;
        LOOP_ASSERT(d + 1, d + 1 == BDLDFP_DECIMAL_DD(-4.0));
        LOOP_ASSERT(1 + d, 1 + d == BDLDFP_DECIMAL_DD(-4.0));

        if (veryVeryVerbose) std::cout << "dec + unsigned int"
            << std::endl;
        LOOP_ASSERT(d + 10u, d + 10u == BDLDFP_DECIMAL_DD(5.0));
        LOOP_ASSERT(10u + d, 10u + d == BDLDFP_DECIMAL_DD(5.0));

        if (veryVeryVerbose) std::cout << "dec + long int" << std::endl;
        LOOP_ASSERT(d + 1l, d + 1l == BDLDFP_DECIMAL_DD(-4.0));
        LOOP_ASSERT(1l + d, 1l + d == BDLDFP_DECIMAL_DD(-4.0));

        if (veryVeryVerbose) std::cout << "dec + unsigned long int"
            << std::endl;
        LOOP_ASSERT(d + 10ul, d + 10ul == BDLDFP_DECIMAL_DD(5.0));
        LOOP_ASSERT(10ul + d, 10ul + d == BDLDFP_DECIMAL_DD(5.0));

        if (veryVeryVerbose) std::cout << "dec + long long int"
            << std::endl;
        LOOP_ASSERT(d + 1ll, d + 1ll == BDLDFP_DECIMAL_DD(-4.0));
        LOOP_ASSERT(1ll + d, 1ll + d == BDLDFP_DECIMAL_DD(-4.0));

        if (veryVeryVerbose) std::cout << "dec + unsigned long long int"
            << std::endl;
        LOOP_ASSERT(d + 10ull, d + 10ull == BDLDFP_DECIMAL_DD(5.0));
        LOOP_ASSERT(10ull + d, 10ull + d == BDLDFP_DECIMAL_DD(5.0));

        if (veryVeryVerbose) std::cout << "dec + dec64" << std::endl;
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DD(-3.0),
            d + BDLDFP_DECIMAL_DD(-3.0) ==
            BDLDFP_DECIMAL_DD(-8.0));

        if (veryVeryVerbose) std::cout << "dec + dec32" << std::endl;
        LOOP_ASSERT(d + BDLDFP_DECIMAL_DF(-3.0),
            d + BDLDFP_DECIMAL_DF(-3.0) ==
            BDLDFP_DECIMAL_DD(-8.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) + d,
            BDLDFP_DECIMAL_DF(-3.0) + d ==
            BDLDFP_DECIMAL_DD(-8.0));
    }

    if (veryVerbose) std::cout << "operator-" << std::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) std::cout << "dec - int" << std::endl;
        LOOP_ASSERT(d - 1, d - 1 == BDLDFP_DECIMAL_DD(-6.0));
        LOOP_ASSERT(1 - d, 1 - d == BDLDFP_DECIMAL_DD(6.0));

        if (veryVeryVerbose) std::cout << "dec - unsigned int"
            << std::endl;
        LOOP_ASSERT(d - 10u, d - 10u == BDLDFP_DECIMAL_DD(-15.0));
        LOOP_ASSERT(10u - d, 10u - d == BDLDFP_DECIMAL_DD(15.0));

        if (veryVeryVerbose) std::cout << "dec - long int" << std::endl;
        LOOP_ASSERT(d - 1l, d - 1l == BDLDFP_DECIMAL_DD(-6.0));
        LOOP_ASSERT(1l - d, 1l - d == BDLDFP_DECIMAL_DD(6.0));

        if (veryVeryVerbose) std::cout << "dec - unsigned long int"
            << std::endl;
        LOOP_ASSERT(d - 10ul, d - 10ul == BDLDFP_DECIMAL_DD(-15.0));
        LOOP_ASSERT(10ul - d, 10ul - d == BDLDFP_DECIMAL_DD(15.0));

        if (veryVeryVerbose) std::cout << "dec - long long int"
            << std::endl;
        LOOP_ASSERT(d - 1ll, d - 1ll == BDLDFP_DECIMAL_DD(-6.0));
        LOOP_ASSERT(1ll - d, 1ll - d == BDLDFP_DECIMAL_DD(6.0));

        if (veryVeryVerbose) std::cout << "dec - unsigned long long int"
            << std::endl;
        LOOP_ASSERT(d - 10ull, d - 10ull == BDLDFP_DECIMAL_DD(-15.0));
        LOOP_ASSERT(10ull - d, 10ull - d == BDLDFP_DECIMAL_DD(15.0));

        if (veryVeryVerbose) std::cout << "dec - dec64" << std::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DD(-3.0),
            d - BDLDFP_DECIMAL_DD(-3.0) ==
            BDLDFP_DECIMAL_DD(-2.0));

        if (veryVeryVerbose) std::cout << "dec - dec32" << std::endl;
        LOOP_ASSERT(d - BDLDFP_DECIMAL_DF(-3.0),
            d - BDLDFP_DECIMAL_DF(-3.0) ==
            BDLDFP_DECIMAL_DD(-2.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) - d,
            BDLDFP_DECIMAL_DF(-3.0) - d ==
            BDLDFP_DECIMAL_DD(2.0));
    }

    if (veryVerbose) std::cout << "operator*" << std::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) std::cout << "dec * int" << std::endl;
        LOOP_ASSERT(d * -2, d * -2 == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2 * d, -2 * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) std::cout << "dec * unsigned int"
            << std::endl;
        LOOP_ASSERT(d * 10u, d * 10u == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10u * d, 10u * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) std::cout << "dec * long int" << std::endl;
        LOOP_ASSERT(d * -2l, d * -2l == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2l * d, -2l * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) std::cout << "dec * unsigned long int"
            << std::endl;
        LOOP_ASSERT(d * 10ul, d * 10ul == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10ul * d, 10ul * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) std::cout << "dec * long long int"
            << std::endl;
        LOOP_ASSERT(d * -2ll, d * -2ll == BDLDFP_DECIMAL_DD(10.0));
        LOOP_ASSERT(-2ll * d, -2ll * d == BDLDFP_DECIMAL_DD(10.0));

        if (veryVeryVerbose) std::cout << "dec * unsigned long long int"
            << std::endl;
        LOOP_ASSERT(d * 10ull, d * 10ull == BDLDFP_DECIMAL_DD(-50.0));
        LOOP_ASSERT(10ull * d, 10ull * d == BDLDFP_DECIMAL_DD(-50.0));

        if (veryVeryVerbose) std::cout << "dec * dec64" << std::endl;
        LOOP_ASSERT(d * BDLDFP_DECIMAL_DD(-3.0),
            d * BDLDFP_DECIMAL_DD(-3.0) ==
            BDLDFP_DECIMAL_DD(15.0));

        if (veryVeryVerbose) std::cout << "dec * dec32" << std::endl;
        LOOP_ASSERT(d * BDLDFP_DECIMAL_DF(-3.0),
            d * BDLDFP_DECIMAL_DF(-3.0) ==
            BDLDFP_DECIMAL_DD(15.0));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-3.0) * d,
            BDLDFP_DECIMAL_DF(-3.0) * d ==
            BDLDFP_DECIMAL_DD(15.0));
    }

    if (veryVerbose) std::cout << "operator/" << std::endl;
    {
        BDEC::Decimal64 d(BDLDFP_DECIMAL_DD(-5.0));

        if (veryVeryVerbose) std::cout << "dec / int" << std::endl;
        LOOP_ASSERT(d / -2, d / -2 == BDLDFP_DECIMAL_DD(2.5));
        LOOP_ASSERT(-2 / d, -2 / d == BDLDFP_DECIMAL_DD(0.4));

        if (veryVeryVerbose) std::cout << "dec / unsigned int"
            << std::endl;
        LOOP_ASSERT(d / 100u, d / 100u == BDLDFP_DECIMAL_DD(-0.05));
        LOOP_ASSERT(100u / d, 100u / d == BDLDFP_DECIMAL_DD(-20.0));

        if (veryVeryVerbose) std::cout << "dec / long int" << std::endl;
        LOOP_ASSERT(d / -2l, d / -2l == BDLDFP_DECIMAL_DD(2.5));
        LOOP_ASSERT(-2l / d, -2l / d == BDLDFP_DECIMAL_DD(0.4));

        if (veryVeryVerbose) std::cout << "dec / unsigned long int"
            << std::endl;
        LOOP_ASSERT(d / 100ul, d / 100ul == BDLDFP_DECIMAL_DD(-0.05));
        LOOP_ASSERT(100ul / d, 100ul / d == BDLDFP_DECIMAL_DD(-20.0));

        if (veryVeryVerbose) std::cout << "dec / long long int"
            << std::endl;
        LOOP_ASSERT(d / -2ll, d / -2ll == BDLDFP_DECIMAL_DD(2.5));
        LOOP_ASSERT(-2ll / d, -2ll / d == BDLDFP_DECIMAL_DD(0.4));

        if (veryVeryVerbose) std::cout << "dec / unsigned long long int"
            << std::endl;
        LOOP_ASSERT(d / 100ull, d / 100ull == BDLDFP_DECIMAL_DD(-0.05));
        LOOP_ASSERT(100ull / d, 100ull / d == BDLDFP_DECIMAL_DD(-20.0));

        if (veryVeryVerbose) std::cout << "dec / dec64" << std::endl;
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DD(-50.0),
            d / BDLDFP_DECIMAL_DD(-50.0) ==
            BDLDFP_DECIMAL_DD(0.1));

        if (veryVeryVerbose) std::cout << "dec / dec32" << std::endl;
        LOOP_ASSERT(d / BDLDFP_DECIMAL_DF(-50.0),
            d / BDLDFP_DECIMAL_DF(-50.0) ==
            BDLDFP_DECIMAL_DD(0.1));
        LOOP_ASSERT(BDLDFP_DECIMAL_DF(-50.0) / d,
            BDLDFP_DECIMAL_DF(-50.0) / d ==
            BDLDFP_DECIMAL_DD(10.0));
    }

    if (veryVerbose) std::cout << "Create test objects" << std::endl;

    BDEC::Decimal32        d32 = BDEC::Decimal32();
    BDEC::Decimal64        d64 = BDEC::Decimal64();
    const BDEC::Decimal64  c64 = BDEC::Decimal64();

    if (veryVerbose) std::cout << "Check return types" << std::endl;

    checkType<BDEC::Decimal64&>(++d64);
    checkType<BDEC::Decimal64>(d64++);
    checkType<BDEC::Decimal64&>(--d64);
    checkType<BDEC::Decimal64>(d64--);
    checkType<BDEC::Decimal64&>(d64 += static_cast<char>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned char>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<signed char>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<short>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned short>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<int>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned int>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<long>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned long>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<long long>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal64&>(d64 += static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<char>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<signed char>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<short>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<int>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<long>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<long long>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal64&>(d64 -= static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<char>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<signed char>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<short>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<int>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<long>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<long long>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal64&>(d64 *= static_cast<BDEC::Decimal128>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<char>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned char>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<signed char>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<short>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned short>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<int>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned int>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<long>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned long>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<long long>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<unsigned long long>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<BDEC::Decimal32>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<BDEC::Decimal64>(1));
    checkType<BDEC::Decimal64&>(d64 /= static_cast<BDEC::Decimal128>(1));

    checkType<BDEC::Decimal64>(+d64);
    checkType<BDEC::Decimal64>(-d64);

    checkType<BDEC::Decimal64>(d64 + d64);
    checkType<BDEC::Decimal64>(d64 - d64);
    checkType<BDEC::Decimal64>(d64 * d64);
    checkType<BDEC::Decimal64>(d64 / d64);
    checkType<bool>(d64 == d64);
    checkType<bool>(d32 == d64);
    checkType<bool>(d64 == d32);
    checkType<bool>(d64 != d64);
    checkType<bool>(d32 != d64);
    checkType<bool>(d64 != d32);
    checkType<bool>(d64 < d64);
    checkType<bool>(d32 < d64);
    checkType<bool>(d64 < d32);
    checkType<bool>(d64 <= d64);
    checkType<bool>(d32 <= d64);
    checkType<bool>(d64 <= d32);
    checkType<bool>(d64 > d64);
    checkType<bool>(d32 > d64);
    checkType<bool>(d64 > d32);
    checkType<bool>(d64 >= d64);
    checkType<bool>(d32 >= d64);
    checkType<bool>(d64 >= d32);

    {
        std::istringstream  in;//(pa);
        std::wistringstream win;//(pa);
        std::ostringstream  out;//(pa);
        std::wostringstream wout;//(pa);

        checkType<std::istream&>(in >> d64);
        checkType<std::wistream&>(win >> d64);
        checkType<std::ostream&>(out << c64);
        checkType<std::wostream&>(wout << c64);
    }

    typedef std::numeric_limits<BDEC::Decimal64> d64_limits;
    checkType<bool>(d64_limits::is_specialized);
    checkType<BDEC::Decimal64>(d64_limits::min());
    checkType<BDEC::Decimal64>(d64_limits::max());
    checkType<int>(d64_limits::digits);
    checkType<int>(d64_limits::digits10);
    checkType<int>(d64_limits::max_digits10);
    checkType<bool>(d64_limits::is_signed);
    checkType<bool>(d64_limits::is_integer);
    checkType<bool>(d64_limits::is_exact);
    checkType<int>(d64_limits::radix);
    checkType<BDEC::Decimal64>(d64_limits::epsilon());
    checkType<BDEC::Decimal64>(d64_limits::round_error());
    checkType<int>(d64_limits::min_exponent);
    checkType<int>(d64_limits::min_exponent10);
    checkType<int>(d64_limits::max_exponent);
    checkType<int>(d64_limits::max_exponent10);
    checkType<bool>(d64_limits::has_infinity);
    checkType<bool>(d64_limits::has_quiet_NaN);
    checkType<bool>(d64_limits::has_signaling_NaN);
    checkType<std::float_denorm_style>(d64_limits::has_denorm);
    checkType<bool>(d64_limits::has_denorm_loss);
    checkType<BDEC::Decimal64>(d64_limits::infinity());
    checkType<BDEC::Decimal64>(d64_limits::quiet_NaN());
    checkType<BDEC::Decimal64>(d64_limits::signaling_NaN());
    checkType<BDEC::Decimal64>(d64_limits::denorm_min());
    checkType<bool>(d64_limits::is_iec559);
    checkType<bool>(d64_limits::is_bounded);
    checkType<bool>(d64_limits::is_modulo);
    checkType<bool>(d64_limits::traps);
    checkType<bool>(d64_limits::tinyness_before);
    checkType<std::float_round_style>(d64_limits::round_style);
}

TEST(Decimal, testCase1)
{
    // ------------------------------------------------------------------------
    // TESTING 'Decimal32'
    //   Ensure that 'Decimal32' functions correctly
    //
    // Concerns:
    //: 1 'Decimal32' operations are properly forwarded to implementation
    //:
    //: 2 All possible operand combinations work correctly.
    //
    // Plan:
    //: 1 Expected value testing will be used.
    //:
    //: 2 All forms of LHS and RHS combinations for all free operators will be
    //:   tested.
    //:
    //: 3 Every member operator will be tested
    //
    // Testing:
    //   'class Decimal32'
    // ------------------------------------------------------------------------
    if (verbose) std::cout << std::endl
        << "Testing class Decimal32" << std::endl
        << "=======================" << std::endl;

    if (veryVerbose) std::cout << "Constructors" << std::endl;

    if (veryVeryVerbose) std::cout << "Copy/convert" << std::endl;
    {
        const BDEC::Decimal32  c32 = BDEC::Decimal32(32);
        const BDEC::Decimal64  c64 = BDEC::Decimal64(64);

        EXPECT_TRUE(BDLDFP_DECIMAL_DF(32.0) == BDEC::Decimal32(c32));
        EXPECT_TRUE(BDLDFP_DECIMAL_DF(64.0) == BDEC::Decimal32(c64));
        // TODO: Conversions from Decimal128 to Decimal32.
        // EXPECT_TRUE(BDLDFP_DECIMAL_DF(128.0) == BDEC::Decimal32(c128));
    }

    if (veryVeryVerbose) std::cout << "Integral" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DF(0.0) == BDEC::Decimal32());  // default
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-42.0) == BDEC::Decimal32(-42)); // int
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(42.0) == BDEC::Decimal32(42u)); // unsigned
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-42.0) == BDEC::Decimal32(-42l)); // long
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(42.0) == BDEC::Decimal32(42ul)); // ulong
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-42.0) == BDEC::Decimal32(-42ll)); // longlong
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(42.0) == BDEC::Decimal32(42ull)); // ulongl

    BDEC::Decimal32 cDefault;
    BDEC::Decimal32 cExpectedDefault = BDLDFP_DECIMAL_DF(0e-101);
    EXPECT_TRUE(0 == memcmp(&cDefault, &cExpectedDefault, sizeof(cDefault)));

    BDEC::Decimal32 cZero(0);
    BDEC::Decimal32 cExpectedZero = BDLDFP_DECIMAL_DF(0e0);
    EXPECT_TRUE(0 == memcmp(&cZero, &cExpectedZero, sizeof(cZero)));

    EXPECT_TRUE(0 != memcmp(&cDefault, &cZero, sizeof(cDefault)));

    if (veryVeryVerbose) std::cout << "Binary FP" << std::endl;

    // Note that to test binary-float taking constructors I use numbers
    // that can be represented exactly in both binary and decimal FP.

    EXPECT_TRUE(BDLDFP_DECIMAL_DF(4.25) == BDEC::Decimal32(4.25f)); // float
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(4.25) == BDEC::Decimal32(4.25)); // double

    if (veryVeryVerbose) std::cout << "Decimal FP" << std::endl;

    // Decimal64
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-42.0) == BDLDFP_DECIMAL_DD(-42.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(42.0) == BDLDFP_DECIMAL_DD(42.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(4.2) == BDLDFP_DECIMAL_DD(4.2));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(4.2e9) == BDLDFP_DECIMAL_DD(4.2e9));

    // Dec128
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-42.0) == BDLDFP_DECIMAL_DL(-42.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(42.0) == BDLDFP_DECIMAL_DL(42.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(4.2) == BDLDFP_DECIMAL_DL(4.2));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(4.2e9) == BDLDFP_DECIMAL_DL(4.2e9));

    if (veryVerbose) std::cout << "Propriatery accessors" << std::endl;
    {
        BDEC::Decimal32 d32(42);
        EXPECT_TRUE((void*)d32.data() == (void*)&d32);

        const BDEC::Decimal32 cd32(42);
        EXPECT_TRUE((const void*)d32.data() == (const void*)&d32);

        EXPECT_TRUE(BDEC::Decimal32(cd32.value()) == BDEC::Decimal32(42));
    }

    if (veryVerbose) std::cout << "Operator==" << std::endl;

    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(4.0) == BDLDFP_DECIMAL_DF(5.0)));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-9.345e27) == BDLDFP_DECIMAL_DF(-9.345e27));

    if (veryVerbose) std::cout << "Operator!=" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DF(4.0) != BDLDFP_DECIMAL_DF(5.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(-9.345e27) !=
        BDLDFP_DECIMAL_DF(-9.345e27)));

    if (veryVerbose) std::cout << "Operator<" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DF(4.0) <
        BDLDFP_DECIMAL_DF(5.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(-9.345e27) <
        BDLDFP_DECIMAL_DF(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(5.0) < BDLDFP_DECIMAL_DF(4.0)));

    if (veryVerbose) std::cout << "Operator>" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DF(5.0) > BDLDFP_DECIMAL_DF(4.0));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(-9.345e27) >
        BDLDFP_DECIMAL_DF(-9.345e27)));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(4.0) > BDLDFP_DECIMAL_DF(5.0)));

    if (veryVerbose) std::cout << "Operator<=" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DF(4.0) <= BDLDFP_DECIMAL_DF(5.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-9.345e27) <= BDLDFP_DECIMAL_DF(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(5.0) <= BDLDFP_DECIMAL_DF(4.0)));

    if (veryVerbose) std::cout << "Operator>=" << std::endl;

    EXPECT_TRUE(BDLDFP_DECIMAL_DF(5.0) >= BDLDFP_DECIMAL_DF(4.0));
    EXPECT_TRUE(BDLDFP_DECIMAL_DF(-9.345e27) >= BDLDFP_DECIMAL_DF(-9.345e27));
    EXPECT_TRUE(!(BDLDFP_DECIMAL_DF(4.0) >= BDLDFP_DECIMAL_DF(5.0)));

    // TODO TBD: Note that I am testing with the maximum number of
    // significant digits and I actually expect stream output to do no
    // rounding.  That is wrong (see later why), but necessary to
    // remote-test the 'format' function and make sure it does not lose
    // digits or does some unwanted rounding.  When 'format' will be in the
    // lower utility, it will be tested directly.  Expecting to see all
    // digits is wrong because that is not how the stream output should
    // behave: it should print with the default precision as it would for
    // binary floating point *or* with the implied precision of the cohort
    // if that is larger.  AFAIU

    if (veryVerbose) std::cout << "Test stream out" << std::endl;
    {
        std::ostringstream out;//(pa);

        BDEC::Decimal32 d1(BDLDFP_DECIMAL_DF(-8.327457e-24));
        out << d1;
        std::string s;//(pa);
        getStringFromStream(out, &s);
        EXPECT_TRUE(decLower(s) == "-8.327457e-24");
    }

    if (veryVerbose) std::cout << "Test stream in" << std::endl;
    {
        std::istringstream in;//(pa);
        std::string ins("-8.327457e-24");//, pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        EXPECT_TRUE(d1 == BDLDFP_DECIMAL_DF(-8.327457e-24));
    }

    if (veryVerbose) std::cout << "Test stream in NaN" << std::endl;
    {
        std::istringstream  in;//(pa);
        std::string ins("NaN");//, pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        EXPECT_TRUE(d1 != d1);
    }

    if (veryVerbose) std::cout << "Test stream in Infinity" << std::endl;
    {
        std::istringstream  in;//(pa);
        std::string ins("Inf");//, pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        EXPECT_TRUE(d1 > std::numeric_limits<BDEC::Decimal32>::max());
    }

    if (veryVerbose) std::cout << "Test stream in -Infinity" << std::endl;
    {
        std::istringstream  in;//(pa);
        std::string ins("-Inf");//, pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        EXPECT_TRUE(d1 < -std::numeric_limits<BDEC::Decimal32>::max());
    }

    if (veryVerbose) std::cout << "Test wide stream out" << std::endl;
    {
        std::wostringstream  out;//(pa);
        BDEC::Decimal32 d1(BDLDFP_DECIMAL_DF(-8.327457e-24));
        out << d1;
        std::wstring s;//(pa);
        getStringFromStream(out, &s);
        EXPECT_TRUE(decLower(s) == L"-8.327457e-24");
    }

    if (veryVerbose) std::cout << "Test wide stream in" << std::endl;
    {
        std::wistringstream  in;//(pa);
        std::wstring ins(L"-8.327457e-24");//, pa);
        in.str(ins);

        BDEC::Decimal32 d1;
        in >> d1;
        EXPECT_TRUE(d1 == BDLDFP_DECIMAL_DF(-8.327457e-24));
    }

    if (veryVerbose) std::cout << "Unary-" << std::endl;
    {
        BDEC::Decimal32 d1(BDLDFP_DECIMAL_DF(-5.0));
        BDEC::Decimal32 d2(BDLDFP_DECIMAL_DF(5.0));
        EXPECT_TRUE(BDLDFP_DECIMAL_DF(5.0) == -d1);
        EXPECT_TRUE(BDLDFP_DECIMAL_DF(-5.0) == -d2);

        // Unary - must make +0 into -0

        std::ostringstream out;//(pa);
        out << -BDLDFP_DECIMAL_DF(0.0);
        std::string s;//(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] == '-'); // it is negative
        EXPECT_TRUE(-BDLDFP_DECIMAL_DF(0.0) == BDEC::Decimal32(0)); // and 0
    }

    if (veryVerbose) std::cout << "Unary+" << std::endl;
    {
        BDEC::Decimal32 d1(BDLDFP_DECIMAL_DF(-5.0));
        BDEC::Decimal32 d2(BDLDFP_DECIMAL_DF(5.0));
        EXPECT_TRUE(BDLDFP_DECIMAL_DF(-5.0) == +d1);
        EXPECT_TRUE(BDLDFP_DECIMAL_DF(5.0) == +d2);

        // Unary + must make -0 into +0

        BDEC::Decimal128 negzero(-BDLDFP_DECIMAL_DF(0.0));
        std::ostringstream out;//(pa);
        out << +negzero;
        std::string s;//(pa);
        getStringFromStream(out, &s);
        LOOP_ASSERT(s, s[0] != '+'); // it is positive
        EXPECT_TRUE(-BDLDFP_DECIMAL_DF(0.0) == BDEC::Decimal32(0)); // and 0
    }


    if (veryVerbose) std::cout << "Create test objects" << std::endl;

    BDEC::Decimal32        d32 = BDEC::Decimal32();
    const BDEC::Decimal32  c32 = BDEC::Decimal32();

    if (veryVerbose) std::cout << "Check return types" << std::endl;

    checkType<bool>(d32 == d32);
    checkType<bool>(d32 != d32);
    checkType<bool>(d32 < d32);
    checkType<bool>(d32 <= d32);
    checkType<bool>(d32 > d32);
    checkType<bool>(d32 >= d32);

    checkType<BDEC::Decimal32>(+d32);
    checkType<BDEC::Decimal32>(-d32);

    {
        std::istringstream  in;//(pa);
        std::ostringstream  out;//(pa);
        std::wistringstream win;//(pa);
        std::wostringstream wout;//(pa);

        checkType<std::istream&>(in >> d32);
        checkType<std::wistream&>(win >> d32);
        checkType<std::ostream&>(out << c32);
        checkType<std::wostream&>(wout << c32);
    }

    typedef std::numeric_limits<BDEC::Decimal32> d32_limits;
    checkType<bool>(d32_limits::is_specialized);
    checkType<BDEC::Decimal32>(d32_limits::min());
    checkType<BDEC::Decimal32>(d32_limits::max());
    checkType<int>(d32_limits::digits);
    checkType<int>(d32_limits::digits10);
    checkType<int>(d32_limits::max_digits10);
    checkType<bool>(d32_limits::is_signed);
    checkType<bool>(d32_limits::is_integer);
    checkType<bool>(d32_limits::is_exact);
    checkType<int>(d32_limits::radix);
    checkType<BDEC::Decimal32>(d32_limits::epsilon());
    checkType<BDEC::Decimal32>(d32_limits::round_error());
    checkType<int>(d32_limits::min_exponent);
    checkType<int>(d32_limits::min_exponent10);
    checkType<int>(d32_limits::max_exponent);
    checkType<int>(d32_limits::max_exponent10);
    checkType<bool>(d32_limits::has_infinity);
    checkType<bool>(d32_limits::has_quiet_NaN);
    checkType<bool>(d32_limits::has_signaling_NaN);
    checkType<std::float_denorm_style>(d32_limits::has_denorm);
    checkType<bool>(d32_limits::has_denorm_loss);
    checkType<BDEC::Decimal32>(d32_limits::infinity());
    checkType<BDEC::Decimal32>(d32_limits::quiet_NaN());
    checkType<BDEC::Decimal32>(d32_limits::signaling_NaN());
    checkType<BDEC::Decimal32>(d32_limits::denorm_min());
    checkType<bool>(d32_limits::is_iec559);
    checkType<bool>(d32_limits::is_bounded);
    checkType<bool>(d32_limits::is_modulo);
    checkType<bool>(d32_limits::traps);
    checkType<bool>(d32_limits::tinyness_before);
    checkType<std::float_round_style>(d32_limits::round_style);
}
