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
#include <optional>

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
    explicit Builder(Class* o)                                                                                         \
        : Builder<typename MetaObject<Class>::Super::Class, Self>(o)                                                   \
    {                                                                                                                  \
    }                                                                                                                  \
    Class* object() const { return self().o; }

#define N_BEGIN_BUILDER_PROPERTY
#define N_END_BUILDER_PROPERTY

#define N_BUILDER_PROPERTY(NAME)                                                                                       \
public:                                                                                                                \
    Self& NAME(const typename decltype(std::declval<::nwidget::MetaObject<Class>>().NAME())::Type& value)              \
    {                                                                                                                  \
        using MetaProp = decltype(std::declval<::nwidget::MetaObject<Class>>().NAME());                                \
        static_assert(MetaProp::isWritable);                                                                           \
        MetaProp::write(self().o, value);                                                                              \
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
        object()->SETTER(N_IMPL_OVERLOAD(N_IMPL_ARGS_IMPL1_, __VA_ARGS__));                                            \
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
        QObject::connect(object(), &Class::SIGNAL_, func, type);                                                       \
        return self();                                                                                                 \
    }                                                                                                                  \
    template <typename Rece, typename Func>                                                                            \
    Self& NAME(Rece* receiver, Func func, Qt::ConnectionType type = Qt::AutoConnection)                                \
    {                                                                                                                  \
        QObject::connect(object(), &Class::SIGNAL_, receiver, func, type);                                             \
        return self();                                                                                                 \
    }

#define N_DECLARE_BUILDER(TYPE) TYPE* nwidget_builder(TYPE* obj);

namespace impl {

// clang-format off
template <typename T, typename = void> struct has_builder_spec                                                    : std::false_type {};
template <typename T>                  struct has_builder_spec<T, std::void_t<decltype(sizeof(Builder<T, int>))>> : std::true_type  {};
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

    template <typename Items> Builder& addItems(const Items& items)
    {
        for (const auto& item : items)
            item.func(&item, o);
        return *this;
    }
};

/* --------------------------------------------------- BuilderItem -------------------------------------------------- */

template <typename Item> using BuilderItemGenerator = std::function<std::optional<Item>()>;

template <typename T> class BuilderItem
{
    template <typename...> friend class Builder;

public:
    using Func = std::function<void(const BuilderItem*, T*)>;

    template <typename Item> BuilderItem(BuilderItemGenerator<Item> gen)
    {
        func = [gen](const BuilderItem*, T* target)
        {
            while (auto item = gen())
                item->func(&*item, target);
        };
    }

    virtual ~BuilderItem() = default;

protected:
    explicit BuilderItem(Func f) { func = f; }

private:
    Func func;
};

/* ----------------------------------------------------- ForEach ---------------------------------------------------- */

template <typename Iterator, typename Generator>
auto ForEach(Iterator begin, Iterator end, Generator gen)
    -> std::enable_if_t<std::is_invocable_v<Generator>, BuilderItemGenerator<std::invoke_result_t<Generator>>>
{
    return [begin, end, gen]() mutable -> std::optional<decltype(gen())>
    {
        if (begin == end)
            return std::nullopt;
        auto item = gen();
        ++begin;
        return item;
    };
}

template <typename Iterator, typename Generator>
auto ForEach(Iterator begin, Iterator end, Generator gen)
    -> std::enable_if_t<std::is_invocable_v<Generator, decltype(*begin)>,
                        BuilderItemGenerator<std::invoke_result_t<Generator, decltype(*begin)>>>
{
    return [begin, end, gen]() mutable -> std::optional<decltype(gen(*begin))>
    {
        if (begin == end)
            return std::nullopt;
        auto item = gen(*begin);
        ++begin;
        return item;
    };
}

template <typename Iterator, typename Generator>
auto ForEach(Iterator begin, Iterator end, Generator gen)
    -> std::enable_if_t<std::is_invocable_v<Generator, int, decltype(*begin)>,
                        BuilderItemGenerator<std::invoke_result_t<Generator, int, decltype(*begin)>>>
{
    return [index = (int)0, begin, end, gen]() mutable -> std::optional<decltype(gen(0, *begin))>
    {
        if (begin == end)
            return std::nullopt;
        auto item = gen(index, *begin);
        ++index;
        ++begin;
        return item;
    };
}

template <typename T, typename Generator>
auto ForEach(T begin, T end, Generator gen)
    -> std::enable_if_t<std::is_integral_v<T>, BuilderItemGenerator<std::invoke_result_t<Generator, T>>>
{
    struct iterator
    {
        T value;

        T operator*() const { return value; }

        bool operator==(const iterator& other) const { return value == other.value; }
        bool operator!=(const iterator& other) const { return value != other.value; }

        iterator& operator++()
        {
            ++value;
            return *this;
        }
    };

    return ForEach(iterator{begin}, iterator{end}, gen);
}

template <typename T, typename Generator> auto ForEach(const T& c, Generator g)
{
    if constexpr (std::is_integral_v<T>)
        return ForEach(0, c, g);
    else
        return ForEach(c.begin(), c.end(), g);
}

template <typename T, typename Generator> auto ForEach(std::initializer_list<T> l, Generator g)
{
    return ForEach(l.begin(), l.end(), g);
}

} // namespace nwidget

#endif // NWIDGET_BUILDER_H
