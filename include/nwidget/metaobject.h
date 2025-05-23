/**
 * @brief Template Meta-Object System
 * @details
 * Example:
 *      @code{.cpp}
 *      using MetaObj = MetaObject<MyClass>;
 *      using Class = MetaObj::Class;
 *      using MetaProp = decltype(MetaObj().value());
 *
 *      ///@note The MetaObject class does not take ownership of the object
 *      auto metaobj = MetaObject<>::from(new MyClass);
 *
 *      auto prop  = metaobj.property();
 *      auto value = prop.get();
 *      prop.set(42);
 *
 *      // If yout class is not registered, as for: class MyClass2 : public QObject;
 *      auto metaobj2 = MetaObject<QObject>(new MyClass2);
 *      @endcode
 *
 * To Register your class to the Template Meta-Object System, as for:
 *      @code{.cpp}
 *      class MyClass : public QObject
 *      {
 *          Q_OBJECT
 *      public:
 *          void value() const;
 *          void setValue(int v);
 *
 *      signals:
 *          void valueChanged(int);
 *      };
 *      @endcode
 *
 * Define a MetaObject specialization for your class:
 *      @code{.cpp}
 *      template <> class nwidget::MetaObject<MyClass> : public MetaObject<QObject>
 *      {
 *          N_OBJECT(MyClass, QObject)
 *
 *          // Property do not need to be defined with Q_PROPERTY
 *          N_PROPERTY(int, value, N_READ value N_WRITE setValue N_NOTIFY valueChanged)
 *      };
 *      @endcode
 */

#ifndef NWIDGET_METAOBJECT_H
#define NWIDGET_METAOBJECT_H

#include "utils.h"

namespace nwidget {

template <typename Action, typename... Args> class BindingExpr;

/* -------------------------------------------------- MetaProperty -------------------------------------------------- */

template <typename...> class MetaProperty;

template <typename C, // Class
          typename I, // PropInfo: struct {
                      //    static constexpr const char* name() { return "propName"; }
                      //    static QString bindingName() { return "nwidget_binding_on_propName"; }
                      // }
          typename T, // Type
          typename G, // Getter: struct { auto operator()(const C* o)  const { return o->Getter(); } }
          typename S, // Setter: struct { void operator()(C* o, const T& v) const { o->Setter(v); } }
          typename N, // Notify: struct { constexpr auto operator()() const { return &C::signal; } }s
          typename R> // Reset : struct { void operator()(C* o) const { o->Reset(); } }
class MetaProperty<C, I, T, G, S, N, R>
{
public:
    using Class  = C;
    using Info   = I;
    using Type   = T;
    using Getter = G;
    using Setter = S;
    using Notify = N;
    using Reset  = R;

    static constexpr bool isReadable      = !std::is_same_v<G, void>;
    static constexpr bool isWritable      = !std::is_same_v<S, void>;
    static constexpr bool hasNotifySignal = !std::is_same_v<N, void>;
    static constexpr bool isResettable    = !std::is_same_v<R, void>;

    static T    read(const C* obj) { return G{}(obj); }
    static void write(C* obj, const T& val) { S{}(obj, val); }
    static void reset(C* obj) { R{}(obj); }

    static constexpr auto notify() { return N{}(); }

public:
    explicit MetaProperty(C* obj)
        : o(obj)
    {
        Q_ASSERT(o);
    }

    MetaProperty(const MetaProperty&) = default;
    MetaProperty(MetaProperty&&)      = default;

    Class* object() const { return o; }

    T get() const { return read(o); }

    void set(const T& val) const { write(o, val); }

    void reset() const { reset(o); }

    // clang-format off

    auto operator++()    {       T v = get(); set(++v    ); return v; }
    auto operator++(int) { const T v = get(); set(++get()); return v; }
    auto operator--()    {       T v = get(); set(--v    ); return v; }
    auto operator--(int) { const T v = get(); set(--get()); return v; }

    void operator+= (const T& v) { set(get() +  v); }
    void operator-= (const T& v) { set(get() -  v); }
    void operator*= (const T& v) { set(get() *  v); }
    void operator/= (const T& v) { set(get() /  v); }
    void operator%= (const T& v) { set(get() %  v); }
    void operator^= (const T& v) { set(get() ^  v); }
    void operator&= (const T& v) { set(get() &  v); }
    void operator|= (const T& v) { set(get() |  v); }
    void operator<<=(const T& v) { set(get() << v); }
    void operator>>=(const T& v) { set(get() >> v); }

    // clang-format on

    template <typename... Ts> void bindTo(MetaProperty<Ts...> prop) const { makeBindingExpr(*this).bindTo(prop); }

    // The return behavior of operator= is undetermined, currently we let it return void
    void                           operator=(const Type& val) { set(val); }
    void                           operator=(const MetaProperty& prop) { prop.bindTo(*this); }
    void                           operator=(MetaProperty&& prop) { prop.bindTo(*this); }
    template <typename... Ts> void operator=(MetaProperty<Ts...> prop) const { prop.bindTo(*this); }
    template <typename... Ts> void operator=(const BindingExpr<Ts...>& expr) const { expr.bindTo(*this); }

    template <typename... Args> auto operator()(Args&&... args) const
    {
        return makeBindingExpr(*this)(std::forward<Args>(args)...);
    }

    template <typename F, typename... Args> auto i(F f, Args&&... args) const
    {
        return makeBindingExpr(*this).i(f, std::forward<Args>(args)...);
    }

private:
    C* o;
};

// clang-format off

namespace impl {
using Getter = void;
using Setter = void;
using Notify = void;
using Reset  = void;
}

#define N_IMPL_READ(FUNC)   struct Getter { auto operator()(const _C* o)  const { return o->FUNC(); } };
#define N_IMPL_WRITE(FUNC)  struct Setter { void operator()(_C* o, const _T& v) const { o->FUNC(v); } };
#define N_IMPL_NOTIFY(FUNC) struct Notify { constexpr auto operator()()  const { return &_C::FUNC; } };
#define N_IMPL_RESET(FUNC)  struct Reset  { void operator()(_C* o) const { o->FUNC(); } };

#define N_IMPL_LEFT_PAREN (

#define N_READ   ); N_IMPL_READ   N_IMPL_LEFT_PAREN
#define N_WRITE  ); N_IMPL_WRITE  N_IMPL_LEFT_PAREN
#define N_NOTIFY ); N_IMPL_NOTIFY N_IMPL_LEFT_PAREN
#define N_RESET  ); N_IMPL_RESET  N_IMPL_LEFT_PAREN

// clang-format on

#define N_BEGIN_PROPERTY
#define N_END_PROPERTY

#define N_METAPROPERTY(TYPENAME, CLASS, TYPE, NAME, ...)                                                               \
    static const auto _create##TYPENAME = [](CLASS* o)                                                                 \
    {                                                                                                                  \
        using namespace ::nwidget::impl;                                                                               \
                                                                                                                       \
        struct _I                                                                                                      \
        {                                                                                                              \
            static constexpr const char* name() { return #NAME; }                                                      \
            static QString               bindingName() { return QStringLiteral("nwidget_binding_on_" #NAME); }         \
        };                                                                                                             \
                                                                                                                       \
        using _C = CLASS;                                                                                              \
        using _T = TYPE;                                                                                               \
                                                                                                                       \
        void(__VA_ARGS__);                                                                                             \
                                                                                                                       \
        return ::nwidget::MetaProperty<_C, _I, _T, Getter, Setter, Notify, Reset>{static_cast<_C*>(o)};                \
    };                                                                                                                 \
    using TYPENAME = std::decay_t<decltype(_create##TYPENAME(nullptr))>

#define N_PROPERTY(TYPE, NAME, ...)                                                                                    \
public:                                                                                                                \
    auto NAME() const                                                                                                  \
    {                                                                                                                  \
        using namespace ::nwidget::impl;                                                                               \
                                                                                                                       \
        struct _I                                                                                                      \
        {                                                                                                              \
            static constexpr const char* name() { return #NAME; }                                                      \
            static QString               bindingName() { return QStringLiteral("nwidget_binding_on_" #NAME); }         \
        };                                                                                                             \
                                                                                                                       \
        using _C = Class;                                                                                              \
        using _T = TYPE;                                                                                               \
                                                                                                                       \
        void(__VA_ARGS__);                                                                                             \
                                                                                                                       \
        return ::nwidget::MetaProperty<_C, _I, _T, Getter, Setter, Notify, Reset>{static_cast<_C*>(o)};                \
    }

/* --------------------------------------------------- MetaObject --------------------------------------------------- */

template <typename...> class MetaObject;

template <> class MetaObject<void>
{
public:
    using Class = void;
};

#define N_IMPL_OBJECT(CLASS)                                                                                           \
public:                                                                                                                \
    using Class = CLASS;                                                                                               \
                                                                                                                       \
    constexpr static char className[] = #CLASS;                                                                        \
                                                                                                                       \
    Class* object() const { return static_cast<Class*>(o); }                                                           \
    operator Class*() const { return object(); }

#define N_IMPL_OBJECT_0(CLASS, ...)                                                                                    \
    N_IMPL_OBJECT(CLASS)                                                                                               \
public:                                                                                                                \
    using Super = MetaObject<void>;                                                                                    \
    MetaObject(Class* obj)                                                                                             \
        : o(obj)                                                                                                       \
    {                                                                                                                  \
        Q_ASSERT(o);                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
protected:                                                                                                             \
    Class* o;

#define N_IMPL_OBJECT_1(CLASS, SUPER)                                                                                  \
    N_IMPL_OBJECT(CLASS)                                                                                               \
public:                                                                                                                \
    using Super = MetaObject<SUPER>;                                                                                   \
    MetaObject(Class* obj)                                                                                             \
        : Super(obj)                                                                                                   \
    {                                                                                                                  \
        Q_ASSERT(this->o);                                                                                             \
    }

#define N_OBJECT(CLASS, ...) N_IMPL_CAT(N_IMPL_OBJECT_, N_IMPL_COUNT(__VA_ARGS__))(CLASS, __VA_ARGS__)

#define N_DECLARE_METAOBJECT(TYPE) TYPE* nwidget_metaobject(TYPE* obj);

namespace impl {

// clang-format off
template <typename T, typename = void> struct has_metaobject_spec                                                  : std::false_type {};
template <typename T>                  struct has_metaobject_spec<T, std::void_t<decltype(sizeof(MetaObject<T>))>> : std::true_type  {};
template <typename T> constexpr bool has_metaobject_spec_v = has_metaobject_spec<T>::value;
// clang-format on

template <typename Class>
using closest_declared_metaobject_class_t =
    std::conditional_t<impl::has_metaobject_spec_v<Class>,
                       Class,
                       std::decay_t<decltype(*nwidget_metaobject(std::declval<Class*>()))>>;

}; // namespace impl

template <typename C> class MetaObject<C> : public MetaObject<impl::closest_declared_metaobject_class_t<C>>
{
public:
    MetaObject(C* obj)
        : MetaObject<impl::closest_declared_metaobject_class_t<C>>(obj)
    {
    }
};

/* ------------------------------------------------------ Utils ----------------------------------------------------- */

template <> class MetaObject<>
{
public:
    template <typename Class> static auto from(Class* obj) { return MetaObject<Class>(obj); }
};

} // namespace nwidget

#endif // NWIDGET_METAOBJECT_H
