#include <QTest>

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QCommandLinkButton>
#include <QDateTimeEdit>
#include <QDial>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QRadioButton>
#include <QScrollBar>
#include <QSplitter>
#include <QStackedLayout>
#include <QTableWidget>
#include <QTextBrowser>
#include <QToolBox>
#include <QToolButton>
#include <QTreeWidget>

#include <nwidget/builders.h>

/* ------------------------------------------------------ Test ------------------------------------------------------ */

static bool compareObject(QObject* a, QObject* b);
static bool compareLayout(QLayout* a, QLayout* b);

class MySlider : public QSlider
{
public:
    using QSlider::QSlider;
};

using namespace nwidget;

class TestBuilder : public QObject
{
    Q_OBJECT

private slots:
    void testBuilder1()
    {
        QSlider _s1;

        auto s1 = Slider(&_s1).objectName("wuhu").range(0, 114514).value(23).minimumSize(14, 23);

        QCOMPARE(s1.object(), &_s1);
        QCOMPARE(_s1.objectName(), "wuhu");
        QCOMPARE(_s1.minimum(), 0);
        QCOMPARE(_s1.maximum(), 114514);
        QCOMPARE(_s1.value(), 23);
        QCOMPARE(_s1.minimumSize(), QSize(14, 23));
    }

    void testBuilder2()
    {
        MySlider s1;
        QSlider  s2;

        Builder<MySlider>(&s1).objectName("qifei").range(0, 1919810).value(23).minimumSize(14, 23);
        Builder<QSlider>(&s2).objectName("qifei").range(0, 1919810).value(23).minimumSize(14, 23);

        QVERIFY(compareObject(&s1, &s2));
    }

    void testBoxLayout()
    {
        QWidget w1;
        QWidget w2;

        Widget(&w1).layout(HBoxLayout{
            PushButton("WDNMD"),
            CheckBox("WTF"),
        });

        {
            auto layout = new QHBoxLayout;
            layout->addWidget(new QPushButton("WDNMD"));
            layout->addWidget(new QCheckBox("WTF"));
            w2.setLayout(layout);
        }

        QVERIFY(compareObject(&w1, &w2));
    }

    void testGridLayout()
    {
        QGridLayout* l1 = GridLayout{
            {0, 0, PushButton("Button")},
            {1, 1, CheckBox("CheckBox")},
            {2, 2, ComboBox()},
            {3, 3, LineEdit()},
        };

        QGridLayout* l2 = new QGridLayout;
        {
            l2->addWidget(new QPushButton("Button"), 0, 0);
            l2->addWidget(new QCheckBox("CheckBox"), 1, 1);
            l2->addWidget(new QComboBox(), 2, 2);
            l2->addWidget(new QLineEdit(), 3, 3);
        }

        QVERIFY(compareLayout(l1, l2));

        l1->deleteLater();
        l2->deleteLater();
    }

    void testFormLayout()
    {
        QFormLayout* l1 = FormLayout{
            {"Label", new QLineEdit()},
            {Label("Label"), CheckBox()},
            {Widget().objectName("widget"), ComboBox()},
        };

        QFormLayout* l2 = new QFormLayout;
        {
            l2->addRow("Label", new QLineEdit());
            l2->addRow(new QLabel("Label"), new QCheckBox());
            auto w = new QWidget;
            w->setObjectName("widget");
            l2->addRow(w, new QComboBox());
        }

        QVERIFY(compareLayout(l1, l2));

        l1->deleteLater();
        l2->deleteLater();
    }

    void testForEach()
    {
        QStringList names = {"Mike", "John", "Tom"};

        static auto func1 = []() { return new QLabel(); };
        static auto func3 = [](const QString& name) { return new QLabel(name); };
        static auto func5 = [](int index, const QString& name)
        { return new QLabel(QString::number(index) + ": " + name); };
        static auto func7 = [](int index) { return new QLabel(QString::number(index)); };

        static auto func2 = []() -> BoxLayoutItem { return func1(); };
        static auto func4 = [](const QString& name) -> BoxLayoutItem { return func3(name); };
        static auto func6 = [](int index, const QString& name) -> BoxLayoutItem { return func5(index, name); };
        static auto func8 = [](int index) -> BoxLayoutItem { return func7(index); };

        // func()
        {
            QLayout* l1 = new QHBoxLayout;
            for (const auto& _ : std::as_const(names))
                l1->addWidget(func1());

            QLayout* l2 = HBoxLayout{
                ForEach(names, func2),
            };

            QVERIFY(compareLayout(l1, l2));

            l1->deleteLater();
            l2->deleteLater();
        }

        // func(const T&)
        {
            QLayout* l1 = new QHBoxLayout;
            for (const auto& name : std::as_const(names))
                l1->addWidget(func3(name));

            QLayout* l2 = HBoxLayout{
                ForEach(names, func4),
            };

            QVERIFY(compareLayout(l1, l2));

            l1->deleteLater();
            l2->deleteLater();
        }

        // func(int, const T&)
        {
            QLayout* l1 = new QHBoxLayout;
            for (int i = 0; i < names.size(); ++i)
                l1->addWidget(func5(i, names[i]));

            QLayout* l2 = HBoxLayout{
                ForEach(names, func6),
            };

            QVERIFY(compareLayout(l1, l2));

            l1->deleteLater();
            l2->deleteLater();
        }

        // ForEach(count, func)
        {
            QLayout* l1 = new QHBoxLayout;
            for (int i = 0; i < 10; ++i)
                l1->addWidget(func7(i));

            QLayout* l2 = HBoxLayout{
                ForEach(10, func8),
            };

            QVERIFY(compareLayout(l1, l2));

            l1->deleteLater();
            l2->deleteLater();
        }

        // ForEach(start, end, func)
        {
            QLayout* l1 = new QHBoxLayout;
            for (int i = 5; i < 10; ++i)
                l1->addWidget(func7(i));

            QLayout* l2 = HBoxLayout{
                ForEach(5, 10, func8),
            };

            QVERIFY(compareLayout(l1, l2));

            l1->deleteLater();
            l2->deleteLater();
        }
    }
};

static bool compareObject(QObject* a, QObject* b)
{
    if (!a && !b)
        return true;

    if (a == nullptr || b == nullptr)
        return false;

    auto metaobj1 = a->metaObject();
    auto metaobj2 = b->metaObject();

    if (metaobj1 != metaobj2)
        return false;

    // QWidget
    {
        auto w1 = qobject_cast<QWidget*>(a);
        auto w2 = qobject_cast<QWidget*>(b);
        if (w1 && w2) {
            if (!compareLayout(w1->layout(), w2->layout()))
                return false;
        }
    }

    for (int i = 0; i < metaobj1->propertyCount(); ++i) {
        auto prop1 = metaobj1->property(i);
        auto prop2 = metaobj2->property(i);

        static const QSet<QString> skip = {"windowIcon", "icon", "pixmap"};

        if (skip.contains(prop1.name()))
            continue;

        if (prop1.name() != prop2.name())
            return false;

        if (prop1.read(a) != prop2.read(b))
            return false;
    }

    return true;
}

static bool compareLayout(QLayout* a, QLayout* b)
{
    if (!a && !b)
        return true;

    if (a == nullptr || b == nullptr)
        return false;

    if (!compareObject(a, b))
        return false;

    if (a->count() != b->count())
        return false;

    for (int i = 0; i < a->count(); ++i) {
        auto i1 = a->itemAt(i);
        auto i2 = b->itemAt(i);

        if (i1->widget() && i2->widget()) {
            if (!compareObject(i1->widget(), i2->widget()))
                return false;
        } else if (i1->layout() && i2->layout()) {
            if (!compareLayout(i1->layout(), i2->layout()))
                return false;
        } else if (i1->layout() || i2->layout()) {
            return false;
        }
    }

    return true;
}

QTEST_MAIN(TestBuilder)
#include "test_builder.moc"
