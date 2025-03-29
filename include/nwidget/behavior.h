/**
 * @brief QML-like animation framework for QWidget
 * @details
 * Create a behavior and attach it to a property:
 *      @code{.cpp}
 *      Behavior::on(MetaObject().property()   , new Animation);
 *      Behavior::on(MetaObject().property()   , new Animation, startValue);
 *      Behavior::on(obj, &MetaObject::property, new Animation);
 *      Behavior::on(obj, &MetaObject::property, new Animation, startValue);
 *      @endcode
 *
 * Get/set properties and trigger animations:
 *      @code{.cpp}
 *      auto value = Behavior::get(MetaObject().property());
 *                 = Behavior::get(obj, &MetaObject::property);
 *
 *      Behavior::set(MetaObject().property()   , value);
 *      Behavior::set(obj, &MetaObject::property, value);
 *      @endcode
 *
 * Work with property binding:
 *      @code{.cpp}
 *      bindingExpr.bindTo(
 *          Behavior::animated(
 *              MetaObject().property()));
 *      @endcode
 */

#ifndef NWIDGET_BEHAVIOR_H
#define NWIDGET_BEHAVIOR_H

#include "metaobject.h"

#include <QMap>
#include <QObject>

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
    Q_DISABLE_COPY_MOVE(Behavior)

public:
    template <typename MetaProp, typename Anim> static void on(MetaProp prop, Anim* anim)
    {
        on<MetaProp, Anim>(prop.object(), anim);
    }

    template <typename MetaProp, typename Anim>
    static void on(MetaProp prop, Anim* anim, const typename MetaProp::Type& startValue)
    {
        on<MetaProp, Anim>(prop.object(), anim, startValue);
    }

    template <typename Class, typename MetaProp, typename Anim>
    static void on(typename MetaProp::Class* obj, MetaProp (MetaObject<Class>::*)() const, Anim* anim)
    {
        on<MetaProp, Anim>(obj, anim);
    }

    template <typename Class, typename MetaProp, typename Anim>
    static void on(typename MetaProp::Class*      obj,
                   MetaProp                       (MetaObject<Class>::*)() const,
                   Anim*                          anim,
                   const typename MetaProp::Type& startValue)
    {
        on<MetaProp, Anim>(obj, anim, startValue);
    }

    // getter / setter

    template <typename MetaProp> static auto get(MetaProp prop) { return get<MetaProp>(prop.object()); }

    template <typename Class, typename MetaProp>
    static auto get(const typename MetaProp::Class* obj, MetaProp (MetaObject<Class>::*)() const)
    {
        return get<MetaProp>(obj);
    }

    template <typename MetaProp> static void set(MetaProp prop, const typename MetaProp::Type& val)
    {
        set<MetaProp>(prop.object(), val);
    }

    template <typename Class, typename MetaProp>
    static void set(typename MetaProp::Class* obj, MetaProp (MetaObject<Class>::*)() const, const typename MetaProp::Type& val)
    {
        set<MetaProp>(obj, val);
    }

public:
    template <typename... Ts> static auto animated(MetaProperty<Ts...> prop)
    {
        using MetaProp = MetaProperty<Ts...>;
        using Class    = typename MetaProp::Class;

        struct Getter
        {
            auto operator()(const Class* o) const { return Behavior::get<MetaProp>(o); }
        };

        struct Setter
        {
            void operator()(Class* o, const typename MetaProp::Type& v) const { Behavior::set<MetaProp>(o, v); }
        };

        return MetaProperty<typename MetaProp::Class,
                            typename MetaProp::Info,
                            typename MetaProp::Type,
                            Getter,
                            Setter,
                            typename MetaProp::Notify,
                            typename MetaProp::Reset>(prop.object());
    }

protected:
    void timerEvent(QTimerEvent* event) override
    {
        constexpr int tick = 1000.0 / N_BEHAVIOR_ANIMATION_FPS;
        for (auto it = animations.begin(); it != animations.end(); ++it) {
            auto anim = it.value();
            if (anim->finished())
                continue;
            it.key()(parent(), anim->tick(tick));
        }

        QObject::timerEvent(event);
    }

private:
    using type_erased_getter = void* (*)(QObject* obj);
    using type_erased_setter = void (*)(QObject* obj, const void* val);

    template <typename MetaProp> static type_erased_setter erase()
    {
        static const auto fn = [](QObject* o, const void* v)
        {
            MetaProp::write(static_cast<typename MetaProp::Class*>(o), *static_cast<const typename MetaProp::Type*>(v));
        };
        return fn;
    }

    QMap<type_erased_setter, Animation*> animations;

    explicit Behavior(QObject* target)
        : QObject(target)
    {
        Q_ASSERT(target);

        setObjectName("nwidget::Behavior");
        startTimer(1000.0 / N_BEHAVIOR_ANIMATION_FPS);
    }

    virtual ~Behavior() { qDeleteAll(animations); }

    template <typename MetaProp, typename Anim> static void on(typename MetaProp::Class* obj, Anim* anim)
    {
        static_assert(MetaProp::isReadable);
        on<MetaProp>(obj, anim, MetaProp::read(obj));
    }

    template <typename MetaProp, typename Anim>
    static void on(typename MetaProp::Class* obj, Anim* anim, const typename MetaProp::Type& startValue)
    {
        static_assert(MetaProp::isWritable);

        static_assert(std::is_base_of_v<Animation, Anim>);
        static_assert(std::is_same_v<typename MetaProp::Type, typename Anim::Type>);

        auto p = erase<MetaProp>();
        auto b =
            static_cast<Behavior*>(obj->template findChild<QObject*>("nwidget::Behavior", Qt::FindDirectChildrenOnly));
        if (!b)
            b = new Behavior(obj);
        auto a = b->animations.value(p);
        if (a)
            delete a;
        b->animations[p] = anim;
        anim->setStart(&startValue);
        anim->setEnd(&startValue);
    }

    template <typename MetaProp> static auto get(const typename MetaProp::Class* obj)
    {
        auto b =
            static_cast<Behavior*>(obj->template findChild<QObject*>("nwidget::Behavior", Qt::FindDirectChildrenOnly));
        return b ? b->get<MetaProp>() : MetaProp::read(obj);
    }

    template <typename MetaProp> static void set(typename MetaProp::Class* obj, const typename MetaProp::Type& val)
    {
        auto b =
            static_cast<Behavior*>(obj->template findChild<QObject*>("nwidget::Behavior", Qt::FindDirectChildrenOnly));
        if (b)
            b->set<MetaProp>(val);
        else
            MetaProp::write(obj, val);
    }

    template <typename MetaProp> auto get() const
    {
        auto a = animations.value(erase<MetaProp>());
        return a ? *static_cast<const typename MetaProp::Type*>(a->end())
                 : MetaProp::read(static_cast<const typename MetaProp::Class*>(parent()));
    }

    template <typename MetaProp> void set(const typename MetaProp::Type& val)
    {
        auto a = animations.value(erase<MetaProp>());
        if (a)
            a->setEnd(&val);
        else
            MetaProp::write(static_cast<typename MetaProp::Class*>(parent()), val);
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

    template <typename... Args> explicit SpringAnimation(Args... args) { (set(args), ...); };

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
            current_ = std::fmod(current_, modulus_);

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
