#include <QtTest>
#include <filesystem>
#include <fstream>
#include <nwidget/metaobjects.h>
#include <nwidget/binding.h>
#include <nwidget/behavior.h>
#include <nwidget/builders.h>
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>

namespace fs = std::filesystem;

class TestNGWidget2 : public QObject
{
    Q_OBJECT

private:
    QWidget* testWidget;

private slots:
    void initTestCase() {
        testWidget = new QWidget;
    }

    void cleanupTestCase() {
        delete testWidget;
    }

    void test_metaobject() {
        using namespace nwidget;

        using MetaObj  = MetaObject<QWidget>;
        using MetaProp = decltype(MetaObj().fullScreen());

        using Class      = MetaObj::Class;        // QWidget
        using SuperClass = MetaObj::Super::Class; // QObject

        using Type              = MetaProp::Type;       // bool
        constexpr bool readable = MetaProp::isReadable; // true
        constexpr bool writable = MetaProp::isWritable; // false
    }

    // void test_property_binding() {
    //     using namespace nwidget;
        
    //     auto label = MetaObject<>::from(new QSLabel);
    //     auto slider = MetaObject<>::from(new QSlider);
        
    //     label->setText("0");
    //     slider->setValue(50);
        
    //     label->text() = nw::asprintf("%d", slider->value());
    //     QCOMPARE(label->text(), QString("50"));
    // }

    // void test_animation() {
    //     using namespace nwidget;
        
    //     auto widget = MetaObject<>::from(new QWidget);
    //     auto checkbox = MetaObject<>::from(new QCheckBox);
        
    //     Behavior::on(widget->minimumWidth(),
    //                new SpringAnimation<int>(spring{2}, damping{0.2}));
                   
    //     Behavior::animated(widget->minimumWidth()) = cond(checkbox->isChecked(), 300, 50);
    //     QCOMPARE(widget->minimumWidth(), 50);
        
    //     checkbox->setChecked(true);
    //     QTest::qWait(100); // Allow animation to start
    //     QVERIFY(widget->minimumWidth() > 50);
    // }

    // void test_declarative_ui() {
    //     using namespace nwidget;
        
    //     QLayout* layout = FormLayout{
    //         {"Label", LineEdit().text("Test")},
    //         {"Slider", Slider(Qt::Horizontal).range(0, 100)},
    //         {GridLayout{
    //             {0, 0, PushButton("Button")}
    //         }}
    //     };
        
    //     QVERIFY(layout != nullptr);
    //     QCOMPARE(layout->count(), 3);
    // }
};

QTEST_MAIN(TestNGWidget2)
#include "test_basics.moc"
