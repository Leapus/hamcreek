#pragma once
#include <type_traits>

/*
 * Metaprogramming stuff
 * 
 */

namespace leapus{
namespace meta{

//A mostly empty class which is used to pass or represent a type
//You could probably accomplish the same thing passing a null pointer,
//but this is less ambiguous.
template<typename T>
struct type{
    using value_type = T;
};

//A function that accepts zero or more arguments and does nothing
//Provides a context for expanding parameter packs
template<typename... Args>
void null_function(Args... args){}

//Demand a specific type. Useful for variadic arguments
template<typename T, typename U>
using permit_type = typename std::enable_if<std::is_same<T, U>::value, T>::type;

//Deterimne whether T is the same type as any in Args...
template<typename T, typename... Args>
struct is_any;

template<typename T, typename U>
struct is_any<T, U>:public std::integral_constant<bool, std::is_same<T, U>::value>{};

template<typename T, typename U, typename... Args>
struct is_any<T, U, Args...>:public std::integral_constant<bool, std::is_same<T, U>::value || is_any<T, Args...>::value>{
};

template<typename T, typename... Args>
using permit_types = typename std::enable_if<is_any<T, Args...>::value, T>::type;

//Permit rval, lconst, and non-const left refs
//Suitable for std::forward<>
//which expects T for rvalues, or T & or const T &
template<typename T, typename U>
using permit_gl_ref = permit_types<T, U, const U &, U &>;

//Permit T to be any CV variation of U
template<typename T, typename U, typename V = std::remove_cv_t<U>>
using permit_any_cv = permit_types< T, V, const V, volatile V, const volatile V>;

template<typename T, typename U>
using const_optional = permit_types<T, U, const U>;

template<typename T, typename U>
using permit_if_converts_to = typename std::enable_if<std::is_convertible<T, U>::value, T>::type;

}
}

