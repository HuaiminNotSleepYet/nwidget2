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

template <typename Action, typename... Args> class BindingExpr;

namespace impl::binding {

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
    : std::bool_constant<MetaProperty<T...>::hasNotifySignal> {};

template <typename ...T> struct is_observable
    : std::bool_constant<(... || is_observable_v<T>)> {};

template <typename Action, typename... Args> struct is_observable<BindingExpr<Action, Args...>>
    : std::bool_constant<(... || is_observable_v<Args>)> {};

template<typename T> struct is_meta_property : std::false_type {};
template<typename T> constexpr bool is_meta_property_v = is_meta_property<T>::value;
template<typename... T> struct is_meta_property<MetaProperty<T...>> : std::true_type {};

template<typename T> struct is_binding_expr : std::false_type {};
template<typename T> constexpr bool is_binding_expr_v = is_binding_expr<T>::value;
template<typename... T> struct is_binding_expr<BindingExpr<T...>> : std::true_type {};

// clang-format on

template <typename T> static auto    eval(const T& val) { return val; }
template <typename... T> static auto eval(const BindingExpr<T...>& expr) { return expr.eval(); }
template <typename... T> static auto eval(MetaProperty<T...> prop) { return prop.get(); }

template <typename T> inline static void bind(QSignalMapper* binding, const T& v);

template <typename... T> static void bind0(QSignalMapper* binding, const BindingExpr<T...>& expr)
{
    impl::utils::for_each([binding](const auto& arg) { impl::binding::bind(binding, arg); }, expr.args);
}

template <typename... T> static void bind1(QSignalMapper* binding, MetaProperty<T...> prop)
{
    using MetaProp = MetaProperty<T...>;
    if constexpr (!MetaProp::hasNotifySignal)
        return;
    auto obj = prop.object();
    QObject::connect(obj, &QObject::destroyed, binding, [binding]() { delete binding; });
    if constexpr (MetaProp::hasNotifySignal)
        QObject::connect(obj, MetaProp::notify(), binding, qOverload<>(&QSignalMapper::map), Qt::UniqueConnection);
    binding->setMapping(obj, 0);
}

template <typename T> static void bind(QSignalMapper* binding, const T& v)
{
    if constexpr (is_binding_expr_v<T>)
        impl::binding::bind0(binding, v);
    else if constexpr (is_meta_property_v<T>)
        impl::binding::bind1(binding, v);
}

struct ActionCall;
struct ActionMember;
struct ActionInvoke;

} // namespace impl::binding

template <typename Action = impl::binding::ActionEmpty, // struct { auto operator()(Args&&...) const { return ... } }
          typename... Args>
BindingExpr<Action, std::decay_t<Args>...> makeBindingExpr(Args&&... args)
{
    return BindingExpr<Action, std::decay_t<Args>...>(std::forward<Args>(args)...);
}

template <typename Action, typename... Args> class BindingExpr
{
    template <typename... T> friend void impl::binding::bind0(QSignalMapper*, const BindingExpr<T...>&);

public:
    using Type = decltype(Action{}(impl::binding::eval(std::declval<Args>())...));

    constexpr static bool isObservable = impl::binding::is_observable_v<BindingExpr<Action, Args...>>;

    explicit BindingExpr(const Args&... args)
        : args(args...)
    {
    }

    template <typename F, typename... As> auto i(F f, As&&... args) const
    {
        if constexpr (std::is_member_object_pointer_v<F>)
            return makeBindingExpr<impl::binding::ActionMember>(*this, f, std::forward<As>(args)...);
        else if constexpr (std::is_member_function_pointer_v<F>)
            return makeBindingExpr<impl::binding::ActionInvoke>(*this, f, std::forward<As>(args)...);
    }

    template <typename... As> auto operator()(As&&... args) const
    {
        return makeBindingExpr<impl::binding::ActionCall>(*this, std::forward<As>(args)...);
    }

    auto eval() const
    {
        return std::apply(Action{},
                          impl::utils::for_each([](const auto& arg) { return impl::binding::eval(arg); }, args));
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
        auto slot = [e = *this, r = receiver, f = func]()
        {
            // e, f maybe unused
            Q_UNUSED(e);
            Q_UNUSED(r);
            if constexpr (impl::binding::is_meta_property_v<Func>)
                return [f, e]() { return f.set(e.eval()); };
            else if constexpr (std::is_invocable_v<Func>)
                return [f]() { return f(); };
            else if constexpr (std::is_invocable_v<Func, Type>)
                return [f, e]() { return f(e.eval()); };
            else if constexpr (std::is_member_function_pointer_v<Func> && std::is_invocable_v<Func, Class*>)
                return [r, f]() { return (r->*f)(); };
            else if constexpr (std::is_member_function_pointer_v<Func> && std::is_invocable_v<Func, Class*, Type>)
                return [r, f, e]() { return (r->*f)(e.eval()); };
        }();

        QSignalMapper* binding = nullptr;

        if (receiver)
            binding = receiver->template findChild<QSignalMapper*>(name, Qt::FindDirectChildrenOnly);

        if (!isObservable) {
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

        impl::binding::bind(binding, *this);
        QObject::connect(binding, &QSignalMapper::mappedInt, binding, slot, type);

        slot();

        return *this;
    }
};

namespace impl::binding {

// clang-format off

template<typename To> struct ActionCast            { template<typename From> auto operator()(From&& from){ return (To)from;                   } };
template<typename To> struct ActionStaticCast      { template<typename From> auto operator()(From&& from){ return static_cast<To>(from);      } };
template<typename To> struct ActionReinterpretCast { template<typename From> auto operator()(From&& from){ return reinterpret_cast<To>(from); } };

template<typename T> struct ActionConstructor { template<typename ...Args> T operator()(Args&&... args){ return T(std::forward<Args>(args)...); } };
struct ActionCond { template<typename A, typename B, typename C> auto operator()(A&& a, B&& b, C&& c) { return a ? b : c; } };
struct ActionCall { template<typename F, typename ...Args> auto operator()(F func, Args&&... args) { return func(std::forward<Args>(args)...); } };

// clang-format on

struct ActionMember
{
    template <class Class, typename M> auto operator()(Class&& obj, M mem)
    {
        if constexpr (!std::is_pointer_v<Class>)
            return obj.*mem;
        else {
            Q_ASSERT(obj);
            return obj->*mem;
        }
    }
};

struct ActionInvoke
{
    template <class Class, typename F, typename... Args> auto operator()(Class&& obj, F fn, Args&&... args)
    {
        if constexpr (!std::is_pointer_v<Class>)
            return (obj.*fn)(std::forward<Args>(args)...);
        else {
            Q_ASSERT(obj);
            return (obj->*fn)(std::forward<Args>(args)...);
        }
    }
};

} // namespace impl::binding

// clang-format off

template<typename To, typename From> auto cast(From&& from)              { return makeBindingExpr<impl::binding::ActionCast           <To>>(std::forward<From>(from)); }
template<typename To, typename From> auto static_cast_(From&& from)      { return makeBindingExpr<impl::binding::ActionStaticCast     <To>>(std::forward<From>(from)); }
template<typename To, typename From> auto reinterpret_cast_(From&& from) { return makeBindingExpr<impl::binding::ActionReinterpretCast<To>>(std::forward<From>(from)); }

template<typename A, typename B, typename C> auto cond(const A& a, const B& b, const C& c) { return makeBindingExpr<impl::binding::ActionCond>(a, b, c); }

template<typename F, typename ...Args> auto call(F func, Args&&... args) { return makeBindingExpr<impl::binding::ActionCall>(func, std::forward<Args>(args)...); }

template<typename T, typename ...Args> auto constructor(Args&&... args) { return makeBindingExpr<impl::binding::ActionConstructor<T>>(std::forward<Args>(args)...); }

// clang-format on

template <typename... Args> auto asprintf_(const char* cformat, const Args&... args)
{
    return call(QString::asprintf, cformat, args...);
}

} // namespace nwidget

#define N_IMPL_ACTION_BE(NAME, OP)                                                                                     \
    namespace nwidget::impl::binding {                                                                                 \
    struct Action##NAME                                                                                                \
    {                                                                                                                  \
        template <typename L, typename R> auto operator()(L&& l, R&& r) const { return l OP r; }                       \
    };                                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    /* MetaProperty OP T            */                                                                                 \
    /* T            OP MetaProperty */                                                                                 \
    /* MetaProperty OP MetaProperty */                                                                                 \
    template <typename... L, typename R> auto operator OP(nwidget::MetaProperty<L...> l, R&& r)                        \
    {                                                                                                                  \
        return nwidget::makeBindingExpr<nwidget::impl::binding::Action##NAME>(l, std::forward<R>(r));                  \
    }                                                                                                                  \
    template <typename L, typename... R> auto operator OP(L&& l, nwidget::MetaProperty<R...> r)                        \
    {                                                                                                                  \
        return nwidget::makeBindingExpr<nwidget::impl::binding::Action##NAME>(std::forward<L>(l), r);                  \
    }                                                                                                                  \
    template <typename... L, typename... R>                                                                            \
    auto operator OP(nwidget::MetaProperty<L...> l, nwidget::MetaProperty<R...> r)                                     \
    {                                                                                                                  \
        return nwidget::makeBindingExpr<nwidget::impl::binding::Action##NAME>(l, r);                                   \
    }                                                                                                                  \
                                                                                                                       \
    /* MetaProperty OP BindingExpr  */                                                                                 \
    /* BindingExpr  OP MetaProperty */                                                                                 \
    template <typename... L, typename... R>                                                                            \
    auto operator OP(nwidget::MetaProperty<L...> l, const nwidget::BindingExpr<R...>& r)                               \
    {                                                                                                                  \
        return nwidget::makeBindingExpr<nwidget::impl::binding::Action##NAME>(l, r);                                   \
    }                                                                                                                  \
    template <typename... L, typename... R>                                                                            \
    auto operator OP(const nwidget::BindingExpr<L...>& l, nwidget::MetaProperty<R...> r)                               \
    {                                                                                                                  \
        return nwidget::makeBindingExpr<nwidget::impl::binding::Action##NAME>(l, r);                                   \
    }                                                                                                                  \
                                                                                                                       \
    /* BindingExpr OP T           */                                                                                   \
    /* T           OP BindingExpr */                                                                                   \
    /* BindingExpr OP BindingExpr */                                                                                   \
    template <typename... L, typename R> auto operator OP(const nwidget::BindingExpr<L...>& l, R&& r)                  \
    {                                                                                                                  \
        return nwidget::makeBindingExpr<nwidget::impl::binding::Action##NAME>(std::move(l), std::forward<R>(r));       \
    }                                                                                                                  \
    template <typename L, typename... R> auto operator OP(L&& l, const nwidget::BindingExpr<R...>& r)                  \
    {                                                                                                                  \
        return nwidget::makeBindingExpr<nwidget::impl::binding::Action##NAME>(std::forward<L>(l), r);                  \
    }                                                                                                                  \
    template <typename... L, typename... R>                                                                            \
    auto operator OP(const nwidget::BindingExpr<L...>& l, const nwidget::BindingExpr<R...>& r)                         \
    {                                                                                                                  \
        return nwidget::makeBindingExpr<nwidget::impl::binding::Action##NAME>(l, r);                                   \
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
    namespace nwidget::impl::binding {                                                                                 \
    struct Action##NAME                                                                                                \
    {                                                                                                                  \
        template <typename T> auto operator()(T&& val) { return OP val; }                                              \
    };                                                                                                                 \
    }                                                                                                                  \
    template <typename... T> auto operator OP(nwidget::MetaProperty<T...> v)                                           \
    {                                                                                                                  \
        return nwidget::makeBindingExpr<nwidget::impl::binding::Action##NAME>(v);                                      \
    }                                                                                                                  \
    template <typename... T> auto operator OP(nwidget::BindingExpr<T...>&& expr)                                       \
    {                                                                                                                  \
        using namespace nwidget::impl::binding;                                                                        \
        return nwidget::makeBindingExpr<nwidget::impl::binding::Action##NAME>(std::move(expr));                        \
    }

N_IMPL_ACTION_UE(Plus, +)
N_IMPL_ACTION_UE(Minus, -)
N_IMPL_ACTION_UE(Not, !)

N_IMPL_ACTION_UE(BitNot, ~)
N_IMPL_ACTION_UE(AddressOf, &)
N_IMPL_ACTION_UE(ContentOf, *)

#endif // NWIDGET_BINDING_H
