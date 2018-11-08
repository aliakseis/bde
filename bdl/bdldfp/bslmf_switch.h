// bslmf_switch.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLMF_SWITCH
#define INCLUDED_BSLMF_SWITCH


//@PURPOSE: Provide a compile-time 'switch' meta-function.
//
//@CLASSES:
//  bslmf::Switch:  'switch' meta-function (variable number of types)
//  bslmf::Switch2: 'switch' meta-function (among two types)
//  bslmf::Switch3: 'switch' meta-function (among three types)
//  bslmf::Switch4: 'switch' meta-function (among four types)
//  bslmf::Switch5: 'switch' meta-function (among five types)
//  bslmf::Switch6: 'switch' meta-function (among six types)
//  bslmf::Switch7: 'switch' meta-function (among seven types)
//  bslmf::Switch8: 'switch' meta-function (among eight types)
//  bslmf::Switch9: 'switch' meta-function (among nine types)
//
//@SEE_ALSO: bslmf_typelist
//
//@DESCRIPTION: This component provides a compile-time 'switch' meta-function.
// Its main class, 'bslmf::Switch', parameterized by an integral 'SELECTOR' and
// a variable number 'N' of types, 'T0' up to 'T{N - 1}', contains a single
// type named 'Type', which is the result of the meta-function and is an alias
// to 'T{SELECTOR}' or to 'bslmf::Nil' if 'SELECTOR' is outside the range
// '[ 0 .. N - 1 ]'.  The analogy between the following "meta-code" and its
// valid C++ version using 'bslmf::Switch' may serve as a useful mental picture
// to understand and memorize the usage of this component.
//..
//  "Meta-code" (not C++)               Valid C++ using 'bslmf::Switch'
//  ---------------------               ------------------------------
//  typedef                             typedef typename
//    switch (SELECTOR) {                       bslmf::Switch<SELECTOR,
//      case 0:     T0;                                      T0,
//      case 1:     T1;                                      T1,
//      // . . .                                             // . . .
//      case N - 1: T{N - 1};                                T{N - 1}
//      default:    bslmf::Nil;                              >
//    }                                                      ::
//                  Type;                                      Type;
//..
// Note the use of the keyword 'typename', necessary *only* if one or more of
// the 'SELECTOR' or 'T0' up to 'T{N - 1}' is dependent on a template parameter
// of the local context (i.e., that of the block using 'bslmf::Switch').  In
// particular, it should be omitted if the 'bslmf::Switch' is not used within a
// class or function template, as in the usage example below.
//
// For most situations, the number 'N' of template type arguments is known and
// the 'bslmf::SwitchN' meta-functions, which take exactly the indicated number
// of arguments, should be preferred.  Their usage leads to shorter mangled
// symbol names in object files (e.g., no extra defaulted template type
// arguments are included in the name), and shorter compilation times, as well.
//
///Usage
///-----
// Assume an external server API for storing and retrieving data:
//..
//  class data_Server {
//      // Dummy implementation of data server
//
//      int d_data;
//
//    public:
//       void store(char  data) { d_data = data | 0Xefface00; }
//       void store(short data) { d_data = data | 0Xdead0000; }
//       void store(int   data) { d_data = data; }
//
//       void retrieve(char  *data) {
//          *data = static_cast<char>(d_data & 0x000000ff);
//       }
//       void retrieve(short *data) {
//          *data = static_cast<short>(d_data & 0x0000ffff);
//       }
//       void retrieve(int   *data) { *data = d_data; }
//  };
//..
// In our application, we need some very small (1, 2, and 4-byte),
// special-purpose string types, so we create the following 'ShortString' class
// template:
//..
//  template <int LEN>
//  class ShortString {
//      // Store a short, fixed-length string.
//
//      char d_buffer[LEN];
//
//    public:
//      ShortString(const char *s = "") { std::strncpy(d_buffer, s, LEN); }
//          // Construct a 'ShortString' from a NTCS.
//
//      void retrieve(data_Server *server);
//          // Retrieve this string from a data server.
//
//      void store(data_Server *server) const;
//          // Store this string to a data server.
//
//      char operator[](int n) const { return d_buffer[n]; }
//          // Return the nth byte in this string.
//  };
//
//  template <int LEN>
//  bool operator==(const ShortString<LEN>& lhs, const ShortString<LEN>& rhs)
//      // Return true if a 'lhs' is equal to 'rhs'
//  {
//      return 0 == std::memcmp(&lhs, &rhs, LEN);
//  }
//
//  template <int LEN>
//  bool operator==(const ShortString<LEN>& lhs, const char *rhs)
//      // Return true if a 'ShortString' 'lhs' is equal to a NTCS 'rhs'.
//  {
//      int i;
//      for (i = 0; LEN > i && lhs[i]; ++i) {
//          if (lhs[i] != rhs[i]) {
//              return false;
//          }
//      }
//
//      return ('\0' == rhs[i]);
//  }
//..
// We would like to store our short strings in the data server, but the data
// server only handles 'char', 'short' and 'int' types.  Since our strings fit
// into these simple types, we can transform 'ShortString' into these integral
// types when calling 'store' and 'retrieve', using 'bslmf::Switch' to choose
// which integral type to use for each 'ShortString' type:
//..
//  template <int LEN>
//  void ShortString<LEN>::retrieve(data_Server *server)
//  {
//      // 'transferType will be 'char' if 'LEN' is 1, 'short' if 'LEN' is 2,
//      // and 'int' if 'LEN' 4.  Will choose 'void' and thus not compile if
//      // 'LEN' is 0 or 3.
//
//      typedef typename
//         bslmf::Switch<LEN, void, char, short, void, int>::Type transferType;
//
//      transferType x = 0;
//      server->retrieve(&x);
//      std::memcpy(d_buffer, &x, LEN);
//  }
//
//  template <int LEN>
//  void ShortString<LEN>::store(data_Server *server) const
//  {
//      // 'transferType will be 'char' if 'LEN' is 1, 'short' if 'LEN' is 2,
//      // and 'int' if 'LEN' 4.  Will choose 'void' and thus not compile if
//      // 'LEN' is 0 or 3.
//      typedef typename
//         bslmf::Switch<LEN, void, char, short, void, int>::Type transferType;
//
//      transferType x = 0;
//      std::memcpy(&x, d_buffer, LEN);
//      server->store(x);
//  }
//..
// In our main program, we first assert our basic assumptions, then we store
// and retrieve strings using our 'ShortString' template.
//..
//  int main()
//  {
//      assert(2 == sizeof(short));
//      assert(4 == sizeof(int));
//
//      data_Server server;
//
//      ShortString<1> a("A");
//      ShortString<1> b("B");
//      assert(a == "A");
//      assert(b == "B");
//      assert(! (a == b));
//
//      a.store(&server);
//      b.retrieve(&server);
//      assert(a == "A");
//      assert(b == "A");
//      assert(a == b);
//
//      ShortString<2> cd("CD");
//      ShortString<2> ef("EF");
//      assert(cd == "CD");
//      assert(ef == "EF");
//      assert(! (cd == ef));
//
//      cd.store(&server);
//      ef.retrieve(&server);
//      assert(cd == "CD");
//      assert(ef == "CD");
//      assert(cd == ef);
//
//      ShortString<4> ghij("GHIJ");
//      ShortString<4> klmn("KLMN");
//      assert(ghij == "GHIJ");
//      assert(klmn == "KLMN");
//      assert(! (ghij == klmn));
//
//      ghij.store(&server);
//      klmn.retrieve(&server);
//      assert(ghij == "GHIJ");
//      assert(klmn == "GHIJ");
//      assert(ghij == klmn);
//
//      return 0;
//  }
//..

#include <bdlscm_version.h>


#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif



#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
#define BSLMF_SWITCH_USING_VARIADIC_TEMPLATES
    // This macro indicates that we have all the needed features for an
    // implementation of the type-switch facility that is source-compatible
    // with the pre-existing C++03 facility.  The main change is that the
    // numbered classes Switch0-Switch9 are no longer distinct classes, but
    // aliases of specific instantiations of the primary Switch template.
    // Eventually these partial template specializations will be eliminated,
    // when the individually named members are no longer used throughout the
    // whole of the Bloomberg codebase.
#endif

namespace BloombergLP {

namespace bslmf {

#if defined(BSLMF_SWITCH_USING_VARIADIC_TEMPLATES)
template <size_t SELECTOR,
          class ...TYPES>
struct Switch {
    // This meta-function, parameterized by an integral 'SELECTOR' and types
    // 'T0' up to 'T9', provides a single type alias, 'Type', which resolves,
    // through specialization for a particular value 'N' of 'SELECTOR', to the
    // type 'TN', or to 'Nil' if 'SELECTOR' is negative or larger than the
    // number of template arguments provided for the types.

    typedef void Type;
        // This 'Type' is an alias to the parameterized 'TN', where 'N' is the
        // integral value of the parameterized 'SELECTOR'.  Note that the first
        // type in the list corresponds to 'T0', not 'T1'.
};

// SPECIALIZATIONS
template <class T0, class ...TYPES>
struct Switch<0u, T0, TYPES...> {

    typedef T0 Type;
};

template <class T0, class T1, class ...TYPES>
struct Switch<1u, T0, T1, TYPES...> {

    typedef T1 Type;
};

template <class T0, class T1, class T2, class ...TYPES>
struct Switch<2u, T0, T1, T2, TYPES...> {

    typedef T2 Type;
};

template <class T0, class T1, class T2, class T3, class ...TYPES>
struct Switch<3u, T0, T1, T2, T3, TYPES...> {

    typedef T3 Type;
};

template <class T0, class T1, class T2, class T3, class T4, class ...TYPES>
struct Switch<4u, T0, T1, T2, T3, T4, TYPES...> {

    typedef T4 Type;
};

template <class T0, class T1, class T2, class T3, class T4, class T5,
          class ...TYPES>
struct Switch<5u, T0, T1, T2, T3, T4, T5, TYPES...> {

    typedef T5 Type;
};

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class ...TYPES>
struct Switch<6u, T0, T1, T2, T3, T4, T5, T6, TYPES...> {

    typedef T6 Type;
};

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class ...TYPES>
struct Switch<7u, T0, T1, T2, T3, T4, T5, T6, T7, TYPES...> {

    typedef T7 Type;
};

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class T8, class ...TYPES>
struct Switch<8u, T0, T1, T2, T3, T4, T5, T6, T7, T8, TYPES...> {

    typedef T8 Type;
};

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class T8, class T9, class ...TYPES>
struct Switch<9u, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, TYPES...> {

    typedef T9 Type;
};

template <size_t SELECTOR, class T0, class T1, class T2, class T3, class T4,
          class T5, class T6, class T7, class T8, class T9, class T10,
          class ...TYPES>
struct Switch<SELECTOR, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, TYPES...>
                                                                              {
    typedef typename Switch<SELECTOR-10, T10, TYPES...>::Type Type;
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
