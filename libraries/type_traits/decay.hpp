//  (C) Copyright John Maddock & Thorsten Ottosen 2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef BOOST_TT_DECAY_HPP_INCLUDED
#define BOOST_TT_DECAY_HPP_INCLUDED

#include "is_array.hpp"
#include "is_function.hpp"
#include "remove_bounds.hpp"
#include "add_pointer.hpp"
#include "remove_reference.hpp"
#include "remove_cv.hpp"

namespace boost 
{

   namespace detail
   {

      template <class T, bool Array, bool Function> struct decay_imp { typedef typename remove_cv<T>::type type; };
      template <class T> struct decay_imp<T, true, false> { typedef typename remove_bounds<T>::type* type; };
      template <class T> struct decay_imp<T, false, true> { typedef T* type; };

   }

    template< class T >
    struct decay
    {
    private:
        typedef typename remove_reference<T>::type Ty;
    public:
       typedef typename boost::detail::decay_imp<Ty, boost::is_array<Ty>::value, boost::is_function<Ty>::value>::type type;
    };
    
} // namespace boost


#endif // BOOST_TT_DECAY_HPP_INCLUDED
