# How to use

English | [简体中文](How-to-use_zh-cn.md)

If you want to make the following type available in nwidget:

```cpp
class MyButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit MyButton(QWidget* parent = nullptr);

    int prop() const;
    void setProp(int value);

signals:
    void propChanged(int value);

    void someSignal();
}
```

## MetaObject

nwidget uses a template-based meta-object system (referred to as the template meta-object system) with an API similar to Qt's meta-object system. This system is based on two template classes: `MetaObject` and `MetaProperty`.

To register MyButton with the template meta-object system, we need to declare a MetaObject<> specialization for it and inherit from the parent class's MetaObject:

```cpp
template <> class nwidget::MetaObject<MyButton> : public nwidget::MetaObject<QPushButton>
{
    N_OBJECT(MyButton, QPushButton)

    N_PROPERTY(int, prop, N_READ prop N_WRITE setProp N_NOTIFY propChanged)
}
```

`N_OBJECT` macro accepts one (current class) or two (current class, parent class) parameters. The parent class must have its specialization declared beforehand, as it is used to create the type's basic information.

`N_PROPERTY` is similar to Q_PROPERTY and creates a member method with the same name as the property. This method returns a MetaProperty instance.

`MetaProperty` is a template type representing a property of a type:

```cpp
template <typename ...>
class MetaProperty
{
public:
    static constexpr bool isReadable      = //...
    static constexpr bool isWritable      = //...
    static constexpr bool hasNotifySignal = //...
    static constexpr bool isResettable    = //...

    T get() const { return read(o); }
    void set(const T& val) const { write(o, val); }
    void reset() const { reset(o); }

    // ...
}
```

Now you can get the MetaObject of MyButton:

```cpp
MyButton btn;

auto obj = MetaObject<MyButton>(&btm);
     // 或 MetaObject<>::from(&btn)

auto prop = obj.prop();

int val = prop.get();
prop.set(42);
```

This usage might seem cumbersome, and indeed it is, because the primary purpose of MetaObject and MetaProperty is to provide information for features like property `binding`, `declarative syntax`, and `Behavior animations`.

## Property Binding

This is a feature similar to QML's property binding. You can create a binding by assigning an expression involving `MetaProperty` to another `MetaProperty`:

```cpp
QSlider s1;
QSlider s2;
QSlider s3;

auto obj1 = MetaObject<QSlider>(&s1);
auto obj2 = MetaObject<QSlider>(&s2);
auto obj3 = MetaObject<QSlider>(&s3);

obj3.value() = obj1.value() + obj2.value();
```

Expressions can not only bind to properties but also to member methods or lambda:

```cpp
auto expr = obj1.value() + obj2.value();
expr.bindTo(s3, &QSlider::setValue);
expr.bindTo([](int value) { /* ... */ });
```

Some special expressions cannot be overloaded, so nwidget provides many alternatives:

| Original Method    | Alternative Method         |
| ------------------ | -------------------------- |
| 强制类型转换       | nwidget::cast              |
| `static_cast`      | nwidget::static_cast_      |
| `reinterpret_cast` | nwidget::reinterpret_cast_ |
| QString::asprintf  | nwidget::asprintf_         |
| func(...)          | nwidget::call(func, ...)   |
| A ? B : C          | nwidget::cond(A, B, C)     |
| Type(...)          | nwidget::constructor(...)  |
| obj.member         | expr.m(&Obj::member)       |
| obj.func(...)      | expr.m(&Obj::func, ...)    |

## Builder

`Builder` provides a mechanism for constructing QWidget interfaces using declarative syntax.

To allow setting properties via declarative syntax, you need to create `MetaObject` and `Builder` specializations for the type:

```cpp
template <typename Self> class nwidget::Builder<MyButton, Self>
    : public nwidget::Builder<QPushButton, Self>
{
    N_BUILDER(QAction)

    N_BUILDER_PROPERTY(prop)

    N_BUILDER_SIGNAL(onSomeSignal, someSignal)
};
```

Now you can set the prop property of MyButton in code. For more usage examples, refer to [Gallery](../examples/Gallery)

```cpp
QWidget w;

w.setLayout(VBoxLayout {
    Builder<MyButton>().prop(1),
    Builder<MyButton>().prop(2),
    Builder<MyButton>().prop(3),
});

w.show();
```

Some member methods behave similarly to properties. You can declare them using the `N_BUILDER_SETTER` series of macros:

```cpp
class MyObject
{
    void foo0();
    void foo1(int);
    void foo2(int, int);
    void foo3(int, int, int);
    void foo4(int, int, int, int);
    void foox(int, int, int, int);
    void foox(QMargins);
}

template <typename Self> class nwidget::Builder<MyObject, Self>
{
    N_BUILDER(MyObject)

    N_BUILDER_SETTER0(foo0)
    N_BUILDER_SETTER1(foo1)
    N_BUILDER_SETTER2(foo2)
    N_BUILDER_SETTER3(foo3)
    N_BUILDER_SETTER4(foo4)
    N_BUILDER_SETTERX(foox, int, int, int, int)
    N_BUILDER_SETTERX(foox, QMargins)
};
```

> `N_BUILDER_SETTER0` - `N_BUILDER_SETTER4` are not suitable for overloaded methods.

## N_DECLARE_METAOBJECT 与 N_DECLARE_BUILDER

Using `MetaObject` and `Builder` does not require registering the type with the template meta-object system. In this case, nwidget will use the nearest declared parent class.

```cpp
using type = MetaObject<MyButton>::Class; // QPushButton
const char* name = MetaObject<MyButton>::className; // "QPushButton"

QPushButton* button = MetaObject<MyButton>().text("Hello");
```

To ensure your specialization can be found during this process, use `N_DECLARE_METAOBJECT` and `N_DECLARE_BUILDER`:

```cpp
template <> class nwidget::MetaObject<MyButton> : public nwidget::MetaObject<QPushButton> { /* ... */ };

N_DECLARE_METAOBJECT(MyButton)

template <typename Self> class nwidget::Builder<MyButton, Self> : public nwidget::Builder<QPushButton, Self> { /* ... */ };

N_DECLARE_BUILDER(MyButton)
```

## Behavior Animation

This is an animation API similar to QML's. Use `Behavior::on` to animate properties, `Behavior::get` and `Behavior::set` to get and set property values, or `Behavior::animated` to support property binding.

```cpp
auto rect     = MetaObject<>::from(new QWidget);
auto checkBox = MetaObject<>::from(new QCheckBox);

Behavior::on(rect.minimumWidth(),
             new SpringAnimation<int>(
                 spring{2},
                 damping{0.2}));

cond(checkBox.checked(), 300, 50).bindTo(Behavior::animated(rect.minimumWidth()));

Behavior::set(rect.minimumWidth(), 300);
int w = Behavior::get(rect.minimumWidth());
```

Behavior animations can also be used on non-QObject subclasses, but you must first create a Behavior instance from a QObject using `Behavior::on(QObject*)`:

```cpp
QWidget* widget = new QWidget;
auto     obj    = MetaObject<>::from(/* ... */);

auto behavior = Behavior::on(widget);

Behavior::on(behavior,
             obj.prop(),
             new SpringAnimation<int>(
                 spring{2},
                 damping{0.2})

cond(checkBox.checked(), 300, 50).bindTo(Behavior::animated(obj.prop()));

Behavior::set(behavior, obj.prop(), 300);
int v = Behavior::get(behavior, obj.prop());
```

## Other

nwidget was developed in `Qt6` and has not been tested in `Qt5`. But I hope it does not depend on a specific Qt version. If you need to use it in Qt5, feel free to discuss it.
