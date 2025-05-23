# 如何使用

[English](How-to-use.md) | 简体中文

假设要让以下类型在 nwidget 中可用：

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

nwidget 使用一个基于模板的元对象系统（下称模板元对象系统），具有与 Qt 元对象系统相似的 API。该系统基于两个模板类：`MetaObject` 和 `MetaProperty`

为了将 MyButton 注册到模板元对象系统中，我们需要为它声明一个 MetaObject<> 特化，并继承父类的 MetaObject

```cpp
template <> class nwidget::MetaObject<MyButton> : public nwidget::MetaObject<QPushButton>
{
    N_OBJECT(MyButton, QPushButton)

    N_PROPERTY(int, prop, N_READ prop N_WRITE setProp N_NOTIFY propChanged)
}
```

`N_OBJECT` 宏接受一个（当前类）或两个（当前类，父类）参数，父类必须已声明特化，用于创建类型的基本信息

`N_PROPERTY` 与 `Q_PROPERTY` 类型，创建一个与属性同名的成员方法，该方法返回一个 `MetaProperty` 实例

`MetaProperty` 是一个模板类型，表示一个类型的属性

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

现在可以获取 MyButton 的 MetaObject 了:

```cpp
MyButton btn;

auto obj = MetaObject<MyButton>(&btm);
     // 或 MetaObject<>::from(&btn)

auto prop = obj.prop();

int val = prop.get();
prop.set(42);
```

这种使用方式似乎更麻烦，实际上也确实如此，因为 MetaObject 与 MetaProperty 的主要目的是为 `属性绑定`、`声明式语法` 和 `Behavior动画` 功能提供信息

## 属性绑定

这是一个与 QML 属性绑定相似的功能，只需要将一个涉及 `MetaProperty` 的表达式赋给另一个 `MetaProperty` 即可创建绑定

```cpp
QSlider s1;
QSlider s2;
QSlider s3;

auto obj1 = MetaObject<QSlider>(&s1);
auto obj2 = MetaObject<QSlider>(&s2);
auto obj3 = MetaObject<QSlider>(&s3);

obj3.value() = obj1.value() + obj2.value();
```

表达式不仅可以绑定到属性，也可以绑定到成员方法或 lambda：

```cpp
auto expr = obj1.value() + obj2.value();
expr.bindTo(s3, &QSlider::setValue);
expr.bindTo([](int value) { /* ... */ });
```

一些特殊表达式无法被重载，nwidget 提供了许多替代方法

| 原方法             | 替代方法                   |
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

`Builder` 提供了一套通过声明式语法构建 QWidget 界面的机制

若要允许通过通过声明式语法设置属性，你需要创建为类型创建 `MetaObject` 和 `Builder` 特化：

```cpp
template <typename Self> class nwidget::Builder<MyButton, Self>
    : public nwidget::Builder<QPushButton, Self>
{
    N_BUILDER(QAction)

    N_BUILDER_PROPERTY(prop)

    N_BUILDER_SIGNAL(onSomeSignal, someSignal)
};
```

现在可以在代码中设置 MyButton 的 prop 属性，更多用法请参考 [Gallery](../examples/Gallery)

```cpp
QWidget w;

w.setLayout(VBoxLayout {
    Builder<MyButton>().prop(1),
    Builder<MyButton>().prop(2),
    Builder<MyButton>().prop(3),
});

w.show();
```

有一些成员方法行为与属性类似，你可以通过 `N_BUILDER_SETTER` 系列宏来声明它们

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

> N_BUILDER_SETTER0 - N_BUILDER_SETTER4 方法不适用于具有重载的方法

## N_DECLARE_METAOBJECT 与 N_DECLARE_BUILDER

使用 `MetaObject` 和 `Builder` 不要求将类型注册到模板元对象系统中，nwidget 会使用已声明的最近父类

```cpp
using type = MetaObject<MyButton>::Class; // QPushButton
const char* name = MetaObject<MyButton>::className; // "QPushButton"

QPushButton* button = MetaObject<MyButton>().text("Hello");
```

要让你的特化可以在此过程中被查找到，请使用 `N_DECLARE_METAOBJECT` 和 `N_DECLARE_BUILDER`：

```cpp
template <> class nwidget::MetaObject<MyButton> : public nwidget::MetaObject<QPushButton> { /* ... */ };

N_DECLARE_METAOBJECT(MyButton)

template <typename Self> class nwidget::Builder<MyButton, Self> : public nwidget::Builder<QPushButton, Self> { /* ... */ };

N_DECLARE_BUILDER(MyButton)
```

## Behavior 动画

这是一套与 QML 相似的动画 API，使用 `Behavior::on` 来为属性添加通过，使用 `Behavior::get`, `Behavior::set` 来设置和获取属性值，或是通过 `Behavior::animated` 支持属性绑定

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

Behavior 动画支持在非 QObject 子类上使用，但需要首先通过 `Behavior::on(QObject*)` 从 QObject 创建一个 Behavior 实例

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

## 其它

nwidget 是在 `Qt6` 中开发的，未在 `Qt5` 中进行过测试，但我希望它不依赖于特定的 Qt 版本，如果你有在 Qt5 中使用的需求，欢迎交流
