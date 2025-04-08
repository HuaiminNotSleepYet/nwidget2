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

#include <nwidget/metaobjects.h>

/* ------------------------------------------- Create Test For MetaObject ------------------------------------------- */

#undef NWIDGET_METAOBJECTS_H

#undef N_OBJECT

#undef N_BEGIN_PROPERTY
#undef N_END_PROPERTY

#undef N_PROPERTY

namespace nwidget::test {

template <typename Class> using MetaObj = ::nwidget::MetaObject<Class>;

template <typename...> class TestMetaObject;

#define MetaObject test::TestMetaObject

#define N_OBJECT(CLASS, ...)                                                                                           \
public:                                                                                                                \
    using Class = CLASS;                                                                                               \
    static void run() { testProperty(); }

#define N_BEGIN_PROPERTY                                                                                               \
public:                                                                                                                \
    static void testProperty()                                                                                         \
    {                                                                                                                  \
        using namespace ::nwidget::test;

#define N_END_PROPERTY }

#define N_PROPERTY(TYPE, NAME, ...)                                                                                    \
    TestGetter::run<decltype(std::declval<MetaObj<Class>>().NAME())>();                                                \
    TestSetter::run<decltype(std::declval<MetaObj<Class>>().NAME())>();                                                \
    TestNotify::run<decltype(std::declval<MetaObj<Class>>().NAME())>();                                                \
    TestReset ::run<decltype(std::declval<MetaObj<Class>>().NAME())>();

struct TestGetter
{
    template <typename MetaProp> static std::enable_if_t<!MetaProp::isReadable> run() {}
    template <typename MetaProp> static std::enable_if_t<MetaProp::isReadable>  run()
    {
        static_assert(std::is_invocable_v<typename MetaProp::Getter, typename MetaProp::Class*>);
    }
};

struct TestSetter
{
    template <typename MetaProp> static std::enable_if_t<!MetaProp::isWritable> run() {}
    template <typename MetaProp> static std::enable_if_t<MetaProp::isWritable>  run()
    {
        static_assert(
            std::is_invocable_v<typename MetaProp::Setter, typename MetaProp::Class*, typename MetaProp::Type>);
    }
};

struct TestNotify
{
    template <typename MetaProp> static std::enable_if_t<!MetaProp::hasNotifySignal> run() {}
    template <typename MetaProp> static std::enable_if_t<MetaProp::hasNotifySignal>  run() {}
};

struct TestReset
{
    template <typename MetaProp> static std::enable_if_t<!MetaProp::isResettable> run() {}
    template <typename MetaProp> static std::enable_if_t<MetaProp::isResettable>  run()
    {
        static_assert(std::is_invocable_v<typename MetaProp::Reset, typename MetaProp::Class*>);
    }
};

template <typename...> class TestBuilder;

} // namespace nwidget::test

#include <nwidget/metaobjects.h>

#undef MetaObject

/* ------------------------------------------------------ Test ------------------------------------------------------ */

using namespace nwidget;

class TestMetaObj : public QObject
{
    Q_OBJECT

private slots:
    void testMetaObject()
    {
        test::TestMetaObject<QObject>::run();
        test::TestMetaObject<QAction>::run();
        test::TestMetaObject<QLayout>::run();
        test::TestMetaObject<QBoxLayout>::run();
        test::TestMetaObject<QHBoxLayout>::run();
        test::TestMetaObject<QVBoxLayout>::run();
        test::TestMetaObject<QFormLayout>::run();
        test::TestMetaObject<QStackedLayout>::run();
        test::TestMetaObject<QWidget>::run();
        test::TestMetaObject<QAbstractButton>::run();
        test::TestMetaObject<QDialogButtonBox>::run();
        test::TestMetaObject<QCheckBox>::run();
        test::TestMetaObject<QPushButton>::run();
        test::TestMetaObject<QCommandLinkButton>::run();
        test::TestMetaObject<QRadioButton>::run();
        test::TestMetaObject<QToolButton>::run();
        test::TestMetaObject<QFrame>::run();
        test::TestMetaObject<QAbstractScrollArea>::run();
        test::TestMetaObject<QAbstractItemView>::run();
        test::TestMetaObject<QHeaderView>::run();
        test::TestMetaObject<QListView>::run();
        test::TestMetaObject<QListWidget>::run();
        test::TestMetaObject<QTableView>::run();
        test::TestMetaObject<QTableWidget>::run();
        test::TestMetaObject<QTreeView>::run();
        test::TestMetaObject<QTreeWidget>::run();
        test::TestMetaObject<QPlainTextEdit>::run();
        test::TestMetaObject<QTextEdit>::run();
        test::TestMetaObject<QTextBrowser>::run();
        test::TestMetaObject<QToolBox>::run();
        test::TestMetaObject<QSplitter>::run();
        test::TestMetaObject<QAbstractSlider>::run();
        test::TestMetaObject<QDial>::run();
        test::TestMetaObject<QSlider>::run();
        test::TestMetaObject<QScrollBar>::run();
        test::TestMetaObject<QAbstractSpinBox>::run();
        test::TestMetaObject<QDateTimeEdit>::run();
        test::TestMetaObject<QDateEdit>::run();
        test::TestMetaObject<QTimeEdit>::run();
        test::TestMetaObject<QSpinBox>::run();
        test::TestMetaObject<QDoubleSpinBox>::run();
        test::TestMetaObject<QComboBox>::run();
        test::TestMetaObject<QGroupBox>::run();
        test::TestMetaObject<QLabel>::run();
        test::TestMetaObject<QLineEdit>::run();
        test::TestMetaObject<QMenu>::run();
        test::TestMetaObject<QMenuBar>::run();
        test::TestMetaObject<QProgressBar>::run();
        test::TestMetaObject<QTabBar>::run();
        test::TestMetaObject<QTabWidget>::run();

        {
            using MetaObj = MetaObject<QObject>;

            static_assert(std::is_same_v<QObject, MetaObj::Class>);
            static_assert(std::is_same_v<void, MetaObj::Super::Class>);

            QCOMPARE(MetaObj::className, "QObject");

            using MetaProp = decltype(MetaObj().objectName());

            static_assert(std::is_same_v<MetaProp::Type, QString>);
            static_assert(MetaProp::isReadable);
            static_assert(MetaProp::isWritable);
            static_assert(MetaProp::hasNotifySignal);

            QCOMPARE(MetaProp::Info::name(), "objectName");
        }

        {
            using MetaObj = MetaObject<QWidget>;

            static_assert(std::is_same_v<QWidget, MetaObj::Class>);
            static_assert(std::is_same_v<QObject, MetaObj::Super::Class>);

            QCOMPARE(MetaObj::className, "QWidget");

            using MetaProp = decltype(MetaObj().x());

            static_assert(std::is_same_v<MetaProp::Type, int>);
            static_assert(MetaProp::isReadable);
            static_assert(!MetaProp::isWritable);
            static_assert(!MetaProp::hasNotifySignal);

            QCOMPARE(MetaProp::Info::name(), "x");
        }
    }

    void testMetaProperty()
    {
        QSlider _s1;

        auto    s1 = MetaObject<>::from(&_s1);
        QSlider s2;

        QCOMPARE(s1.value().get(), s2.value());

        s1.value() = 10;
        s2.setValue(10);

        QCOMPARE(s1.value().get(), 10);
        QCOMPARE(s1.value().get(), s2.value());

        s1.value() = 20;
        s2.setValue(30);

        QVERIFY(s1.value().get() != s2.value());
    }
};

QTEST_MAIN(TestMetaObj)
#include "test_metaobj.moc"
