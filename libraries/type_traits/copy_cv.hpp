#ifndef BOOST_TYPE_TRAITS_COPY_CV_HPP_INCLUDED
#define BOOST_TYPE_TRAITS_COPY_CV_HPP_INCLUDED

//
//  Copyright 2015 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//

#include "is_const.hpp"
#include "is_volatile.hpp"
#include "add_const.hpp"
#include "add_volatile.hpp"
#include "conditional.hpp"

namespace boost
{

template<class T, class U> struct copy_cv
{
private:

    typedef typename boost::conditional<boost::is_const<U>::value, typename boost::add_const<T>::type, T>::type CT;

public:

    typedef typename boost::conditional<boost::is_volatile<U>::value, typename boost::add_volatile<CT>::type, CT>::type type;
};

} // namespace boost

#endif // #ifndef BOOST_TYPE_TRAITS_COPY_CV_HPP_INCLUDED
