/**
 * @brief QML-like animation framework for QWidget
 * @details
 * Create a behavior and attach it to a property:
 *      @code{.cpp}
 *      Behavior::on(behavior, MetaObject().property(), Animation*);
 *      Behavior::on(          MetaObject().property(), Animation*);
 *      @endcode
 *
 * Get/set properties and trigger animations:
 *      @code{.cpp}
 *      auto value =
 *      Behavior::get(behavior, obj, prop, default);
 *      Behavior::get(          obj, prop, default);
 *      Behavior::get(behavior, metaProp);
 *      Behavior::get(          metaProp);
 *
 *      Behavior::set(behavior, obj, prop, val);
 *      Behavior::set(          obj, prop, val);
 *      Behavior::set(behavior, metaProp, val);
 *      Behavior::set(          metaProp, val);
 *      @endcode
 *
 * Work with property binding:
 *      @code{.cpp}
 *      bindingExpr.bindTo(
 *          Behavior::animated(
 *              metaProp));
 *      @endcode
 */

#ifndef NWIDGET_BEHAVIOR_H
#define NWIDGET_BEHAVIOR_H

#include <cmath>

#include <QMap>
#include <QObject>

#include "utils.h"

#ifndef N_BEHAVIOR_ANIMATION_FPS
#define N_BEHAVIOR_ANIMATION_FPS 60
#endif

namespace nwidget {

class Animation
{
public:
    // using T = animated property type

    virtual ~Animation() = default;

    virtual const void* start() const   = 0;
    virtual const void* end() const     = 0;
    virtual const void* current() const = 0;

    virtual void setStart(const void* value) = 0;
    virtual void setEnd(const void* value)   = 0;

    virtual bool finished() const = 0;

    virtual const void* tick(int ms) = 0;
};

class Behavior : public QObject
{
    N_DISABLE_COPY_MOVE(Behavior)

public:
    using type_erased_setter = void (*)(void* obj, const void* val);

    template <typename Class, typename T, void (Class::*Setter)(T)> static type_erased_setter erase()
    {
        return [](void* o, const void* v) { (static_cast<Class*>(o)->*Setter)(*static_cast<const T*>(v)); };
    }

    template <typename Class, typename T, void (Class::*Setter)(const T&)> static type_erased_setter erase()
    {
        return [](void* o, const void* v) { (static_cast<Class*>(o)->*Setter)(*static_cast<const T*>(v)); };
    }

    template <typename MetaProp> static type_erased_setter erase()
    {
        return [](void* o, const void* v)
        {
            MetaProp::write(static_cast<typename MetaProp::Class*>(o), *static_cast<const typename MetaProp::Type*>(v));
        };
    }

public:
    static Behavior* create(QObject* parent) { return findOrCreateBehavior(parent); }

    template <typename Anim>
    static void on(Behavior* b, void* obj, type_erased_setter prop, Anim* anim, const typename Anim::Type& startValue)
    {
        Q_ASSERT(b);
        bool oldAnim = false;
        for (auto& it : b->animations) {
            const auto p = std::get<0>(it);
            const auto o = std::get<1>(it);
            auto&      a = std::get<2>(it);
            if (prop == p && obj == o) {
                delete a;
                a       = anim;
                oldAnim = true;
                break;
            }
        }
        if (!oldAnim)
            b->animations.append({prop, obj, anim});
        anim->setStart(&startValue);
        anim->setEnd(&startValue);
    }

    // Behavior::on(behavior, MetaObject().property(), Animation*);
    // Behavior::on(          MetaObject().property(), Animation*);

    template <typename MetaProp, typename Anim> static void on(Behavior* b, MetaProp prop, Anim* anim)
    {
        static_assert(MetaProp::isReadable, "");
        static_assert(MetaProp::isWritable, "");
        static_assert(std::is_base_of<Animation, Anim>::value, "");
        static_assert(std::is_same<typename MetaProp::Type, typename Anim::Type>::value);
        on(b, prop.object(), erase<MetaProp>(), anim, prop.get());
    }

    template <typename MetaProp, typename Anim> static void on(MetaProp prop, Anim* anim)
    {
        static_assert(MetaProp::isReadable, "");
        static_assert(MetaProp::isWritable, "");
        static_assert(std::is_base_of<Animation, Anim>::value, "");
        static_assert(std::is_same<typename MetaProp::Type, typename Anim::Type>::value, "");
        on(findOrCreateBehavior(prop.object()), prop.object(), erase<MetaProp>(), anim, prop.get());
    }

public:
    // Behavior::get(behavior, obj, prop, default);
    // Behavior::get(          obj, prop, default);
    // Behavior::set(behavior, obj, prop, val);
    // Behavior::set(          obj, prop, val);

    template <typename T>
    static T get(const Behavior* behavior, void* obj, type_erased_setter prop, const T& default_ = {})
    {
        if (!behavior)
            return default_;
        for (const auto& it : qAsConst(behavior->animations)) {
            const auto p = std::get<0>(it);
            const auto o = std::get<1>(it);
            const auto a = std::get<2>(it);
            if (prop == p && obj == o)
                return *static_cast<const T*>(a->current());
        }
        return default_;
    }

    template <typename T> static void set(Behavior* behavior, void* obj, type_erased_setter prop, const T& val)
    {
        if (!behavior)
            prop(obj, &val);
        for (const auto& it : impl::as_const(behavior->animations)) {
            const auto p = std::get<0>(it);
            const auto o = std::get<1>(it);
            const auto a = std::get<2>(it);
            if (prop == p && obj == o) {
                a->setEnd(&val);
                return;
            }
        }
        prop(obj, &val);
    }

    template <typename T> static T get(QObject* obj, type_erased_setter prop, const T& default_ = {})
    {
        return get<T>(findBehavior(obj), obj, prop, default_);
    }

    template <typename T> static void set(QObject* obj, type_erased_setter prop, const T& val)
    {
        set(findBehavior(obj), obj, prop, val);
    }

    // Behavior::get(behavior, metaProp);
    // Behavior::get(          metaProp);

    template <typename MetaProp> static auto get(const Behavior* behavior, MetaProp prop)
    {
        return get(findBehavior(prop.object()), prop.object(), erase<MetaProp>(), MetaProp::read(prop.object()));
    }

    template <typename MetaProp> static auto get(MetaProp prop) { return get(findBehavior(prop.object()), prop); }

    // Behavior::set(behavior, metaProp, val);
    // Behavior::set(          metaProp, val);

    template <typename MetaProp> static void set(Behavior* b, MetaProp prop, const typename MetaProp::Type& val)
    {
        return set(b, prop.object(), erase<MetaProp>(), val);
    }

    template <typename MetaProp> static void set(MetaProp prop, const typename MetaProp::Type& val)
    {
        return set(findBehavior(prop.object()), prop, val);
    }

public:
    template <typename MetaProp> static auto animated(Behavior* b, MetaProp p)
    {
        return [b, p](const typename MetaProp::Type& v) { Behavior::set(b, p, v); };
    }

    template <typename MetaProp> static auto animated(MetaProp prop)
    {
        static_assert(std::is_base_of<QObject, typename MetaProp::Class>::value, "");
        return animated(findOrCreateBehavior(prop.object()), prop);
    }

protected:
    void timerEvent(QTimerEvent* event) override
    {
        constexpr int tick = 1000.0 / N_BEHAVIOR_ANIMATION_FPS;

        for (const auto& it : impl::as_const(animations)) {
            const auto prop = std::get<0>(it);
            const auto obj  = std::get<1>(it);
            const auto anim = std::get<2>(it);
            if (anim->finished())
                continue;
            prop(obj, anim->tick(tick));
        }

        QObject::timerEvent(event);
    }

private:
    QList<std::tuple<type_erased_setter, void*, Animation*>> animations;

    explicit Behavior(QObject* target)
        : QObject(target)
    {
        Q_ASSERT(target);

        setObjectName("nwidget::Behavior");
        startTimer(1000.0 / N_BEHAVIOR_ANIMATION_FPS);
    }

    virtual ~Behavior()
    {
        for (const auto& it : impl::as_const(animations))
            delete std::get<2>(it);
    }

    static Behavior* findBehavior(QObject* obj)
    {
        return static_cast<Behavior*>(obj->findChild<QObject*>("nwidget::Behavior", Qt::FindDirectChildrenOnly));
    }

    static Behavior* findOrCreateBehavior(QObject* obj)
    {
        auto b = findBehavior(obj);
        return b ? b : new Behavior(obj);
    }
};

/* ------------------------------------------------ Builtin Animation ----------------------------------------------- */

// clang-format off
struct damping  { qreal value; };
struct duration { qreal value; };
struct epsilon  { qreal value; };
struct mass     { qreal value; };
struct modulus  { qreal value; };
struct spring   { qreal value; };
struct velocity { qreal value; };

/* ------------------------------------------------ SmoothedAnimation ----------------------------------------------- */

struct EasingCurve
{
    struct Linear  { qreal operator()(qreal progress) const { return progress; } };
    struct InQuad  { qreal operator()(qreal progress) const { return progress * progress; } };
    struct OutQuad { qreal operator()(qreal progress) const { return progress * (2 - progress); } };
};

template <typename T> struct Interpolator
{
    T operator()(const T& start, const T& end, qreal progress) const { return start + (end - start) * progress; }
};
// clang-format on

template <typename T, typename E = EasingCurve::Linear> class SmoothedAnimation : public Animation
{
public:
    using Type = T;

    SmoothedAnimation() {}
    explicit SmoothedAnimation(duration v, E e = {})
        : SmoothedAnimation(velocity{1000 / v.value}, e)
    {
    }
    explicit SmoothedAnimation(velocity v, E e = {})
        : velocity_(v.value / 1000)
        , easing_(e)
    {
    }

    const void* start() const override { return &start_; }
    const void* end() const override { return &end_; }
    const void* current() const override { return &current_; }

    void setStart(const void* value) override
    {
        start_    = *static_cast<const T*>(value);
        end_      = start_;
        current_  = start_;
        progress_ = 1;
    }

    void setEnd(const void* value) override
    {
        start_    = current_;
        end_      = *static_cast<const T*>(value);
        progress_ = 0;
    }

    bool finished() const override { return progress_ >= 1; }

    const void* tick(int ms) override
    {
        if (finished())
            return current();
        progress_ += ms * velocity_;
        progress_ = progress_ > 1 ? 1 : progress_;
        current_  = Interpolator<T>{}(start_, end_, easing_(progress_));
        return current();
    }

private:
    qreal velocity_ = 1;
    qreal progress_ = 1;

    E easing_;

    T start_;
    T end_;
    T current_;
};

/* ------------------------------------------------- SpringAnimation ------------------------------------------------ */

template <typename T> class SpringAnimation : public Animation
{
public:
    using Type = T;

    template <typename... Args> explicit SpringAnimation(Args... args) { int _[]{(set(args), 0)...}; };

    qreal damping() const { return damping_; }
    qreal epsilon() const { return epsilon_; }
    qreal mass() const { return mass_; }
    qreal modulus() const { return modulus_; }
    qreal spring() const { return spring_; }
    qreal velocity() const { return maxVelocity_; }

    void setDamping(qreal v) { damping_ = v; }
    void setEpsilon(qreal v) { epsilon_ = v; }
    void setMass(qreal v) { mass_ = v; }
    void setModulus(qreal v) { modulus_ = v; }
    void setSpring(qreal v) { spring_ = v; }
    void setVelocity(qreal v) { maxVelocity_ = v; }

public:
    const void* start() const override { return &start_; }
    const void* end() const override { return &end_; }
    const void* current() const override
    {
        const_cast<T&>(value_) = current_;
        return &value_;
    }

    void setStart(const void* value) override
    {
        start_    = *static_cast<const T*>(value);
        current_  = start_;
        velocity_ = 0;
    }

    void setEnd(const void* value) override { end_ = *static_cast<const T*>(value); }

    const void* tick(int ms) override
    {
        if (finished())
            return current();

        if (modulus_ > 0)
            current_ = fmodf(current_, modulus_);

        const int steps = (ms / (1000 / N_BEHAVIOR_ANIMATION_FPS));
        for (int i = 0; i < steps; ++i) {
            qreal diff = end_ - current_;
            if (modulus_ > 0 && qAbs(diff) > modulus_ / 2)
                diff += diff > 0 ? -modulus_ : modulus_;

            if (mass_ != 1)
                velocity_ += (spring_ * diff - damping_ * velocity_) / mass_;
            else
                velocity_ += spring_ * diff - damping_ * velocity_;

            if (maxVelocity_ > 0)
                velocity_ = qBound(-maxVelocity_, velocity_, maxVelocity_);

            current_ += velocity_ * ms / 1000.0;

            if (modulus_ > 0) {
                current_ = std::fmod(current_, modulus_);
                if (current_ < 0)
                    current_ += modulus_;
            }
        }

        if (qAbs(velocity_) < epsilon_ && qAbs(end_ - current_) < epsilon_) {
            velocity_ = 0;
            current_  = end_;
        }

        return current();
    }

    bool finished() const override { return velocity_ == 0 && current_ == end_; }

private:
    qreal damping_     = 0;
    qreal epsilon_     = 2;
    qreal mass_        = 1.0;
    qreal modulus_     = 0;
    qreal spring_      = 0;
    qreal maxVelocity_ = 0;

    qreal start_    = 0;
    qreal end_      = 0;
    qreal current_  = 0;
    qreal velocity_ = 0;

    T value_;

    void set(::nwidget::damping v) { setDamping(v.value); }
    void set(::nwidget::epsilon v) { setEpsilon(v.value); }
    void set(::nwidget::mass v) { setMass(v.value); }
    void set(::nwidget::modulus v) { setModulus(v.value); }
    void set(::nwidget::spring v) { setSpring(v.value); }
    void set(::nwidget::velocity v) { setVelocity(v.value); }
};

} // namespace nwidget

#endif // NWIDGET_BEHAVIOR_H
