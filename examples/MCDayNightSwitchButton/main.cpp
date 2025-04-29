#include "mcdaynightswitchbutton.h"

#include <QApplication>
#include <QBoxLayout>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    using namespace nwidget;

    QWidget w;

    auto button1 = new MCDayNightSwitchButton;
    auto button2 = new MCDayNightSwitchButton;
    auto button3 = new MCDayNightSwitchButton;

    button2->setTristate();
    button2->setCheckState(Qt::PartiallyChecked);
    button3->setChecked(true);

    auto layout = new QVBoxLayout;
    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->addWidget(button3);
    layout->setAlignment(Qt::AlignHCenter);
    layout->setSpacing(16);
    w.setLayout(layout);

    w.show();

    return a.exec();
}
