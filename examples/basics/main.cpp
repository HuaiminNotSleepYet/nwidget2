#include "mainwindow.hpp"
#include <QApplication>
#include <QScreen>

QScreen *screenAt(const QPoint &pos)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return qApp->screenAt(pos);
#else
    const QList<QScreen *> screens = QGuiApplication::screens();
    for (QScreen *screen : screens) {
        if (screen->geometry().contains(pos))
            return screen;
    }
    return nullptr;
#endif
}

void centerOnScreen(QWidget *widget)
{
    if (const QScreen *screen = screenAt(QCursor::pos())) {
        QRect rct = screen->geometry();
        widget->move((rct.width() - widget->width()) / 2,
                     (rct.height() - widget->height()) / 2);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(icons);
    
    MainWindow w;
    w.setWindowTitle("nwidget2 Demo Application");
    w.resize(512, 256);
    w.show();
    // centerOnScreen(&w);
    return a.exec();
}
