#include "mainwindow.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QFormLayout>

#include <nwidget/metaobjects.h>
#include <nwidget/binding.h>
#include <nwidget/behavior.h>
#include <nwidget/builders.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    using namespace nwidget;

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainlayout = new QVBoxLayout(centralWidget);

    QLayout* layout = FormLayout{
    {"Label 0", LineEdit().text("Hello")},
    {"Label 1", Slider(Qt::Horizontal).range(0, 100).value(25)},
    {GridLayout{
        {0, 0,       PushButton("Button 0")},
        {1, 0,       PushButton("Button 1")},
        {0, 1, 2, 1, PushButton("Button 2").sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding),
        },
    }}};

    mainlayout->addLayout(layout);

    this->setCentralWidget(centralWidget);
    // this->setWindowState(Qt::WindowMaximized);
}
