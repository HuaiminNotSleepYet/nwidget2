#include <QLabel>
#include <QTest>

#include <QSlider>
#include <nwidget/binding.h>
#include <nwidget/metaobjects.h>

struct MyValue
{
    MyValue() {}
    explicit MyValue(qreal v) : v(v) {}

    qreal v = 0;

    bool operator==(const MyValue& other) const { return v == other.v; }
    bool operator!=(const MyValue& other) const { return v != other.v; }

    qreal foo() const { return 123; }
    qreal baz(qreal val) const { return val + 456; }

    qreal operator()(qreal a, qreal b) const { return a + b; }
};

class MyObject : public QObject
{
public:
    MyValue value() const { return value_; }
    void    setValue(MyValue v) { value_ = v; }

private:
    MyValue value_;
};

template <> class nwidget::MetaObject<MyObject> : public MetaObject<QObject>
{
    N_OBJECT(MyObject, QObject)

    N_BEGIN_PROPERTY
    N_PROPERTY(MyValue, value, N_READ value N_WRITE setValue)
    N_END_PROPERTY
};

using namespace nwidget;

class TestBinding : public QObject
{
    Q_OBJECT

private slots:
    void testBindingExpr()
    {
        using MetaObj = MetaObject<QSlider>;

        using Expr1 = decltype(std::declval<MetaObj>().value());
        static_assert(std::is_same<Expr1::Type, int>::value, "");
        static_assert(impl::is_observable_v<Expr1>, "");

        using Expr2 = decltype(std::declval<MetaObj>().value() + std::declval<MetaObj>().value());
        static_assert(std::is_same<Expr2::Type, int>::value, "");
        static_assert(impl::is_observable_v<Expr2>, "");

        using Expr3 = decltype(std::declval<MetaObj>().x());
        static_assert(std::is_same<Expr3::Type, int>::value, "");
        static_assert(!impl::is_observable_v<Expr3>, "");

        using Expr4 = decltype(std::declval<MetaObj>().value() + std::declval<MetaObj>().x());
        static_assert(std::is_same<Expr4::Type, int>::value, "");
        static_assert(impl::is_observable_v<Expr4>, "");
    }

    void testExpr()
    {
        QSlider _s1;
        QSlider _s2;
        QSlider _s3;
        QSlider _s4;

        auto s1 = MetaObject<>::from(&_s1);
        auto s2 = MetaObject<>::from(&_s2);
        auto s3 = MetaObject<>::from(&_s3);
        auto s4 = MetaObject<>::from(&_s4);

        // binary operators
        {
#define TEST_BINARY_EXPR(OP)                                                                                           \
    {                                                                                                                  \
        auto expr1 = s1.value() OP s2.value();                                                                         \
        auto expr2 = [&_s1, &_s2]() { return _s1.value() OP _s2.value(); };                                            \
                                                                                                                       \
        s1.value() = 1;                                                                                                \
        s2.value() = 1;                                                                                                \
        QCOMPARE(expr1.eval(), expr2());                                                                               \
                                                                                                                       \
        s1.value() = 2;                                                                                                \
        s2.value() = 3;                                                                                                \
        QCOMPARE(expr1.eval(), expr2());                                                                               \
    }

            TEST_BINARY_EXPR(+)
            TEST_BINARY_EXPR(-)
            TEST_BINARY_EXPR(*)
            TEST_BINARY_EXPR(/)

            TEST_BINARY_EXPR(==)
            TEST_BINARY_EXPR(!=)
            TEST_BINARY_EXPR(<)
            TEST_BINARY_EXPR(<=)
            TEST_BINARY_EXPR(>)
            TEST_BINARY_EXPR(>=)

            TEST_BINARY_EXPR(&&)
            TEST_BINARY_EXPR(||)

            TEST_BINARY_EXPR(&)
            TEST_BINARY_EXPR(|)
            TEST_BINARY_EXPR(^)
            TEST_BINARY_EXPR(<<)
            TEST_BINARY_EXPR(>>)

#undef TEST_BINARY_EXPR
        }

        // unary operator
        {
#define TEST_UNARY_EXPR(OP)                                                                                            \
    {                                                                                                                  \
        auto expr1 = OP s1.value();                                                                                    \
        auto expr2 = [&_s1]() { return OP _s1.value(); };                                                              \
                                                                                                                       \
        s1.value() = 10;                                                                                               \
        QCOMPARE(expr1.eval(), expr2());                                                                               \
                                                                                                                       \
        s1.value() = 24;                                                                                               \
        QCOMPARE(expr1.eval(), expr2());                                                                               \
    }

            TEST_UNARY_EXPR(-)
            TEST_UNARY_EXPR(!)
            TEST_UNARY_EXPR(~)

            TEST_UNARY_EXPR(~)

            // TODO:
            // - address of
            // - dereference
#undef TEST_UNARY_EXPR
        }

        // complex expr
        {
            auto expr1 = s1.value() + s2.value() * s3.value() / (s4.value() + 15);
            auto expr2 = [&_s1, &_s2, &_s3, &_s4]()
            { return _s1.value() + _s2.value() * _s3.value() / (_s4.value() + 15); };

            QCOMPARE(expr1.eval(), expr2());

            s1.value() = 10;

            QCOMPARE(expr1.eval(), expr2());

            s1.value() = 20;
            s2.value() = 30;
            s3.value() = 40;

            QCOMPARE(expr1.eval(), expr2());

            s2.value() = 50;
            s4.value() = 60;

            QCOMPARE(expr1.eval(), expr2());
        }
    }

    void testCast()
    {
        QSlider _s1;

        auto s1 = MetaObject<>::from(&_s1);

        {
            auto expr1 = cast<qreal>(s1.value()) / 3;
            auto expr2 = [&_s1]() { return (qreal)_s1.value() / 3; };

            QCOMPARE(expr1.eval(), expr2());

            s1.value() = 10;
            QCOMPARE(expr1.eval(), expr2());
        }

        {
            auto expr1 = static_cast_<qreal>(s1.value()) / 3;
            auto expr2 = [&_s1]() { return static_cast<qreal>(_s1.value()) / 3; };

            QCOMPARE(expr1.eval(), expr2());

            s1.value() = 10;
            QCOMPARE(expr1.eval(), expr2());
        }

        {
            static auto func = [](int v)
            {
                static std::conditional_t<sizeof(qint64) == sizeof(qreal), qint64, qint32> val;
                val = v;
                return &val;
            };

            auto expr1 = *reinterpret_cast_<qreal*>(invoke(func, s1.value())) / 3;
            auto expr2 = [&_s1]() { return *reinterpret_cast<qreal*>(func(_s1.value())) / 3; };

            QCOMPARE(expr1.eval(), expr2());

            s1.value() = 10;
            QCOMPARE(expr1.eval(), expr2());
        }
    }

    void testCond()
    {
        QSlider _s1;
        QSlider _s2;

        auto s1 = MetaObject<>::from(&_s1);
        auto s2 = MetaObject<>::from(&_s2);

        auto expr1 = cond(s1.value() <= 10, s1.value(), s2.value());
        auto expr2 = [&_s1, &_s2]() { return _s1.value() <= 10 ? _s1.value() : _s2.value(); };

        QCOMPARE(expr1.eval(), expr2());

        s1.value() = 20;
        s2.value() = 30;
        QCOMPARE(expr1.eval(), expr2());
    }

    void testCall()
    {
        QSlider _s1;
        QSlider _s2;

        auto s1 = MetaObject<>::from(&_s1);
        auto s2 = MetaObject<>::from(&_s2);

        static const auto func = [](int a, int b) { return a + b; };

        auto expr1 = invoke(func, s1.value(), s2.value());
        auto expr2 = [&_s1, &_s2]() { return func(_s1.value(), _s2.value()); };

        QCOMPARE(expr1.eval(), expr2());

        s1.value() = 10;
        s2.value() = 20;
        QCOMPARE(expr1.eval(), expr2());
    }

    void testConstructor()
    {
        QSlider _s1;
        QSlider _s2;

        auto s1 = MetaObject<>::from(&_s1);
        auto s2 = MetaObject<>::from(&_s2);

        auto expr1 = constructor<MyValue>(s1.value() + s2.value());
        auto expr2 = [&_s1, &_s2]() { return MyValue(_s1.value() + _s2.value()); };

        QCOMPARE(expr1.eval(), expr2());

        s1.value() = 10;
        s2.value() = 20;
        QCOMPARE(expr1.eval(), expr2());
    }

    void testInvoke()
    {
        {
            QLabel _label;

            auto label = MetaObject<>::from(&_label);

            auto expr1 = label.text().m(&QString::length) + 2;
            auto expr2 = [&_label]() { return _label.text().length() + 2; };

            QCOMPARE(expr1.eval(), expr2());

            label.text() = "Hello";
            QCOMPARE(expr1.eval(), expr2());
        }

        MyObject _obj;

        auto obj = MetaObject<>::from(&_obj);

        {

            auto expr1 = obj.value().m(&MyValue::v) + 1;
            auto expr2 = [&_obj]() { return _obj.value().v + 1; };

            QCOMPARE(expr1.eval(), expr2());

            obj.value() = MyValue(123);
            QCOMPARE(expr1.eval(), expr2());
        }
        {
            auto exp1 = obj.value().m(&MyValue::foo) + 1;
            auto exp2 = [&_obj]() { return _obj.value().foo() + 1; };

            QCOMPARE(exp1.eval(), exp2());

            obj.value() = MyValue(123);
            QCOMPARE(exp1.eval(), exp2());
        }
        {
            QSlider _s1;

            auto s1 = MetaObject<>::from(&_s1);

            auto expr1 = obj.value().m(&MyValue::baz, s1.value()) + 789;
            auto expr2 = [&_obj, &_s1]() { return _obj.value().baz(_s1.value()) + 789; };

            QCOMPARE(expr1.eval(), expr2());

            obj.value() = MyValue(321);
            s1.value()  = 654;
            QCOMPARE(expr1.eval(), expr2());
        }
        {
            QSlider _s1;
            QSlider _s2;

            auto s1 = MetaObject<>::from(&_s1);
            auto s2 = MetaObject<>::from(&_s2);

            auto expr1 = obj.value()(s1.value(), s2.value());
            auto expr2 = [&_obj, &_s1, &_s2]() { return _obj.value()(_s1.value(), _s2.value()); };

            QCOMPARE(expr1.eval(), expr2());

            s1.value() = 123;
            s2.value() = 456;
            QCOMPARE(expr1.eval(), expr2());
        }
    }

    void testasprintf()
    {
        QSlider _s1;
        QSlider _s2;

        auto s1 = MetaObject<>::from(&_s1);
        auto s2 = MetaObject<>::from(&_s2);

        auto expr1 = asprintf_("%02d + %03d = %4d", s1.value(), s2.value(), s1.value() + s2.value());
        auto expr2 = [&_s1, &_s2]()
        { return QString::asprintf("%02d + %03d = %4d", _s1.value(), _s2.value(), _s1.value() + _s2.value()); };

        s1.value() = 25;
        s2.value() = 30;
        QCOMPARE(expr1.eval(), expr2());
    }

    void testCreateBinding()
    {
        QSlider _s1;
        QSlider _s2;

        auto s1 = MetaObject<>::from(&_s1);
        auto s2 = MetaObject<>::from(&_s2);

        // create by call bindTo
        {
            QSlider _s3;

            auto s3 = MetaObject<>::from(&_s3);

            (s1.value() + s2.value()).bindTo(s3.value());

            QCOMPARE(s3.value().get(), _s1.value() + _s2.value());

            s1.value() = 10;
            s2.value() = 20;
            QCOMPARE(s3.value().get(), _s1.value() + _s2.value());
        }

        // create by operator=
        {
            QSlider _s3;

            auto s3 = MetaObject<>::from(&_s3);

            s3.value() = s1.value();

            QCOMPARE(s3.value().get(), _s1.value());

            s1.value() = 10;
            QCOMPARE(s3.value().get(), _s1.value());

            s3.value() = s1.value() + s2.value();

            QCOMPARE(s3.value().get(), _s1.value() + _s2.value());

            s1.value() = 30;
            s2.value() = 40;
            QCOMPARE(s3.value().get(), _s1.value() + _s2.value());
        }
    }

    void testRemoveBinding()
    {
        QSlider _s1;
        QSlider _s2;

        auto s1 = MetaObject<>::from(&_s1);
        auto s2 = MetaObject<>::from(&_s2);

        // remove by MetaProperty destroy
        {
            auto s3 = MetaObject<>::from(new QSlider);

            s1.value() = s2.value() + s3.value();

            s2.value() = 10;
            s3.value() = 20;
            QCOMPARE(s1.value().get(), 10 + 20);

            delete s3.object_();

            s2.value() = 20;
            QCOMPARE(s1.value().get(), 10 + 20);
        }

        // remove by QObject destroy
        {
            auto s3 = new QSlider;

            s1.value() = s2.value() + invoke(&QSlider::value, s3);

            s3->setValue(20);
            s2.value() = 10;
            QCOMPARE(s1.value().get(), 10 + 20);

            delete s3;

            s2.value() = 20;
            QCOMPARE(s1.value().get(), 10 + 20);
        }

        // remove by rebinding
        {
            s1.value() = s2.value() + 10;
            QCOMPARE(s1.value().get(), s2.value().get() + 10);

            s1.value() = s2.value() + 20;
            QCOMPARE(s1.value().get(), s2.value().get() + 20);
        }

        // remove by bind from unobserable expr
        {
            s1.value() = s2.value() + 10;
            QCOMPARE(s1.value().get(), s2.value().get() + 10);

            s1.value() = s2.x() + 20;
            auto value = _s2.x() + 20;
            QCOMPARE(s1.value().get(), value);

            _s2.move(20, 30);
            QCOMPARE(s1.value().get(), value);
        }

        // TODO: BindingExpr<>::remove
    }

    void testBinding()
    {
        QSlider _s1;
        QSlider _s2;

        auto s1 = MetaObject<>::from(&_s1);
        auto s2 = MetaObject<>::from(&_s2);

        // observable expr
        {
            QSlider s3;

            auto expr = s1.value() + s2.value();

            // bind To slot
            expr.bindTo(&s3, &QSlider::setValue);

            QCOMPARE(s3.value(), _s1.value() + _s2.value());

            s1.value() = 10;
            s2.value() = 20;
            QCOMPARE(s3.value(), _s1.value() + _s2.value());

            // bind To func
            int  val  = 0;
            auto func = [&val](int v) { val = v; };

            expr.bindTo(func);

            QCOMPARE(val, _s1.value() + _s2.value());

            s1.value() = 30;
            s2.value() = 40;
            QCOMPARE(val, _s1.value() + _s2.value());
        }

        // unobservable expr
        {
            QSlider s3;

            auto expr = s1.x() + 10;
            auto val1 = _s1.x() + 10;

            // bind To slot
            expr.bindTo(&s3, &QSlider::setValue);

            QCOMPARE(val1, s3.value());

            _s1.move(20, 30);
            QCOMPARE(val1, s3.value());

            // bind To func
            int  val2 = _s1.x() + 10;
            auto func = [&val1](int v) { val1 = v; };
            expr.bindTo(func);

            QCOMPARE(val1, val2);

            _s1.move(30, 40);
            QCOMPARE(val1, val2);
        }

        // complex expr
        {
            QSlider  _s3;
            MyObject _obj;

            auto s3  = MetaObject<>::from(&_s3);
            auto obj = MetaObject<>::from(&_obj);

            s3.value() = obj.value()(s1.value(), s2.value());
            auto expr  = [&_obj, &_s1, &_s2]() { return _obj.value()(_s1.value(), _s2.value()); };

            QCOMPARE(s3.value().get(), expr());

            s1.value() = 11;
            s2.value() = 45;
            QCOMPARE(s3.value().get(), expr());
        }
    }
};

QTEST_MAIN(TestBinding)
#include "test_binding.moc"
