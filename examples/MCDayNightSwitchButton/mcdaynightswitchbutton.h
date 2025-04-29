#ifndef MCDAYNIGHTSWITCHBUTTON_H
#define MCDAYNIGHTSWITCHBUTTON_H

#include <QAbstractButton>

#include <nwidget/metaobjects.h>

class MCDayNightSwitchButton : public QAbstractButton
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MCDayNightSwitchButton)

public:
    explicit MCDayNightSwitchButton(QWidget* parent = nullptr);

    bool isTristate() const;
    void setTristate(bool y = true);

    Qt::CheckState checkState() const;
    void           setCheckState(Qt::CheckState value);

    qreal progress() const;
    void  setProgress(qreal progress);

signals:
    void checkStateChanged(Qt::CheckState);

protected:
    void nextCheckState() override;
    void paintEvent(QPaintEvent* event) override;

private:
    bool           tristate_ = false;
    Qt::CheckState state_    = Qt::Unchecked;

    qreal progress_ = 0;
};

template <> class nwidget::MetaObject<MCDayNightSwitchButton> : public MetaObject<QAbstractButton>
{
    N_OBJECT(MCDayNightSwitchButton, QAbstractButton)

    N_BEGIN_PROPERTY
    N_PROPERTY(bool, tristate, N_READ isTristate N_WRITE setTristate)
    N_PROPERTY(Qt::CheckState, checkState, N_READ checkState N_WRITE setCheckState N_NOTIFY checkStateChanged)
    N_PROPERTY(qreal, progress, N_READ progress N_WRITE setProgress)
    N_END_PROPERTY
};

#endif // MCDAYNIGHTSWITCHBUTTON_H
