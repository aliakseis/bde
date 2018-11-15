// bdldfp_decimal.cpp                                                 -*-C++-*-
#include <bdldfp_decimal.h>


#include <bdldfp_decimalimputil.h>
#include <bdldfp_decimalimputil_decnumber.h>
#include <bsls_performancehint.h>
#include <bslim_printer.h>

#include <istream>


#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
#include <math.h>
#include <stdio.h>
#endif

// For better ways of binary-decimal FP conversion see:
// http://www.serpentine.com/blog/2011/06/29/...
//  ...here-be-dragons-advances-in-problems-you-didnt-even-know-you-had/
// http://tinyurl.com/3go5o7z

// Implementation notes:
//
// 48 buffer size for printing binary floats: according to the IEEE specs
// _Decimal128 would need a maximum 44 character buffer for scientific format
//  printing (34 digits, 4 chars for +/- of the mantissa and exponent, the 'e'
// for scientific, 1 more for the decimal point and then 1 more for the NUL
// terminator).  Since we would like to be cache friendly we use 48 (dividable
// by 8, so it is also alignment friendly).  For simplicity, we use the same
// size everywhere.  (see buff[48] in the code).
//
// 24 characters buffer is used for printing integers.  Unsigned long long (64
// bits) may print 20 digits at its max value.  The test driver contains an
// assert to check that 24 is indeed enough.  Why don't we use the
// numeric_limits<>::digits10?  Because it is "helpfully" broken on MS Visual
// C++ 2008, reports 18 & not 19.


namespace BloombergLP {
namespace bdldfp {

namespace {
                    // ================
                    // class NotIsSpace
                    // ================

template <class CHARTYPE>
class NotIsSpace {
    // Helper function object type used to skip spaces in strings
    const std::ctype<CHARTYPE>& d_ctype;
  public:
    explicit NotIsSpace(const std::ctype<CHARTYPE>& ctype);
        // Construct a 'NotIsSpace' object, using the specified 'ctype'.
    bool operator()(CHARTYPE character) const;
        // Return 'true' if the specified 'character' is a space (according to
        // the 'ctype' provided at construction), and 'false' otherwise.
};

                    // ----------------
                    // class NotIsSpace
                    // ----------------

template <class CHARTYPE>
NotIsSpace<CHARTYPE>::NotIsSpace(const std::ctype<CHARTYPE>& ctype)
: d_ctype(ctype)
{
}

template <class CHARTYPE>
bool
NotIsSpace<CHARTYPE>::operator()(CHARTYPE character) const
{
    return !this->d_ctype.is(std::ctype_base::space, character);
}

                         // Print helper function

template <class CHARTYPE, class TRAITS, class DECIMAL>
std::basic_ostream<CHARTYPE, TRAITS>&
printImpl(std::basic_ostream<CHARTYPE, TRAITS>& out,
          DECIMAL                               value)
{
    try {
        typename std::basic_ostream<CHARTYPE, TRAITS>::sentry kerberos(out);
        if (kerberos) {
            typedef BloombergLP::bdldfp::DecimalNumPut<CHARTYPE> Facet;
            const Facet& facet(std::has_facet<Facet>(out.getloc())
                               ? std::use_facet<Facet>(out.getloc())
                               : Facet::object());

            std::ostreambuf_iterator<CHARTYPE, TRAITS> itr =
                facet.put(std::ostreambuf_iterator<CHARTYPE, TRAITS>(out),
                          out,
                          out.fill(),
                          value);
            if (itr.failed()) {
                out.setstate(std::ios::failbit | std::ios::badbit);
            }
        }
    }
    catch(...) {
        out.setstate(std::ios::badbit);
    }
    return out;
}

                         // Read helper function

template <class CHARTYPE, class TRAITS, class DECIMAL>
std::basic_istream<CHARTYPE, TRAITS>&
read(std::basic_istream<CHARTYPE, TRAITS>& in,
     DECIMAL&                              value)
{
    typename std::basic_istream<CHARTYPE, TRAITS>::sentry kerberos(in);
    if (kerberos) {
        typedef BloombergLP::bdldfp::DecimalNumGet<CHARTYPE> Facet;
        const Facet& facet(std::has_facet<Facet>(in.getloc())
                           ? std::use_facet<Facet>(in.getloc())
                           : Facet::object());
        std::ios_base::iostate err = std::ios_base::iostate();
        facet.get(std::istreambuf_iterator<CHARTYPE, TRAITS>(in),
                  std::istreambuf_iterator<CHARTYPE, TRAITS>(),
                  in,
                  err,
                  value);
        if (err) {
            in.setstate(err);
        }
    }
    return in;
}


template <class ITER_TYPE, class CHAR_TYPE>
ITER_TYPE fillN(ITER_TYPE iter, int numCharacters, CHAR_TYPE character)
    // Assign to the specified output 'iter' the specified 'character' the
    // specified 'numCharacters' times, incrementing 'iter' between each
    // assignment, and then return the resulting incremented iterator.   Note
    // that this is an implementation of C++11 standard 'std::fill_n' that has
    // been specifialized slightly for filling characters; it is provided here
    // because the C++98 definition of 'fill_n' returns 'void'.
{
  while (numCharacters > 0) {
    *iter = character;
    ++iter;
    --numCharacters;
  }
  return iter;
}

template <class ITER_TYPE, class CHAR_TYPE>
ITER_TYPE
doPutCommon(ITER_TYPE       out,
            std::ios_base&  format,
            CHAR_TYPE       fillCharacter,
            char           *buffer)
    // Widen the specified 'buffer' into a string of the specified 'CHAR_TYPE',
    // and output the represented decimal number to the specified 'out',
    // adjusting for the formatting flags in the specified 'format' and using
    // the specified 'fillCharacter'.  Currently, formatting for the
    // formatting flags of justification, width, uppercase, and showpos are
    // supported.
{
    const int size = static_cast<int>(std::strlen(buffer));
    char *end = buffer + size;

    // Widen the buffer.
    CHAR_TYPE wbuffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];

    std::use_facet<std::ctype<CHAR_TYPE> >(
                                  format.getloc()).widen(buffer, end, wbuffer);

    const int  width   = static_cast<int>(format.width());
    const bool showPos = format.flags() & std::ios_base::showpos;
    const bool hasSign = wbuffer[0] == std::use_facet<std::ctype<CHAR_TYPE> >(
                                                 format.getloc()).widen('-') ||
                         wbuffer[0] == std::use_facet<std::ctype<CHAR_TYPE> >(
                                                 format.getloc()).widen('+');
    const bool addPlusSign = showPos & !hasSign;  // Do we need to add '+'?

    int surplus = std::max(0, width - size);  // Emit this many fillers.
    if (addPlusSign) {
        // Need to add a '+' character.
        --surplus;
    }

    CHAR_TYPE *wend       = wbuffer + size;
    CHAR_TYPE *wbufferPos = wbuffer;


    // Make use of the 'uppercase' flag to fix the capitalization of the
    // alphabets in the number.

    if (format.flags() & std::ios_base::uppercase) {
        std::use_facet<std::ctype<CHAR_TYPE> >(
                                       format.getloc()).toupper(wbuffer, wend);
    }
    else {
        std::use_facet<std::ctype<CHAR_TYPE> >(
                                       format.getloc()).tolower(wbuffer, wend);
    }

    switch (format.flags() & std::ios_base::adjustfield) {
      case std::ios_base::left: {

          // Left justify. Pad characters to the right.

          if (addPlusSign) {
              *out++ = '+';
          }

          out = std::copy(wbufferPos, wend, out);
          out = fillN(out, surplus, fillCharacter);
          break;
      }

      case std::ios_base::internal: {

          // Internal justify. Pad characters after sign.

          if (hasSign) {
              *out++ = *wbufferPos++;
          }
          else if (addPlusSign) {
              *out++ = '+';
          }

          out = fillN(out, surplus, fillCharacter);
          out = std::copy(wbufferPos, wend, out);
          break;
      }

      case std::ios_base::right:
      default: {

          // Right justify. Pad characters to the left.

          out = fillN(out, surplus, fillCharacter);

          if (addPlusSign) {
              *out++ = '+';
          }

          out = std::copy(wbufferPos, wend, out);
          break;
      }
    }

    return out;
}

template <class CHAR_TYPE, class ITER_TYPE>
ITER_TYPE
doGetCommon(ITER_TYPE                    begin,
            ITER_TYPE                    end,
            std::ctype<CHAR_TYPE> const& ctype,
            char*                        to,
            const char*                  toEnd,
            CHAR_TYPE                    separator,
            bool                        *hasDigit)
    // Gather (narrowed versions of) characters in the specified range
    // ['begin'..'end') that syntactically form a floating-point number
    // (including 'NaN', 'Inf', and 'Infinity') into the buffer defined by the
    // specified range ['to'..'toEnd'), qualifying (and, where needed,
    // converting) input characters using the specified 'ctype', accepting
    // instances of the specified digit group separator 'separator'.  On
    // success, sets the referent of the specified 'hasDigit' to 'true';
    // otherwise, 'false'.  Returns an iterator prior to 'end' indicating the
    // last character examined, or equal to 'end' if parsing terminated there.
{
    *hasDigit = false;
    // optional sign
    if (begin != end) {
        char sign = ctype.narrow(*begin, ' ');
        if ((sign == '-' || sign == '+') && to != toEnd) {
            *to = sign;
            ++to, ++begin;
        }
    }
    // spaces between sign and value
    begin = std::find_if(begin, end, NotIsSpace<CHAR_TYPE>(ctype));
    // non-fractional part
    while (begin != end && to != toEnd
             && (ctype.is(std::ctype_base::digit, *begin)
                 || *begin == separator)) {
        if (*begin != separator) {
                //-dk:TODO TBD store separators for later check
            *hasDigit = true;
            *to = ctype.narrow(*begin, ' ');
            ++to;
        }
        ++begin;
    }
    // fractional part
    if (begin != end && to != toEnd && ctype.narrow(*begin, ' ') == '.') {
            // -nm:TODO TBD use numpunct notion of decimal separator
        *to = '.';
        ++to, ++begin;
        char* start = to;
        while (begin != end && to != toEnd
                 && ctype.is(std::ctype_base::digit, *begin)) {
            *hasDigit = true;
            *to = ctype.narrow(*begin, ' ');
            ++begin, ++to;
        }
        if (start == to && !*hasDigit) {
                 // A fractional-part needs at least one digit, somewhere.
            return begin;                                             // RETURN
        }
    }
    // exponent (but not a stand-alone exponent)
    if (*hasDigit && begin != end && to != toEnd
            && ctype.narrow(ctype.tolower(*begin), ' ') == 'e') {
        *to = 'e';
        ++to, ++begin;
        // optional exponent sign
        if (begin != end) {
            char sign = ctype.narrow(*begin, ' ');
            if ((sign == '-' || sign == '+') && to != toEnd) {
                *to = sign;
                ++to, ++begin;
            }
        }
        char* start = to;
        while (begin != end && to != toEnd
                && ctype.is(std::ctype_base::digit, *begin)) {
            *to = ctype.narrow(*begin, ' ');
            ++to, ++begin;
        }
        if (start == to) { // exponent needs to have at least one digit
            *hasDigit = false;
            return begin;                                             // RETURN
        }
    }

    // inf, -inf, +inf, -nan, +nan, or nan

    if (!*hasDigit && begin != end && to != toEnd) {
        const char pats[] = "0infinity\0nan";
        char c = ctype.narrow(ctype.tolower(*begin), ' ');
        int infNanPos = (c == pats[1]) ? 1 : (c == pats[10]) ? 10 : 0;
        if (infNanPos != 0) {
            do {
                *to++ = pats[infNanPos++], ++begin;
            } while (begin != end && to != toEnd &&
                ctype.narrow(ctype.tolower(*begin), ' ') == pats[infNanPos]);
        }
        if ((pats[infNanPos] == '\0' || infNanPos == 4) &&
                (begin == end || !ctype.is(std::ctype_base::alpha, *begin))) {
            *hasDigit = true;
        }
    }
    if (*hasDigit) {
        if (to != toEnd) {
            *to++ = '\0';
        } else {
            *hasDigit = false;
        }
    }
    return begin;
}

}  // close unnamed namespace


                            // --------------------
                            // class Decimal_Type64
                            // --------------------

// ACCESSORS
std::ostream& Decimal_Type64::print(std::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(stream.bad())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);
    printImpl(stream, *this);
    printer.end(true);
    return stream;
}

                            // -------------------
                            // class DecimalNumGet
                            // -------------------

template <class CHARTYPE, class INPUTITERATOR>
std::locale::id DecimalNumGet<CHARTYPE, INPUTITERATOR>::id;

#ifdef BSLS_PLATFORM_CMP_SUN
template <class CHARTYPE, class INPUTITERATOR>
bsl::locale::id& DecimalNumGet<CHARTYPE, INPUTITERATOR>::__get_id() const
{
    return id;
}
#endif

template <class CHARTYPE, class INPUTITERATOR>
const DecimalNumGet<CHARTYPE, INPUTITERATOR>&
DecimalNumGet<CHARTYPE, INPUTITERATOR>::object()
{
    static DecimalNumGet<CHARTYPE, INPUTITERATOR> rc;
    return rc;
}

template <class CHARTYPE, class INPUTITERATOR>
DecimalNumGet<CHARTYPE, INPUTITERATOR>::DecimalNumGet(std::size_t refs)
    : std::locale::facet(refs)
{
}

template <class CHARTYPE, class INPUTITERATOR>
DecimalNumGet<CHARTYPE, INPUTITERATOR>::~DecimalNumGet()
{
}

template <class CHARTYPE, class INPUTITERATOR>
typename DecimalNumGet<CHARTYPE, INPUTITERATOR>::iter_type
DecimalNumGet<CHARTYPE, INPUTITERATOR>::get(
                                           iter_type               begin,
                                           iter_type               end,
                                           std::ios_base&          str,
                                           std::ios_base::iostate& err,
                                           Decimal32&              value) const
{
    return this->do_get(begin, end, str, err, value);
}
template <class CHARTYPE, class INPUTITERATOR>
typename DecimalNumGet<CHARTYPE, INPUTITERATOR>::iter_type
DecimalNumGet<CHARTYPE, INPUTITERATOR>::get(
                                           iter_type               begin,
                                           iter_type               end,
                                           std::ios_base&          str,
                                           std::ios_base::iostate& err,
                                           Decimal64&              value) const
{
    return this->do_get(begin, end, str, err, value);
}
template <class CHARTYPE, class INPUTITERATOR>
typename DecimalNumGet<CHARTYPE, INPUTITERATOR>::iter_type
DecimalNumGet<CHARTYPE, INPUTITERATOR>::get(
                                           iter_type               begin,
                                           iter_type               end,
                                           std::ios_base&          str,
                                           std::ios_base::iostate& err,
                                           Decimal128&             value) const
{
    return this->do_get(begin, end, str, err, value);
}

                        // do_get Functions

template <class CHARTYPE, class INPUTITERATOR>
typename DecimalNumGet<CHARTYPE, INPUTITERATOR>::iter_type
DecimalNumGet<CHARTYPE, INPUTITERATOR>::do_get(
                                           iter_type               begin,
                                           iter_type               end,
                                           std::ios_base&          str,
                                           std::ios_base::iostate& err,
                                           Decimal32&              value) const
{
    typedef std::ctype<CHARTYPE> Ctype;
    Ctype const& ctype(std::use_facet<Ctype>(str.getloc()));

    char        buffer[512];
    char*       to(buffer);
    char* const toEnd(buffer + (sizeof(buffer) - 1));
    CHARTYPE    separator(std::use_facet<std::numpunct<CHARTYPE> >(
                                                str.getloc()).thousands_sep());
    bool        hasDigit(false);

    begin = doGetCommon(begin, end, ctype, to, toEnd, separator, &hasDigit);
    if (hasDigit) {
        value = DecimalImpUtil::parse32(buffer);
    } else {
        err = std::ios_base::failbit;
    }
    return begin;
}
template <class CHARTYPE, class INPUTITERATOR>
typename DecimalNumGet<CHARTYPE, INPUTITERATOR>::iter_type
DecimalNumGet<CHARTYPE, INPUTITERATOR>::do_get(
                                           iter_type               begin,
                                           iter_type               end,
                                           std::ios_base&          str,
                                           std::ios_base::iostate& err,
                                           Decimal64&              value) const
{
    typedef std::ctype<CHARTYPE> Ctype;
    Ctype const& ctype(std::use_facet<Ctype>(str.getloc()));

    char        buffer[512];
    char*       to(buffer);
    char* const toEnd(buffer + (sizeof(buffer) - 1));
    CHARTYPE    separator(std::use_facet<std::numpunct<CHARTYPE> >(
                                                str.getloc()).thousands_sep());
    bool        hasDigit(false);

    begin = doGetCommon(begin, end, ctype, to, toEnd, separator, &hasDigit);
    if (hasDigit) {
        value = DecimalImpUtil::parse64(buffer);
    } else {
        err = std::ios_base::failbit;
    }
    return begin;
}
template <class CHARTYPE, class INPUTITERATOR>
typename DecimalNumGet<CHARTYPE, INPUTITERATOR>::iter_type
DecimalNumGet<CHARTYPE, INPUTITERATOR>::do_get(
                                           iter_type               begin,
                                           iter_type               end,
                                           std::ios_base&          str,
                                           std::ios_base::iostate& err,
                                           Decimal128&             value) const
{
    typedef std::ctype<CHARTYPE> Ctype;
    Ctype const& ctype(std::use_facet<Ctype>(str.getloc()));

    char        buffer[512];
    char*       to(buffer);
    char* const toEnd(buffer + (sizeof(buffer) - 1));
    CHARTYPE    separator(std::use_facet<std::numpunct<CHARTYPE> >(
                                                str.getloc()).thousands_sep());
    bool        hasDigit(false);

    begin = doGetCommon(begin, end, ctype, to, toEnd, separator, &hasDigit);
    if (hasDigit) {
        value = DecimalImpUtil::parse128(buffer);
    } else {
        err = std::ios_base::failbit;
    }
    return begin;
}

                            // -------------------
                            // class DecimalNumPut
                            // -------------------

template <class CHARTYPE, class OUTPUTITERATOR>
std::locale::id DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::id;

#ifdef BSLS_PLATFORM_CMP_SUN
template <class CHARTYPE, class OUTPUTITERATOR>
std::locale::id& DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::__get_id() const
{
    return id;
}
#endif

template <class CHARTYPE, class OUTPUTITERATOR>
const DecimalNumPut<CHARTYPE, OUTPUTITERATOR>&
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::object()
{
    static DecimalNumPut<CHARTYPE, OUTPUTITERATOR> rc;
    return rc;
}

template <class CHARTYPE, class OUTPUTITERATOR>
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::DecimalNumPut(std::size_t refs)
    : std::locale::facet(refs)
{
}

template <class CHARTYPE, class OUTPUTITERATOR>
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::~DecimalNumPut()
{
}

template <class CHARTYPE, class OUTPUTITERATOR>
typename DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::iter_type
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::put(iter_type      out,
                                             std::ios_base& str,
                                             char_type      fill,
                                             Decimal32      value) const
{
    return this->do_put(out, str, fill, value);
}
template <class CHARTYPE, class OUTPUTITERATOR>
typename DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::iter_type
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::put(iter_type      out,
                                             std::ios_base& str,
                                             char_type      fill,
                                             Decimal64      value) const
{
    return this->do_put(out, str, fill, value);
}
template <class CHARTYPE, class OUTPUTITERATOR>
typename DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::iter_type
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::put(iter_type      out,
                                             std::ios_base& str,
                                             char_type      fill,
                                             Decimal128     value) const
{
    return this->do_put(out, str, fill, value);
}

template <class CHARTYPE, class OUTPUTITERATOR>
typename DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::iter_type
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::do_put(iter_type      out,
                                                std::ios_base& ios_format,
                                                char_type      fill,
                                                Decimal32      value) const
{
    char  buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];

    DenselyPackedDecimalImpUtil::StorageType32 dpdStorage;
    dpdStorage = DecimalImpUtil::convertToDPD(*value.data());

    DecimalImpUtil_DecNumber::ValueType32 dpdValue;
    std::memcpy(&dpdValue, &dpdStorage, sizeof(dpdValue));

    DecimalImpUtil_DecNumber::format(dpdValue, buffer);

    return doPutCommon(out, ios_format, fill, &buffer[0]);
}
template <class CHARTYPE, class OUTPUTITERATOR>
typename DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::iter_type
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::do_put(iter_type      out,
                                                std::ios_base& ios_format,
                                                char_type      fill,
                                                Decimal64      value) const
{
    char  buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];

    DenselyPackedDecimalImpUtil::StorageType64 dpdStorage;
    dpdStorage = DecimalImpUtil::convertToDPD(*value.data());

    DecimalImpUtil_DecNumber::ValueType64 dpdValue;
    std::memcpy(&dpdValue, &dpdStorage, sizeof(dpdValue));

    DecimalImpUtil_DecNumber::format(dpdValue, buffer);

    return doPutCommon(out, ios_format, fill, &buffer[0]);
}
template <class CHARTYPE, class OUTPUTITERATOR>
typename DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::iter_type
DecimalNumPut<CHARTYPE, OUTPUTITERATOR>::do_put(iter_type      out,
                                                std::ios_base& ios_format,
                                                char_type      fill,
                                                Decimal128     value) const
{
    char  buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];

    DenselyPackedDecimalImpUtil::StorageType128 dpdStorage;
    dpdStorage = DecimalImpUtil::convertToDPD(*value.data());

    DecimalImpUtil_DecNumber::ValueType128 dpdValue;
    std::memcpy(&dpdValue, &dpdStorage, sizeof(dpdValue));

    DecimalImpUtil_DecNumber::format(dpdValue, buffer);

    return doPutCommon(out, ios_format, fill, &buffer[0]);
}

                       // Explicit instantiations

template class DecimalNumPut<char, std::ostreambuf_iterator<char> >;
template class DecimalNumPut<wchar_t, std::ostreambuf_iterator<wchar_t> >;

}  // close package namespace

                     // Streaming operators implementations

                                  // Output

template <class CHARTYPE, class TRAITS>
std::basic_ostream<CHARTYPE, TRAITS>&
bdldfp::operator<<(std::basic_ostream<CHARTYPE, TRAITS>& stream,
                   Decimal32                             object)
{
    return printImpl(stream, object);
}

template <class CHARTYPE, class TRAITS>
std::basic_ostream<CHARTYPE, TRAITS>&
bdldfp::operator<<(std::basic_ostream<CHARTYPE, TRAITS>& stream,
                   Decimal64                             object)
{
    return printImpl(stream, object);
}

template <class CHARTYPE, class TRAITS>
std::basic_ostream<CHARTYPE, TRAITS>&
bdldfp::operator<<(std::basic_ostream<CHARTYPE, TRAITS>& stream,
                   Decimal128                            object)
{
    return printImpl(stream, object);
}

                                  // Input

template <class CHARTYPE, class TRAITS>
std::basic_istream<CHARTYPE, TRAITS>&
bdldfp::operator>>(std::basic_istream<CHARTYPE, TRAITS>& stream,
                   Decimal32&                            object)
{
    return read(stream, object);
}

template <class CHARTYPE, class TRAITS>
std::basic_istream<CHARTYPE, TRAITS>&
bdldfp::operator>>(std::basic_istream<CHARTYPE, TRAITS>& stream,
                   Decimal64&                            object)
{
    return read(stream, object);
}

template <class CHARTYPE, class TRAITS>
std::basic_istream<CHARTYPE, TRAITS>&
bdldfp::operator>>(std::basic_istream<CHARTYPE, TRAITS>& stream,
                   Decimal128&                           object)
{
    return read(stream, object);
}

                // Streaming operators explicit instantiations

                                // Decimal32

template
std::basic_istream<char>&
bdldfp::operator>> <char, std::char_traits<char> >(
                                        std::basic_istream<char>& in,
                                        bdldfp::Decimal32&        value);
template
std::basic_istream<wchar_t>&
bdldfp::operator>> <wchar_t, std::char_traits<wchar_t> >(
                                        std::basic_istream<wchar_t>& in,
                                        bdldfp::Decimal32&           value);

template
std::basic_ostream<char>&
bdldfp::operator<< <char, std::char_traits<char> >(
                                         std::basic_ostream<char>& out,
                                         bdldfp::Decimal32         value);
template
std::basic_ostream<wchar_t>&
bdldfp::operator<< <wchar_t, std::char_traits<wchar_t> >(
                                         std::basic_ostream<wchar_t>& out,
                                         bdldfp::Decimal32            value);

                                // Decimal64

template
std::basic_istream<char>&
bdldfp::operator>> <char, std::char_traits<char> >(
                                        std::basic_istream<char>& in,
                                        bdldfp::Decimal64&        value);
template
std::basic_istream<wchar_t>&
bdldfp::operator>> <wchar_t, std::char_traits<wchar_t> >(
                                        std::basic_istream<wchar_t>& in,
                                        bdldfp::Decimal64&           value);

template
std::basic_ostream<char>&
bdldfp::operator<< <char, std::char_traits<char> >(
                                         std::basic_ostream<char>& out,
                                         bdldfp::Decimal64         value);
template
std::basic_ostream<wchar_t>&
bdldfp::operator<< <wchar_t, std::char_traits<wchar_t> >(
                                         std::basic_ostream<wchar_t>& out,
                                         bdldfp::Decimal64            value);

                                // Decimal128

template
std::basic_istream<char>&
bdldfp::operator>> <char, std::char_traits<char> >(
                                       std::basic_istream<char>& in,
                                       bdldfp::Decimal128&       value);
template
std::basic_istream<wchar_t>&
bdldfp::operator>> <wchar_t, std::char_traits<wchar_t> >(
                                       std::basic_istream<wchar_t>& in,
                                       bdldfp::Decimal128&          value);

template
std::basic_ostream<char>&
bdldfp::operator<< <char, std::char_traits<char> >(
                                        std::basic_ostream<char>& out,
                                        bdldfp::Decimal128        value);
template
std::basic_ostream<wchar_t>&
bdldfp::operator<< <wchar_t, std::char_traits<wchar_t> >(
                                        std::basic_ostream<wchar_t>& out,
                                        bdldfp::Decimal128           value);

}  // close enterprise namespace
                  // std::numeric_limits<> specializations

#if defined(BSL_OVERRIDES_STD) && defined(std)
#   undef std
#   define BDLDFP_DECIMAL_RESTORE_STD
#endif

            // ---------------------------------------------------
            // std::numeric_limits<BloombergLP::bdldfp::Decimal32>
            // ---------------------------------------------------

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::min()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::min32();
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::max()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::max32();
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::epsilon()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::epsilon32();
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::round_error()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::roundError32();
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::infinity()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::infinity32();
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::quiet_NaN()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::quietNaN32();
}

BloombergLP::bdldfp::Decimal32
   std::numeric_limits<BloombergLP::bdldfp::Decimal32>::signaling_NaN()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::signalingNaN32();
}

BloombergLP::bdldfp::Decimal32
    std::numeric_limits<BloombergLP::bdldfp::Decimal32>::denorm_min()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::denormMin32();
}

            // ---------------------------------------------------
            // std::numeric_limits<BloombergLP::bdldfp::Decimal64>
            // ---------------------------------------------------

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::min()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::min64();
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::max()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::max64();
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::epsilon()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::epsilon64();
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::round_error()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::roundError64();
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::infinity()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::infinity64();
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::quiet_NaN()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::quietNaN64();
}

BloombergLP::bdldfp::Decimal64
   std::numeric_limits<BloombergLP::bdldfp::Decimal64>::signaling_NaN()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::signalingNaN64();
}

BloombergLP::bdldfp::Decimal64
    std::numeric_limits<BloombergLP::bdldfp::Decimal64>::denorm_min()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::denormMin64();
}

            // ----------------------------------------------------
            // std::numeric_limits<BloombergLP::bdldfp::Decimal128>
            // ----------------------------------------------------

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::min()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::min128();
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::max()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::max128();
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::epsilon()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::epsilon128();
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::round_error()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::roundError128();
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::infinity()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::infinity128();
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::quiet_NaN()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::quietNaN128();
}

BloombergLP::bdldfp::Decimal128
   std::numeric_limits<BloombergLP::bdldfp::Decimal128>::signaling_NaN()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::signalingNaN128();
}

BloombergLP::bdldfp::Decimal128
    std::numeric_limits<BloombergLP::bdldfp::Decimal128>::denorm_min()
    noexcept
{
    return BloombergLP::bdldfp::DecimalImpUtil::denormMin128();
}



///////////////////////////////////////////////////////////////////////////////

// ============================================================================
//                            FORMERLY INLINE DEFINITIONS
// ============================================================================


namespace BloombergLP {
namespace bdldfp {

                      // THE DECIMAL FLOATING-POINT TYPES

                            // --------------------
                            // class Decimal_Type32
                            // --------------------


Decimal_Type32::Decimal_Type32() : d_value{}
{
}


Decimal_Type32::Decimal_Type32(DecimalImpUtil::ValueType32 value)
: d_value(value)
{
}


Decimal_Type32::Decimal_Type32(Decimal_Type64 other)
: d_value(DecimalImpUtil::convertToDecimal32(*other.data()))
{
}


Decimal_Type32::Decimal_Type32(float other)
: d_value(DecimalImpUtil::binaryToDecimal32(other))
{
}


Decimal_Type32::Decimal_Type32(double other)
: d_value(DecimalImpUtil::binaryToDecimal32(other))
{
}


Decimal_Type32::Decimal_Type32(int other)
: d_value(DecimalImpUtil::int32ToDecimal32(other))
{
}


Decimal_Type32::Decimal_Type32(unsigned int other)
: d_value(DecimalImpUtil::uint32ToDecimal32(other))
{
}


Decimal_Type32::Decimal_Type32(long int other)
: d_value(DecimalImpUtil::int64ToDecimal32(other))
{
}


Decimal_Type32::Decimal_Type32(unsigned long int other)
: d_value(DecimalImpUtil::uint64ToDecimal32(other))
{
}


Decimal_Type32::Decimal_Type32(long long other)
: d_value(DecimalImpUtil::int64ToDecimal32(other))
{
}


Decimal_Type32::Decimal_Type32(unsigned long long other)
: d_value(DecimalImpUtil::uint64ToDecimal32(other))
{
}


Decimal_Type32::Decimal_Type32(const char* value)
    : d_value(DecimalImpUtil::parse32(value))
{
}



DecimalImpUtil::ValueType32 *Decimal_Type32::data()
{
    return &d_value;
}


const DecimalImpUtil::ValueType32 *Decimal_Type32::data() const
{
    return &d_value;
}


DecimalImpUtil::ValueType32 Decimal_Type32::value() const
{
    return d_value;
}

                            // --------------------
                            // class Decimal_Type64
                            // --------------------

// CREATORS

Decimal_Type64::Decimal_Type64() : d_value{}
{
}


Decimal_Type64::Decimal_Type64(DecimalImpUtil::ValueType64 value)
: d_value(value)
{
}


Decimal_Type64::Decimal_Type64(Decimal32 other)
: d_value(DecimalImpUtil::convertToDecimal64(*other.data()))
{
}


Decimal_Type64::Decimal_Type64(Decimal128 other)
: d_value(DecimalImpUtil::convertToDecimal64(*other.data()))
{
}

                     // Numerical Conversion Constructors


Decimal_Type64::Decimal_Type64(float other)
: d_value(DecimalImpUtil::binaryToDecimal64(other))
{
}


Decimal_Type64::Decimal_Type64(double other)
: d_value(DecimalImpUtil::binaryToDecimal64(other))
{
}

                      // Integral Conversion Constructors


Decimal_Type64::Decimal_Type64(int other)
: d_value(DecimalImpUtil::int32ToDecimal64(other))
{
}


Decimal_Type64::Decimal_Type64(unsigned int other)
: d_value(DecimalImpUtil::uint32ToDecimal64(other))
{
}


Decimal_Type64::Decimal_Type64(long other)
: d_value(DecimalImpUtil::int64ToDecimal64(other))
{
}


Decimal_Type64::Decimal_Type64(unsigned long other)
: d_value(DecimalImpUtil::uint64ToDecimal64(other))
{
}


Decimal_Type64::Decimal_Type64(long long other)
: d_value(DecimalImpUtil::int64ToDecimal64(other))
{
}


Decimal_Type64::Decimal_Type64(unsigned long long other)
: d_value(DecimalImpUtil::uint64ToDecimal64(other))
{
}


Decimal_Type64::Decimal_Type64(const char* value)
    : d_value(DecimalImpUtil::parse64(value))
{
}


// MANIPULATORS

                     // Incrementation and Decrementation

 Decimal_Type64& Decimal_Type64::operator++()
{
    return *this += Decimal64(1);
}

 Decimal_Type64& Decimal_Type64::operator--()
{
    return *this -= Decimal64(1);
}

                                  // Addition

 Decimal_Type64& Decimal_Type64::operator+=(Decimal32 rhs)
{
    return *this += Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator+=(Decimal64 rhs)
{
    this->d_value = DecimalImpUtil::add(this->d_value, rhs.d_value);
    return *this;
}

 Decimal_Type64& Decimal_Type64::operator+=(Decimal128 rhs)
{
    return *this += Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator+=(int rhs)
{
    return *this += Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator+=(unsigned int rhs)
{
    return *this += Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator+=(long rhs)
{
    return *this += Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator+=(unsigned long rhs)
{
    return *this += Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator+=(long long rhs)
{
    return *this += Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator+=(unsigned long long rhs)
{
    return *this += Decimal64(rhs);
}

                                // Subtraction

 Decimal_Type64& Decimal_Type64::operator-=(Decimal32 rhs)
{
    return *this -= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator-=(Decimal64 rhs)
{
    this->d_value = DecimalImpUtil::subtract(this->d_value, rhs.d_value);
    return *this;
}

 Decimal_Type64& Decimal_Type64::operator-=(Decimal128 rhs)
{
    return *this -= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator-=(int rhs)
{
    return *this -= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator-=(unsigned int rhs)
{
    return *this -= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator-=(long rhs)
{
    return *this -= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator-=(unsigned long rhs)
{
    return *this -= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator-=(long long rhs)
{
    return *this -= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator-=(unsigned long long rhs)
{
    return *this -= Decimal64(rhs);
}

                               // Multiplication

 Decimal_Type64& Decimal_Type64::operator*=(Decimal32 rhs)
{
    return *this *= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator*=(Decimal64 rhs)
{
    this->d_value = DecimalImpUtil::multiply(this->d_value, rhs.d_value);
    return *this;
}

 Decimal_Type64& Decimal_Type64::operator*=(Decimal128 rhs)
{
    return *this *= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator*=(int rhs)
{
    return *this *= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator*=(unsigned int rhs)
{
    return *this *= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator*=(long rhs)
{
    return *this *= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator*=(unsigned long rhs)
{
    return *this *= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator*=(long long rhs)
{
    return *this *= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator*=(unsigned long long rhs)
{
    return *this *= Decimal64(rhs);
}

                                  // Division

 Decimal_Type64& Decimal_Type64::operator/=(Decimal32 rhs)
{
    return *this /= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator/=(Decimal64 rhs)
{
    this->d_value = DecimalImpUtil::divide(this->d_value, rhs.d_value);
    return *this;
}

 Decimal_Type64& Decimal_Type64::operator/=(Decimal128 rhs)
{
    return *this /= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator/=(int rhs)
{
    return *this /= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator/=(unsigned int rhs)
{
    return *this /= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator/=(long rhs)
{
    return *this /= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator/=(unsigned long rhs)
{
    return *this /= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator/=(long long rhs)
{
    return *this /= Decimal64(rhs);
}

 Decimal_Type64& Decimal_Type64::operator/=(unsigned long long rhs)
{
    return *this /= Decimal64(rhs);
}

//ACCESSORS

                            // Internals Accessors

 DecimalImpUtil::ValueType64 *Decimal_Type64::data()
{
    return &d_value;
}

 const DecimalImpUtil::ValueType64 *Decimal_Type64::data() const
{
    return &d_value;
}

 DecimalImpUtil::ValueType64 Decimal_Type64::value() const
{
    return d_value;
}

                           // ---------------------
                           // class Decimal_Type128
                           // ---------------------


Decimal_Type128::Decimal_Type128() : d_value{}
{
}


Decimal_Type128::Decimal_Type128(DecimalImpUtil::ValueType128 value)
: d_value(value)
{
}


Decimal_Type128::Decimal_Type128(Decimal32 value)
: d_value(DecimalImpUtil::convertToDecimal128(*value.data()))
{
}


Decimal_Type128::Decimal_Type128(Decimal64 value)
: d_value(DecimalImpUtil::convertToDecimal128(*value.data()))
{
}


Decimal_Type128::Decimal_Type128(float other)
: d_value(DecimalImpUtil::binaryToDecimal128(other))
{
}


Decimal_Type128::Decimal_Type128(double other)
: d_value(DecimalImpUtil::binaryToDecimal128(other))
{
}


Decimal_Type128::Decimal_Type128(int value)
: d_value(DecimalImpUtil::int32ToDecimal128(value))
{
}

 Decimal_Type128::Decimal_Type128(unsigned int value)
: d_value(DecimalImpUtil::uint32ToDecimal128(value))
{
}

 Decimal_Type128::Decimal_Type128(long value)
: d_value(DecimalImpUtil::int64ToDecimal128(value))
{
}

 Decimal_Type128::Decimal_Type128(unsigned long value)
: d_value(DecimalImpUtil::uint64ToDecimal128(value))
{
}

 Decimal_Type128::Decimal_Type128(long long value)
: d_value(DecimalImpUtil::int64ToDecimal128(value))
{
}

 Decimal_Type128::Decimal_Type128(unsigned long long value)
: d_value(DecimalImpUtil::uint64ToDecimal128(value))
{
}


 Decimal_Type128::Decimal_Type128(const char* value)
     : d_value(DecimalImpUtil::parse128(value))
 {
 }



Decimal_Type128& Decimal_Type128::operator++()
{
    return *this += Decimal128(1);
}


Decimal_Type128& Decimal_Type128::operator--()
{
    return *this -= Decimal128(1);
}

                                  // Addition


Decimal_Type128& Decimal_Type128::operator+=(Decimal32 rhs)
{
    return *this += Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator+=(Decimal64 rhs)
{
    return *this += Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator+=(Decimal128 rhs)
{
    this->d_value = DecimalImpUtil::add(this->d_value, rhs.d_value);
    return *this;
}


Decimal_Type128& Decimal_Type128::operator+=(int rhs)
{
    return *this += Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator+=(unsigned int rhs)
{
    return *this += Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator+=(long rhs)
{
    return *this += Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator+=(unsigned long rhs)
{
    return *this += Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator+=(long long rhs)
{
    return *this += Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator+=(unsigned long long rhs)
{
    return *this += Decimal128(rhs);
}

                                // Subtraction


Decimal_Type128& Decimal_Type128::operator-=(Decimal32 rhs)
{
    return *this -= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator-=(Decimal64 rhs)
{
    return *this -= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator-=(Decimal128 rhs)
{
    this->d_value = DecimalImpUtil::subtract(this->d_value, rhs.d_value);
    return *this;
}



Decimal_Type128& Decimal_Type128::operator-=(int rhs)
{
    return *this -= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator-=(unsigned int rhs)
{
    return *this -= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator-=(long rhs)
{
    return *this -= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator-=(unsigned long rhs)
{
    return *this -= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator-=(long long rhs)
{
    return *this -= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator-=(unsigned long long rhs)
{
    return *this -= Decimal128(rhs);
}

                               // Multiplication


Decimal_Type128& Decimal_Type128::operator*=(Decimal32 rhs)
{
    return *this *= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator*=(Decimal64 rhs)
{
    return *this *= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator*=(Decimal128 rhs)
{
    this->d_value = DecimalImpUtil::multiply(this->d_value, rhs.d_value);
    return *this;
}



Decimal_Type128& Decimal_Type128::operator*=(int rhs)
{
    return *this *= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator*=(unsigned int rhs)
{
    return *this *= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator*=(long rhs)
{
    return *this *= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator*=(unsigned long rhs)
{
    return *this *= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator*=(long long rhs)
{
    return *this *= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator*=(unsigned long long rhs)
{
    return *this *= Decimal128(rhs);
}

                                  // Division


Decimal_Type128& Decimal_Type128::operator/=(Decimal32 rhs)
{
    return *this /= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator/=(Decimal64 rhs)
{
    return *this /= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator/=(Decimal128 rhs)
{
    this->d_value = DecimalImpUtil::divide(this->d_value, rhs.d_value);
    return *this;
}



Decimal_Type128& Decimal_Type128::operator/=(int rhs)
{
    return *this /= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator/=(unsigned int rhs)
{
    return *this /= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator/=(long rhs)
{
    return *this /= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator/=(unsigned long rhs)
{
    return *this /= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator/=(long long rhs)
{
    return *this /= Decimal128(rhs);
}


Decimal_Type128& Decimal_Type128::operator/=(unsigned long long rhs)
{
    return *this /= Decimal128(rhs);
}

                            // Internals Accessors


DecimalImpUtil::ValueType128 *Decimal_Type128::data()
{
    return &d_value;
}


const DecimalImpUtil::ValueType128 *Decimal_Type128::data() const
{
    return &d_value;
}


DecimalImpUtil::ValueType128 Decimal_Type128::value() const
{
    return d_value;
}

}  // close package namespace

// FREE OPERATORS


bdldfp::Decimal32 bdldfp::operator+(bdldfp::Decimal32 value)
{
    return value;
}


bdldfp::Decimal32 bdldfp::operator-(bdldfp::Decimal32 value)
{
    return Decimal32(DecimalImpUtil::negate(value.value()));
}


bool bdldfp::operator==(bdldfp::Decimal32 lhs, bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::equal(*lhs.data(), *rhs.data());
}


bool bdldfp::operator!=(bdldfp::Decimal32 lhs, bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::notEqual(*lhs.data(), *rhs.data());
}


bool bdldfp::operator<(bdldfp::Decimal32 lhs, bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::less(*lhs.data(), *rhs.data());
}


bool bdldfp::operator<=(bdldfp::Decimal32 lhs, bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::lessEqual(*lhs.data(), *rhs.data());
}


bool bdldfp::operator>(bdldfp::Decimal32 lhs, bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::greater(*lhs.data(), *rhs.data());
}


bool bdldfp::operator>=(bdldfp::Decimal32 lhs, bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::greaterEqual(*lhs.data(), *rhs.data());
}

// FREE OPERATORS

bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 value)
{
    return value;
}


bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 value)
{
    return DecimalImpUtil::negate(*value.data());
}


bdldfp::Decimal64 bdldfp::operator++(bdldfp::Decimal64& value, int)
{
    bdldfp::Decimal64 result(value);
    ++value;
    return result;
}


bdldfp::Decimal64 bdldfp::operator--(bdldfp::Decimal64& value, int)
{
    bdldfp::Decimal64 result(value);
    --value;
    return result;
}

                                  // Addition


bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(DecimalImpUtil::add(*lhs.data(), *rhs.data()));
}


bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal32 lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) + rhs;
}


bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 lhs,
                                    bdldfp::Decimal32 rhs)
{
    return lhs + Decimal64(rhs);
}


bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 lhs,
                                    int               rhs)
{
    return lhs + Decimal64(rhs);
}


bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 lhs,
                                    unsigned int      rhs)
{
    return lhs + Decimal64(rhs);
}


bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 lhs,
                                    long              rhs)
{
    return lhs + Decimal64(rhs);
}


bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 lhs,
                                    unsigned long     rhs)
{
    return lhs + Decimal64(rhs);
}


bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 lhs,
                                    long long         rhs)
{
    return lhs + Decimal64(rhs);
}


bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64  lhs,
                                    unsigned long long rhs)
{
    return lhs + Decimal64(rhs);
}


bdldfp::Decimal64 bdldfp::operator+(int               lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) + rhs;
}


bdldfp::Decimal64 bdldfp::operator+(unsigned int      lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) + rhs;
}


bdldfp::Decimal64 bdldfp::operator+(long              lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) + rhs;
}


bdldfp::Decimal64 bdldfp::operator+(unsigned long     lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) + rhs;
}


bdldfp::Decimal64 bdldfp::operator+(long long         lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) + rhs;
}


bdldfp::Decimal64 bdldfp::operator+(unsigned long long lhs,
                                    bdldfp::Decimal64  rhs)
{
    return Decimal64(lhs) + rhs;
}

                                // Subtraction


bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(DecimalImpUtil::subtract(*lhs.data(), *rhs.data()));
}


bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal32 lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) - rhs;
}


bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 lhs,
                                    bdldfp::Decimal32 rhs)
{
    return lhs - Decimal64(rhs);
}


bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 lhs,
                                    int               rhs)
{
    return lhs - Decimal64(rhs);
}


bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 lhs,
                                    unsigned int      rhs)
{
    return lhs - Decimal64(rhs);
}


bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 lhs,
                                    long              rhs)
{
    return lhs - Decimal64(rhs);
}


bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 lhs,
                                    unsigned long     rhs)
{
    return lhs - Decimal64(rhs);
}


bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 lhs,
                                    long long         rhs)
{
    return lhs - Decimal64(rhs);
}


bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64  lhs,
                                    unsigned long long rhs)
{
    return lhs - Decimal64(rhs);
}


bdldfp::Decimal64 bdldfp::operator-(int               lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) - rhs;
}


bdldfp::Decimal64 bdldfp::operator-(unsigned int      lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) - rhs;
}


bdldfp::Decimal64 bdldfp::operator-(long              lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) - rhs;
}


bdldfp::Decimal64 bdldfp::operator-(unsigned long     lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) - rhs;
}


bdldfp::Decimal64 bdldfp::operator-(long long         lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) - rhs;
}


bdldfp::Decimal64 bdldfp::operator-(unsigned long long lhs,
                                    bdldfp::Decimal64  rhs)
{
    return Decimal64(lhs) - rhs;
}

                               // Multiplication

 bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64 lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(DecimalImpUtil::multiply(*lhs.data(), *rhs.data()));
}

 bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal32 lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) * rhs;
}

 bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64 lhs,
                                           bdldfp::Decimal32 rhs)
{
    return lhs * Decimal64(rhs);
}

 bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64 lhs,
                                           int               rhs)
{
    return lhs * Decimal64(rhs);
}

 bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64 lhs,
                                           unsigned int      rhs)
{
    return lhs * Decimal64(rhs);
}

 bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64 lhs,
                                           long              rhs)
{
    return lhs * Decimal64(rhs);
}

 bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64 lhs,
                                           unsigned long     rhs)
{
    return lhs * Decimal64(rhs);
}

 bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64 lhs,
                                           long long         rhs)
{
    return lhs * Decimal64(rhs);
}

 bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64  lhs,
                                           unsigned long long rhs)
{
    return lhs * Decimal64(rhs);
}

 bdldfp::Decimal64 bdldfp::operator*(int               lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) * rhs;
}

 bdldfp::Decimal64 bdldfp::operator*(unsigned int      lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) * rhs;
}

 bdldfp::Decimal64 bdldfp::operator*(long              lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) * rhs;
}

 bdldfp::Decimal64 bdldfp::operator*(unsigned long     lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) * rhs;
}

 bdldfp::Decimal64 bdldfp::operator*(long long         lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) * rhs;
}

 bdldfp::Decimal64 bdldfp::operator*(unsigned long long lhs,
                                           bdldfp::Decimal64  rhs)
{
    return Decimal64(lhs) * rhs;
}

                                  // Division

 bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64 lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(DecimalImpUtil::divide(*lhs.data(), *rhs.data()));
}

 bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal32 lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) / rhs;
}

 bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64 lhs,
                                           bdldfp::Decimal32 rhs)
{
    return lhs / Decimal64(rhs);
}

 bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64 lhs,
                                           int               rhs)
{
    return lhs / Decimal64(rhs);
}

 bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64 lhs,
                                           unsigned int      rhs)
{
    return lhs / Decimal64(rhs);
}

 bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64 lhs,
                                           long              rhs)
{
    return lhs / Decimal64(rhs);
}

 bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64 lhs,
                                           unsigned long     rhs)
{
    return lhs / Decimal64(rhs);
}

 bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64 lhs,
                                           long long         rhs)
{
    return lhs / Decimal64(rhs);
}

 bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64  lhs,
                                           unsigned long long rhs)
{
    return lhs / Decimal64(rhs);
}

 bdldfp::Decimal64 bdldfp::operator/(int               lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) / rhs;
}

 bdldfp::Decimal64 bdldfp::operator/(unsigned int      lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) / rhs;
}

 bdldfp::Decimal64 bdldfp::operator/(long              lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) / rhs;
}

 bdldfp::Decimal64 bdldfp::operator/(unsigned long     lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) / rhs;
}

 bdldfp::Decimal64 bdldfp::operator/(long long         lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) / rhs;
}

 bdldfp::Decimal64 bdldfp::operator/(unsigned long long lhs,
                                           bdldfp::Decimal64  rhs)
{
    return Decimal64(lhs) / rhs;
}

                                  // Equality

 bool bdldfp::operator==(bdldfp::Decimal64 lhs, bdldfp::Decimal64 rhs)
{
    return DecimalImpUtil::equal(*lhs.data(), *rhs.data());
}

 bool bdldfp::operator==(bdldfp::Decimal32 lhs, bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) == rhs;
}

 bool bdldfp::operator==(bdldfp::Decimal64 lhs, bdldfp::Decimal32 rhs)
{
    return lhs == Decimal64(rhs);
}

                                 // Inequality

 bool bdldfp::operator!=(bdldfp::Decimal64 lhs, bdldfp::Decimal64 rhs)
{
    return DecimalImpUtil::notEqual(*lhs.data(), *rhs.data());
}

 bool bdldfp::operator!=(bdldfp::Decimal32 lhs, bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) != rhs;
}

 bool bdldfp::operator!=(bdldfp::Decimal64 lhs, bdldfp::Decimal32 rhs)
{
    return lhs != Decimal64(rhs);
}

                                 // Less Than

 bool bdldfp::operator<(bdldfp::Decimal64 lhs, bdldfp::Decimal64 rhs)
{
    return DecimalImpUtil::less(*lhs.data(), *rhs.data());
}

 bool bdldfp::operator<(bdldfp::Decimal32 lhs, bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) < rhs;
}

 bool bdldfp::operator<(bdldfp::Decimal64 lhs, bdldfp::Decimal32 rhs)
{
    return lhs < Decimal64(rhs);
}

                                 // Less Equal

 bool bdldfp::operator<=(bdldfp::Decimal64 lhs, bdldfp::Decimal64 rhs)
{
    return DecimalImpUtil::lessEqual(*lhs.data(), *rhs.data());
}

 bool bdldfp::operator<=(bdldfp::Decimal32 lhs, bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) <= rhs;
}

 bool bdldfp::operator<=(bdldfp::Decimal64 lhs, bdldfp::Decimal32 rhs)
{
    return lhs <= Decimal64(rhs);
}

                                // Greater Than

 bool bdldfp::operator>(bdldfp::Decimal64 lhs, bdldfp::Decimal64 rhs)
{
    return DecimalImpUtil::greater(*lhs.data(), *rhs.data());
}

 bool bdldfp::operator>(bdldfp::Decimal32 lhs, bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) > rhs;
}

 bool bdldfp::operator>(bdldfp::Decimal64 lhs, bdldfp::Decimal32 rhs)
{
    return lhs > Decimal64(rhs);
}

                               // Greater Equal

 bool bdldfp::operator>=(bdldfp::Decimal64 lhs, bdldfp::Decimal64 rhs)
{
    return DecimalImpUtil::greaterEqual(*lhs.data(), *rhs.data());
}

 bool bdldfp::operator>=(bdldfp::Decimal32 lhs, bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) >= rhs;
}

 bool bdldfp::operator>=(bdldfp::Decimal64 lhs, bdldfp::Decimal32 rhs)
{
    return lhs >= Decimal64(rhs);
}

// FREE OPERATORS


bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 value)
{
    return value;
}


bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 value)
{
    return Decimal128(DecimalImpUtil::negate(*value.data()));
}


bdldfp::Decimal128 bdldfp::operator++(bdldfp::Decimal128& value, int)
{
    Decimal128 result = value;
    ++value;
    return result;
}


bdldfp::Decimal128 bdldfp::operator--(bdldfp::Decimal128& value, int)
{
    Decimal128 result = value;
    --value;
    return result;
}

                                  // Addition


bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(DecimalImpUtil::add(*lhs.data(), *rhs.data()));
}


bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal32  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}


bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal32  rhs)
{
    return lhs + Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal64  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}


bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal64  rhs)
{
    return lhs + Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     int                rhs)
{
    return lhs + Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     unsigned int       rhs)
{
    return lhs + Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     long               rhs)
{
    return lhs + Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     unsigned long      rhs)
{
    return lhs + Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     long long          rhs)
{
    return lhs + Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     unsigned long long rhs)
{
    return lhs + Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator+(int                lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}


bdldfp::Decimal128 bdldfp::operator+(unsigned int       lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}


bdldfp::Decimal128 bdldfp::operator+(long               lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}


bdldfp::Decimal128 bdldfp::operator+(unsigned long      lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}


bdldfp::Decimal128 bdldfp::operator+(long long          lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}


bdldfp::Decimal128 bdldfp::operator+(unsigned long long lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}

                                // Subtraction


bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(DecimalImpUtil::subtract(*lhs.data(), *rhs.data()));
}


bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal32  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}


bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal32  rhs)
{
    return lhs - Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal64  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}


bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal64  rhs)
{
    return lhs - Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     int                rhs)
{
    return lhs - Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     unsigned int       rhs)
{
    return lhs - Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     long               rhs)
{
    return lhs - Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     unsigned long      rhs)
{
    return lhs - Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     long long          rhs)
{
    return lhs - Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     unsigned long long rhs)
{
    return lhs - Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator-(int                lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}


bdldfp::Decimal128 bdldfp::operator-(unsigned int       lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}


bdldfp::Decimal128 bdldfp::operator-(long               lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}


bdldfp::Decimal128 bdldfp::operator-(unsigned long      lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}


bdldfp::Decimal128 bdldfp::operator-(long long          lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}


bdldfp::Decimal128 bdldfp::operator-(unsigned long long lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}

                               // Multiplication


bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(DecimalImpUtil::multiply(*lhs.data(), *rhs.data()));
}


bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal32  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}


bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal32  rhs)
{
    return lhs * Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal64  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}


bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal64  rhs)
{
    return lhs * Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     int                rhs)
{
    return lhs * Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     unsigned int       rhs)
{
    return lhs * Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     long               rhs)
{
    return lhs * Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     unsigned long      rhs)
{
    return lhs * Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     long long          rhs)
{
    return lhs * Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     unsigned long long rhs)
{
    return lhs * Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator*(int                lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}


bdldfp::Decimal128 bdldfp::operator*(unsigned int       lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}


bdldfp::Decimal128 bdldfp::operator*(long               lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}


bdldfp::Decimal128 bdldfp::operator*(unsigned long      lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}


bdldfp::Decimal128 bdldfp::operator*(long long          lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}


bdldfp::Decimal128 bdldfp::operator*(unsigned long long lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}

                                  // Division


bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(DecimalImpUtil::divide(*lhs.data(), *rhs.data()));
}


bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal32  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}


bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal32  rhs)
{
    return lhs / Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal64  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}


bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal64  rhs)
{
    return lhs / Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     int                rhs)
{
    return lhs / Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     unsigned int       rhs)
{
    return lhs / Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     long               rhs)
{
    return lhs / Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     unsigned long      rhs)
{
    return lhs / Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     long long          rhs)
{
    return lhs / Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     unsigned long long rhs)
{
    return lhs / Decimal128(rhs);
}


bdldfp::Decimal128 bdldfp::operator/(int                lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}


bdldfp::Decimal128 bdldfp::operator/(unsigned int       lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}


bdldfp::Decimal128 bdldfp::operator/(long               lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}


bdldfp::Decimal128 bdldfp::operator/(unsigned long      lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}


bdldfp::Decimal128 bdldfp::operator/(long long          lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}


bdldfp::Decimal128 bdldfp::operator/(unsigned long long lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}

                                  // Equality


bool bdldfp::operator==(bdldfp::Decimal128 lhs, bdldfp::Decimal128 rhs)
{
    return DecimalImpUtil::equal(*lhs.data(), *rhs.data());
}


bool bdldfp::operator==(bdldfp::Decimal32 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) == rhs;
}


bool bdldfp::operator==(bdldfp::Decimal128 lhs, bdldfp::Decimal32 rhs)
{
    return lhs == Decimal128(rhs);
}


bool bdldfp::operator==(bdldfp::Decimal64 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) == rhs;
}


bool bdldfp::operator==(bdldfp::Decimal128 lhs, bdldfp::Decimal64 rhs)
{
    return lhs == Decimal128(rhs);
}

                                 // Inequality


bool bdldfp::operator!=(bdldfp::Decimal128 lhs, bdldfp::Decimal128 rhs)
{
    return DecimalImpUtil::notEqual(*lhs.data(), *rhs.data());
}


bool bdldfp::operator!=(bdldfp::Decimal32 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) != rhs;
}


bool bdldfp::operator!=(bdldfp::Decimal128 lhs, bdldfp::Decimal32 rhs)
{
    return lhs != Decimal128(rhs);
}


bool bdldfp::operator!=(bdldfp::Decimal64 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) != rhs;
}


bool bdldfp::operator!=(bdldfp::Decimal128 lhs, bdldfp::Decimal64 rhs)
{
    return lhs != Decimal128(rhs);
}

                                 // Less Than


bool bdldfp::operator<(bdldfp::Decimal128 lhs, bdldfp::Decimal128 rhs)
{
    return DecimalImpUtil::less(*lhs.data(), *rhs.data());
}


bool bdldfp::operator<(bdldfp::Decimal32 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) < rhs;
}


bool bdldfp::operator<(bdldfp::Decimal128 lhs, bdldfp::Decimal32 rhs)
{
    return lhs < Decimal128(rhs);
}


bool bdldfp::operator<(bdldfp::Decimal64 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) < rhs;
}


bool bdldfp::operator<(bdldfp::Decimal128 lhs, bdldfp::Decimal64 rhs)
{
    return lhs < Decimal128(rhs);
}

                                 // Less Equal


bool bdldfp::operator<=(bdldfp::Decimal128 lhs, bdldfp::Decimal128 rhs)
{
    return DecimalImpUtil::lessEqual(*lhs.data(), *rhs.data());
}


bool bdldfp::operator<=(bdldfp::Decimal32 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) <= rhs;
}


bool bdldfp::operator<=(bdldfp::Decimal128 lhs, bdldfp::Decimal32 rhs)
{
    return lhs <= Decimal128(rhs);
}


bool bdldfp::operator<=(bdldfp::Decimal64 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) <= rhs;
}


bool bdldfp::operator<=(bdldfp::Decimal128 lhs, bdldfp::Decimal64 rhs)
{
    return lhs <= Decimal128(rhs);
}

                                  // Greater


bool bdldfp::operator>(bdldfp::Decimal128 lhs, bdldfp::Decimal128 rhs)
{
    return DecimalImpUtil::greater(*lhs.data(), *rhs.data());
}


bool bdldfp::operator>(bdldfp::Decimal32 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) > rhs;
}


bool bdldfp::operator>(bdldfp::Decimal128 lhs, bdldfp::Decimal32 rhs)
{
    return lhs > Decimal128(rhs);
}


bool bdldfp::operator>(bdldfp::Decimal64 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) > rhs;
}


bool bdldfp::operator>(bdldfp::Decimal128 lhs, bdldfp::Decimal64 rhs)
{
    return lhs > Decimal128(rhs);
}

                               // Greater Equal


bool bdldfp::operator>=(bdldfp::Decimal128 lhs, bdldfp::Decimal128 rhs)
{
    return DecimalImpUtil::greaterEqual(*lhs.data(), *rhs.data());
}


bool bdldfp::operator>=(bdldfp::Decimal32 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) >= rhs;
}


bool bdldfp::operator>=(bdldfp::Decimal128 lhs, bdldfp::Decimal32 rhs)
{
    return lhs >= Decimal128(rhs);
}


bool bdldfp::operator>=(bdldfp::Decimal64 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) >= rhs;
}


bool bdldfp::operator>=(bdldfp::Decimal128 lhs, bdldfp::Decimal64 rhs)
{
    return lhs >= Decimal128(rhs);
}

}  // close enterprise namespace


///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
