#ifndef NWIDGET_BUILDERS_H
#define NWIDGET_BUILDERS_H

#include "builder.h"
#include "metaobjects.h"

class QWidget;
class QLayout;
class QLayoutItem;

namespace nwidget {

#ifdef QOBJECT_H
template <typename Self> class Builder<QObject, Self> : public Builder<void, Self>
{
    N_BUILDER(QObject)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(objectName)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTERX(property, setProperty, const char*, const QVariant&)
    N_BUILDER_SETTERX(property, setProperty, const char*, QVariant&&)
    N_BUILDER_SETTER1(eventFilter, installEventFilter)
    N_END_BUILDER_SETTER

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onDestroyed, destroyed)
    N_BUILDER_SIGNAL(onObjectNameChanged, objectNameChanged)
    N_END_BUILDER_SIGNAL
};

using Object = Builder<QObject>;
#endif

#ifdef QACTION_H
template <typename Self> class Builder<QAction, Self> : public Builder<QObject, Self>
{
    N_BUILDER(QAction)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(checkable)
    N_BUILDER_PROPERTY(checked)
    N_BUILDER_PROPERTY(enabled)
    N_BUILDER_PROPERTY(icon)
    N_BUILDER_PROPERTY(text)
    N_BUILDER_PROPERTY(iconText)
    N_BUILDER_PROPERTY(toolTip)
    N_BUILDER_PROPERTY(statusTip)
    N_BUILDER_PROPERTY(whatsThis)
    N_BUILDER_PROPERTY(font)
#if QT_CONFIG(shortcut)
    N_BUILDER_PROPERTY(shortcut)
    N_BUILDER_PROPERTY(shortcutContext)
    N_BUILDER_PROPERTY(autoRepeat)
#endif
    N_BUILDER_PROPERTY(visible)
    N_BUILDER_PROPERTY(menuRole)
    N_BUILDER_PROPERTY(iconVisibleInMenu)
    N_BUILDER_PROPERTY(shortcutVisibleInContextMenu)
    N_BUILDER_PROPERTY(priority)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onChanged, changed)
    N_BUILDER_SIGNAL(onEnabledChanged, enabledChanged)
    N_BUILDER_SIGNAL(onCheckableChanged, checkableChanged)
    N_BUILDER_SIGNAL(onVisibleChanged, visibleChanged)
    N_BUILDER_SIGNAL(onTriggered, triggered)
    N_BUILDER_SIGNAL(onHovered, hovered)
    N_BUILDER_SIGNAL(onToggled, toggled)
    N_END_BUILDER_SIGNAL

    explicit Builder(const QString& text) { self().text(text); }
};

using Action = Builder<QAction>;
#endif

#ifdef QLAYOUT_H
template <typename T> class LayoutItem : public BuilderItem<T>
{
public:
    using Layout = T;

    LayoutItem(QWidget* widget)
        : LayoutItem(widget,
                     [](const BuilderItem<T>* item, T* l)
                     { l->addWidget(static_cast<QWidget*>(static_cast<const LayoutItem*>(item)->item)); })
    {
    }

    LayoutItem(QLayoutItem* item)
        : LayoutItem(item,
                     [](const BuilderItem<T>* item, T* l)
                     { l->addItem(static_cast<QLayoutItem*>(static_cast<const LayoutItem*>(item)->item)); })
    {
    }

    template <typename Class>
    LayoutItem(MetaObject<Class> metaobj)
        : LayoutItem(metaobj.object())
    {
    }

    template <typename Class>
    LayoutItem(Builder<Class> builder)
        : LayoutItem(builder.object())
    {
    }

    template <typename Item>
    LayoutItem(BuilderItemGenerator<Item> generator)
        : BuilderItem<T>(generator)
    {
    }

protected:
    void* item;

    explicit LayoutItem(typename BuilderItem<T>::Func f)
        : BuilderItem<T>(f)
        , item(nullptr)
    {
    }

    LayoutItem(void* item, typename BuilderItem<T>::Func f)
        : BuilderItem<T>(f)
        , item(item)
    {
    }
};

template <typename Self> class Builder<QLayout, Self> : public Builder<QObject, Self>
{
    N_BUILDER(QLayout)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(spacing)
    N_BUILDER_PROPERTY(contentsMargins)
    N_BUILDER_PROPERTY(sizeConstraint)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTERX(contentsMargins, setContentsMargins, int, int, int, int)
    N_END_BUILDER_SETTER
};

using Layout = Builder<QLayout>;
#endif

#ifdef QBOXLAYOUT_H
class BoxLayoutItem : public LayoutItem<QBoxLayout>
{
public:
    using LayoutItem::LayoutItem;

    // clang-format off
    BoxLayoutItem(int stretch,                      QWidget* widget) : BoxLayoutItem(stretch, {}, widget) {}
    BoxLayoutItem(int stretch, Qt::Alignment align, QWidget* widget)
        : LayoutItem(widget,
                     [](const BuilderItem<QBoxLayout>* item, QBoxLayout* l)
                     {
                         auto s = static_cast<const BoxLayoutItem*>(item);
                         l->addWidget(static_cast<QWidget*>(s->item), s->v.stretch, s->align);
                     })
        , align(align)
        { v.stretch = stretch; }

    BoxLayoutItem(             QLayout* layout) : BoxLayoutItem(0, layout) {}
    BoxLayoutItem(int stretch, QLayout* layout)
        : LayoutItem(layout,
                     [](const BuilderItem* item, QBoxLayout* l)
                     {
                         auto s = static_cast<const BoxLayoutItem*>(item);
                         l->addLayout(static_cast<QLayout*>(s->item), s->align);
                     })
        { v.stretch = stretch; }

    BoxLayoutItem(QLayoutItem* layout)
        : LayoutItem(layout,
                     [](const BuilderItem* item, QBoxLayout* l)
                     {
                         auto s = static_cast<const BoxLayoutItem*>(item);
                         l->addItem(static_cast<QLayout*>(s->item));
                     }) {}

    enum class spacing { tag };
    enum class stretch { tag };
    enum class strut   { tag };

    BoxLayoutItem(spacing, int val)
        : LayoutItem([](const BuilderItem* item, QBoxLayout* l)
                     { l->addSpacing(static_cast<const BoxLayoutItem*>(item)->v.spacing); })
    { v.spacing = val; }

    BoxLayoutItem(stretch, int val = 0)
        : LayoutItem([](const BuilderItem* item, QBoxLayout* l)
                     { l->addStretch(static_cast<const BoxLayoutItem*>(item)->v.stretch); })
    { v.stretch = val; }

    BoxLayoutItem(strut, int val)
        : LayoutItem([](const BuilderItem* item, QBoxLayout* l)
                     { l->addStrut(static_cast<const BoxLayoutItem*>(item)->v.strut); })
    { v.strut = val; }
    // clang-format on

private:
    union {
        int stretch;
        int spacing;
        int strut;
    } v;
    Qt::Alignment align;
};

template <typename Self> class Builder<QBoxLayout, Self> : public Builder<QLayout, Self>
{
    N_BUILDER(QBoxLayout)

    // clang-format off
    static auto Spacing(int v    ) { return BoxLayoutItem(BoxLayoutItem::spacing::tag, v); }
    static auto Stretch(int v = 0) { return BoxLayoutItem(BoxLayoutItem::stretch::tag, v); }
    static auto Strut  (int v    ) { return BoxLayoutItem(BoxLayoutItem::strut  ::tag, v); }
    // clang-format on
};

template <typename Self> class Builder<QHBoxLayout, Self> : public Builder<QBoxLayout, Self>
{
    N_BUILDER(QHBoxLayout)

    Builder(std::initializer_list<BoxLayoutItem> items) { self().addItems(items); }
};

template <typename Self> class Builder<QVBoxLayout, Self> : public Builder<QBoxLayout, Self>
{
    N_BUILDER(QVBoxLayout)

    Builder(std::initializer_list<BoxLayoutItem> items) { self().addItems(items); }
};

using BoxLayout  = Builder<QBoxLayout>;
using HBoxLayout = Builder<QHBoxLayout>;
using VBoxLayout = Builder<QVBoxLayout>;
#endif

#ifdef QFORMLAYOUT_H
class FormLayoutItem : public LayoutItem<QFormLayout>
{
public:
    using LayoutItem::LayoutItem;

    // clang-format off
    FormLayoutItem(const QString& label, QWidget* field) : LayoutItem([](const BuilderItem* item, Layout* layout){ auto s = static_cast<const FormLayoutItem*>(item); layout->addRow(s->text , static_cast<QWidget*>(s->field)); }), text( label), field(field) {}
    FormLayoutItem(const QString& label, QLayout* field) : LayoutItem([](const BuilderItem* item, Layout* layout){ auto s = static_cast<const FormLayoutItem*>(item); layout->addRow(s->text , static_cast<QLayout*>(s->field)); }), text( label), field(field) {}
    FormLayoutItem(QWidget* label      , QWidget* field) : LayoutItem([](const BuilderItem* item, Layout* layout){ auto s = static_cast<const FormLayoutItem*>(item); layout->addRow(s->label, static_cast<QWidget*>(s->field)); }), label(label), field(field) {}
    FormLayoutItem(QWidget* label      , QLayout* field) : LayoutItem([](const BuilderItem* item, Layout* layout){ auto s = static_cast<const FormLayoutItem*>(item); layout->addRow(s->label, static_cast<QLayout*>(s->field)); }), label(label), field(field) {}
    // clang-format on

private:
    QString  text;
    QWidget* label = nullptr;
    void*    field = nullptr;
};

template <typename Self> class Builder<QFormLayout, Self> : public Builder<QLayout, Self>
{
    N_BUILDER(QFormLayout)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(fieldGrowthPolicy)
    N_BUILDER_PROPERTY(rowWrapPolicy)
    N_BUILDER_PROPERTY(labelAlignment)
    N_BUILDER_PROPERTY(formAlignment)
    N_BUILDER_PROPERTY(horizontalSpacing)
    N_BUILDER_PROPERTY(verticalSpacing)
    N_END_BUILDER_PROPERTY

    Builder(std::initializer_list<FormLayoutItem> items) { self().addItems(items); }
};

using FormLayout = Builder<QFormLayout>;
#endif

#ifdef QGRIDLAYOUT_H
class GridLayoutItem : public LayoutItem<QGridLayout>
{
public:
    using LayoutItem::LayoutItem;

    // clang-format off
    GridLayoutItem(int row, int col,                                                QWidget* widget  ) : GridLayoutItem(row, col, 1      , 1      , {}   , widget) {}
    GridLayoutItem(int row, int col,                                                QLayoutItem* item) : GridLayoutItem(row, col, 1      , 1      , {}   , item  ) {}
    GridLayoutItem(int row, int col, int rowSpan, int colSpan,                      QWidget* widget  ) : GridLayoutItem(row, col, rowSpan, colSpan, {}   , widget) {}
    GridLayoutItem(int row, int col, int rowSpan, int colSpan,                      QLayoutItem* item) : GridLayoutItem(row, col, rowSpan, colSpan, {}   , item  ) {}
    GridLayoutItem(int row, int col,                           Qt::Alignment align, QWidget* widget  ) : GridLayoutItem(row, col, 1      , 1      , align, widget) {}
    GridLayoutItem(int row, int col,                           Qt::Alignment align, QLayoutItem* item) : GridLayoutItem(row, col, 1      , 1      , align, item  ) {}
    GridLayoutItem(int row, int col, int rowSpan, int colSpan, Qt::Alignment align, QWidget* widget  )
        : LayoutItem(widget,
                     [](const BuilderItem* item, Layout* l){ auto s = static_cast<const GridLayoutItem*>(item); l->addWidget(static_cast<QWidget    *>(s->item), s->row, s->col, s->rowSpan, s->colSpan, s->align); })
        , row(row), col(col), rowSpan(rowSpan), colSpan(colSpan), align(align) {}
    GridLayoutItem(int row, int col, int rowSpan, int colSpan, Qt::Alignment align, QLayoutItem* item)
        : LayoutItem(item,
                     [](const BuilderItem* item, Layout* l){ auto s = static_cast<const GridLayoutItem*>(item); l->addItem(static_cast<::QLayoutItem*>(s->item), s->row, s->col, s->rowSpan, s->colSpan, s->align); })
        , row(row), col(col), rowSpan(rowSpan), colSpan(colSpan), align(align) {}

    // clang-format on

private:
    int           row;
    int           col;
    int           rowSpan;
    int           colSpan;
    Qt::Alignment align;
};

template <typename Self> class Builder<QGridLayout, Self> : public Builder<QLayout, Self>
{
    N_BUILDER(QGridLayout)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(horizontalSpacing)
    N_BUILDER_PROPERTY(verticalSpacing)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER2(rowStretch, setRowStretch)
    N_BUILDER_SETTER2(columnStretch, setColumnStretch)
    N_BUILDER_SETTER2(rowMinimumHeight, setRowMinimumHeight)
    N_BUILDER_SETTER2(columnMinimumWidth, setColumnMinimumWidth)
    N_BUILDER_SETTER1(originCorner, setOriginCorner)
    N_BUILDER_SETTER2(defaultPositioning, setDefaultPositioning)
    N_END_BUILDER_SETTER

    Builder(std::initializer_list<GridLayoutItem> items) { self().addItems(items); }
};

using GridLayout = Builder<QGridLayout>;
#endif

#ifdef QSTACKEDLAYOUT_H
template <typename Self> class Builder<QStackedLayout, Self> : public Builder<QLayout, Self>
{
    N_BUILDER(QStackedLayout)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(currentIndex)
    N_BUILDER_PROPERTY(stackingMode)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER1(currentWidget, setCurrentWidget)
    N_END_BUILDER_SETTER
};

using StackedLayout = Builder<QStackedLayout>;
#endif

#ifdef QWIDGET_H
template <typename Self> class Builder<QWidget, Self> : public Builder<QObject, Self>
{
    N_BUILDER(QWidget)

    // property
    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(windowModality)
    N_BUILDER_PROPERTY(enabled)
    N_BUILDER_PROPERTY(geometry)
    N_BUILDER_PROPERTY(pos)
    N_BUILDER_PROPERTY(size)
    N_BUILDER_PROPERTY(sizePolicy)
    N_BUILDER_PROPERTY(minimumSize)
    N_BUILDER_PROPERTY(maximumSize)
    N_BUILDER_PROPERTY(minimumWidth)
    N_BUILDER_PROPERTY(minimumHeight)
    N_BUILDER_PROPERTY(maximumWidth)
    N_BUILDER_PROPERTY(maximumHeight)
    N_BUILDER_PROPERTY(sizeIncrement)
    N_BUILDER_PROPERTY(baseSize)
    N_BUILDER_PROPERTY(alette)
    N_BUILDER_PROPERTY(font)
#ifndef QT_NO_CURSOR
    N_BUILDER_PROPERTY(cursor)
#endif
    N_BUILDER_PROPERTY(mouseTracking)
    N_BUILDER_PROPERTY(tabletTracking)
    N_BUILDER_PROPERTY(focusPolicy)
    N_BUILDER_PROPERTY(contextMenuPolicy)
    N_BUILDER_PROPERTY(updatesEnabled)
    N_BUILDER_PROPERTY(visible)
    N_BUILDER_PROPERTY(acceptDrops)
    N_BUILDER_PROPERTY(windowTitle)
    N_BUILDER_PROPERTY(windowIcon)
    N_BUILDER_PROPERTY(windowIconText)
    N_BUILDER_PROPERTY(windowOpacity)
    N_BUILDER_PROPERTY(windowModified)
#if QT_CONFIG(tooltip)
    N_BUILDER_PROPERTY(toolTip)
    N_BUILDER_PROPERTY(toolTipDuration)
#endif
#if QT_CONFIG(statustip)
    N_BUILDER_PROPERTY(statusTip)
#endif
#if QT_CONFIG(whatsthis)
    N_BUILDER_PROPERTY(whatsThis)
#endif
#if QT_CONFIG(accessibility)
    N_BUILDER_PROPERTY(accessibleName)
    N_BUILDER_PROPERTY(accessibleDescription)
#endif
    N_BUILDER_PROPERTY(layoutDirection)
    N_BUILDER_PROPERTY(windowFlags)
    N_BUILDER_PROPERTY(autoFillBackground)
#ifndef QT_NO_STYLE_STYLESHEET
    N_BUILDER_PROPERTY(styleSheet)
#endif
    N_BUILDER_PROPERTY(locale)
    N_BUILDER_PROPERTY(windowFilePath)
    N_BUILDER_PROPERTY(inputMethodHints)
    N_END_BUILDER_PROPERTY

    // setter
    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER1(layout, setLayout)
    N_BUILDER_SETTER1(style, setStyle)
    N_BUILDER_SETTERX(minimumSize, setMinimumSize, int, int)
    N_BUILDER_SETTERX(maximumSize, setMaximumSize, int, int)
    N_BUILDER_SETTERX(sizeIncrement, setSizeIncrement, int, int)
    N_BUILDER_SETTERX(baseSize, setBaseSize, int, int)
    N_BUILDER_SETTERX(fixedSize, setFixedSize, const QMargins&)
    N_BUILDER_SETTERX(fixedSize, setFixedSize, int, int)
    N_BUILDER_SETTER1(fixedWidth, setFixedWidth)
    N_BUILDER_SETTER1(fixedHeight, setFixedHeight)
    N_BUILDER_SETTER1(palette, setPalette)
    N_BUILDER_SETTER1(backgroundRole, setBackgroundRole)
    N_BUILDER_SETTER1(foregroundRole, setForegroundRole)
    N_BUILDER_SETTER1(mouseTracking, setMouseTracking)
    N_BUILDER_SETTER1(tabletTracking, setTabletTracking)
    N_BUILDER_SETTERX(mask, setMask, const QBitmap&)
    N_BUILDER_SETTERX(mask, setMask, const QRegion&)
    N_BUILDER_SETTERX(focus, setFocus)
    N_BUILDER_SETTERX(focus, setFocus, Qt::FocusReason)
    N_BUILDER_SETTER1(contextMenuPolicy, setContextMenuPolicy)
    N_BUILDER_SETTER1(updateEnabled, setUpdatesEnabled)
    N_BUILDER_SETTER1(focusProxy, setFocusProxy)
    N_BUILDER_SETTERX(geometry, setGeometry, int, int, int, int)
    N_BUILDER_SETTER1(windowState, setWindowState)
    N_BUILDER_SETTERX(sizePolicy, setSizePolicy, QSizePolicy::Policy, QSizePolicy::Policy)
    N_BUILDER_SETTERX(contentsMargins, setContentsMargins, const QMargins&)
    N_BUILDER_SETTERX(contentsMargins, setContentsMargins, int, int, int, int)
#if QT_CONFIG(graphicseffect)
    N_BUILDER_SETTER1(graphicsEffect, setGraphicsEffect)
#endif
#if QT_CONFIG(accessibility)
    N_BUILDER_SETTER1(asccessibleName, setAccessibleName)
    N_BUILDER_SETTER1(asccessibleDescription, setAccessibleDescription)
#endif
    N_END_BUILDER_SETTER

    // signal
    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onWindowTitleChanged, windowTitleChanged)
    N_BUILDER_SIGNAL(onWindowIconChanged, windowIconChanged)
    N_BUILDER_SIGNAL(onWindowIconTextChanged, windowIconTextChanged)
    N_BUILDER_SIGNAL(onCustomContextMenuRequested, customContextMenuRequested)
    N_END_BUILDER_SIGNAL

    // constructor
    explicit Builder(QLayout* l) { self().layout(l); }
};

using Widget = Builder<QWidget>;
#endif

#ifdef QABSTRACTBUTTON_H
template <typename Self> class Builder<QAbstractButton, Self> : public Builder<QWidget, Self>
{
    N_BUILDER(QAbstractButton)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(text)
    N_BUILDER_PROPERTY(icon)
    N_BUILDER_PROPERTY(iconSize)
#ifndef QT_NO_SHORTCUT
    N_BUILDER_PROPERTY(shortcut)
#endif
    N_BUILDER_PROPERTY(checkable)
    N_BUILDER_PROPERTY(checked)
    N_BUILDER_PROPERTY(autoRepeat)
    N_BUILDER_PROPERTY(autoExclusive)
    N_BUILDER_PROPERTY(autoRepeatDelay)
    N_BUILDER_PROPERTY(autoRepeatInterval)
    N_BUILDER_PROPERTY(down)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onClicked, clicked)
    N_BUILDER_SIGNAL(onPressed, pressed)
    N_BUILDER_SIGNAL(onReleased, released)
    N_BUILDER_SIGNAL(onToggled, toggled)
    N_END_BUILDER_SIGNAL
};

using AbstractButton = Builder<QAbstractButton>;
#endif

#ifdef QCHECKBOX_H
template <typename Self> class Builder<QCheckBox, Self> : public Builder<QAbstractButton, Self>
{
    N_BUILDER(QCheckBox)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(tristate)
    N_BUILDER_PROPERTY(checkState)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SIGNAL
    N_SINCE(6, 7, N_BUILDER_SIGNAL(onCheckStateChanged, checkStateChanged))
    N_UNTIL(6, 9, N_BUILDER_SIGNAL(onStateChanged, stateChanged))
    N_END_BUILDER_SIGNAL

    explicit Builder(const QString& text) { self().text(text); }
};

using CheckBox = Builder<QCheckBox>;
#endif

#ifdef QDIALOGBUTTONBOX_H
template <typename Self> class Builder<QDialogButtonBox, Self> : public Builder<QWidget, Self>
{
    N_BUILDER(QDialogButtonBox)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(orientation)
    N_BUILDER_PROPERTY(standardButtons)
    N_BUILDER_PROPERTY(centerButtons)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onClicked, clicked)
    N_BUILDER_SIGNAL(onAccepted, accepted)
    N_BUILDER_SIGNAL(onHelpRequested, helpRequested)
    N_BUILDER_SIGNAL(onRejected, rejected)
    N_END_BUILDER_SIGNAL

    explicit Builder(QDialogButtonBox::StandardButtons buttons) { self().standardButtons(buttons); }
};

using DialogButtonBox = Builder<QDialogButtonBox>;
#endif

#ifdef QPUSHBUTTON_H
template <typename Self> class Builder<QPushButton, Self> : public Builder<QAbstractButton, Self>
{
    N_BUILDER(QPushButton)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(autoDefault)
    N_BUILDER_PROPERTY(default_)
    N_BUILDER_PROPERTY(flat)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER1(menu, setMenu)
    N_END_BUILDER_SETTER

    explicit Builder(const QString& text) { self().text(text); }
};

using PushButton = Builder<QPushButton>;
#endif

#ifdef QCOMMANDLINKBUTTON_H
template <typename Self> class Builder<QCommandLinkButton, Self> : public Builder<QPushButton, Self>
{
    N_BUILDER(QCommandLinkButton)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(description)
    N_END_BUILDER_PROPERTY

    explicit Builder(const QString& text, const QString& desc = {}) { self().text(text).description(desc); }
};

using CommandLinkButton = Builder<QCommandLinkButton>;
#endif

#ifdef QRADIOBUTTON_H
template <typename Self> class Builder<QRadioButton, Self> : public Builder<QAbstractButton, Self>
{
    N_BUILDER(QRadioButton)

    explicit Builder(const QString& text) { self().text(text); }
};

using RadioButton = Builder<QRadioButton>;
#endif

#ifdef QTOOLBUTTON_H
template <typename Self> class Builder<QToolButton, Self> : public Builder<QAbstractButton, Self>
{
    N_BUILDER(QToolButton)

    N_BEGIN_BUILDER_PROPERTY
#if QT_CONFIG(menu)
    N_BUILDER_PROPERTY(popupMode)
#endif
    N_BUILDER_PROPERTY(toolButtonStyle)
    N_BUILDER_PROPERTY(autoRaise)
    N_BUILDER_PROPERTY(arrowType)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER1(menu, setMenu)
    N_END_BUILDER_SETTER
};

using ToolButton = Builder<QToolButton>;
#endif

#ifdef QFRAME_H
template <typename Self> class Builder<QFrame, Self> : public Builder<QWidget, Self>
{
    N_BUILDER(QFrame)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(frameShape)
    N_BUILDER_PROPERTY(frameShadow)
    N_BUILDER_PROPERTY(lineWidth)
    N_BUILDER_PROPERTY(midLineWidth)
    N_BUILDER_PROPERTY(frameRect)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER1(frameStyle, setFrameStyle)
    N_END_BUILDER_SETTER
};
#endif

#ifdef QABSTRACTSCROLLAREA_H
template <typename Self> class Builder<QAbstractScrollArea, Self> : public Builder<QFrame, Self>
{
    N_BUILDER(QAbstractScrollArea)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(verticalScrollBarPolicy)
    N_BUILDER_PROPERTY(horizontalScrollBarPolicy)
    N_BUILDER_PROPERTY(sizeAdjustPolicy)
    N_END_BUILDER_PROPERTY
};

using AbstractScrollArea = Builder<QAbstractScrollArea>;
#endif

#ifdef QABSTRACTITEMVIEW_H
template <typename Self> class Builder<QAbstractItemView, Self> : public Builder<QAbstractScrollArea, Self>
{
    N_BUILDER(QAbstractItemView)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(autoScroll)
    N_BUILDER_PROPERTY(autoScrollMargin)
    N_BUILDER_PROPERTY(editTriggers)
    N_BUILDER_PROPERTY(tabKeyNavigation)
#if QT_CONFIG(draganddrop)
    N_BUILDER_PROPERTY(showDropIndicator)
    N_BUILDER_PROPERTY(dragEnabled)
    N_BUILDER_PROPERTY(dragDropOverwriteMode)
    N_BUILDER_PROPERTY(dragDropMode)
    N_BUILDER_PROPERTY(defaultDropAction)
#endif
    N_BUILDER_PROPERTY(alternatingRowColors)
    N_BUILDER_PROPERTY(selectionMode)
    N_BUILDER_PROPERTY(selectionBehavior)
    N_BUILDER_PROPERTY(iconSize)
    N_BUILDER_PROPERTY(textElideMode)
    N_BUILDER_PROPERTY(verticalScrollMode)
    N_BUILDER_PROPERTY(horizontalScrollMode)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER1(model, setModel)
    N_END_BUILDER_SETTER
};

using AbstractItemView = Builder<QAbstractItemView>;
#endif

#ifdef QHEADERVIEW_H
template <typename Self> class Builder<QHeaderView, Self> : public Builder<QAbstractItemView, Self>
{
    N_BUILDER(QHeaderView)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(firstSectionMovable)
    N_BUILDER_PROPERTY(showSortIndicator)
    N_BUILDER_PROPERTY(highlightSections)
    N_BUILDER_PROPERTY(stretchLastSection)
    N_BUILDER_PROPERTY(defaultSectionSize)
    N_BUILDER_PROPERTY(minimumSectionSize)
    N_BUILDER_PROPERTY(maximumSectionSize)
    N_BUILDER_PROPERTY(defaultAlignment)
    N_BUILDER_PROPERTY(sortIndicatorClearable)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER1(hideSection, hideSection)
    N_BUILDER_SETTER1(showSection, showSection)
    N_BUILDER_SETTER1(sectionsMovable, setSectionsMovable)
    N_BUILDER_SETTER1(sectionsClickable, setSectionsClickable)
    N_BUILDER_SETTERX(sectionResizeMode, setSectionResizeMode, QHeaderView::ResizeMode)
    N_BUILDER_SETTERX(sectionResizeMode, setSectionResizeMode, int, QHeaderView::ResizeMode)
    N_BUILDER_SETTER1(resizeContentsPrecision, setResizeContentsPrecision)
    N_BUILDER_SETTER1(sortIndicatorShown, setSortIndicatorShown)
    N_BUILDER_SETTER2(sortIndicator, setSortIndicator)
    N_BUILDER_SETTER1(cascadingSectionResizes, setCascadingSectionResizes)
    N_BUILDER_SETTER1(offset, setOffset)
    N_BUILDER_SETTER1(offsetToSectionPosition, setOffsetToSectionPosition)
    N_BUILDER_SETTER0(offsetToLastSection, setOffsetToLastSection)
    N_END_BUILDER_SETTER

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onSectionMoved, sectionMoved)
    N_BUILDER_SIGNAL(onSectionResized, sectionResized)
    N_BUILDER_SIGNAL(onSectionPressed, sectionPressed)
    N_BUILDER_SIGNAL(onSectionClicked, sectionClicked)
    N_BUILDER_SIGNAL(onSectionEntered, sectionEntered)
    N_BUILDER_SIGNAL(onSectionDoubleClicked, sectionDoubleClicked)
    N_BUILDER_SIGNAL(onSectionCountChanged, sectionCountChanged)
    N_BUILDER_SIGNAL(onSectionHandleDoubleClicked, sectionHandleDoubleClicked)
    N_BUILDER_SIGNAL(onGeometriesChanged, geometriesChanged)
    N_BUILDER_SIGNAL(onSortIndicatorChanged, sortIndicatorChanged)
    N_BUILDER_SIGNAL(onSortIndicatorClearableChanged, sortIndicatorClearableChanged)
    N_END_BUILDER_SIGNAL
};

using HeaderView = Builder<QHeaderView>;
#endif

#ifdef QLISTVIEW_H
template <typename Self> class Builder<QListView, Self> : public Builder<QAbstractItemView, Self>
{
    N_BUILDER(QListView)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(movement)
    N_BUILDER_PROPERTY(flow)
    N_BUILDER_PROPERTY(isWrapping)
    N_BUILDER_PROPERTY(resizeMode)
    N_BUILDER_PROPERTY(layoutMode)
    N_BUILDER_PROPERTY(spacing)
    N_BUILDER_PROPERTY(gridSize)
    N_BUILDER_PROPERTY(viewMode)
    N_BUILDER_PROPERTY(modelColumn)
    N_BUILDER_PROPERTY(uniformItemSizes)
    N_BUILDER_PROPERTY(batchSize)
    N_BUILDER_PROPERTY(wordWrap)
    N_BUILDER_PROPERTY(selectionRectVisible)
    N_BUILDER_PROPERTY(itemAlignment)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER1(wrapping, setWrapping)
    N_BUILDER_SETTER2(rowHidden, setRowHidden)
    N_END_BUILDER_SETTER

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onIndexsMoved, indexesMoved)
    N_END_BUILDER_SIGNAL
};

using ListView = Builder<QListView>;
#endif

#ifdef QLISTWIDGET_H
template <typename Self> class Builder<QListWidget, Self> : public Builder<QListView, Self>
{
    N_BUILDER(QListWidget)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(currentRow)
    N_BUILDER_PROPERTY(sortingEnabled)
    N_END_BUILDER_PROPERTY
};

using ListWidget = Builder<QListWidget>;
#endif

#ifdef QTABLEVIEW_H
template <typename Self> class Builder<QTableView, Self> : public Builder<QAbstractItemView, Self>
{
    N_BUILDER(QTableView)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(showGrid)
    N_BUILDER_PROPERTY(gridStyle)
    N_BUILDER_PROPERTY(sortingEnabled)
    N_BUILDER_PROPERTY(wordWrap)
#if QT_CONFIG(abstractbutton)
    N_BUILDER_PROPERTY(cornerButtonEnabled)
#endif
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER1(horizontalHeader, setHorizontalHeader)
    N_BUILDER_SETTER1(verticalHeader, setVerticalHeader)
    N_BUILDER_SETTER2(rowHeight, setRowHeight)
    N_BUILDER_SETTER2(columnWidth, setColumnWidth)
    N_BUILDER_SETTER2(rowHidden, setRowHidden)
    N_BUILDER_SETTER2(columnHidden, setColumnHidden)
    N_BUILDER_SETTER4(span, setSpan)
    N_END_BUILDER_SETTER
};

using TableView = Builder<QTableView>;
#endif

#ifdef QTABLEWIDGET_H
template <typename Self> class Builder<QTableWidget, Self> : public Builder<QAbstractItemView, Self>
{
    N_BUILDER(QTableWidget)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(rowCount)
    N_BUILDER_PROPERTY(columnCount)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER3(item, setItem)
    N_BUILDER_SETTER2(verticalHeaderItem, setVerticalHeaderItem)
    N_BUILDER_SETTER2(horizontalHeaderItem, setHorizontalHeaderItem)
    N_BUILDER_SETTER1(verticalHeaderLabels, setVerticalHeaderLabels)
    N_BUILDER_SETTER1(horizontalHeaderLabels, setHorizontalHeaderLabels)
    N_BUILDER_SETTERX(currentItem, setCurrentItem, QTableWidgetItem*)
    N_BUILDER_SETTERX(currentItem, setCurrentItem, QTableWidgetItem*, QItemSelectionModel::SelectionFlags)
    N_BUILDER_SETTERX(currentCell, setCurrentCell, int, int)
    N_BUILDER_SETTERX(currentCell, setCurrentCell, int, int, QItemSelectionModel::SelectionFlags)
    N_BUILDER_SETTER1(sortingEnabled, setSortingEnabled)
    N_BUILDER_SETTER3(cellWidget, setCellWidget)
    N_BUILDER_SETTER2(rangeSelected, setRangeSelected)
    N_BUILDER_SETTER1(itemPrototype, setItemPrototype)
    N_END_BUILDER_SETTER

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onItemPressed, itemPressed)
    N_BUILDER_SIGNAL(onItemClicked, itemClicked)
    N_BUILDER_SIGNAL(onItemDoubleClicked, itemDoubleClicked)
    N_BUILDER_SIGNAL(onItemActivated, itemActivated)
    N_BUILDER_SIGNAL(onItemEntered, itemEntered)
    N_BUILDER_SIGNAL(onItemChanged, itemChanged)
    N_BUILDER_SIGNAL(onCurrentItemChanged, currentItemChanged)
    N_BUILDER_SIGNAL(onItemSelectionChanged, itemSelectionChanged)
    N_BUILDER_SIGNAL(onCellPressed, cellPressed)
    N_BUILDER_SIGNAL(onCellClicked, cellClicked)
    N_BUILDER_SIGNAL(onCellDoubleClicked, cellDoubleClicked)
    N_BUILDER_SIGNAL(onCellActivated, cellActivated)
    N_BUILDER_SIGNAL(onCellEntered, cellEntered)
    N_BUILDER_SIGNAL(onCellChanged, cellChanged)
    N_END_BUILDER_SIGNAL
};

using TableWidget = Builder<QTableWidget>;
#endif

#ifdef QTREEVIEW_H
template <typename Self> class Builder<QTreeView, Self> : public Builder<QAbstractItemView, Self>
{
    N_BUILDER(QTreeView)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(autoExpandDelay)
    N_BUILDER_PROPERTY(indentation)
    N_BUILDER_PROPERTY(rootIsDecorated)
    N_BUILDER_PROPERTY(uniformRowHeights)
    N_BUILDER_PROPERTY(itemsExpandable)
    N_BUILDER_PROPERTY(sortingEnabled)
    N_BUILDER_PROPERTY(animated)
    N_BUILDER_PROPERTY(allColumnsShowFocus)
    N_BUILDER_PROPERTY(wordWrap)
    N_BUILDER_PROPERTY(headerHidden)
    N_BUILDER_PROPERTY(expandsOnDoubleClick)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER1(header, setHeader)
    N_BUILDER_SETTER2(columnWidth, setColumnWidth)
    N_BUILDER_SETTER2(columnHidden, setColumnHidden)
    N_BUILDER_SETTER1(headerHidden, setHeaderHidden)
    N_BUILDER_SETTER3(rowHidden, setRowHidden)
    N_BUILDER_SETTER3(firstColumnSpanned, setFirstColumnSpanned)
    N_BUILDER_SETTER2(expanded, setExpanded)
    N_BUILDER_SETTER1(treePosition, setTreePosition)
    N_END_BUILDER_SETTER

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onExpanded, expanded)
    N_BUILDER_SIGNAL(onCollapsed, collapsed)
    N_END_BUILDER_SIGNAL
};

using TreeView = Builder<QTreeView>;
#endif

#ifdef QTREEWIDGET_H
template <typename Self> class Builder<QTreeWidget, Self> : public Builder<QAbstractItemView, Self>
{
    N_BUILDER(QTreeWidget)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(columnCount)
};

using TreeWidget = Builder<QTreeWidget>;
#endif

#ifdef QPLAINTEXTEDIT_H
template <typename Self> class Builder<QPlainTextEdit, Self> : public Builder<QAbstractScrollArea, Self>
{
    N_BUILDER(QPlainTextEdit)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(tabChangesFocus)
    N_BUILDER_PROPERTY(documentTitle)
    N_BUILDER_PROPERTY(undoRedoEnabled)
    N_BUILDER_PROPERTY(lineWrapMode)
    N_BUILDER_PROPERTY(wordWrapMode)
    N_BUILDER_PROPERTY(readOnly)
    N_BUILDER_PROPERTY(plainText)
    N_BUILDER_PROPERTY(overwriteMode)
    N_BUILDER_PROPERTY(tabStopDistance)
    N_BUILDER_PROPERTY(cursorWidth)
    N_BUILDER_PROPERTY(textInteractionFlags)
    N_BUILDER_PROPERTY(maximumBlockCount)
    N_BUILDER_PROPERTY(backgroundVisible)
    N_BUILDER_PROPERTY(centerOnScroll)
    N_BUILDER_PROPERTY(placeholderText)
    N_END_BUILDER_PROPERTY

    explicit Builder(const QString& text) { self().plainText(text); }
};

using PlainTextEdit = Builder<QPlainTextEdit>;
#endif

#ifdef QTEXTEDIT_H
template <typename Self> class Builder<QTextEdit, Self> : public Builder<QAbstractScrollArea, Self>
{
    N_BUILDER(QTextEdit)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(autoFormatting)
    N_BUILDER_PROPERTY(tabChangesFocus)
    N_BUILDER_PROPERTY(documentTitle)
    N_BUILDER_PROPERTY(undoRedoEnabled)
    N_BUILDER_PROPERTY(lineWrapMode)
    N_BUILDER_PROPERTY(wordWrapMode)
    N_BUILDER_PROPERTY(lineWrapColumnOrWidth)
    N_BUILDER_PROPERTY(readOnly)
    N_BUILDER_PROPERTY(markdown)
    N_BUILDER_PROPERTY(html)
    N_BUILDER_PROPERTY(plainText)
    N_BUILDER_PROPERTY(overwriteMode)
    N_BUILDER_PROPERTY(tabStopDistance)
    N_BUILDER_PROPERTY(acceptRichText)
    N_BUILDER_PROPERTY(cursorWidth)
    N_BUILDER_PROPERTY(textInteractionFlags)
    N_BUILDER_PROPERTY(document)
    N_BUILDER_PROPERTY(placeholderText)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onTextChanged, textChanged)
    N_BUILDER_SIGNAL(onUndoAvailable, undoAvailable)
    N_BUILDER_SIGNAL(onRedoAvailable, redoAvailable)
    N_BUILDER_SIGNAL(onCurrentCharFormatChanged, currentCharFormatChanged)
    N_BUILDER_SIGNAL(onCopyAvailable, copyAvailable)
    N_BUILDER_SIGNAL(onSelectionChanged, selectionChanged)
    N_BUILDER_SIGNAL(onCursorPositionChanged, cursorPositionChanged)
    N_END_BUILDER_SIGNAL

    explicit Builder(const QString& text) { self().plainText(text); }
};

using TextEdit = Builder<QTextEdit>;
#endif

#ifdef QTEXTBROWSER_H
template <typename Self> class Builder<QTextBrowser, Self> : public Builder<QTextEdit, Self>
{
    N_BUILDER(QTextBrowser)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(source)
    N_BUILDER_PROPERTY(searchPaths)
    N_BUILDER_PROPERTY(openExternalLinks)
    N_BUILDER_PROPERTY(openLinks)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onBackwardAvailable, backwardAvailable)
    N_BUILDER_SIGNAL(onForwardAvailable, forwardAvailable)
    N_BUILDER_SIGNAL(onHistoryChanged, historyChanged)
    N_BUILDER_SIGNAL(onSourceChanged, sourceChanged)
    N_BUILDER_SIGNAL(onHighlighted, highlighted)
    N_BUILDER_SIGNAL(onAnchorClicked, anchorClicked)
    N_END_BUILDER_SIGNAL
};

using TextBrowser = Builder<QTextBrowser>;
#endif

#ifdef QTOOLBOX_H
class ToolBoxItem : public BuilderItem<QToolBox>
{
public:
    using BuilderItem::BuilderItem;

    ToolBoxItem(const QString& text, QWidget* widget)
        : ToolBoxItem({}, text, widget)
    {
    }

    ToolBoxItem(const QIcon& icon, const QString& text, QWidget* widget)
        : BuilderItem(
              [](const BuilderItem<QToolBox>* item, QToolBox* toolbox)
              {
                  auto self = static_cast<const ToolBoxItem*>(item);
                  toolbox->addItem(self->widget, self->icon, self->text);
              })
        , icon(icon)
        , text(text)
        , widget(widget)
    {
    }

private:
    QIcon    icon;
    QString  text;
    QWidget* widget;
};

template <typename Self> class Builder<QToolBox, Self> : public Builder<QFrame, Self>
{
    N_BUILDER(QToolBox)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(currentIndex)
    N_END_BUILDER_SIGNAL

    Builder(std::initializer_list<ToolBoxItem> items) { self().addItems(items); }

    Self& items(std::initializer_list<ToolBoxItem> items) { return self().addItems(items); }
};

using ToolBox = Builder<QToolBox>;
#endif

#ifdef QSPLITTER_H
class SplitterItem : public BuilderItem<QSplitter>
{
public:
    using BuilderItem::BuilderItem;

    SplitterItem(QWidget* item)
        : BuilderItem(
              [](const BuilderItem* item, QSplitter* splitter)
              {
                  auto self = static_cast<const SplitterItem*>(item);
                  splitter->addWidget(static_cast<QWidget*>(self->item));
              })
        , item(item)
    {
    }

private:
    QWidget* item;
};

template <typename Self> class Builder<QSplitter, Self> : public Builder<QFrame, Self>
{
    N_BUILDER(QSplitter)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(orientation)
    N_BUILDER_PROPERTY(opaqueResize)
    N_BUILDER_PROPERTY(handleWidth)
    N_BUILDER_PROPERTY(childrenCollapsible)
    N_END_BUILDER_PROPERTY
};

using Splitter = Builder<QSplitter>;
#endif

#ifdef QABSTRACTSLIDER_H
template <typename Self> class Builder<QAbstractSlider, Self> : public Builder<QWidget, Self>
{
    N_BUILDER(QAbstractSlider)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(minimum)
    N_BUILDER_PROPERTY(maximum)
    N_BUILDER_PROPERTY(singleStep)
    N_BUILDER_PROPERTY(pageStep)
    N_BUILDER_PROPERTY(value)
    N_BUILDER_PROPERTY(sliderPosition)
    N_BUILDER_PROPERTY(tracking)
    N_BUILDER_PROPERTY(orientation)
    N_BUILDER_PROPERTY(invertedAppearance)
    N_BUILDER_PROPERTY(invertedControls)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER2(range, setRange)
    N_END_BUILDER_SETTER
};

using AbstractSlider = Builder<QAbstractSlider>;
#endif

#ifdef QDIAL_H
template <typename Self> class Builder<QDial, Self> : public Builder<QAbstractSlider, Self>
{
    N_BUILDER(QDial)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(wrapping)
    N_BUILDER_PROPERTY(notchTarget)
    N_BUILDER_PROPERTY(notchesVisible)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER1(orientation, setOrientation)
    N_END_BUILDER_SETTER
};

using Dial = Builder<QDial>;
#endif

#ifdef QSLIDER_H
template <typename Self> class Builder<QSlider, Self> : public Builder<QAbstractSlider, Self>
{
    N_BUILDER(QSlider)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(tickPosition)
    N_BUILDER_PROPERTY(tickInterval)
    N_END_BUILDER_PROPERTY

    Builder(Qt::Orientation orientation) { self().orientation(orientation); }
};

using Slider = Builder<QSlider>;
#endif

#ifdef QSCROLLBAR_H
template <typename Self> class Builder<QScrollBar, Self> : public Builder<QAbstractSlider, Self>
{
    N_BUILDER(QScrollBar)

    Builder(Qt::Orientation orientation) { self().orientation(orientation); }
};

using ScrollBar = Builder<QScrollBar>;
#endif

#ifdef QABSTRACTSPINBOX_H
template <typename Self> class Builder<QAbstractSpinBox, Self> : public Builder<QWidget, Self>
{
    N_BUILDER(QAbstractSpinBox)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(wrapping)
    N_BUILDER_PROPERTY(frame)
    N_BUILDER_PROPERTY(alignment)
    N_BUILDER_PROPERTY(readOnly)
    N_BUILDER_PROPERTY(buttonSymbols)
    N_BUILDER_PROPERTY(specialValueText)
    N_BUILDER_PROPERTY(accelerated)
    N_BUILDER_PROPERTY(correctionMode)
    N_BUILDER_PROPERTY(keyboardTracking)
    N_BUILDER_PROPERTY(showGroupSeparator)
    N_END_BUILDER_PROPERTY
};

using AbstractSpinBox = Builder<QAbstractSpinBox>;
#endif

#ifdef QDATETIMEEDIT_H
template <typename Self> class Builder<QDateTimeEdit, Self> : public Builder<QAbstractSpinBox, Self>
{
    N_BUILDER(QDateTimeEdit)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(dateTime)
    N_BUILDER_PROPERTY(date)
    N_BUILDER_PROPERTY(time)
    N_BUILDER_PROPERTY(maximumDateTime)
    N_BUILDER_PROPERTY(minimumDateTime)
    N_BUILDER_PROPERTY(maximumDate)
    N_BUILDER_PROPERTY(minimumDate)
    N_BUILDER_PROPERTY(maximumTime)
    N_BUILDER_PROPERTY(minimumTime)
    N_BUILDER_PROPERTY(currentSection)
    N_BUILDER_PROPERTY(displayFormat)
    N_BUILDER_PROPERTY(calendarPopup)
    N_BUILDER_PROPERTY(currentSectionIndex)
    N_UNTIL(6, 7, N_BUILDER_PROPERTY(timeSpec))
    N_SINCE(6, 7, N_BUILDER_PROPERTY(timeZone))
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER1(calendar, setCalendar)
    N_BUILDER_SETTER2(dateTimeRange, setDateTimeRange)
    N_BUILDER_SETTER2(dateRange, setDateRange)
    N_BUILDER_SETTER2(timeRange, setTimeRange)
    N_BUILDER_SETTER1(calendarWidget, setCalendarWidget)
    N_BUILDER_SETTER1(selectedSection, setSelectedSection)
    N_END_BUILDER_SETTER

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onDateTimeChanged, dateTimeChanged)
    N_BUILDER_SIGNAL(onTimeChanged, timeChanged)
    N_BUILDER_SIGNAL(onDateChanged, dateChanged)
    N_END_BUILDER_SIGNAL
};

template <typename Self> class Builder<QDateEdit, Self> : public Builder<QDateTimeEdit, Self>
{
    N_BUILDER(QDateEdit)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(date)
    N_END_BUILDER_PROPERTY
};

template <typename Self> class Builder<QTimeEdit, Self> : public Builder<QDateTimeEdit, Self>
{
    N_BUILDER(QTimeEdit)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(time)
    N_END_BUILDER_PROPERTY
};

using DateTimeEdit = Builder<QDateTimeEdit>;
using DateEdit     = Builder<QDateEdit>;
using TimeEdit     = Builder<QTimeEdit>;
#endif

#ifdef QSPINBOX_H
template <typename Self> class Builder<QSpinBox, Self> : public Builder<QAbstractSpinBox, Self>
{
    N_BUILDER(QSpinBox)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(suffix)
    N_BUILDER_PROPERTY(prefix)
    N_BUILDER_PROPERTY(minimum)
    N_BUILDER_PROPERTY(maximum)
    N_BUILDER_PROPERTY(singleStep)
    N_BUILDER_PROPERTY(stepType)
    N_BUILDER_PROPERTY(value)
    N_BUILDER_PROPERTY(displayIntegerBase)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER2(range, setRange)
    N_END_BUILDER_SETTER

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onValueChanged, valueChanged)
    N_BUILDER_SIGNAL(onTextChanged, textChanged)
    N_END_BUILDER_SIGNAL
};

template <typename Self> class Builder<QDoubleSpinBox, Self> : public Builder<QAbstractSpinBox, Self>
{
    N_BUILDER(QDoubleSpinBox)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(suffix)
    N_BUILDER_PROPERTY(prefix)
    N_BUILDER_PROPERTY(decimals)
    N_BUILDER_PROPERTY(minimum)
    N_BUILDER_PROPERTY(maximum)
    N_BUILDER_PROPERTY(singleStep)
    N_BUILDER_PROPERTY(stepType)
    N_BUILDER_PROPERTY(value)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER2(range, setRange)
    N_END_BUILDER_SETTER

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onValueChanged, valueChanged)
    N_BUILDER_SIGNAL(onTextChanged, textChanged)
    N_END_BUILDER_SIGNAL
};

using SpinBox       = Builder<QSpinBox>;
using DoubleSpinBox = Builder<QDoubleSpinBox>;
#endif

#ifdef QCOMBOBOX_H
class QComboBoxItem : public BuilderItem<QComboBox>
{
public:
    using BuilderItem::BuilderItem;

    template <typename T>
    QComboBoxItem(T&& text)
        : QComboBoxItem({}, std::forward<T>(text), {})
    {
    }

    QComboBoxItem(const QString& text, const QVariant& userData)
        : QComboBoxItem({}, text, userData)
    {
    }

    QComboBoxItem(const QIcon& icon, const QString& text, const QVariant& userData = {})
        : BuilderItem(
              [](const BuilderItem* item, QComboBox* box)
              {
                  auto self = static_cast<const QComboBoxItem*>(item);
                  box->addItem(self->icon, self->text, self->data);
              })
        , icon(icon)
        , text(text)
        , data(userData)
    {
    }

private:
    QIcon    icon;
    QString  text;
    QVariant data;
};

template <typename Self> class Builder<QComboBox, Self> : public Builder<QWidget, Self>
{
    N_BUILDER(QComboBox)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(editable)
    N_BUILDER_PROPERTY(currentText)
    N_BUILDER_PROPERTY(currentIndex)
    N_BUILDER_PROPERTY(maxVisibleItems)
    N_BUILDER_PROPERTY(maxCount)
    N_BUILDER_PROPERTY(insertPolicy)
    N_BUILDER_PROPERTY(sizeAdjustPolicy)
    N_BUILDER_PROPERTY(minimumContentsLength)
    N_BUILDER_PROPERTY(iconSize)
    N_BUILDER_PROPERTY(placeholderText)
    N_BUILDER_PROPERTY(duplicatesEnabled)
    N_BUILDER_PROPERTY(frame)
    N_BUILDER_PROPERTY(modelColumn)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onEditTextChanged, editTextChanged)
    N_BUILDER_SIGNAL(onActivated, activated)
    N_BUILDER_SIGNAL(onTextActivated, textActivated)
    N_BUILDER_SIGNAL(onHighlighted, highlighted)
    N_BUILDER_SIGNAL(onTextHighlighted, textHighlighted)
    N_BUILDER_SIGNAL(onCurrentIndexChanged, currentIndexChanged)
    N_BUILDER_SIGNAL(onCurrentTextChanged, currentTextChanged)
    N_END_BUILDER_SETTER

    // clang-format off
    Builder(const QStringList& items)                   { self(); object()->addItems(items); }
    Builder(std::initializer_list<QComboBoxItem> items) { self().addItems(items); }
    // clang-format on
};

using ComboBox = Builder<QComboBox>;
#endif

#ifdef QGROUPBOX_H
template <typename Self> class Builder<QGroupBox, Self> : public Builder<QWidget, Self>
{
    N_BUILDER(QGroupBox)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(title)
    N_BUILDER_PROPERTY(alignment)
    N_BUILDER_PROPERTY(flat)
    N_BUILDER_PROPERTY(checkable)
    N_BUILDER_PROPERTY(checked)
    N_END_BUILDER_PROPERTY

    // clang-format off
    explicit
    Builder(const QString& title)                  { self().init( ).title(title); }
    Builder(const QString& title, QLayout* layout) { self().init( ).title(title).layout(layout); }
    // clang-format on
};

using GroupBox = Builder<QGroupBox>;
#endif

#ifdef QLABEL_H
template <typename Self> class Builder<QLabel, Self> : public Builder<QWidget, Self>
{
    N_BUILDER(QLabel)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(text)
    N_BUILDER_PROPERTY(textFormat)
    N_BUILDER_PROPERTY(pixmap)
    N_BUILDER_PROPERTY(scaledContents)
    N_BUILDER_PROPERTY(alignment)
    N_BUILDER_PROPERTY(wordWrap)
    N_BUILDER_PROPERTY(margin)
    N_BUILDER_PROPERTY(indent)
    N_BUILDER_PROPERTY(openExternalLinks)
    N_BUILDER_PROPERTY(InteractionFlags)
    N_END_BUILDER_PROPERTY

    explicit Builder(const QString& text) { self().text(text); }
};

using Label = Builder<QLabel>;
#endif

#ifdef QLINEEDIT_H
template <typename Self> class Builder<QLineEdit, Self> : public Builder<QWidget, Self>
{
    N_BUILDER(QLineEdit)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(inputMask)
    N_BUILDER_PROPERTY(text)
    N_BUILDER_PROPERTY(maxLength)
    N_BUILDER_PROPERTY(frame)
    N_BUILDER_PROPERTY(echoMode)
    N_BUILDER_PROPERTY(cursorPosition)
    N_BUILDER_PROPERTY(alignment)
    N_BUILDER_PROPERTY(modified)
    N_BUILDER_PROPERTY(dragEnabled)
    N_BUILDER_PROPERTY(readOnly)
    N_BUILDER_PROPERTY(placeholderText)
    N_BUILDER_PROPERTY(cursorMoveStyle)
    N_BUILDER_PROPERTY(clearButtonEnabled)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER2(selection, setSelection)
    N_BUILDER_SETTERX(textMargins, setTextMargins, const QMargins&)
    N_BUILDER_SETTERX(textMargins, setTextMargins, int, int, int, int)
#ifndef QT_NO_VALIDATOR
    N_BUILDER_SETTER1(validator, setValidator)
#endif
#if QT_CONFIG(completer)
    N_BUILDER_SETTER1(completer, setCompleter)
#endif
    N_END_BUILDER_SETTER

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onTextChanged, textChanged)
    N_BUILDER_SIGNAL(onTextEdited, textEdited)
    N_BUILDER_SIGNAL(onCursorPositionChanged, cursorPositionChanged)
    N_BUILDER_SIGNAL(onReturnPressed, returnPressed)
    N_BUILDER_SIGNAL(onEditingFinished, editingFinished)
    N_BUILDER_SIGNAL(onSelectionChanged, selectionChanged)
    N_BUILDER_SIGNAL(onInputRejected, inputRejected)
    N_END_BUILDER_SIGNAL

    explicit Builder(const QString& text) { self().text(text); }
};

using LineEdit = Builder<QLineEdit>;
#endif

#ifdef QMENU_H
class MenuItem : public BuilderItem<QMenu>
{
public:
    using BuilderItem::BuilderItem;

    MenuItem(QAction* action)
        : BuilderItem([](const BuilderItem* item, QMenu* m)
                      { m->addAction(static_cast<QAction*>(static_cast<const MenuItem*>(item)->item)); })
        , item(action)
    {
    }
    MenuItem(QMenu* menu)
        : BuilderItem([](const BuilderItem* item, QMenu* m)
                      { m->addMenu(static_cast<QMenu*>(static_cast<const MenuItem*>(item)->item)); })
        , item(menu)
    {
    }

    template <typename Class, typename Self>
    MenuItem(const Builder<Class, Self>& builder)
        : MenuItem(builder.object())
    {
    }

    enum class separator { tag };
    MenuItem(separator)
        : BuilderItem([](const BuilderItem* item, QMenu* m) { m->addSeparator(); })
    {
    }

private:
    QObject* item;
};

template <typename Self> class Builder<QMenu, Self> : public Builder<QWidget, Self>
{
    N_BUILDER(QMenu)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(tearOffEnabled)
    N_BUILDER_PROPERTY(title)
    N_BUILDER_PROPERTY(icon)
    N_BUILDER_PROPERTY(separatorsCollapsible)
    N_BUILDER_PROPERTY(toolTipsVisible)
    N_END_BUILDER_PROPERTY

    Builder(std::initializer_list<MenuItem> items) { self().addItems(items); }

    static auto Separator() { return MenuItem(MenuItem::separator::tag); }

    Self& items(std::initializer_list<MenuItem> items) { return self().addItems(items); }
};

using Menu = Builder<QMenu>;
#endif

#ifdef QMENUBAR_H
class MenuBarItem : public BuilderItem<QMenuBar>
{
public:
    using BuilderItem::BuilderItem;

    MenuBarItem(QAction* action)
        : BuilderItem([](const BuilderItem* item, QMenuBar* m)
                      { m->addAction(static_cast<QAction*>(static_cast<const MenuBarItem*>(item)->item)); })
        , item(action)
    {
    }

    MenuBarItem(QMenu* menu)
        : BuilderItem([](const BuilderItem* item, QMenuBar* m)
                      { m->addMenu(static_cast<QMenu*>(static_cast<const MenuBarItem*>(item)->item)); })
        , item(menu)
    {
    }

private:
    QObject* item;
};

template <typename Self> class Builder<QMenuBar, Self> : public Builder<QWidget, Self>
{
    N_BUILDER(QMenuBar)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(defaultUp)
    N_BUILDER_PROPERTY(nativeMenuBar)
    N_END_BUILDER_PROPERTY

    Self& items(std::initializer_list<MenuBarItem> items) { return self().addItems(items); }
};

using MenuBar = Builder<QMenuBar>;
#endif

#ifdef QPROGRESSBAR_H
template <typename Self> class Builder<QProgressBar, Self> : public Builder<QWidget, Self>
{
    N_BUILDER(QProgressBar)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(minimum)
    N_BUILDER_PROPERTY(maximum)
    N_BUILDER_PROPERTY(value)
    N_BUILDER_PROPERTY(alignment)
    N_BUILDER_PROPERTY(textVisible)
    N_BUILDER_PROPERTY(orientation)
    N_BUILDER_PROPERTY(invertedAppearance)
    N_BUILDER_PROPERTY(textDirection)
    N_BUILDER_PROPERTY(format)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER2(range, setRange)
    N_END_BUILDER_SETTER

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onValueChanged, valueChanged)
    N_END_BUILDER_SIGNAL
};

using ProgressBar = Builder<QProgressBar>;
#endif

#ifdef QTABBAR_H
class TabBarItem : public BuilderItem<QTabBar>
{
public:
    using BuilderItem::BuilderItem;

    TabBarItem(const QString& text)
        : TabBarItem({}, text)
    {
    }

    TabBarItem(const QIcon& icon, const QString& text)
        : BuilderItem(
              [](const BuilderItem* item, QTabBar* bar)
              {
                  auto s = static_cast<const TabBarItem*>(item);
                  bar->addTab(s->icon, s->text);
              })
        , icon(icon)
        , text(text)
    {
    }

private:
    QIcon   icon;
    QString text;
};

template <typename Self> class Builder<QTabBar, Self> : public Builder<QWidget, Self>
{
    N_BUILDER(QTabBar)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(shape)
    N_BUILDER_PROPERTY(currentIndex)
    N_BUILDER_PROPERTY(drawBase)
    N_BUILDER_PROPERTY(iconSize)
    N_BUILDER_PROPERTY(elideMode)
    N_BUILDER_PROPERTY(usesScrollButtons)
    N_BUILDER_PROPERTY(tabsClosable)
    N_BUILDER_PROPERTY(selectionBehaviorOnRemove)
    N_BUILDER_PROPERTY(expanding)
    N_BUILDER_PROPERTY(movable)
    N_BUILDER_PROPERTY(documentMode)
    N_BUILDER_PROPERTY(autoHide)
    N_BUILDER_PROPERTY(changeCurrentOnDrag)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER2(tabEnabled, setTabEnabled)
    N_BUILDER_SETTER2(tabVisible, setTabVisible)
    N_BUILDER_SETTER2(tabTextColor, setTabTextColor)
    N_BUILDER_SETTER1(ElideMode, setElideMode)
    N_BUILDER_SETTER2(tabData, setTabData)
    N_BUILDER_SETTER3(tabButton, setTabButton)
#if QT_CONFIG(tooltip)
    N_BUILDER_SETTER2(tabToolTip, setTabToolTip)
#endif
#if QT_CONFIG(whatsthis)
    N_BUILDER_SETTER2(tabWhatsThis, setTabWhatsThis)
#endif
#if QT_CONFIG(accessibility)
    N_BUILDER_SETTER2(accessibleTabName, setAccessibleTabName)
#endif
    N_END_BUILDER_SETTER

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onCurrentChanged, currentChanged)
    N_BUILDER_SIGNAL(onTabCloseRequested, tabCloseRequested)
    N_BUILDER_SIGNAL(onTabMoved, tabMoved)
    N_BUILDER_SIGNAL(onTabBarClicked, tabBarClicked)
    N_BUILDER_SIGNAL(onTabBarDoubleClicked, tabBarDoubleClicked)
    N_END_BUILDER_SIGNAL

    Self& items(std::initializer_list<TabBarItem> items) { return self().addItems(items); }
};

using TabBar = Builder<QTabBar>;
#endif

#ifdef QTABWIDGET_H
class TabWidgetItem : public BuilderItem<QTabWidget>
{
public:
    using BuilderItem::BuilderItem;

    TabWidgetItem(const QString& text, QWidget* page)
        : TabWidgetItem({}, text, page)
    {
    }

    TabWidgetItem(const QString& icon, const QString& text, QWidget* page)
        : BuilderItem(
              [](const BuilderItem* item, QTabWidget* tab)
              {
                  auto s = static_cast<const TabWidgetItem*>(item);
                  tab->addTab(s->page, s->icon, s->text);
              })
        , icon(icon)
        , text(text)
        , page(page)
    {
    }

private:
    QIcon    icon;
    QString  text;
    QWidget* page;
};

template <typename Self> class Builder<QTabWidget, Self> : public Builder<QWidget, Self>
{
    N_BUILDER(QTabWidget)

    N_BEGIN_BUILDER_PROPERTY
    N_BUILDER_PROPERTY(tabPosition)
    N_BUILDER_PROPERTY(tabShape)
    N_BUILDER_PROPERTY(documentMode)
    N_BUILDER_PROPERTY(usesScrollButtons)
    N_BUILDER_PROPERTY(tabsClosable)
    N_END_BUILDER_PROPERTY

    N_BEGIN_BUILDER_SETTER
    N_BUILDER_SETTER2(tabEnabled, setTabEnabled)
    N_BUILDER_SETTER2(tabVisible, setTabVisible)
    N_BUILDER_SETTER2(tabText, setTabText)
    N_BUILDER_SETTER2(tabIcon, setTabIcon)
    N_BUILDER_SETTER1(currentWidget, setCurrentWidget)
    N_BUILDER_SETTERX(cornerWidget, setCornerWidget, QWidget*)
    N_BUILDER_SETTERX(cornerWidget, setCornerWidget, QWidget*, Qt::Corner)
#if QT_CONFIG(tooltip)
    N_BUILDER_SETTER2(tabToolTip, setTabToolTip)
#endif
#if QT_CONFIG(whatsthis)
    N_BUILDER_SETTER2(tabWhatsThis, setTabWhatsThis)
#endif
    N_END_BUILDER_SETTER

    N_BEGIN_BUILDER_SIGNAL
    N_BUILDER_SIGNAL(onCurrentChanged, currentChanged)
    N_BUILDER_SIGNAL(onTabCloseRequested, tabCloseRequested)
    N_BUILDER_SIGNAL(onTabBarClicked, tabBarClicked)
    N_BUILDER_SIGNAL(onTabBarDoubleClicked, tabBarDoubleClicked)
    N_END_BUILDER_SIGNAL

    Builder(std::initializer_list<TabWidgetItem> items) { self().addItems(items); }

    Self& items(std::initializer_list<TabWidgetItem> items) { return self().addItems(items); }
};

using TabWidget = Builder<QTabWidget>;
#endif

} // namespace nwidget

#endif // NWIDGET_BUILDERS_H
