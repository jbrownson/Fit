/*=============================================================================
    Copyright (c) 2015 Paul Fultz II
    apply.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FIT_GUARD_APPLY_H
#define BOOST_FIT_GUARD_APPLY_H

/// apply
/// =====
/// 
/// Description
/// -----------
/// 
/// The `apply` function calls the function given to it with its arguments.
/// 
/// Synopsis
/// --------
/// 
///     template<class F, class... Ts>
///     constexpr auto apply(F&& f, Ts&&... xs);
/// 
/// Semantics
/// ---------
/// 
///     assert(apply(f)(xs...) == f(xs...));
/// 
/// Requirements
/// ------------
/// 
/// F must be:
/// 
/// * [Callable](concepts.md#callable)
/// 
/// Example
/// -------
/// 
///     struct sum_f
///     {
///         template<class T, class U>
///         T operator()(T x, U y) const
///         {
///             return x+y;
///         }
///     };
///     assert(boost::fit::apply(sum_f(), 1, 2) == 3);
/// 

#include <boost/fit/detail/result_of.hpp>
#include <boost/fit/detail/forward.hpp>
#include <boost/fit/detail/static_const_var.hpp>


namespace boost { namespace fit {

namespace detail {
#if BOOST_FIT_HAS_MANUAL_DEDUCTION || BOOST_FIT_NO_EXPRESSION_SFINAE
struct apply_mem_fn
{
    template<class...>
    struct convertible_args;

    template<class T, class U>
    struct is_convertible_args;

    template<class... Ts, class... Us>
    struct is_convertible_args<convertible_args<Ts...>, convertible_args<Us...>>
    : and_<std::is_convertible<Ts, Us>...>
    {};

#define BOOST_FIT_APPLY_MEM_FN_CALL(cv) \
    template <class R, class Base, class Derived, class... Ts, class... Us, class=typename std::enable_if<and_< \
        std::is_base_of<Base, typename std::decay<Derived>::type>, \
        is_convertible_args<convertible_args<Us...>, convertible_args<Ts...>> \
    >::value>::type> \
    constexpr R operator()(R (Base::*mf)(Ts...) cv, Derived&& ref, Us &&... xs) const \
    { \
        return (boost::fit::forward<Derived>(ref).*mf)(boost::fit::forward<Us>(xs)...); \
    }
    BOOST_FIT_APPLY_MEM_FN_CALL()
    BOOST_FIT_APPLY_MEM_FN_CALL(const)
    BOOST_FIT_APPLY_MEM_FN_CALL(volatile)
    BOOST_FIT_APPLY_MEM_FN_CALL(const volatile)
};

struct apply_mem_data
{
    template <class Base, class R, class Derived, class=typename std::enable_if<(
        std::is_base_of<Base, typename std::decay<Derived>::type>::value
    )>::type>
    constexpr R operator()(R Base::*pmd, Derived&& ref) const
    {
        return boost::fit::forward<Derived>(ref).*pmd;
    }
};

template<class T, class U=decltype(*std::declval<T>())>
struct apply_deref
{ typedef U type; };

#endif

struct apply_f
{
#if BOOST_FIT_HAS_MANUAL_DEDUCTION || BOOST_FIT_NO_EXPRESSION_SFINAE
    template<class F, class T, class... Ts, class=typename std::enable_if<(
        std::is_member_function_pointer<typename std::decay<F>::type>::value
    )>::type>
    constexpr BOOST_FIT_SFINAE_MANUAL_RESULT(apply_mem_fn, id_<F>, id_<T>, id_<Ts>...) 
    operator()(F&& f, T&& obj, Ts&&... xs) const BOOST_FIT_SFINAE_MANUAL_RETURNS
    (
        apply_mem_fn()(f, boost::fit::forward<T>(obj), boost::fit::forward<Ts>(xs)...)
    );

    template<class F, class T, class... Ts, class U=typename apply_deref<T>::type, class=typename std::enable_if<(
        std::is_member_function_pointer<typename std::decay<F>::type>::value
    )>::type>
    constexpr BOOST_FIT_SFINAE_MANUAL_RESULT(apply_mem_fn, id_<F>, id_<U>, id_<Ts>...) 
    operator()(F&& f, T&& obj, Ts&&... xs) const BOOST_FIT_SFINAE_MANUAL_RETURNS
    (
        apply_mem_fn()(f, *boost::fit::forward<T>(obj), boost::fit::forward<Ts>(xs)...)
    );

    template<class F, class T, class=typename std::enable_if<(
        std::is_member_object_pointer<typename std::decay<F>::type>::value
    )>::type>
    constexpr BOOST_FIT_SFINAE_MANUAL_RESULT(apply_mem_data, id_<F>, id_<T>) 
    operator()(F&& f, T&& obj) const BOOST_FIT_SFINAE_MANUAL_RETURNS
    (
        apply_mem_data()(f, boost::fit::forward<T>(obj))
    );

    template<class F, class T, class U=typename apply_deref<T>::type, class=typename std::enable_if<(
        std::is_member_object_pointer<typename std::decay<F>::type>::value
    )>::type>
    constexpr BOOST_FIT_SFINAE_MANUAL_RESULT(apply_mem_data, id_<F>, id_<U>) 
    operator()(F&& f, T&& obj) const BOOST_FIT_SFINAE_MANUAL_RETURNS
    (
        apply_mem_data()(f, *boost::fit::forward<T>(obj))
    );

#else

    template <class Base, class T, class Derived>
    constexpr auto operator()(T Base::*pmd, Derived&& ref) const
    BOOST_FIT_RETURNS(boost::fit::forward<Derived>(ref).*pmd);
     
    template <class PMD, class Pointer>
    constexpr auto operator()(PMD&& pmd, Pointer&& ptr) const
    BOOST_FIT_RETURNS((*boost::fit::forward<Pointer>(ptr)).*boost::fit::forward<PMD>(pmd));
     
    template <class Base, class T, class Derived, class... Args>
    constexpr auto operator()(T Base::*pmf, Derived&& ref, Args&&... args) const
    BOOST_FIT_RETURNS((boost::fit::forward<Derived>(ref).*pmf)(boost::fit::forward<Args>(args)...));
     
    template <class PMF, class Pointer, class... Args>
    constexpr auto operator()(PMF&& pmf, Pointer&& ptr, Args&&... args) const
    BOOST_FIT_RETURNS(((*boost::fit::forward<Pointer>(ptr)).*boost::fit::forward<PMF>(pmf))(boost::fit::forward<Args>(args)...));

#endif
    template<class F, class... Ts>
    constexpr BOOST_FIT_SFINAE_RESULT(F, id_<Ts>...) 
    operator()(F&& f, Ts&&... xs) const BOOST_FIT_SFINAE_RETURNS
    (
        f(boost::fit::forward<Ts>(xs)...)
    );
};

}

BOOST_FIT_DECLARE_STATIC_VAR(apply, detail::apply_f);

}} // namespace boost::fit

#endif
