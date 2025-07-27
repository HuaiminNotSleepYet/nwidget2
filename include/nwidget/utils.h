#ifndef NWIDGET_UTILS_H
#define NWIDGET_UTILS_H

#include <functional>
#include <tuple>
#include <type_traits>

#include <QtGlobal>

#define N_DISABLE_COPY(Class)                                                                                          \
    Class(const Class&)            = delete;                                                                           \
    Class& operator=(const Class&) = delete;

#define N_DISABLE_COPY_MOVE(Class)                                                                                     \
    N_DISABLE_COPY(Class)                                                                                              \
    Class(Class&&)            = delete;                                                                                \
    Class& operator=(Class&&) = delete;

// clang-format off

#define N_IMPL_CAT(A, B) N_IMPL_CAT_IMPL(A, B)
#define N_IMPL_CAT_IMPL(A, B) A##B

#define N_IMPL_CHOOSE32(_32,_31,_30,_29,_28,_27,_26,_25,_24,_23,_22,_21,_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0, N, ...) N

#define N_IMPL_EXPAND(...) __VA_ARGS__ // Needed for MSVC

#ifdef _MSC_VER
#define N_IMPL_COUNT_IMPL0(...) N_IMPL_EXPAND(N_IMPL_CHOOSE32(__VA_ARGS__, 32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))
#define N_IMPL_COUNT_IMPL1(...) unused,  __VA_ARGS__
#define N_IMPL_COUNT(...) N_IMPL_COUNT_IMPL0(N_IMPL_COUNT_IMPL1(__VA_ARGS__))
#else
#define N_IMPL_COUNT(...) N_IMPL_CHOOSE32(0, ##__VA_ARGS__, 32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#endif

#define N_IMPL_OVERLOAD(FUNC, ...) N_IMPL_EXPAND(N_IMPL_CAT(FUNC, N_IMPL_COUNT(__VA_ARGS__))(__VA_ARGS__))

// clang-format on

namespace nwidget {
namespace impl {

/* ---------------------------------------------- For C++ Versions < 17 --------------------------------------------- */

// void_t

template <typename...> using void_t = void;

// fold expr

template <typename Op, typename...> struct fold;

template <typename Op, typename Arg> struct fold<Op, Arg> : Arg
{
};

template <typename Op, typename Arg, typename... ArgN> struct fold<Op, Arg, ArgN...>
{
    static constexpr auto value = Op{}(Arg::value, fold<Op, ArgN...>::value);
    using value_type            = decltype(value);
};

// apply

template <typename Fn, typename Tup, size_t... I> auto apply(Fn f, const Tup& t, std::index_sequence<I...>)
{
    return f(std::get<I>(t)...);
}

template <typename Fn, typename Tup> auto apply(Fn f, const Tup& t)
{
    return apply(f, t, std::make_index_sequence<std::tuple_size<Tup>::value>{});
}

// as_const

template <typename T> constexpr std::add_const_t<T>& as_const(T& t) noexcept { return t; }
template <typename T> void                           as_const(const T&&) = delete;

/* ------------------------------------------------------ Utils ----------------------------------------------------- */

// for_each

template <typename Fn, typename Tup, size_t... I>
constexpr auto for_each(const Fn& f, const Tup& t, std::index_sequence<I...>)
    -> std::enable_if_t<fold<std::logical_and<bool>, std::is_void<decltype(f(std::get<I>(t)))>...>::value>
{
    int _[]{(f(std::get<I>(t)), 0)...};
}

template <typename Fn, typename Tup, size_t... I>
constexpr auto for_each(const Fn& f, const Tup& t, std::index_sequence<I...>)
    -> decltype(std::make_tuple(f(std::get<I>(t))...))
{
    return std::make_tuple(f(std::get<I>(t))...);
}

template <typename Fn, typename Tup> constexpr auto for_each(const Fn& f, const Tup& t)
{
    return for_each(f, t, std::make_index_sequence<std::tuple_size<Tup>::value>{});
}

// member function type traits

template <typename T> struct mem_fn;

template <typename C, typename R, typename... T> struct mem_fn<R (C::*)(T...)>
{
    using result = R;
    using object = C;

    static constexpr std::size_t argc = sizeof...(T);

    template <std::size_t i> using arg = std::tuple_element_t<i, std::tuple<T...>>;
};

template <typename C, typename R, typename... T> struct mem_fn<R (C::*)(T...) const>
{
    using result = R;
    using object = C;

    static constexpr std::size_t argc = sizeof...(T);

    template <std::size_t i> using arg = std::tuple_element_t<i, std::tuple<T...>>;
};

} // namespace impl
} // namespace nwidget

/* -------------------------------------------------- Version Check ------------------------------------------------- */

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#define N_IMPL_UNTIL_5_14(F) F
#else
#define N_IMPL_UNTIL_5_14(F)
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#define N_IMPL_SINCE_5_14(F) F
#else
#define N_IMPL_SINCE_5_14(F)
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
#define N_IMPL_UNTIL_5_15(F) F
#else
#define N_IMPL_UNTIL_5_15(F)
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#define N_IMPL_SINCE_5_15(F) F
#else
#define N_IMPL_SINCE_5_15(F)
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define N_IMPL_UNTIL_6_0(F) F
#else
#define N_IMPL_UNTIL_6_0(F)
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define N_IMPL_SINCE_6_0(F) F
#else
#define N_IMPL_SINCE_6_0(F)
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 1, 0)
#define N_IMPL_UNTIL_6_1(F) F
#else
#define N_IMPL_UNTIL_6_1(F)
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 1, 0)
#define N_IMPL_SINCE_6_1(F) F
#else
#define N_IMPL_SINCE_6_1(F)
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
#define N_IMPL_UNTIL_6_2(F) F
#else
#define N_IMPL_UNTIL_6_2(F)
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
#define N_IMPL_SINCE_6_2(F) F
#else
#define N_IMPL_SINCE_6_2(F)
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 3, 0)
#define N_IMPL_UNTIL_6_3(F) F
#else
#define N_IMPL_UNTIL_6_3(F)
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
#define N_IMPL_SINCE_6_3(F) F
#else
#define N_IMPL_SINCE_6_3(F)
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 4, 0)
#define N_IMPL_UNTIL_6_4(F) F
#else
#define N_IMPL_UNTIL_6_4(F)
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
#define N_IMPL_SINCE_6_4(F) F
#else
#define N_IMPL_SINCE_6_4(F)
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
#define N_IMPL_UNTIL_6_5(F) F
#else
#define N_IMPL_UNTIL_6_5(F)
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
#define N_IMPL_SINCE_6_5(F) F
#else
#define N_IMPL_SINCE_6_5(F)
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 6, 0)
#define N_IMPL_UNTIL_6_6(F) F
#else
#define N_IMPL_UNTIL_6_6(F)
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
#define N_IMPL_SINCE_6_6(F) F
#else
#define N_IMPL_SINCE_6_6(F)
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
#define N_IMPL_UNTIL_6_7(F) F
#else
#define N_IMPL_UNTIL_6_7(F)
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
#define N_IMPL_SINCE_6_7(F) F
#else
#define N_IMPL_SINCE_6_7(F)
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 8, 0)
#define N_IMPL_UNTIL_6_8(F) F
#else
#define N_IMPL_UNTIL_6_8(F)
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
#define N_IMPL_SINCE_6_8(F) F
#else
#define N_IMPL_SINCE_6_8(F)
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 9, 0)
#define N_IMPL_UNTIL_6_9(F) F
#else
#define N_IMPL_UNTIL_6_9(F)
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
#define N_IMPL_SINCE_6_9(F) F
#else
#define N_IMPL_SINCE_6_9(F)
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 10, 0)
#define N_IMPL_UNTIL_6_10(F) F
#else
#define N_IMPL_UNTIL_6_10(F)
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
#define N_IMPL_SINCE_6_10(F) F
#else
#define N_IMPL_SINCE_6_10(F)
#endif

#define N_SINCE(major, minor, F) N_IMPL_SINCE_##major##_##minor(N_IMPL_EXPAND(F))
#define N_UNTIL(major, minor, F) N_IMPL_UNTIL_##major##_##minor(N_IMPL_EXPAND(F))

#endif // NWIDGET_UTILS_H
