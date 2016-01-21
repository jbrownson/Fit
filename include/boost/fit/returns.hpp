/*=============================================================================
    Copyright (c) 2014 Paul Fultz II
    returns.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FIT_GUARD_RETURNS_H
#define BOOST_FIT_GUARD_RETURNS_H

/// returns
/// =======
/// 
/// Description
/// -----------
/// 
/// The `BOOST_FIT_RETURNS` macro helps deduce the return type of an expression
/// using the trailing return type. Instead of repeating the expression for
/// the return type and using the expression in the function body, this macro
/// helps reduce the code duplication from that.
/// 
/// Note: The expression used to deduce the return the type will also
/// constrain the template function as well, which is different behaviour than
/// using C++14's return type deduction.
/// 
/// Synopsis
/// --------
/// 
///     #define BOOST_FIT_RETURNS(...) 
/// 
/// 
/// Example
/// -------
/// 
///     template<class T, class U>
///     auto sum(T x, U y) BOOST_FIT_RETURNS(x+y);
/// 
///     assert(3 == sum(1, 2));
/// 
/// 
/// Incomplete this
/// ===============
/// 
/// Description
/// -----------
/// 
/// On older compilers the `this` variable cannot be used inside the
/// `BOOST_FIT_RETURNS` macro because it is considered an incomplete type. So the
/// following macros are provided to help workaround the issue.
/// 
/// 
/// Synopsis
/// --------
/// 
///     // Declares the type of the `this` variable
///     #define BOOST_FIT_RETURNS_CLASS(...) 
///     // Used to refer to the `this` variable in the BOOST_FIT_RETURNS macro
///     #define BOOST_FIT_THIS
///     // Used to refer to the const `this` variable in the BOOST_FIT_RETURNS macro
///     #define BOOST_FIT_CONST_THIS
/// 
/// 
/// Example
/// -------
/// 
///     struct add_1
///     {
///         int a;
///         add_1() : a(1) {}
///         
///         BOOST_FIT_RETURNS_CLASS(add_1)
///         
///         template<class T>
///         auto operator()(T x) const 
///         BOOST_FIT_RETURNS(x+BOOST_FIT_CONST_THIS->a);
///     };
/// 
///     assert(3 == add_1()(2));
/// 
/// 
/// Mangling overloads
/// ==================
/// 
/// Description
/// -----------
/// 
/// On older compilers some operations done in the expressions cannot be
/// properly mangled. These macros help provide workarounds for these
/// operations on older compilers.
/// 
/// 
/// Synopsis
/// --------
/// 
///     // Explicitly defines the type for name mangling
///     #define BOOST_FIT_MANGLE_CAST(...) 
///     // C cast for name mangling
///     #define BOOST_FIT_RETURNS_C_CAST(...) 
///     // Reinterpret cast for name mangling
///     #define BOOST_FIT_RETURNS_REINTERPRET_CAST(...) 
///     // Static cast for name mangling
///     #define BOOST_FIT_RETURNS_STATIC_CAST(...) 
///     // Construction for name mangling
///     #define BOOST_FIT_RETURNS_CONSTRUCT(...) 
/// 


#ifndef BOOST_FIT_HAS_MANGLE_OVERLOAD
#if defined(__GNUC__) && !defined (__clang__) && __GNUC__ == 4 && __GNUC_MINOR__ < 7
#define BOOST_FIT_HAS_MANGLE_OVERLOAD 0
#else
#define BOOST_FIT_HAS_MANGLE_OVERLOAD 1
#endif
#endif

#ifndef BOOST_FIT_HAS_COMPLETE_DECLTYPE
#if !BOOST_FIT_HAS_MANGLE_OVERLOAD || (defined(__GNUC__) && !defined (__clang__) && __GNUC__ == 4 && __GNUC_MINOR__ < 8)
#define BOOST_FIT_HAS_COMPLETE_DECLTYPE 0
#else
#define BOOST_FIT_HAS_COMPLETE_DECLTYPE 1
#endif
#endif

#include <utility>
#include <boost/fit/detail/forward.hpp>

#define BOOST_FIT_EAT(...)
#define BOOST_FIT_REM(...) __VA_ARGS__

#if BOOST_FIT_HAS_COMPLETE_DECLTYPE && BOOST_FIT_HAS_MANGLE_OVERLOAD
#define BOOST_FIT_RETURNS(...) -> decltype(__VA_ARGS__) { return __VA_ARGS__; }
#define BOOST_FIT_THIS this
#define BOOST_FIT_CONST_THIS this
#define BOOST_FIT_RETURNS_CLASS(...) \
void fit_returns_class_check() \
{ \
    static_assert(std::is_same<__VA_ARGS__*, decltype(this)>::value, \
        "Returns class " #__VA_ARGS__ " type doesn't match"); \
}

#define BOOST_FIT_MANGLE_CAST(...) BOOST_FIT_REM

#define BOOST_FIT_RETURNS_C_CAST(...) (__VA_ARGS__) BOOST_FIT_REM
#define BOOST_FIT_RETURNS_REINTERPRET_CAST(...) reinterpret_cast<__VA_ARGS__>
#define BOOST_FIT_RETURNS_STATIC_CAST(...) static_cast<__VA_ARGS__>
#define BOOST_FIT_RETURNS_CONSTRUCT(...) __VA_ARGS__
#else
#include <boost/fit/detail/pp.hpp>

#define BOOST_FIT_RETURNS_RETURN(...) return BOOST_FIT_RETURNS_RETURN_X(BOOST_FIT_PP_WALL(__VA_ARGS__))
#define BOOST_FIT_RETURNS_RETURN_X(...) __VA_ARGS__

#define BOOST_FIT_RETURNS_DECLTYPE(...) decltype(BOOST_FIT_RETURNS_DECLTYPE_CONTEXT(__VA_ARGS__))

#define BOOST_FIT_RETURNS_DECLTYPE_CONTEXT(...) BOOST_FIT_RETURNS_DECLTYPE_CONTEXT_X(BOOST_FIT_PP_WALL(__VA_ARGS__))
#define BOOST_FIT_RETURNS_DECLTYPE_CONTEXT_X(...) __VA_ARGS__

#define BOOST_FIT_RETURNS_THAT(...) BOOST_FIT_PP_IIF(BOOST_FIT_PP_IS_PAREN(BOOST_FIT_RETURNS_DECLTYPE_CONTEXT(())))(\
    (boost::fit::detail::check_this<__VA_ARGS__, decltype(this)>(), this), \
    std::declval<__VA_ARGS__>() \
)

#define BOOST_FIT_THIS BOOST_FIT_PP_RAIL(BOOST_FIT_RETURNS_THAT)(fit_this_type)
#define BOOST_FIT_CONST_THIS BOOST_FIT_PP_RAIL(BOOST_FIT_RETURNS_THAT)(fit_const_this_type)

#define BOOST_FIT_RETURNS_CLASS(...) typedef __VA_ARGS__* fit_this_type; typedef const __VA_ARGS__* fit_const_this_type

#define BOOST_FIT_RETURNS(...) -> BOOST_FIT_RETURNS_DECLTYPE(__VA_ARGS__) { BOOST_FIT_RETURNS_RETURN(__VA_ARGS__); }


namespace boost { namespace fit { namespace detail {
template<class Assumed, class T>
struct check_this
{
    static_assert(std::is_same<T, Assumed>::value, "Incorret BOOST_FIT_THIS or BOOST_FIT_CONST_THIS used.");
};

}}} // namespace boost::fit

#endif


#if BOOST_FIT_HAS_MANGLE_OVERLOAD

#define BOOST_FIT_MANGLE_CAST(...) BOOST_FIT_REM

#define BOOST_FIT_RETURNS_C_CAST(...) (__VA_ARGS__) BOOST_FIT_REM
#define BOOST_FIT_RETURNS_REINTERPRET_CAST(...) reinterpret_cast<__VA_ARGS__>
#define BOOST_FIT_RETURNS_STATIC_CAST(...) static_cast<__VA_ARGS__>
#define BOOST_FIT_RETURNS_CONSTRUCT(...) __VA_ARGS__

#else

#define BOOST_FIT_RETURNS_DERAIL_MANGLE_CAST(...) BOOST_FIT_PP_IIF(BOOST_FIT_PP_IS_PAREN(BOOST_FIT_RETURNS_DECLTYPE_CONTEXT(())))(\
    BOOST_FIT_REM, \
    std::declval<__VA_ARGS__>() BOOST_FIT_EAT \
)
#define BOOST_FIT_MANGLE_CAST BOOST_FIT_PP_RAIL(BOOST_FIT_RETURNS_DERAIL_MANGLE_CAST)


#define BOOST_FIT_RETURNS_DERAIL_C_CAST(...) BOOST_FIT_PP_IIF(BOOST_FIT_PP_IS_PAREN(BOOST_FIT_RETURNS_DECLTYPE_CONTEXT(())))(\
    (__VA_ARGS__) BOOST_FIT_REM, \
    std::declval<__VA_ARGS__>() BOOST_FIT_EAT \
)
#define BOOST_FIT_RETURNS_C_CAST BOOST_FIT_PP_RAIL(BOOST_FIT_RETURNS_DERAIL_C_CAST)


#define BOOST_FIT_RETURNS_DERAIL_REINTERPRET_CAST(...) BOOST_FIT_PP_IIF(BOOST_FIT_PP_IS_PAREN(BOOST_FIT_RETURNS_DECLTYPE_CONTEXT(())))(\
    reinterpret_cast<__VA_ARGS__>, \
    std::declval<__VA_ARGS__>() BOOST_FIT_EAT \
)
#define BOOST_FIT_RETURNS_REINTERPRET_CAST BOOST_FIT_PP_RAIL(BOOST_FIT_RETURNS_DERAIL_REINTERPRET_CAST)

#define BOOST_FIT_RETURNS_DERAIL_STATIC_CAST(...) BOOST_FIT_PP_IIF(BOOST_FIT_PP_IS_PAREN(BOOST_FIT_RETURNS_DECLTYPE_CONTEXT(())))(\
    static_cast<__VA_ARGS__>, \
    std::declval<__VA_ARGS__>() BOOST_FIT_EAT \
)
#define BOOST_FIT_RETURNS_STATIC_CAST BOOST_FIT_PP_RAIL(BOOST_FIT_RETURNS_DERAIL_STATIC_CAST)

#define BOOST_FIT_RETURNS_DERAIL_CONSTRUCT(...) BOOST_FIT_PP_IIF(BOOST_FIT_PP_IS_PAREN(BOOST_FIT_RETURNS_DECLTYPE_CONTEXT(())))(\
    __VA_ARGS__, \
    std::declval<__VA_ARGS__>() BOOST_FIT_EAT \
)
#define BOOST_FIT_RETURNS_CONSTRUCT BOOST_FIT_PP_RAIL(BOOST_FIT_RETURNS_DERAIL_CONSTRUCT)

#endif

#define BOOST_FIT_AUTO_FORWARD(...) static_cast<decltype(__VA_ARGS__)>(__VA_ARGS__)

#endif
