// bslmf_typelist.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_TYPELIST
#define INCLUDED_BSLMF_TYPELIST


//@PURPOSE: Provide a typelist component.
//
//@CLASSES:
//  bslmf::TypeList: Typelist of up to 20 types
//  bslmf::TypeListTypeOf: Meta-function to get the type of a typelist member
//  bslmf::TypeList0: Typelist of 0 types
//  bslmf::TypeList1: Typelist of 1 type
//  bslmf::TypeList2: Typelist of 2 types
//  bslmf::TypeList3: Typelist of 3 types
//  bslmf::TypeList4: Typelist of 4 types
//  bslmf::TypeList5: Typelist of 5 types
//  bslmf::TypeList6: Typelist of 6 types
//  bslmf::TypeList7: Typelist of 7 types
//  bslmf::TypeList8: Typelist of 8 types
//  bslmf::TypeList9: Typelist of 9 types
//  bslmf::TypeList10: Typelist of 10 types
//  bslmf::TypeList11: Typelist of 11 types
//  bslmf::TypeList12: Typelist of 12 types
//  bslmf::TypeList13: Typelist of 13 types
//  bslmf::TypeList14: Typelist of 14 types
//  bslmf::TypeList15: Typelist of 15 types
//  bslmf::TypeList16: Typelist of 16 types
//  bslmf::TypeList17: Typelist of 17 types
//  bslmf::TypeList18: Typelist of 18 types
//  bslmf::TypeList19: Typelist of 19 types
//  bslmf::TypeList20: Typelist of 20 types
//
//@DESCRIPTION: 'bslmf::TypeList' provides a compile time list that holds up to
// 20 types.  Users can access the different types the list contains (by
// index), and the length of the typelist.  A 'bslmf::TypeList' is typically
// used when writing templatized classes that can store a variable amount of
// types, such as a bind or variant class.
//
///Usage
///-----
// The following usage example demonstrates how to retrieve information from a
// 'bslmf::TypeList':
//..
//  typedef bslmf::TypeList<int, double, char> List;
//..
// We can access the length of the list using the 'LENGTH' member:
//..
//  assert(3 == List::LENGTH);  // accessing the length of the list
//..
// We can also access the different types stored in the typelist using
// predefined type members 'TypeN' (where '1 <= N <= 20'), or another
// meta-function 'bslmf::TypeListTypeOf':
//..
//  assert(1 == bsl::is_same<int,    List::Type1>::value)
//  assert(1 == bsl::is_same<double, List::Type2>::value)
//  assert(1 == bsl::is_same<char,   List::Type3>::value)
//
//  typedef bslmf::TypeListTypeOf<1, List>::Type my_Type1;
//  typedef bslmf::TypeListTypeOf<2, List>::Type my_Type2;
//  typedef bslmf::TypeListTypeOf<3, List>::Type my_Type3;
//
//  assert(1 == bsl::is_same<int,    my_Type1>::value)
//  assert(1 == bsl::is_same<double, my_Type2>::value)
//  assert(1 == bsl::is_same<char,   my_Type3>::value)
//..

#include <bdlscm_version.h>

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif


#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
#define BSLMF_TYPELIST_USING_VARIADIC_TEMPLATES
    // This macro indicates that we have all the needed features for an
    // implementation of the typelist facility that is source-compatible with
    // the pre-existing C++03 facility.  The main change is that the numbered
    // classes TypeList0-TypeList20 are no longer distinct classes, but aliases
    // of specific instantiations of the primary TypeList template.  Eventually
    // these partial template specializations will be eliminated, when the
    // individually named members are no longer used throughout the whole of
    // the Bloomberg codebase.
#endif

namespace BloombergLP {

namespace bslmf {


#if defined(BSLMF_TYPELIST_USING_VARIADIC_TEMPLATES)

                         // =====================
                         // struct TypeListTypeAt
                         // =====================

template <unsigned INDEX,  // 'unsigned' is a proxy for 'size_t'
          class LIST,
          class DEFAULTTYPE = void,
          bool  INRANGE = (INDEX < LIST::LENGTH)>
struct TypeListTypeAt {
    // This template is specialized below to return the type the 'INDEX'th
    // member of the typelist 'LIST'.  If '0 > INDEX <= LIST::LENGTH' then
    // 'Type' will be defined as the type of the member.  Note that INDEX is
    // relative to 1.

    // PUBLIC TYPES
    typedef LIST ListType;
    enum {LENGTH = ListType::LENGTH};

    typedef DEFAULTTYPE TypeOrDefault;
};

                         // =====================
                         // struct TypeListTypeOf
                         // =====================

template <int INDEX, class LIST, class DEFAULTTYPE = void>
using TypeListTypeOf =
       TypeListTypeAt<static_cast<unsigned int>(INDEX - 1), LIST, DEFAULTTYPE>;
    // Classic Bloomberg code uses a 1-based index into the type list.

                            // ===============
                            // struct TypeList
                            // ===============

template <class ...TYPES>
struct TypeList {
    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = sizeof...(TYPES)};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;
};


using TypeList0 = TypeList<>;

template <class A1>
using TypeList1 = TypeList< A1>;

template <class A1,  class A2>
using TypeList2 = TypeList< A1, A2>;

template <class A1,  class A2,  class A3>
using TypeList3 = TypeList< A1, A2, A3>;

template <class A1,  class A2,  class A3,  class A4>
using TypeList4 = TypeList< A1, A2, A3, A4>;

template <class A1,  class A2,  class A3,  class A4, class A5>
using TypeList5 = TypeList< A1, A2, A3, A4, A5>;

template <class A1,  class A2,  class A3,  class A4,  class A5, class A6>
using TypeList6 = TypeList< A1, A2, A3, A4, A5, A6>;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7>
using TypeList7 = TypeList< A1, A2, A3, A4, A5, A6, A7>;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7, class A8>
using TypeList8 = TypeList< A1, A2, A3, A4, A5, A6, A7, A8>;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8, class A9>
using TypeList9 = TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9>;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10>
using TypeList10 = TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10>;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11>
using TypeList11 = TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12>
using TypeList12 = TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9,A10, A11, A12>;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13>
using TypeList13 = TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10,
                            A11,A12,A13>;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14>
using TypeList14 = TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10,
                            A11,A12,A13,A14>;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15>
using TypeList15 = TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10,
                            A11,A12,A13,A14,A15>;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
using TypeList16 = TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10,
                            A11,A12,A13,A14,A15,A16>;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
using TypeList17 = TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10,
                            A11,A12,A13,A14,A15,A16,A17>;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
using TypeList18 = TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10,
                            A11,A12,A13,A14,A15,A16,A17,A18>;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
using TypeList19 = TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10,
                            A11,A12,A13,A14,A15,A16,A17,A18,A19>;

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
using TypeList20 = TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10,
                            A11,A12,A13,A14,A15,A16,A17,A18,A19,A20>;


         // =========================================================
         // TypeList speciailizations for fixed number of  parameters
         // =========================================================

template <class A1>
struct TypeList<A1> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 1};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
};

template <class A1,  class A2>
struct TypeList<A1, A2> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 2};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
};

template <class A1,  class A2,  class A3>
struct TypeList<A1, A2, A3> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 3};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
};

template <class A1,  class A2,  class A3,  class A4>
struct TypeList<A1, A2, A3, A4> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 4};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
};

template <class A1,  class A2,  class A3,  class A4,  class A5>
struct TypeList<A1, A2, A3, A4, A5> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 5};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6>
struct TypeList<A1, A2, A3, A4, A5, A6> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 6};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7>
struct TypeList<A1, A2, A3, A4, A5, A6, A7> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 7};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 8};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 9};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 10};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11>
struct TypeList<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 11};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12>
struct TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10, A11, A12> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 12};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13>
struct TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10
               ,A11,A12,A13> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 13};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14>
struct TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10
               ,A11,A12,A13,A14> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 14};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
    typedef A14 Type14;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15>
struct TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10
               ,A11,A12,A13,A14,A15> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 15};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
    typedef A14 Type14;
    typedef A15 Type15;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
struct TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10
               ,A11,A12,A13,A14,A15,A16> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 16};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
    typedef A14 Type14;
    typedef A15 Type15;
    typedef A16 Type16;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
struct TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10
               ,A11,A12,A13,A14,A15,A16,A17> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 17};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
    typedef A14 Type14;
    typedef A15 Type15;
    typedef A16 Type16;
    typedef A17 Type17;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
struct TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10
               ,A11,A12,A13,A14,A15,A16,A17,A18> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 18};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
    typedef A14 Type14;
    typedef A15 Type15;
    typedef A16 Type16;
    typedef A17 Type17;
    typedef A18 Type18;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
struct TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10
               ,A11,A12,A13,A14,A15,A16,A17,A18,A19> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 19};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
    typedef A14 Type14;
    typedef A15 Type15;
    typedef A16 Type16;
    typedef A17 Type17;
    typedef A18 Type18;
    typedef A19 Type19;
};

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
struct TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10
               ,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20> {

    typedef TypeList ListType;
    typedef TypeList Type;

    enum {LENGTH = 20};

    template <unsigned INDEX>
    using TypeOf = TypeListTypeAt<INDEX-1, TypeList>;

    typedef A1  Type1;
    typedef A2  Type2;
    typedef A3  Type3;
    typedef A4  Type4;
    typedef A5  Type5;
    typedef A6  Type6;
    typedef A7  Type7;
    typedef A8  Type8;
    typedef A9  Type9;
    typedef A10 Type10;
    typedef A11 Type11;
    typedef A12 Type12;
    typedef A13 Type13;
    typedef A14 Type14;
    typedef A15 Type15;
    typedef A16 Type16;
    typedef A17 Type17;
    typedef A18 Type18;
    typedef A19 Type19;
    typedef A20 Type20;
};

                         // ---------------------
                         // struct TypeListTypeAt
                         // ---------------------

// SPECIALIZATIONS
template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<0u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type1 Type;
    typedef typename LIST::Type1 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<1u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type2 Type;
    typedef typename LIST::Type2 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<2u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type3 Type;
    typedef typename LIST::Type3 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<3u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type4 Type;
    typedef typename LIST::Type4 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<4u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type5 Type;
    typedef typename LIST::Type5 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<5u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type6 Type;
    typedef typename LIST::Type6 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<6u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type7 Type;
    typedef typename LIST::Type7 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<7u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type8 Type;
    typedef typename LIST::Type8 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<8u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type9 Type;
    typedef typename LIST::Type9 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<9u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type10 Type;
    typedef typename LIST::Type10 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<10u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type11 Type;
    typedef typename LIST::Type11 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<11u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type12 Type;
    typedef typename LIST::Type12 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<12u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type13 Type;
    typedef typename LIST::Type13 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<13u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type14 Type;
    typedef typename LIST::Type14 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<14u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type15 Type;
    typedef typename LIST::Type15 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<15u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type16 Type;
    typedef typename LIST::Type16 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<16u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type17 Type;
    typedef typename LIST::Type17 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<17u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type18 Type;
    typedef typename LIST::Type18 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<18u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type19 Type;
    typedef typename LIST::Type19 TypeOrDefault;
};

template <class LIST, class DEFAULTTYPE>
struct TypeListTypeAt<19u, LIST, DEFAULTTYPE, true> {
    typedef typename LIST::Type20 Type;
    typedef typename LIST::Type20 TypeOrDefault;
};

template <unsigned N,
          class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20, class A21, class DEFAULTTYPE, class ...TYPES>
struct TypeListTypeAt<N,
                      TypeList< A1, A2, A3, A4, A5, A6, A7, A8, A9,A10,
                         A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,TYPES...>,
                      DEFAULTTYPE,
                      true> {
    typedef typename
    TypeListTypeAt<N-20, TypeList<A21, TYPES...>, DEFAULTTYPE>::Type Type;

    typedef typename
    TypeListTypeAt<N-20, TypeList<A21, TYPES...>, DEFAULTTYPE>::TypeOrDefault
                                                                TypeOrDefault;
};

#else

static_assert(false, "Use variadics");

#endif

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
