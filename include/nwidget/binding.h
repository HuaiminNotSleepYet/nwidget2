/**
 * @brief Property binding framework for Qt Widgets
 * @details
 * Example:
 *      @code{.cpp}
 *      auto slider1 = MetaObject<>::from(new QSlider(Qt::Horizontal));
 *      auto slider2 = MetaObject<>::from(new QSlider(Qt::Horizontal));
 *      auto slider3 = MetaObject<>::from(new QSlider(Qt::Horizontal));
 *
 *      // Binding will be created in the following way:
 *      slider3.value() = slider1.value()
 *                      + slider2.value();
 *
 *      // or
 *      auto expr = (slider1.value() + slider2.value());
 *      expr.bindTo(slider3.value());
 *
 *      // Complex expr is also supported
 *      auto label = MetaObject<>::from(new QLabel);
 *      label.text() = cond(slider1.value() + slider2.value() >= 100,
 *                          ">=100",
 *                          "< 100");
 *      @endcode
 *
 * A MetaProperty or BindingExpr can bind to a MetaProperty, slot, functor:
 *      @code{.cpp}
 *      auto expr = ...;
 *      expr.bindTo(MetaObject<>::from(obj).property());
 *      expr.bindTo(obj, &Class::setProperty);
 *      expr.bindTo([](int v){});
 *      @endcode
 */

#ifndef NWIDGET_BINDING_H
#define NWIDGET_BINDING_H

#include "metaobject.h"

#include <QSignalMapper>

namespace nwidget {

namespace impl {

struct ActionEmpty
{
    template <typename T> auto operator()(T&& val) const { return val; }
};

// clang-format off

template <typename ...T> struct is_observable;

template <typename T> constexpr bool is_observable_v = is_observable<T>::value;

template <typename T> struct is_observable<T>
    : std::false_type {};

template <typename ...T> struct is_observable<MetaProperty<T...>>
    : std::integral_constant<bool, MetaProperty<T...>::hasNotifySignal> {};

template <typename Action, typename... Args> struct is_observable<BindingExpr<Action, Args...>>
    : std::integral_constant<bool, impl::fold<std::logical_or<bool>, std::false_type, is_observable<Args>...>::value> {};

template<typename T> struct is_meta_property : std::false_type {};
template<typename T> constexpr bool is_meta_property_v = is_meta_property<T>::value;
template<typename... T> struct is_meta_property<MetaProperty<T...>> : std::true_type {};

template<typename T> struct is_binding_expr : std::false_type {};
template<typename T> constexpr bool is_binding_expr_v = is_binding_expr<T>::value;
template<typename... T> struct is_binding_expr<BindingExpr<T...>> : std::true_type {};

// clang-format on

} // namespace impl

template <typename Action = impl::ActionEmpty, // struct { auto operator()(Args&&...) const { return ... } }
          typename... Args>
BindingExpr<Action, std::decay_t<Args>...> makeBindingExpr(Args&&... args)
{
    return BindingExpr<Action, std::decay_t<Args>...>(std::forward<Args>(args)...);
}

template <> class BindingExpr<>
{
public:
    template <typename...> friend class BindingExpr;

    template <typename T> static auto    eval(const T& val) { return val; }
    template <typename... T> static auto eval(const BindingExpr<T...>& expr) { return expr.eval(); }
    template <typename... T> static auto eval(MetaProperty<T...> prop) { return prop.get(); }

    template <typename T, std::enable_if_t<!impl::is_meta_property_v<T> && !impl::is_binding_expr_v<T>, bool> = true>
    static void bind(QSignalMapper* binding, T)
    {
    }

    template <typename T, std::enable_if_t<impl::is_meta_property_v<T> && T::hasNotifySignal, bool> = true>
    static void bind(QSignalMapper* binding, T prop)
    {
        auto obj = prop.object();
        QObject::connect(obj, &QObject::destroyed, binding, [binding]() { delete binding; });
        QObject::connect(obj, T::notify(), binding, qOverload<>(&QSignalMapper::map), Qt::UniqueConnection);
        binding->setMapping(obj, 0);
    }

    template <typename T, std::enable_if_t<impl::is_meta_property_v<T> && !T::hasNotifySignal, bool> = true>
    static void bind(QSignalMapper* binding, T prop)
    {
        QObject::connect(prop.object(), &QObject::destroyed, binding, [binding]() { delete binding; });
    }

    template <typename T, std::enable_if_t<impl::is_binding_expr_v<T>, bool> = true>
    static void bind(QSignalMapper* binding, const T& expr)
    {
        impl::for_each([binding](const auto& arg) { bind(binding, arg); }, expr.args);
    }

    // clang-format off
    template <typename E, typename C, typename F> static auto makeFunctor(const E& e, C*  , const F& f, std::enable_if_t< impl::is_meta_property_v<F>                                                          , bool> = true) { return [e,    f]() { f.set(e.eval()); }; }
    template <typename E, typename C, typename F> static auto makeFunctor(const E&  , C*  , const F& f, std::enable_if_t<!impl::is_meta_property_v<F> && std::is_void<decltype((void)f(        )      )>::value, bool> = true) { return [      f]() { f(        ); }; }
    template <typename E, typename C, typename F> static auto makeFunctor(const E& e, C*  , const F& f, std::enable_if_t<!impl::is_meta_property_v<F> && std::is_void<decltype((void)f(e.eval())      )>::value, bool> = true) { return [e,    f]() { f(e.eval()); }; }
    template <typename E, typename C, typename F> static auto makeFunctor(const E&  , C* r, const F& f, std::enable_if_t<!impl::is_meta_property_v<F> && std::is_void<decltype((void)(r->*f)(        ))>::value, bool> = true) { return [   r, f]() { (r->*f)(        ); }; }
    template <typename E, typename C, typename F> static auto makeFunctor(const E& e, C* r, const F& f, std::enable_if_t<!impl::is_meta_property_v<F> && std::is_void<decltype((void)(r->*f)(e.eval()))>::value, bool> = true) { return [e, r, f]() { (r->*f)(e.eval()); }; }
    // clang-format on
};

template <typename Action, typename... Args> class BindingExpr<Action, Args...>
{
    template <typename...> friend class BindingExpr;

public:
    using Type = decltype(Action{}(BindingExpr<>::eval(std::declval<Args>())...));

    explicit BindingExpr(const Args&... args)
        : args(args...)
    {
    }

    template <typename... As> auto i(As&&... args) const { return invoke(*this, std::forward<As>(args)...); }

    template <typename F, typename... As> auto m(F f, As&&... args)
    {
        return invoke(f, *this, std::forward<As>(args)...);
    }

    auto eval() const
    {
        return impl::apply(Action{}, impl::for_each([](const auto& arg) { return BindingExpr<>::eval(arg); }, args));
    }

    template <typename... T> auto bindTo(MetaProperty<T...> prop, Qt::ConnectionType type = Qt::AutoConnection) const
    {
        return bindTo(prop.object(), prop, MetaProperty<T...>::Info::bindingName(), type);
    }

    template <typename Func> auto bindTo(Func func) const
    {
        static const QString bindingName =
#ifdef Q_CC_MSVC
            QStringLiteral("nwidget_binding_to_func::") + __FUNCSIG__;
#elif Q_CC_GNU
            QStringLiteral("nwidget_binding_to_func::") + __PRETTY_FUNCTION__;
#else
            QStringLiteral("nwidget_binding_to_func");
#endif
        return bindTo((QObject*)nullptr, func, bindingName, Qt::DirectConnection);
    }

    template <typename Class, typename Func>
    auto bindTo(Class* receiver, Func func, Qt::ConnectionType type = Qt::AutoConnection) const
    {
        static const QString bindingName =
#ifdef Q_CC_MSVC
            QStringLiteral("nwidget_binding_to_func::") + __FUNCSIG__;
#elif Q_CC_GNU
            QStringLiteral("nwidget_binding_to_func::") + __PRETTY_FUNCTION__;
#else
            QStringLiteral("nwidget_binding_to_func");
#endif
        return bindTo(receiver, func, bindingName, Qt::DirectConnection);
    }

private:
    std::tuple<Args...> args;

    template <typename Class, typename Func>
    auto bindTo(Class* receiver, Func func, const QString& name, Qt::ConnectionType type = Qt::AutoConnection) const
    {
        const auto slot = BindingExpr<>::makeFunctor(*this, receiver, func);

        QSignalMapper* binding = nullptr;

        if (receiver)
            binding = receiver->template findChild<QSignalMapper*>(name, Qt::FindDirectChildrenOnly);

        if (!impl::is_observable_v<BindingExpr>) {
            if (binding)
                binding->disconnect();
            slot();
            return *this;
        }

        if (binding) {
            binding->disconnect();
        } else {
            binding = new QSignalMapper(receiver);
            binding->setObjectName(name);
        }

        BindingExpr<>::bind(binding, *this);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QObject::connect(binding, &QSignalMapper::mappedInt, binding, slot, type);
#else
        QObject::connect(binding, QOverload<int>::of(&QSignalMapper::mapped), binding, slot, type);
#endif

        slot();

        return *this;
    }
};

namespace impl {

// clang-format off

template<typename To> struct ActionCast            { template<typename From> auto operator()(From&& from){ return (To)from;                   } };
template<typename To> struct ActionStaticCast      { template<typename From> auto operator()(From&& from){ return static_cast<To>(from);      } };
template<typename To> struct ActionReinterpretCast { template<typename From> auto operator()(From&& from){ return reinterpret_cast<To>(from); } };

template<typename T> struct ActionConstructor { template<typename ...Args> T operator()(Args&&... args){ return T(std::forward<Args>(args)...); } };
struct ActionCond { template<typename A, typename B, typename C> auto operator()(A&& a, B&& b, C&& c) { return a ? b : c; } };

// clang-format on

struct ActionInvoke
{
    template <typename F, typename C, std::enable_if_t<std::is_member_object_pointer<F>::value, bool> = true>
    auto operator()(F fn, C&& obj) const -> decltype(obj->*fn)
    {
        return obj->*fn;
    }

    template <typename F, typename C, std::enable_if_t<std::is_member_object_pointer<F>::value, bool> = true>
    auto operator()(F fn, C&& obj) const -> decltype(obj.*fn)
    {
        return obj.*fn;
    }

    template <typename F, typename C, typename... Args>
    auto operator()(F fn, C&& obj, Args&&... args) const -> decltype((obj->*fn)(std::forward<Args>(args)...))
    {
        return (obj->*fn)(std::forward<Args>(args)...);
    }

    template <typename F, typename C, typename... Args>
    auto operator()(F fn, C&& obj, Args&&... args) const -> decltype((obj.*fn)(std::forward<Args>(args)...))
    {
        return (obj.*fn)(std::forward<Args>(args)...);
    }

    template <typename F, typename... Args>
    auto operator()(F fn, Args&&... args) const -> decltype(fn(std::forward<Args>(args)...))
    {
        return fn(std::forward<Args>(args)...);
    }
};

} // namespace impl

// clang-format off

template<typename To, typename From> auto cast(From&& from)              { return makeBindingExpr<impl::ActionCast           <To>>(std::forward<From>(from)); }
template<typename To, typename From> auto static_cast_(From&& from)      { return makeBindingExpr<impl::ActionStaticCast     <To>>(std::forward<From>(from)); }
template<typename To, typename From> auto reinterpret_cast_(From&& from) { return makeBindingExpr<impl::ActionReinterpretCast<To>>(std::forward<From>(from)); }

template<typename A, typename B, typename C> auto cond(const A& a, const B& b, const C& c) { return makeBindingExpr<impl::ActionCond>(a, b, c); }

template<typename F, typename ...Args> auto invoke(F func, Args&&... args) { return makeBindingExpr<impl::ActionInvoke>(func, std::forward<Args>(args)...); }

template<typename T, typename ...Args> auto constructor(Args&&... args) { return makeBindingExpr<impl::ActionConstructor<T>>(std::forward<Args>(args)...); }

// clang-format on

template <typename... Args> auto asprintf_(const char* cformat, const Args&... args)
{
    return invoke(QString::asprintf, cformat, args...);
}

} // namespace nwidget

#define N_IMPL_ACTION_BE(NAME, OP)                                                                                     \
    namespace nwidget::impl {                                                                                          \
    struct Action##NAME                                                                                                \
    {                                                                                                                  \
        template <typename L, typename R> auto operator()(L&& l, R&& r) const { return l OP r; }                       \
    };                                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    template <typename L,                                                                                              \
              typename R,                                                                                              \
              std::enable_if_t<nwidget::impl::is_meta_property_v<std::decay_t<L>>                                      \
                                   || nwidget::impl::is_meta_property_v<std::decay_t<R>>                               \
                                   || nwidget::impl::is_binding_expr_v<std::decay_t<L>>                                \
                                   || nwidget::impl::is_binding_expr_v<std::decay_t<R>>,                               \
                               int> = 0>                                                                               \
    auto operator OP(L&& l, R&& r)                                                                                     \
    {                                                                                                                  \
        return nwidget::makeBindingExpr<nwidget::impl::Action##NAME>(std::forward<L>(l), std::forward<R>(r));          \
    }

N_IMPL_ACTION_BE(Add, +)
N_IMPL_ACTION_BE(Sub, -)
N_IMPL_ACTION_BE(Mul, *)
N_IMPL_ACTION_BE(Div, /)

N_IMPL_ACTION_BE(EQ, ==)
N_IMPL_ACTION_BE(NE, !=)
N_IMPL_ACTION_BE(LT, <)
N_IMPL_ACTION_BE(LE, <=)
N_IMPL_ACTION_BE(GT, >)
N_IMPL_ACTION_BE(GE, >=)

N_IMPL_ACTION_BE(And, &&)
N_IMPL_ACTION_BE(Or, ||)

N_IMPL_ACTION_BE(BitAnd, &)
N_IMPL_ACTION_BE(BitOR, |)
N_IMPL_ACTION_BE(BitXOR, ^)
N_IMPL_ACTION_BE(BitLShift, <<)
N_IMPL_ACTION_BE(BitRShift, >>)

#define N_IMPL_ACTION_UE(NAME, OP)                                                                                     \
    namespace nwidget::impl {                                                                                          \
    struct Action##NAME                                                                                                \
    {                                                                                                                  \
        template <typename T> auto operator()(T&& val) { return OP val; }                                              \
    };                                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    template <typename T,                                                                                              \
              std::enable_if_t<nwidget::impl::is_meta_property_v<std::decay_t<T>>                                      \
                                   || nwidget::impl::is_binding_expr_v<std::decay_t<T>>,                               \
                               int> = 0>                                                                               \
    auto operator OP(T&& val)                                                                                          \
    {                                                                                                                  \
        return nwidget::makeBindingExpr<nwidget::impl::Action##NAME>(std::forward<T>(val));                            \
    }

N_IMPL_ACTION_UE(Plus, +)
N_IMPL_ACTION_UE(Minus, -)
N_IMPL_ACTION_UE(Not, !)

N_IMPL_ACTION_UE(BitNot, ~)
N_IMPL_ACTION_UE(AddressOf, &)
N_IMPL_ACTION_UE(ContentOf, *)

#endif // NWIDGET_BINDING_H
