// bslalg_hasstliterators.h                                           -*-C++-*-
#ifndef INCLUDED_BSLALG_HASSTLITERATORS
#define INCLUDED_BSLALG_HASSTLITERATORS


//@PURPOSE: Provide a tag type used to detect STL-like iterators traits.
//
//@CLASSES:
//  bslalg::HasStlIterators: tag type to detect STL-like iterators traits
//
//@SEE_ALSO: bslmf_detecttestedtraits, bslalg_typetraithasstliterators
//
//@DESCRIPTION: This component defines a tag type 'HasStlIterators' derived
// from 'bslmf::DetectNestedTrait' type.  The type defines a metafunction that
// detects if a class has STL-like iterators, and provides a type
// 'HasStilIterators<TYPE>::value' which aliases 'true_type' if 'TYPE' has
// STL-like iterators, and 'false_type' otherwise.
//
// A 'TYPE' that has this trait fulfills the following requirements, where 'mX'
// is a modifiable object and 'X' a non-modifiable object of 'TYPE':
//..
//  Valid expression     Type              Note
//  ----------------     ----              ----
//  TYPE::iterator                         Iterator type (has 'operator->',
//                                         'operator*', and possibly more
//                                         depending on the iterator
//                                         category).
//
//  TYPE::const_iterator                   Iterator type (has 'operator->',
//                                         'operator*', and possibly more
//                                         depending on the iterator
//                                         category).  The value type of this
//                                         iterator is not modifiable.
//
//  mX.begin()           iterator          Similar to standard containers
//  mX.end()             iterator
//  X.begin()            const_iterator
//  X.end()              const_iterator
//  X.cbegin()           const_iterator
//  X.cend()             const_iterator
//..

#include <bdlscm_version.h>

#include <type_traits>
#include <utility>

namespace BloombergLP {

namespace bslalg {

                        //=======================
                        // struct HasStlIterators
                        //=======================

template <typename T, typename = void>
struct is_iterable : std::false_type {};
template <typename T>
struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin()),
                                  decltype(std::declval<T>().end())>>
    : std::true_type {};

template <typename T>
struct HasStlIterators : is_iterable<T> {};


}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
