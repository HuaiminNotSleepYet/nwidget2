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

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{

    this->setWindowState(Qt::WindowMaximized);
}
