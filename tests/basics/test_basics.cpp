#include <QtTest>
#include <filesystem>
#include <fstream>
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <nwidget/metaobjects.h>
#include <nwidget/binding.h>
#include <nwidget/behavior.h>
#include <nwidget/builders.h>

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

    void test_property_binding() {
        using namespace nwidget;

        QWidget* centralWidget = new QWidget();
        QVBoxLayout* mainlayout = new QVBoxLayout(centralWidget);
        
        auto label = new QLabel;
        auto slider = new QSlider;

        QLayout* layout = FormLayout{
            {"Label", Label(label)},
            {"Slider", Slider(Qt::Horizontal).range(0, 100).value(25)},
        };

        mainlayout->addLayout(layout);

        label->setText("0");
        slider->setValue(50);
        
        label->text() = nw::asprintf("%d", slider->value());
        QCOMPARE(label->text(), QString("50"));
    }

};

QTEST_MAIN(TestNGWidget2)
#include "test_basics.moc"
