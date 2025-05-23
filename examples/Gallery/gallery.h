#ifndef GALLERY_H
#define GALLERY_H

#include <QDialog>

class QProgressBar;
class QTextBrowser;

class Gallery : public QDialog
{
    Q_OBJECT
public:
    explicit Gallery(QWidget* parent = nullptr);

    void setVisible(bool visible) override;

private slots:
    void changeStyle(const QString& styleName);
    void advanceProgressBar();
    void helpOnCurrentWidget();
    void updateSystemInfo();

private:
    QProgressBar* progressBar;
    QTextBrowser* systemInfoTextBrowser;
};

#endif // GALLERY_H
