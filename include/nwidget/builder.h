/**
 * @brief Declarative UI framework for QWidget
 * @details
 * Example:
 *      @code{.cpp}
 *      QLayout* layout = FormLayout{
 *          {"Label 0", LineEdit().text("Hello")},
 *          {"Label 1", Slider(Qt::Horizontal).range(0, 100).value(25)},
 *          {GridLayout{
 *              {0, 0,       PushButton("Button 0")},
 *              {1, 0,       PushButton("Button 1")},
 *              {0, 1, 2, 1, PushButton("Button 2").sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding),
 *              },
 *          }}};
 *      @endcode
 *
 * Use ForEach(data, generator) to create multiple items:
 *      @code{.cpp}
 *      QLayout* layout = VBoxLayout{
 *          ForEach(1, 4, [](int i) -> QPushButton* { return new QPushButton(QString::number(i)); }),
 *      };
 *      @endcode
 *
 *      The data can be a
 *        - Container
 *        - Range
 *        - Number
 *        - Number Range
 *
 *      The generator can be:
 *        - []()                      -> BuilderItem { ... }
 *        - [](const T& e)            -> BuilderItem { ... }
 *        - [](int index, const T& e) -> BuilderItem { ... }
 *
 * To register builder for a class, as for:
 *      @code{.cpp}
 *      class MyClass : public QObject
 *      {
 *          Q_OBJECT
 *      public:
 *          void value() const;
 *          void setValue(int v);
 *
 *          void setFoo(int);
 *
 *          void setBar(int, int);
 *
 *          void setBaz(double);
 *          void setBaz(int);
 *          void setBaz(int, int);
 *
 *      signals:
 *          void valueChanged(int);
 *      };
 *      @endcode
 *
 * Define a builder specialization for your class:
 *      @code{.cpp}
 *      template <typename Self>
 *      class Builder<MyClass, Self> : public Builder<QObject, Self>
 *                                         // ^ Use Builder<void, Self> if the class has no super class
 *      {
 *          ^ Make sure your class has registered to the Template Meta-Object System
 *          N_BUILDER(MyClass)
 *
 *          N_BUILDER_PROPERTY(value)
 *
 *          // For the func without overload, use N_BUILDER_SETTER0/1/2/3/4
 *          N_BUILDER_SETTER1(foo, setFoo)
 *          N_BUILDER_SETTER2(bar, setBar)
 *
 *          // For the func with overload, use N_BUILDER_SETTERX
 *          N_BUILDER_SETTERX(baz, setBaz, double)
 *          N_BUILDER_SETTERX(baz, setBaz, int)
 *          N_BUILDER_SETTERX(baz, setBaz, int, int)
 *      }
 *
 *      using MyClassBuilder = Builder<MyClass>;
 *      @endcode
 */

#ifndef NWIDGET_BUILDER_H
#define NWIDGET_BUILDER_H

#include <functional>

#include "metaobjects.h"

namespace nwidget {

/* ----------------------------------------------------- Builder ---------------------------------------------------- */

template <typename...> class Builder;

#define N_BUILDER(CLASS)                                                                                               \
    static_assert(impl::has_metaobject_spec_v<CLASS>, "Class must be registered to the Template Meta-Object System");  \
                                                                                                                       \
private:                                                                                                               \
    Self&       self() { return *static_cast<Self*>(this); }                                                           \
    const Self& self() const { return *static_cast<const Self*>(this); }                                               \
                                                                                                                       \
public:                                                                                                                \
    using Class = CLASS;                                                                                               \
    Builder() {}                                                                                                       \
    explicit Builder(Class* o) : Builder<typename MetaObject<Class>::Super::Class, Self>(o) {}                         \
                                                                                                                       \
    /* Add trailing underscore to avoid duplication with possible prop name. */                                        \
    Class* object_() const { return self().o; }

#define N_BEGIN_BUILDER_PROPERTY
#define N_END_BUILDER_PROPERTY

#define N_BUILDER_PROPERTY(NAME)                                                                                       \
public:                                                                                                                \
    Self& NAME(const typename decltype(std::declval<::nwidget::MetaObject<Class>>().NAME())::Type& value)              \
    {                                                                                                                  \
        using MetaProp = decltype(std::declval<::nwidget::MetaObject<Class>>().NAME());                                \
        static_assert(MetaProp::isWritable, "");                                                                       \
        MetaProp::write(self().o, value);                                                                              \
        return self();                                                                                                 \
    }                                                                                                                  \
    template <typename... T> Self& NAME(::nwidget::MetaProperty<T...> prop)                                            \
    {                                                                                                                  \
        prop.bindTo(::nwidget::MetaObject<Class>(self().o).NAME());                                                    \
        return self();                                                                                                 \
    }                                                                                                                  \
    template <typename... T> Self& NAME(const ::nwidget::BindingExpr<T...>& expr)                                      \
    {                                                                                                                  \
        expr.bindTo(MetaObject<Class>(self().o).NAME());                                                               \
        return self();                                                                                                 \
    }

#define N_BEGIN_BUILDER_SETTER
#define N_END_BUILDER_SETTER

#define N_IMPL_ARGS_IMPL0_0()
#define N_IMPL_ARGS_IMPL0_1(TYPE)      TYPE _0
#define N_IMPL_ARGS_IMPL0_2(TYPE, ...) TYPE _1, N_IMPL_EXPAND(N_IMPL_ARGS_IMPL0_1(__VA_ARGS__))
#define N_IMPL_ARGS_IMPL0_3(TYPE, ...) TYPE _2, N_IMPL_EXPAND(N_IMPL_ARGS_IMPL0_2(__VA_ARGS__))
#define N_IMPL_ARGS_IMPL0_4(TYPE, ...) TYPE _3, N_IMPL_EXPAND(N_IMPL_ARGS_IMPL0_3(__VA_ARGS__))

#define N_IMPL_ARGS_IMPL1_0()
#define N_IMPL_ARGS_IMPL1_1(TYPE)      _0
#define N_IMPL_ARGS_IMPL1_2(TYPE, ...) _1, N_IMPL_EXPAND(N_IMPL_ARGS_IMPL1_1(__VA_ARGS__))
#define N_IMPL_ARGS_IMPL1_3(TYPE, ...) _2, N_IMPL_EXPAND(N_IMPL_ARGS_IMPL1_2(__VA_ARGS__))
#define N_IMPL_ARGS_IMPL1_4(TYPE, ...) _3, N_IMPL_EXPAND(N_IMPL_ARGS_IMPL1_3(__VA_ARGS__))

#define N_BUILDER_SETTERX(NAME, SETTER, ...)                                                                           \
    Self& NAME(N_IMPL_OVERLOAD(N_IMPL_ARGS_IMPL0_, __VA_ARGS__))                                                       \
    {                                                                                                                  \
        object_()->SETTER(N_IMPL_OVERLOAD(N_IMPL_ARGS_IMPL1_, __VA_ARGS__));                                           \
        return self();                                                                                                 \
    }

// clang-format off
#define N_BUILDER_SETTER0(NAME, SETTER) N_BUILDER_SETTERX(NAME, SETTER)
#define N_BUILDER_SETTER1(NAME, SETTER) N_BUILDER_SETTERX(NAME, SETTER, typename ::nwidget::impl::mem_fn<decltype(&Class::SETTER)>::template arg<0>)
#define N_BUILDER_SETTER2(NAME, SETTER) N_BUILDER_SETTERX(NAME, SETTER, typename ::nwidget::impl::mem_fn<decltype(&Class::SETTER)>::template arg<0>,\
                                                                        typename ::nwidget::impl::mem_fn<decltype(&Class::SETTER)>::template arg<1>)
#define N_BUILDER_SETTER3(NAME, SETTER) N_BUILDER_SETTERX(NAME, SETTER, typename ::nwidget::impl::mem_fn<decltype(&Class::SETTER)>::template arg<0>,\
                                                                        typename ::nwidget::impl::mem_fn<decltype(&Class::SETTER)>::template arg<1>,\
                                                                        typename ::nwidget::impl::mem_fn<decltype(&Class::SETTER)>::template arg<2>)
#define N_BUILDER_SETTER4(NAME, SETTER) N_BUILDER_SETTERX(NAME, SETTER, typename ::nwidget::impl::mem_fn<decltype(&Class::SETTER)>::template arg<0>,\
                                                                        typename ::nwidget::impl::mem_fn<decltype(&Class::SETTER)>::template arg<1>,\
                                                                        typename ::nwidget::impl::mem_fn<decltype(&Class::SETTER)>::template arg<2>,\
                                                                        typename ::nwidget::impl::mem_fn<decltype(&Class::SETTER)>::template arg<3>)
// clang-format on

#define N_BEGIN_BUILDER_SIGNAL
#define N_END_BUILDER_SIGNAL

#define N_BUILDER_SIGNAL(NAME, SIGNAL_)                                                                                \
    template <typename Func> Self& NAME(Func func, Qt::ConnectionType type = Qt::AutoConnection)                       \
    {                                                                                                                  \
        QObject::connect(object_(), &Class::SIGNAL_, object_(), func, type);                                           \
        return self();                                                                                                 \
    }                                                                                                                  \
    template <typename Rece, typename Func>                                                                            \
    Self& NAME(Rece* receiver, Func func, Qt::ConnectionType type = Qt::AutoConnection)                                \
    {                                                                                                                  \
        QObject::connect(object_(), &Class::SIGNAL_, receiver, func, type);                                            \
        return self();                                                                                                 \
    }

#define N_DECLARE_BUILDER(TYPE) TYPE* nwidget_builder(TYPE* obj);

namespace impl {

// clang-format off
template <typename T, typename = void> struct has_builder_spec                                                     : std::false_type {};
template <typename T>                  struct has_builder_spec<T, impl::void_t<decltype(sizeof(Builder<T, int>))>> : std::true_type  {};
template <typename T> constexpr bool has_builder_spec_v = has_builder_spec<T>::value;
// clang-format on

template <typename Class>
using closest_declared_builder_class_t =
    std::conditional_t<impl::has_builder_spec_v<Class>,
                       Class,
                       std::decay_t<decltype(*nwidget_builder(std::declval<Class*>()))>>;

}; // namespace impl

template <typename Self> class Builder<void, Self>
{
public:
    Builder() { static_cast<Self*>(this)->o = new typename Self::Class; }

    explicit Builder(void* o) { static_cast<Self*>(this)->o = static_cast<typename Self::Class*>(o); }
};

template <typename C> class Builder<C> : public Builder<impl::closest_declared_builder_class_t<C>, Builder<C>>
{
    template <typename...> friend class Builder;

public:
    using Class = C;

    using Builder<impl::closest_declared_builder_class_t<C>, Builder<C>>::Builder;

    operator Class*() const { return this->o; }

private:
    Class* o;
};

/* --------------------------------------------------- BuilderItem -------------------------------------------------- */

#define N_ITEM_BUILDER(ITEM)                                                                                           \
    explicit Builder(std::initializer_list<ITEM> items) { self().items_(items); }                                      \
                                                                                                                       \
    /* Add trailing underscore to avoid duplication with possible prop name. */                                        \
    Self& items_(std::initializer_list<ITEM> items)                                                                    \
    {                                                                                                                  \
        for (const auto& item : items)                                                                                 \
            item.func(&item, object_());                                                                               \
        return self();                                                                                                 \
    }

template <typename It, typename Fn> struct ItemGenerator
{
    It begin;
    It end;
    Fn func;
};

namespace impl {
// clang-format off
template <typename T, typename Fn> auto invokeGeneratorFunc(int  , const T&  , Fn f) -> decltype(std::declval<Fn>()(                                      )) { return f(    ); }
template <typename T, typename Fn> auto invokeGeneratorFunc(int  , const T& e, Fn f) -> decltype(std::declval<Fn>()(                     std::declval<T>())) { return f(   e); }
template <typename T, typename Fn> auto invokeGeneratorFunc(int i, const T& e, Fn f) -> decltype(std::declval<Fn>()(std::declval<int>(), std::declval<T>())) { return f(i, e); }
// clang-format on
}; // namespace impl

template <typename T> class BuilderItem
{
    template <typename...> friend class Builder;

public:
    using Func = std::function<void(const BuilderItem*, T*)>;

    virtual ~BuilderItem() = default;

protected:
    explicit BuilderItem(Func f) { func = f; }

    template <typename Derived, typename It, typename Fn> BuilderItem(Derived*, const ItemGenerator<It, Fn>& gen)
    {
        func = [gen](const BuilderItem*, T* target)
        {
            int  i  = 0;
            auto it = gen.begin;
            while (it != gen.end) {
                Derived d = std::move(impl::invokeGeneratorFunc(i, *it, gen.func));
                d.func(&d, target);
                ++i;
                ++it;
            }
        };
    };

private:
    Func func;
};

/* ----------------------------------------------------- ForEach ---------------------------------------------------- */

template <typename It, typename Fn> auto ForEach(It begin, It end, Fn fn, std::true_type /* is_integral */)
{
    struct iterator
    {
        It value;

        It operator*() const { return value; }

        bool operator==(iterator other) const { return value == other.value; }
        bool operator!=(iterator other) const { return value != other.value; }

        iterator operator++()
        {
            ++value;
            return *this;
        }
    };
    return ItemGenerator<iterator, Fn>{iterator{begin}, iterator{end}, fn};
}

template <typename It, typename Fn> auto ForEach(It begin, It end, Fn func, std::false_type /* is_integral */)
{
    return ItemGenerator<It, Fn>{begin, end, func};
}

// clang-format off
template <typename T, typename Fn> auto ForEach(      T  n, Fn func, std::true_type  is_integral) { return ForEach(0         , n       , func, is_integral); }
template <typename T, typename Fn> auto ForEach(const T& c, Fn func, std::false_type is_integral) { return ForEach(c.cbegin(), c.cend(), func, is_integral); }

template <typename It, typename Fn> auto ForEach(It begin, It end,          Fn func) { return ForEach(begin, end,       func, typename std::is_integral<It>::type{}); }
template <typename T , typename Fn> auto ForEach(const T& num_or_container, Fn func) { return ForEach(num_or_container, func, typename std::is_integral<T >::type{}); }

template <typename T, typename F> auto ForEach(std::initializer_list<T> l, F f) { return ForEach(l.begin(), l.end(), f); }
// clang-format on
} // namespace nwidget

#endif // NWIDGET_BUILDER_H
