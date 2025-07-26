#include "mcdaynightswitchbutton.h"

#include <QPainter>

#include <nwidget/behavior.h>
#include <nwidget/binding.h>

constexpr auto Radius = 32;
constexpr auto Margin = 8;

MCDayNightSwitchButton::MCDayNightSwitchButton(QWidget* parent)
    : QAbstractButton(parent)
{
    setCheckable(true);
    setFixedWidth(Radius * 2 * 3 + Margin * 4);
    setFixedHeight(Radius * 2 + Margin * 2);

    {
        using namespace nwidget;

        auto button = MetaObject<>::from(this);

        button.checkState() = cond(button.checked(), Qt::Checked, Qt::Unchecked);

        Behavior::on(button.progress(), new SpringAnimation<qreal>(spring{2.5}, damping{0.3}, epsilon{0.025}));

        invoke(
            [](Qt::CheckState s) -> qreal
            {
                switch (s) {
                case Qt::Unchecked       : return 0.0;
                case Qt::PartiallyChecked: return 0.5;
                case Qt::Checked         : return 1.0;
                default                  : return 0.0;
                }
            },
            button.checkState())
            .bindTo(Behavior::animated(button.progress()));
    }
}

bool MCDayNightSwitchButton::isTristate() const { return tristate_; }

void MCDayNightSwitchButton::setTristate(bool y)
{
    tristate_ = y;
    if (!y && state_ == Qt::PartiallyChecked)
        setCheckState(Qt::Unchecked);
}

Qt::CheckState MCDayNightSwitchButton::checkState() const { return state_; }

void MCDayNightSwitchButton::setCheckState(Qt::CheckState value)
{
    if (state_ == value)
        return;
    state_ = value;
    setChecked(value == Qt::Checked);
    emit checkStateChanged(value);
}

void MCDayNightSwitchButton::nextCheckState()
{
    if (!isTristate()) {
        setCheckState(isChecked() ? Qt::Unchecked : Qt::Checked);
    } else {
        switch (checkState()) {
        case Qt::Unchecked       : setCheckState(Qt::PartiallyChecked); break;
        case Qt::PartiallyChecked: setCheckState(Qt::Checked); break;
        case Qt::Checked         : setCheckState(Qt::Unchecked); break;
        }
    }
}

qreal MCDayNightSwitchButton::progress() const { return progress_; }

void MCDayNightSwitchButton::setProgress(qreal progress)
{
    if (progress_ == progress)
        return;
    progress_ = progress;
    update();
}

void MCDayNightSwitchButton::paintEvent(QPaintEvent* event)
{
    static const uint8_t sunImageData[] = {
        0xFF, 0xD5, 0x4A, 0xFF, 0xD5, 0x4A, 0xFF, 0xD5, 0x4A, 0xFF, 0xD5, 0x4A, //
        0xFF, 0xD5, 0x4A, 0xFF, 0xD5, 0x4A, 0xFF, 0xD5, 0x4A, 0xFF, 0xD5, 0x4A, //
        0xFF, 0xD5, 0x4A, 0xFF, 0xFF, 0xAA, 0xFF, 0xFF, 0xAA, 0xFF, 0xFF, 0xAA, //
        0xFF, 0xFF, 0xAA, 0xFF, 0xFF, 0xAA, 0xFF, 0xFF, 0xAA, 0xFF, 0xD5, 0x4A, //
        0xFF, 0xD5, 0x4A, 0xFF, 0xFF, 0xAA, 0xFF, 0xFF, 0xD9, 0xFF, 0xFF, 0xD9, //
        0xFF, 0xFF, 0xD9, 0xFF, 0xFF, 0xD9, 0xFF, 0xFF, 0xAA, 0xFF, 0xD5, 0x4A, //
        0xFF, 0xD5, 0x4A, 0xFF, 0xFF, 0xAA, 0xFF, 0xFF, 0xD9, 0xFF, 0xFF, 0xD9, //
        0xFF, 0xFF, 0xD9, 0xFF, 0xFF, 0xD9, 0xFF, 0xFF, 0xAA, 0xFF, 0xD5, 0x4A, //
        0xFF, 0xD5, 0x4A, 0xFF, 0xFF, 0xAA, 0xFF, 0xFF, 0xD9, 0xFF, 0xFF, 0xD9, //
        0xFF, 0xFF, 0xD9, 0xFF, 0xFF, 0xD9, 0xFF, 0xFF, 0xAA, 0xFF, 0xD5, 0x4A, //
        0xFF, 0xD5, 0x4A, 0xFF, 0xFF, 0xAA, 0xFF, 0xFF, 0xD9, 0xFF, 0xFF, 0xD9, //
        0xFF, 0xFF, 0xD9, 0xFF, 0xFF, 0xD9, 0xFF, 0xFF, 0xAA, 0xFF, 0xD5, 0x4A, //
        0xFF, 0xD5, 0x4A, 0xFF, 0xFF, 0xAA, 0xFF, 0xFF, 0xAA, 0xFF, 0xFF, 0xAA, //
        0xFF, 0xFF, 0xAA, 0xFF, 0xFF, 0xAA, 0xFF, 0xFF, 0xAA, 0xFF, 0xD5, 0x4A, //
        0xFF, 0xD5, 0x4A, 0xFF, 0xD5, 0x4A, 0xFF, 0xD5, 0x4A, 0xFF, 0xD5, 0x4A, //
        0xFF, 0xD5, 0x4A, 0xFF, 0xD5, 0x4A, 0xFF, 0xD5, 0x4A, 0xFF, 0xD5, 0x4A  //
    };

    static const uint8_t moonImageData[] = {
        0xD9, 0xE4, 0xFF, 0xD9, 0xE4, 0xFF, 0xD9, 0xE4, 0xFF, 0xD9, 0xE4, 0xFF, //
        0x90, 0x97, 0xA5, 0xD9, 0xE4, 0xFF, 0x50, 0x56, 0x66, 0x5F, 0x67, 0x7A, //
        0xD9, 0xE4, 0xFF, 0x90, 0x97, 0xA5, 0xAF, 0xB8, 0xCC, 0xD9, 0xE4, 0xFF, //
        0xD9, 0xE4, 0xFF, 0xD9, 0xE4, 0xFF, 0x5F, 0x67, 0x7A, 0x5F, 0x67, 0x7A, //
        0xD9, 0xE4, 0xFF, 0x90, 0x97, 0xA5, 0xAF, 0xB8, 0xCC, 0xD9, 0xE4, 0xFF, //
        0xAF, 0xB8, 0xCC, 0xD9, 0xE4, 0xFF, 0x74, 0x7D, 0x93, 0x5F, 0x67, 0x7A, //
        0xD9, 0xE4, 0xFF, 0xAF, 0xB8, 0xCC, 0xAF, 0xB8, 0xCC, 0x90, 0x97, 0xA5, //
        0xAF, 0xB8, 0xCC, 0xD9, 0xE4, 0xFF, 0x50, 0x56, 0x66, 0x50, 0x56, 0x66, //
        0x90, 0x97, 0xA5, 0xD9, 0xE4, 0xFF, 0xD9, 0xE4, 0xFF, 0x90, 0x97, 0xA5, //
        0xAF, 0xB8, 0xCC, 0xAF, 0xB8, 0xCC, 0x5F, 0x67, 0x7A, 0x5F, 0x67, 0x7A, //
        0xD9, 0xE4, 0xFF, 0xAF, 0xB8, 0xCC, 0xAF, 0xB8, 0xCC, 0xAF, 0xB8, 0xCC, //
        0xAF, 0xB8, 0xCC, 0xD9, 0xE4, 0xFF, 0x74, 0x7D, 0x93, 0x5F, 0x67, 0x7A, //
        0x74, 0x7D, 0x93, 0x50, 0x56, 0x66, 0x5F, 0x67, 0x7A, 0x5F, 0x67, 0x7A, //
        0x5F, 0x67, 0x7A, 0x74, 0x7D, 0x93, 0x50, 0x56, 0x66, 0x5F, 0x67, 0x7A, //
        0x5F, 0x67, 0x7A, 0x5F, 0x67, 0x7A, 0x5F, 0x67, 0x7A, 0x50, 0x56, 0x66, //
        0x5F, 0x67, 0x7A, 0x5F, 0x67, 0x7A, 0x5F, 0x67, 0x7A, 0x5F, 0x67, 0x7A, //
    };

    static auto sun  = QImage(sunImageData, 8, 8, QImage::Format_RGB888);
    static auto moon = QImage(moonImageData, 8, 8, QImage::Format_RGB888);

    static const auto daySkyColor   = QColor(0x6EB1FF);
    static const auto nightSkyColor = QColor(0, 0, 0);
    static const auto cloudColor    = QColor(240, 240, 240, 0.8 * 255);

    static const auto clamp = [](qreal val, qreal min, qreal max) { return std::max(min, std::min(val, max)); };

    static const auto interpolateColor = [](const QColor& c1, const QColor& c2, qreal factor)
    {
        factor = clamp(factor, 0.0, 1.0);
        return QColor::fromRgbF(c1.redF() + (c2.redF() - c1.redF()) * factor,
                                c1.greenF() + (c2.greenF() - c1.greenF()) * factor,
                                c1.blueF() + (c2.blueF() - c1.blueF()) * factor,
                                c1.alphaF() + (c2.alphaF() - c1.alphaF()) * factor);
    };

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Sky
    p.fillRect(rect(), interpolateColor(daySkyColor, nightSkyColor, progress()));

    // Twilight
    {
        p.save();
        p.setOpacity(1 - std::abs(clamp((progress() - 0.5) * 4, -1.0, 1.0)));
        QLinearGradient gradient({0, 0}, QPointF(0, height()));
        gradient.setColorAt(0, 0x4F6EA8);
        gradient.setColorAt(1, 0xD6743C);
        p.setPen(Qt::NoPen);
        p.setBrush(gradient);
        p.drawRect(rect());
        p.restore();
    }

    // Stars
    {
        static const QList<std::tuple<qreal, qreal, qreal>> stars = {
            {0.86, 0.42, 15.21},
            {0.79, 0.31, 45.76},
            {0.69, 0.49, 56.10},
            {0.30, 0.21, 13.47},
            {0.61, 0.14, 32.43},
            {0.26, 0.59, 55.43},
            {0.08, 0.62, 41.42},
            {0.24, 0.16, 60.78},
            {0.68, 0.32, 35.89},
            {0.82, 0.15, 9.38},
        };
        const auto yOffset = height() * (1 - clamp(progress() - 0.5, 0.0, 0.5) * 2);
        for (const auto& it : stars) {
            qreal x = std::get<0>(it);
            qreal y = std::get<1>(it);
            qreal r = std::get<2>(it);
            p.save();
            p.translate(width() * x, height() * y + yOffset);
            p.rotate(r);
            p.fillRect(0, 0, 4, 4, 0x7F7F7F7F);
            p.restore();
        }
    }

    // Sun & Moon
    {
        p.save();
        p.translate(Margin + progress() * (width() - Radius * 2 - Margin * 2), Margin);

        // Halo
        for (int i = 0; i < 7; ++i) {
            constexpr auto haloStep = 10;
            p.save();
            p.translate(-haloStep * i, -haloStep * i);
            p.fillRect(0, 0, (Radius + haloStep * i) * 2, (Radius + haloStep * i) * 2, QColor(255, 255, 255, 10));
            p.restore();
        }

        p.drawImage(QRect(0, 0, Radius * 2, Radius * 2), sun);

        p.setOpacity(clamp((progress()) * 2 - 1, 0.0, 1.0));
        p.drawImage(QRect(0, 0, Radius * 2, Radius * 2), moon);

        p.setOpacity(1 - clamp(progress() * 2, 0.0, 1.0));
        p.fillRect(0, 0, Radius * 2, Radius * 2, 0xFFFFFF);

        p.restore();
    }

    // Cloud
    {
        const auto interp = [factor = progress()](const QPointF& p1, const QPointF& p2)
        { return QPointF(p1.x() + (p2.x() - p1.x()) * factor, p1.y() + (p2.y() - p1.y()) * factor); };

        const std::tuple<QPointF, QPointF> cloud[] = {
            {interp({0.4, 0.8}, {0.8, 1.05}), interp({0.8, 0.4}, {0.5, 0.4})},
            {interp({0.6, 0.6}, {1.1, 0.85}), interp({0.3, 0.2}, {0.3, 0.2})},
            {interp({0.0, 0.6}, {0.0, 0.85}), interp({-0.2, 0.5}, {0.6, 0.3})},
        };

        for (const auto& it : cloud) {
            const auto pos  = std::get<0>(it);
            const auto size = std::get<1>(it);
            p.fillRect(pos.x() * width(), pos.y() * height(), size.x() * width(), size.y() * height(), cloudColor);
        }
    }
}
