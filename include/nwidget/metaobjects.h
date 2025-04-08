#ifndef NWIDGET_METAOBJECTS_H
#define NWIDGET_METAOBJECTS_H

#include "metaobject.h"

#ifdef QOBJECT_H
template <> class nwidget::MetaObject<QObject>
{
    N_OBJECT(QObject)

    N_BEGIN_PROPERTY
    N_PROPERTY(QString, objectName, N_READ objectName N_WRITE setObjectName N_NOTIFY objectNameChanged)
    N_END_PROPERTY
};
#endif

#ifdef QACTION_H
template <> class nwidget::MetaObject<QAction> : public MetaObject<QObject>
{
    N_OBJECT(QAction, QObject)

    N_BEGIN_PROPERTY
    N_PROPERTY(bool, checkable, N_READ isCheckable N_WRITE setCheckable N_NOTIFY checkableChanged)
    N_PROPERTY(bool, checked, N_READ isChecked N_WRITE setChecked N_NOTIFY toggled)
    N_PROPERTY(bool, enabled, N_READ isEnabled N_WRITE setEnabled N_NOTIFY enabledChanged)
    N_PROPERTY(QIcon, icon, N_READ icon N_WRITE setIcon N_NOTIFY changed)
    N_PROPERTY(QString, text, N_READ text N_WRITE setText N_NOTIFY changed)
    N_PROPERTY(QString, iconText, N_READ iconText N_WRITE setIconText N_NOTIFY changed)
    N_PROPERTY(QString, toolTip, N_READ toolTip N_WRITE setToolTip N_NOTIFY changed)
    N_PROPERTY(QString, statusTip, N_READ statusTip N_WRITE setStatusTip N_NOTIFY changed)
    N_PROPERTY(QString, whatsThis, N_READ whatsThis N_WRITE setWhatsThis N_NOTIFY changed)
    N_PROPERTY(QFont, font, N_READ font N_WRITE setFont N_NOTIFY changed)
#if QT_CONFIG(shortcut)
    N_PROPERTY(QKeySequence, shortcut, N_READ shortcut N_WRITE setShortcut N_NOTIFY changed)
    N_PROPERTY(Qt::ShortcutContext, shortcutContext, N_READ shortcutContext N_WRITE setShortcutContext N_NOTIFY changed)
    N_PROPERTY(bool, autoRepeat, N_READ autoRepeat N_WRITE setAutoRepeat N_NOTIFY changed)
#endif
    N_PROPERTY(bool, visible, N_READ isVisible N_WRITE setVisible N_NOTIFY visibleChanged)
    N_PROPERTY(QAction::MenuRole, menuRole, N_READ menuRole N_WRITE setMenuRole N_NOTIFY changed)
    N_PROPERTY(bool, iconVisibleInMenu, N_READ isIconVisibleInMenu N_WRITE setIconVisibleInMenu N_NOTIFY changed)
    N_PROPERTY(bool,
               shortcutVisibleInContextMenu,
               N_READ isShortcutVisibleInContextMenu N_WRITE setShortcutVisibleInContextMenu N_NOTIFY changed)
    N_PROPERTY(QAction::Priority, priority, N_READ priority N_WRITE setPriority N_NOTIFY changed)
    N_END_PROPERTY
};
#endif

#ifdef QLAYOUT_H
template <> class nwidget::MetaObject<QLayout> : public MetaObject<QObject>
{
    N_OBJECT(QLayout, QObject)

    N_BEGIN_PROPERTY
    N_PROPERTY(int, spacing, N_READ spacing N_WRITE setSpacing)
    N_PROPERTY(QMargins, contentsMargins, N_READ contentsMargins N_WRITE setContentsMargins)
    N_PROPERTY(QLayout::SizeConstraint, sizeConstraint, N_READ sizeConstraint N_WRITE setSizeConstraint)
    N_END_PROPERTY
};
#endif

#ifdef QBOXLAYOUT_H
template <> class nwidget::MetaObject<QBoxLayout> : public MetaObject<QLayout>
{
    N_OBJECT(QBoxLayout, QLayout)
};

template <> class nwidget::MetaObject<QHBoxLayout> : public MetaObject<QBoxLayout>
{
    N_OBJECT(QHBoxLayout, QBoxLayout)
};

template <> class nwidget::MetaObject<QVBoxLayout> : public MetaObject<QBoxLayout>
{
    N_OBJECT(QVBoxLayout, QBoxLayout)
};
#endif

#ifdef QFORMLAYOUT_H
template <> class nwidget::MetaObject<QFormLayout> : public MetaObject<QLayout>
{
    N_OBJECT(QFormLayout, QLayout)

    N_BEGIN_PROPERTY
    N_PROPERTY(QFormLayout::FieldGrowthPolicy, fieldGrowthPolicy, N_READ fieldGrowthPolicy N_WRITE setFieldGrowthPolicy)
    N_PROPERTY(QFormLayout::RowWrapPolicy, rowWrapPolicy, N_READ rowWrapPolicy N_WRITE setRowWrapPolicy)
    N_PROPERTY(Qt::Alignment, labelAlignment, N_READ labelAlignment N_WRITE setLabelAlignment)
    N_PROPERTY(Qt::Alignment, formAlignment, N_READ formAlignment N_WRITE setFormAlignment)
    N_PROPERTY(int, horizontalSpacing, N_READ horizontalSpacing N_WRITE setHorizontalSpacing)
    N_PROPERTY(int, verticalSpacing, N_READ verticalSpacing N_WRITE setVerticalSpacing)
    N_END_PROPERTY
};
#endif

#ifdef QGRIDLAYOUT_H
template <> class nwidget::MetaObject<QGridLayout> : public MetaObject<QLayout>
{
    N_OBJECT(QGridLayout, QLayout)

    N_BEGIN_PROPERTY
    N_PROPERTY(int, horizontalSpacing, N_READ horizontalSpacing N_WRITE setHorizontalSpacing)
    N_PROPERTY(int, verticalSpacing, N_READ verticalSpacing N_WRITE setVerticalSpacing)
    N_END_PROPERTY
};
#endif

#ifdef QSTACKEDLAYOUT_H
template <> class nwidget::MetaObject<QStackedLayout> : public MetaObject<QLayout>
{
    N_OBJECT(QStackedLayout, QLayout)

    N_BEGIN_PROPERTY
    N_PROPERTY(int, currentIndex, N_READ currentIndex N_WRITE setCurrentIndex N_NOTIFY currentChanged)
    N_PROPERTY(QStackedLayout::StackingMode, stackingMode, N_READ stackingMode N_WRITE setStackingMode)
    N_PROPERTY(int, count, N_READ count)
    N_END_PROPERTY
};
#endif

#ifdef QWIDGET_H
#include <QLocale>

template <> class nwidget::MetaObject<QWidget> : public MetaObject<QObject>
{
    N_OBJECT(QWidget, QObject)

    N_BEGIN_PROPERTY
    N_PROPERTY(bool, modal, N_READ isModal)
    N_PROPERTY(Qt::WindowModality, windowModality, N_READ windowModality N_WRITE setWindowModality)
    N_PROPERTY(bool, enabled, N_READ isEnabled N_WRITE setEnabled)
    N_PROPERTY(QRect, geometry, N_READ geometry N_WRITE setGeometry)
    N_PROPERTY(QRect, frameGeometry, N_READ frameGeometry)
    N_PROPERTY(QRect, normalGeometry, N_READ normalGeometry)
    N_PROPERTY(int, x, N_READ x)
    N_PROPERTY(int, y, N_READ y)
    N_PROPERTY(QPoint, pos, N_READ pos N_WRITE move)
    N_PROPERTY(QSize, frameSize, N_READ frameSize)
    N_PROPERTY(QSize, size, N_READ size N_WRITE resize)
    N_PROPERTY(int, width, N_READ width)
    N_PROPERTY(int, height, N_READ height)
    N_PROPERTY(QRect, rect, N_READ rect)
    N_PROPERTY(QRect, childrenRect, N_READ childrenRect)
    N_PROPERTY(QRegion, childrenRegion, N_READ childrenRegion)
    N_PROPERTY(QSizePolicy, sizePolicy, N_READ sizePolicy N_WRITE setSizePolicy)
    N_PROPERTY(QSize, minimumSize, N_READ minimumSize N_WRITE setMinimumSize)
    N_PROPERTY(QSize, maximumSize, N_READ maximumSize N_WRITE setMaximumSize)
    N_PROPERTY(int, minimumWidth, N_READ minimumWidth N_WRITE setMinimumWidth)
    N_PROPERTY(int, minimumHeight, N_READ minimumHeight N_WRITE setMinimumHeight)
    N_PROPERTY(int, maximumWidth, N_READ maximumWidth N_WRITE setMaximumWidth)
    N_PROPERTY(int, maximumHeight, N_READ maximumHeight N_WRITE setMaximumHeight)
    N_PROPERTY(QSize, sizeIncrement, N_READ sizeIncrement N_WRITE setSizeIncrement)
    N_PROPERTY(QSize, baseSize, N_READ baseSize N_WRITE setBaseSize)
    N_PROPERTY(QPalette, alette, N_READ palette N_WRITE setPalette)
    N_PROPERTY(QFont, font, N_READ font N_WRITE setFont)
#ifndef QT_NO_CURSOR
    N_PROPERTY(QCursor, cursor, N_READ cursor N_WRITE setCursor)
#endif
    N_PROPERTY(bool, mouseTracking, N_READ hasMouseTracking N_WRITE setMouseTracking)
    N_PROPERTY(bool, tabletTracking, N_READ hasTabletTracking N_WRITE setTabletTracking)
    N_PROPERTY(bool, isActiveWindow, N_READ isActiveWindow)
    N_PROPERTY(Qt::FocusPolicy, focusPolicy, N_READ focusPolicy N_WRITE setFocusPolicy)
    N_PROPERTY(bool, focus, N_READ hasFocus)
    N_PROPERTY(Qt::ContextMenuPolicy, contextMenuPolicy, N_READ contextMenuPolicy N_WRITE setContextMenuPolicy)
    N_PROPERTY(bool, updatesEnabled, N_READ updatesEnabled N_WRITE setUpdatesEnabled)
    N_PROPERTY(bool, visible, N_READ isVisible N_WRITE setVisible)
    N_PROPERTY(bool, minimized, N_READ isMinimized)
    N_PROPERTY(bool, maximized, N_READ isMaximized)
    N_PROPERTY(bool, fullScreen, N_READ isFullScreen)
    N_PROPERTY(QSize, sizeHint, N_READ sizeHint)
    N_PROPERTY(QSize, minimumSizeHint, N_READ minimumSizeHint)
    N_PROPERTY(bool, acceptDrops, N_READ acceptDrops N_WRITE setAcceptDrops)
    N_PROPERTY(QString, windowTitle, N_READ windowTitle N_WRITE setWindowTitle N_NOTIFY windowTitleChanged)
    N_PROPERTY(QIcon, windowIcon, N_READ windowIcon N_WRITE setWindowIcon N_NOTIFY windowIconChanged)
    N_PROPERTY(QString, windowIconText, N_READ windowIconText N_WRITE setWindowIconText N_NOTIFY windowIconTextChanged)
    N_PROPERTY(double, windowOpacity, N_READ windowOpacity N_WRITE setWindowOpacity)
    N_PROPERTY(bool, windowModified, N_READ isWindowModified N_WRITE setWindowModified)
#if QT_CONFIG(tooltip)
    N_PROPERTY(QString, toolTip, N_READ toolTip N_WRITE setToolTip)
    N_PROPERTY(int, toolTipDuration, N_READ toolTipDuration N_WRITE setToolTipDuration)
#endif
#if QT_CONFIG(statustip)
    N_PROPERTY(QString, statusTip, N_READ statusTip N_WRITE setStatusTip)
#endif
#if QT_CONFIG(whatsthis)
    N_PROPERTY(QString, whatsThis, N_READ whatsThis N_WRITE setWhatsThis)
#endif
#if QT_CONFIG(accessibility)
    N_PROPERTY(QString, accessibleName, N_READ accessibleName N_WRITE setAccessibleName)
    N_PROPERTY(QString, accessibleDescription, N_READ accessibleDescription N_WRITE setAccessibleDescription)
#endif
    N_PROPERTY(Qt::LayoutDirection, layoutDirection, N_READ layoutDirection N_WRITE setLayoutDirection)
    N_PROPERTY(Qt::WindowFlags, windowFlags, N_READ windowFlags N_WRITE setWindowFlags)
    N_PROPERTY(bool, autoFillBackground, N_READ autoFillBackground N_WRITE setAutoFillBackground)
#ifndef QT_NO_STYLE_STYLESHEET
    N_PROPERTY(QString, styleSheet, N_READ styleSheet N_WRITE setStyleSheet)
#endif
    N_PROPERTY(QLocale, locale, N_READ locale N_WRITE setLocale)
    N_PROPERTY(QString, windowFilePath, N_READ windowFilePath N_WRITE setWindowFilePath)
    N_PROPERTY(Qt::InputMethodHints, inputMethodHints, N_READ inputMethodHints N_WRITE setInputMethodHints)
    N_END_PROPERTY
};
#endif

#ifdef QABSTRACTBUTTON_H
template <> class nwidget::MetaObject<QAbstractButton> : public MetaObject<QWidget>
{
    N_OBJECT(QAbstractButton, QWidget)

    N_BEGIN_PROPERTY
    N_PROPERTY(QString, text, N_READ text N_WRITE setText)
    N_PROPERTY(QIcon, icon, N_READ icon N_WRITE setIcon)
    N_PROPERTY(QSize, iconSize, N_READ iconSize N_WRITE setIconSize)
#ifndef QT_NO_SHORTCUT
    N_PROPERTY(QKeySequence, shortcut, N_READ shortcut N_WRITE setShortcut)
#endif
    N_PROPERTY(bool, checkable, N_READ isCheckable N_WRITE setCheckable)
    N_PROPERTY(bool, checked, N_READ isChecked N_WRITE setChecked N_NOTIFY toggled)
    N_PROPERTY(bool, autoRepeat, N_READ autoRepeat N_WRITE setAutoRepeat)
    N_PROPERTY(bool, autoExclusive, N_READ autoExclusive N_WRITE setAutoExclusive)
    N_PROPERTY(int, autoRepeatDelay, N_READ autoRepeatDelay N_WRITE setAutoRepeatDelay)
    N_PROPERTY(int, autoRepeatInterval, N_READ autoRepeatInterval N_WRITE setAutoRepeatInterval)
    N_PROPERTY(bool, down, N_READ isDown N_WRITE setDown)
    N_END_PROPERTY
};
#endif

#ifdef QDIALOGBUTTONBOX_H
template <> class nwidget::MetaObject<QDialogButtonBox> : public MetaObject<QWidget>
{
    N_OBJECT(QDialogButtonBox, QWidget)

    N_BEGIN_PROPERTY
    N_PROPERTY(Qt::Orientation, orientation, N_READ orientation N_WRITE setOrientation)
    N_PROPERTY(QDialogButtonBox::StandardButtons, standardButtons, N_READ standardButtons N_WRITE setStandardButtons)
    N_PROPERTY(bool, centerButtons, N_READ centerButtons N_WRITE setCenterButtons)
    N_END_PROPERTY
};
#endif

#ifdef QCHECKBOX_H
template <> class nwidget::MetaObject<QCheckBox> : public MetaObject<QAbstractButton>
{
    N_OBJECT(QCheckBox, QAbstractButton)

    N_BEGIN_PROPERTY
    N_PROPERTY(bool, tristate, N_READ isTristate N_WRITE setTristate)
    N_PROPERTY(Qt::CheckState, checkState, N_READ checkState N_WRITE setCheckState)
    N_END_PROPERTY
};
#endif

#ifdef QPUSHBUTTON_H
template <> class nwidget::MetaObject<QPushButton> : public MetaObject<QAbstractButton>
{
    N_OBJECT(QPushButton, QAbstractButton)

    N_BEGIN_PROPERTY
    N_PROPERTY(bool, autoDefault, N_READ autoDefault N_WRITE setAutoDefault)
    N_PROPERTY(bool, default_, N_READ isDefault N_WRITE setDefault)
    N_PROPERTY(bool, flat, N_READ isFlat N_WRITE setFlat)
    N_END_PROPERTY
};
#endif

#ifdef QCOMMANDLINKBUTTON_H
template <> class nwidget::MetaObject<QCommandLinkButton> : public MetaObject<QPushButton>
{
    N_OBJECT(QCommandLinkButton, QPushButton)

    N_BEGIN_PROPERTY
    N_PROPERTY(QString, description, N_READ description N_WRITE setDescription)
    N_PROPERTY(bool, flat, N_READ isFlat N_WRITE setFlat)
    N_END_PROPERTY
};
#endif

#ifdef QRADIOBUTTON_H
template <> class nwidget::MetaObject<QRadioButton> : public MetaObject<QAbstractButton>
{
    N_OBJECT(QRadioButton, QAbstractButton)
};
#endif

#ifdef QTOOLBUTTON_H
template <> class nwidget::MetaObject<QToolButton> : public MetaObject<QAbstractButton>
{
    N_OBJECT(QToolButton, QAbstractButton)

    N_BEGIN_PROPERTY
#if QT_CONFIG(menu)
    N_PROPERTY(QToolButton::ToolButtonPopupMode, popupMode, N_READ popupMode N_WRITE setPopupMode)
#endif
    N_PROPERTY(Qt::ToolButtonStyle, toolButtonStyle, N_READ toolButtonStyle N_WRITE setToolButtonStyle)
    N_PROPERTY(bool, autoRaise, N_READ autoRaise N_WRITE setAutoRaise)
    N_PROPERTY(Qt::ArrowType, arrowType, N_READ arrowType N_WRITE setArrowType)
    N_END_PROPERTY
};
#endif

#ifdef QFRAME_H
template <> class nwidget::MetaObject<QFrame> : public MetaObject<QWidget>
{
    N_OBJECT(QFrame, QWidget)

    N_BEGIN_PROPERTY
    N_PROPERTY(QFrame::Shape, frameShape, N_READ frameShape N_WRITE setFrameShape)
    N_PROPERTY(QFrame::Shadow, frameShadow, N_READ frameShadow N_WRITE setFrameShadow)
    N_PROPERTY(int, lineWidth, N_READ lineWidth N_WRITE setLineWidth)
    N_PROPERTY(int, midLineWidth, N_READ midLineWidth N_WRITE setMidLineWidth)
    N_PROPERTY(int, frameWidth, N_READ frameWidth)
    N_PROPERTY(QRect, frameRect, N_READ frameRect N_WRITE setFrameRect)
    N_END_PROPERTY
};
#endif

#ifdef QABSTRACTSCROLLAREA_H
template <> class nwidget::MetaObject<QAbstractScrollArea> : public MetaObject<QFrame>
{
    N_OBJECT(QAbstractScrollArea, QFrame)

    N_BEGIN_PROPERTY
    N_PROPERTY(Qt::ScrollBarPolicy,
               verticalScrollBarPolicy,
               N_READ verticalScrollBarPolicy N_WRITE setVerticalScrollBarPolicy)
    N_PROPERTY(Qt::ScrollBarPolicy,
               horizontalScrollBarPolicy,
               N_READ horizontalScrollBarPolicy N_WRITE setHorizontalScrollBarPolicy)
    N_PROPERTY(QAbstractScrollArea::SizeAdjustPolicy,
               sizeAdjustPolicy,
               N_READ sizeAdjustPolicy N_WRITE setSizeAdjustPolicy)
    N_END_PROPERTY
};
#endif

#ifdef QABSTRACTITEMVIEW_H
template <> class nwidget::MetaObject<QAbstractItemView> : public MetaObject<QAbstractScrollArea>
{
    N_OBJECT(QAbstractItemView, QAbstractScrollArea)

    N_BEGIN_PROPERTY
    N_PROPERTY(bool, autoScroll, N_READ hasAutoScroll N_WRITE setAutoScroll)
    N_PROPERTY(int, autoScrollMargin, N_READ autoScrollMargin N_WRITE setAutoScrollMargin)
    N_PROPERTY(QAbstractItemView::EditTriggers, editTriggers, N_READ editTriggers N_WRITE setEditTriggers)
    N_PROPERTY(bool, tabKeyNavigation, N_READ tabKeyNavigation N_WRITE setTabKeyNavigation)
#if QT_CONFIG(draganddrop)
    N_PROPERTY(bool, showDropIndicator, N_READ showDropIndicator N_WRITE setDropIndicatorShown)
    N_PROPERTY(bool, dragEnabled, N_READ dragEnabled N_WRITE setDragEnabled)
    N_PROPERTY(bool, dragDropOverwriteMode, N_READ dragDropOverwriteMode N_WRITE setDragDropOverwriteMode)
    N_PROPERTY(QAbstractItemView::DragDropMode, dragDropMode, N_READ dragDropMode N_WRITE setDragDropMode)
    N_PROPERTY(Qt::DropAction, defaultDropAction, N_READ defaultDropAction N_WRITE setDefaultDropAction)
#endif
    N_PROPERTY(bool, alternatingRowColors, N_READ alternatingRowColors N_WRITE setAlternatingRowColors)
    N_PROPERTY(QAbstractItemView::SelectionMode, selectionMode, N_READ selectionMode N_WRITE setSelectionMode)
    N_PROPERTY(QAbstractItemView::SelectionBehavior,
               selectionBehavior,
               N_READ selectionBehavior N_WRITE setSelectionBehavior)
    N_PROPERTY(QSize, iconSize, N_READ iconSize N_WRITE setIconSize N_NOTIFY iconSizeChanged)
    N_PROPERTY(Qt::TextElideMode, textElideMode, N_READ textElideMode N_WRITE setTextElideMode)
    N_PROPERTY(QAbstractItemView::ScrollMode,
               verticalScrollMode,
               N_READ verticalScrollMode N_WRITE setVerticalScrollMode)
    N_PROPERTY(QAbstractItemView::ScrollMode,
               horizontalScrollMode,
               N_READ horizontalScrollMode N_WRITE setHorizontalScrollMode)
    N_END_PROPERTY
};
#endif

#ifdef QHEADERVIEW_H
template <> class nwidget::MetaObject<QHeaderView> : public MetaObject<QAbstractItemView>
{
    N_OBJECT(QHeaderView, QAbstractItemView)

    N_BEGIN_PROPERTY
    N_PROPERTY(bool, firstSectionMovable, N_READ isFirstSectionMovable N_WRITE setFirstSectionMovable)
    N_PROPERTY(bool, showSortIndicator, N_READ isSortIndicatorShown N_WRITE setSortIndicatorShown)
    N_PROPERTY(bool, highlightSections, N_READ highlightSections N_WRITE setHighlightSections)
    N_PROPERTY(bool, stretchLastSection, N_READ stretchLastSection N_WRITE setStretchLastSection)
    N_PROPERTY(bool, cascadingSectionResizes, N_READ cascadingSectionResizes)
    N_PROPERTY(int, defaultSectionSize, N_READ defaultSectionSize N_WRITE setDefaultSectionSize)
    N_PROPERTY(int, minimumSectionSize, N_READ minimumSectionSize N_WRITE setMinimumSectionSize)
    N_PROPERTY(int, maximumSectionSize, N_READ maximumSectionSize N_WRITE setMaximumSectionSize)
    N_PROPERTY(Qt::Alignment, defaultAlignment, N_READ defaultAlignment N_WRITE setDefaultAlignment)
    N_PROPERTY(bool,
               sortIndicatorClearable,
               N_READ isSortIndicatorClearable N_WRITE setSortIndicatorClearable N_NOTIFY sortIndicatorClearableChanged)
    N_END_PROPERTY
};
#endif

#ifdef QLISTVIEW_H
template <> class nwidget::MetaObject<QListView> : public MetaObject<QAbstractItemView>
{
    N_OBJECT(QListView, QAbstractItemView)

    N_BEGIN_PROPERTY
    N_PROPERTY(QListView::Movement, movement, N_READ movement N_WRITE setMovement)
    N_PROPERTY(QListView::Flow, flow, N_READ flow N_WRITE setFlow)
    N_PROPERTY(bool, isWrapping, N_READ isWrapping N_WRITE setWrapping)
    N_PROPERTY(QListView::ResizeMode, resizeMode, N_READ resizeMode N_WRITE setResizeMode)
    N_PROPERTY(QListView::LayoutMode, layoutMode, N_READ layoutMode N_WRITE setLayoutMode)
    N_PROPERTY(int, spacing, N_READ spacing N_WRITE setSpacing)
    N_PROPERTY(QSize, gridSize, N_READ gridSize N_WRITE setGridSize)
    N_PROPERTY(QListView::ViewMode, viewMode, N_READ viewMode N_WRITE setViewMode)
    N_PROPERTY(int, modelColumn, N_READ modelColumn N_WRITE setModelColumn)
    N_PROPERTY(bool, uniformItemSizes, N_READ uniformItemSizes N_WRITE setUniformItemSizes)
    N_PROPERTY(int, batchSize, N_READ batchSize N_WRITE setBatchSize)
    N_PROPERTY(bool, wordWrap, N_READ wordWrap N_WRITE setWordWrap)
    N_PROPERTY(bool, selectionRectVisible, N_READ isSelectionRectVisible N_WRITE setSelectionRectVisible)
    N_PROPERTY(Qt::Alignment, itemAlignment, N_READ itemAlignment N_WRITE setItemAlignment)
    N_END_PROPERTY
};
#endif

#ifdef QLISTWIDGET_H
template <> class nwidget::MetaObject<QListWidget> : public MetaObject<QListView>
{
    N_OBJECT(QListWidget, QListView)

    N_BEGIN_PROPERTY
    N_PROPERTY(int, count, N_READ count)
    N_PROPERTY(int, currentRow, N_READ currentRow N_WRITE setCurrentRow N_NOTIFY currentRowChanged)
    N_PROPERTY(bool, sortingEnabled, N_READ isSortingEnabled N_WRITE setSortingEnabled)
    N_END_PROPERTY
};
#endif

#ifdef QTABLEVIEW_H
template <> class nwidget::MetaObject<QTableView> : public MetaObject<QAbstractItemView>
{
    N_OBJECT(QTableView, QAbstractItemView)

    N_BEGIN_PROPERTY
    N_PROPERTY(bool, showGrid, N_READ showGrid N_WRITE setShowGrid)
    N_PROPERTY(Qt::PenStyle, gridStyle, N_READ gridStyle N_WRITE setGridStyle)
    N_PROPERTY(bool, sortingEnabled, N_READ isSortingEnabled N_WRITE setSortingEnabled)
    N_PROPERTY(bool, wordWrap, N_READ wordWrap N_WRITE setWordWrap)
#if QT_CONFIG(abstractbutton)
    N_PROPERTY(bool, cornerButtonEnabled, N_READ isCornerButtonEnabled N_WRITE setCornerButtonEnabled)
#endif
    N_END_PROPERTY
};
#endif

#ifdef QTABLEWIDGET_H
template <> class nwidget::MetaObject<QTableWidget> : public MetaObject<QAbstractItemView>
{
    N_OBJECT(QTableWidget, QAbstractItemView)

    N_BEGIN_PROPERTY
    N_PROPERTY(int, rowCount, N_READ rowCount N_WRITE setRowCount)
    N_PROPERTY(int, columnCount, N_READ columnCount N_WRITE setColumnCount)
    N_END_PROPERTY
};
#endif

#ifdef QTREEVIEW_H
template <> class nwidget::MetaObject<QTreeView> : public MetaObject<QAbstractItemView>
{
    N_OBJECT(QTreeView, QAbstractItemView)

    N_BEGIN_PROPERTY
    N_PROPERTY(int, autoExpandDelay, N_READ autoExpandDelay N_WRITE setAutoExpandDelay)
    N_PROPERTY(int, indentation, N_READ indentation N_WRITE setIndentation)
    N_PROPERTY(bool, rootIsDecorated, N_READ rootIsDecorated N_WRITE setRootIsDecorated)
    N_PROPERTY(bool, uniformRowHeights, N_READ uniformRowHeights N_WRITE setUniformRowHeights)
    N_PROPERTY(bool, itemsExpandable, N_READ itemsExpandable N_WRITE setItemsExpandable)
    N_PROPERTY(bool, sortingEnabled, N_READ isSortingEnabled N_WRITE setSortingEnabled)
    N_PROPERTY(bool, animated, N_READ isAnimated N_WRITE setAnimated)
    N_PROPERTY(bool, allColumnsShowFocus, N_READ allColumnsShowFocus N_WRITE setAllColumnsShowFocus)
    N_PROPERTY(bool, wordWrap, N_READ wordWrap N_WRITE setWordWrap)
    N_PROPERTY(bool, headerHidden, N_READ isHeaderHidden N_WRITE setHeaderHidden)
    N_PROPERTY(bool, expandsOnDoubleClick, N_READ expandsOnDoubleClick N_WRITE setExpandsOnDoubleClick)
    N_END_PROPERTY
};
#endif

#ifdef QTREEWIDGET_H
template <> class nwidget::MetaObject<QTreeWidget> : public MetaObject<QAbstractItemView>
{
    N_OBJECT(QTreeWidget, QAbstractItemView)

    N_BEGIN_PROPERTY
    N_PROPERTY(int, columnCount, N_READ columnCount N_WRITE setColumnCount)
    N_PROPERTY(int, topLevelItemCount, N_READ topLevelItemCount)
    N_END_PROPERTY
};
#endif

#ifdef QPLAINTEXTEDIT_H
template <> class nwidget::MetaObject<QPlainTextEdit> : public MetaObject<QAbstractScrollArea>
{
    N_OBJECT(QPlainTextEdit, QAbstractScrollArea)

    N_BEGIN_PROPERTY
    N_PROPERTY(bool, tabChangesFocus, N_READ tabChangesFocus N_WRITE setTabChangesFocus)
    N_PROPERTY(QString, documentTitle, N_READ documentTitle N_WRITE setDocumentTitle)
    N_PROPERTY(bool, undoRedoEnabled, N_READ isUndoRedoEnabled N_WRITE setUndoRedoEnabled)
    N_PROPERTY(QPlainTextEdit::LineWrapMode, lineWrapMode, N_READ lineWrapMode N_WRITE setLineWrapMode)
    N_PROPERTY(QTextOption::WrapMode, wordWrapMode, N_READ wordWrapMode N_WRITE setWordWrapMode)
    N_PROPERTY(bool, readOnly, N_READ isReadOnly N_WRITE setReadOnly)
    N_PROPERTY(QString, plainText, N_READ toPlainText N_WRITE setPlainText N_NOTIFY textChanged)
    N_PROPERTY(bool, overwriteMode, N_READ overwriteMode N_WRITE setOverwriteMode)
    N_PROPERTY(qreal, tabStopDistance, N_READ tabStopDistance N_WRITE setTabStopDistance)
    N_PROPERTY(int, cursorWidth, N_READ cursorWidth N_WRITE setCursorWidth)
    N_PROPERTY(Qt::TextInteractionFlags,
               textInteractionFlags,
               N_READ textInteractionFlags N_WRITE setTextInteractionFlags)
    N_PROPERTY(int, blockCount, N_READ blockCount)
    N_PROPERTY(int, maximumBlockCount, N_READ maximumBlockCount N_WRITE setMaximumBlockCount)
    N_PROPERTY(bool, backgroundVisible, N_READ backgroundVisible N_WRITE setBackgroundVisible)
    N_PROPERTY(bool, centerOnScroll, N_READ centerOnScroll N_WRITE setCenterOnScroll)
    N_PROPERTY(QString, placeholderText, N_READ placeholderText N_WRITE setPlaceholderText)
    N_END_PROPERTY
};
#endif

#ifdef QTEXTEDIT_H
template <> class nwidget::MetaObject<QTextEdit> : public MetaObject<QAbstractScrollArea>
{
    N_OBJECT(QTextEdit, QAbstractScrollArea)

    N_BEGIN_PROPERTY
    N_PROPERTY(QTextEdit::AutoFormatting, autoFormatting, N_READ autoFormatting N_WRITE setAutoFormatting)
    N_PROPERTY(bool, tabChangesFocus, N_READ tabChangesFocus N_WRITE setTabChangesFocus)
    N_PROPERTY(QString, documentTitle, N_READ documentTitle N_WRITE setDocumentTitle)
    N_PROPERTY(bool, undoRedoEnabled, N_READ isUndoRedoEnabled N_WRITE setUndoRedoEnabled)
    N_PROPERTY(QTextEdit::LineWrapMode, lineWrapMode, N_READ lineWrapMode N_WRITE setLineWrapMode)
    N_PROPERTY(QTextOption::WrapMode, wordWrapMode, N_READ wordWrapMode N_WRITE setWordWrapMode)
    N_PROPERTY(int, lineWrapColumnOrWidth, N_READ lineWrapColumnOrWidth N_WRITE setLineWrapColumnOrWidth)
    N_PROPERTY(bool, readOnly, N_READ isReadOnly N_WRITE setReadOnly)
    N_PROPERTY(QString, markdown, N_READ toMarkdown N_WRITE setMarkdown N_NOTIFY textChanged)
    N_PROPERTY(QString, html, N_READ toHtml N_WRITE setHtml N_NOTIFY textChanged)
    N_PROPERTY(QString, plainText, N_READ toPlainText N_WRITE setPlainText)
    N_PROPERTY(bool, overwriteMode, N_READ overwriteMode N_WRITE setOverwriteMode)
    N_PROPERTY(qreal, tabStopDistance, N_READ tabStopDistance N_WRITE setTabStopDistance)
    N_PROPERTY(bool, acceptRichText, N_READ acceptRichText N_WRITE setAcceptRichText)
    N_PROPERTY(int, cursorWidth, N_READ cursorWidth N_WRITE setCursorWidth)
    N_PROPERTY(Qt::TextInteractionFlags,
               textInteractionFlags,
               N_READ textInteractionFlags N_WRITE setTextInteractionFlags)
    N_PROPERTY(QTextDocument*, document, N_READ document N_WRITE setDocument)
    N_PROPERTY(QString, placeholderText, N_READ placeholderText N_WRITE setPlaceholderText)
    N_END_PROPERTY
};
#endif

#ifdef QTEXTBROWSER_H
template <> class nwidget::MetaObject<QTextBrowser> : public MetaObject<QTextEdit>
{
    N_OBJECT(QTextBrowser, QTextEdit)

    N_BEGIN_PROPERTY
    N_PROPERTY(QUrl, source, N_READ source N_WRITE setSource)
    N_PROPERTY(QTextDocument::ResourceType, sourceType, N_READ sourceType)
    N_PROPERTY(QStringList, searchPaths, N_READ searchPaths N_WRITE setSearchPaths)
    N_PROPERTY(bool, openExternalLinks, N_READ openExternalLinks N_WRITE setOpenExternalLinks)
    N_PROPERTY(bool, openLinks, N_READ openLinks N_WRITE setOpenLinks)
    N_END_PROPERTY
};
#endif

#ifdef QTOOLBOX_H
template <> class nwidget::MetaObject<QToolBox> : public MetaObject<QFrame>
{
    N_OBJECT(QToolBox, QFrame)

    N_BEGIN_PROPERTY
    N_PROPERTY(int, currentIndex, N_READ currentIndex N_WRITE setCurrentIndex N_NOTIFY currentChanged)
    N_PROPERTY(int, count, N_READ count)
    N_END_PROPERTY
};
#endif

#ifdef QSPLITTER_H
template <> class nwidget::MetaObject<QSplitter> : public MetaObject<QFrame>
{
    N_OBJECT(QSplitter, QFrame)

    N_BEGIN_PROPERTY
    N_PROPERTY(Qt::Orientation, orientation, N_READ orientation N_WRITE setOrientation)
    N_PROPERTY(bool, opaqueResize, N_READ opaqueResize N_WRITE setOpaqueResize)
    N_PROPERTY(int, handleWidth, N_READ handleWidth N_WRITE setHandleWidth)
    N_PROPERTY(bool, childrenCollapsible, N_READ childrenCollapsible N_WRITE setChildrenCollapsible)
    N_END_PROPERTY
};
#endif

#ifdef QABSTRACTSLIDER_H
template <> class nwidget::MetaObject<QAbstractSlider> : public MetaObject<QWidget>
{
    N_OBJECT(QAbstractSlider, QWidget)

    N_BEGIN_PROPERTY
    N_PROPERTY(int, minimum, N_READ minimum N_WRITE setMinimum)
    N_PROPERTY(int, maximum, N_READ maximum N_WRITE setMaximum)
    N_PROPERTY(int, singleStep, N_READ singleStep N_WRITE setSingleStep)
    N_PROPERTY(int, pageStep, N_READ pageStep N_WRITE setPageStep)
    N_PROPERTY(int, value, N_READ value N_WRITE setValue N_NOTIFY valueChanged)
    N_PROPERTY(int, sliderPosition, N_READ sliderPosition N_WRITE setSliderPosition N_NOTIFY sliderMoved)
    N_PROPERTY(bool, tracking, N_READ hasTracking N_WRITE setTracking)
    N_PROPERTY(Qt::Orientation, orientation, N_READ orientation N_WRITE setOrientation)
    N_PROPERTY(bool, invertedAppearance, N_READ invertedAppearance N_WRITE setInvertedAppearance)
    N_PROPERTY(bool, invertedControls, N_READ invertedControls N_WRITE setInvertedControls)
    N_END_PROPERTY
};
#endif

#ifdef QDIAL_H
template <> class nwidget::MetaObject<QDial> : public MetaObject<QAbstractSlider>
{
    N_OBJECT(QDial, QAbstractSlider)

    N_BEGIN_PROPERTY
    N_PROPERTY(bool, wrapping, N_READ wrapping N_WRITE setWrapping)
    N_PROPERTY(int, notchSize, N_READ notchSize)
    N_PROPERTY(qreal, notchTarget, N_READ notchTarget N_WRITE setNotchTarget)
    N_PROPERTY(bool, notchesVisible, N_READ notchesVisible N_WRITE setNotchesVisible)
    N_END_PROPERTY
};
#endif

#ifdef QSLIDER_H
template <> class nwidget::MetaObject<QSlider> : public MetaObject<QAbstractSlider>
{
    N_OBJECT(QSlider, QAbstractSlider)

    N_BEGIN_PROPERTY
    N_PROPERTY(QSlider::TickPosition, tickPosition, N_READ tickPosition N_WRITE setTickPosition)
    N_PROPERTY(int, tickInterval, N_READ tickInterval N_WRITE setTickInterval)
    N_END_PROPERTY
};
#endif

#ifdef QSCROLLBAR_H
template <> class nwidget::MetaObject<QScrollBar> : public MetaObject<QAbstractSlider>
{
    N_OBJECT(QScrollBar, QAbstractSlider)
};
#endif

#ifdef QABSTRACTSPINBOX_H
template <> class nwidget::MetaObject<QAbstractSpinBox> : public MetaObject<QWidget>
{
    N_OBJECT(QAbstractSpinBox, QWidget)

    N_BEGIN_PROPERTY
    N_PROPERTY(bool, wrapping, N_READ wrapping N_WRITE setWrapping)
    N_PROPERTY(bool, frame, N_READ hasFrame N_WRITE setFrame)
    N_PROPERTY(Qt::Alignment, alignment, N_READ alignment N_WRITE setAlignment)
    N_PROPERTY(bool, readOnly, N_READ isReadOnly N_WRITE setReadOnly)
    N_PROPERTY(QAbstractSpinBox::ButtonSymbols, buttonSymbols, N_READ buttonSymbols N_WRITE setButtonSymbols)
    N_PROPERTY(QString, specialValueText, N_READ specialValueText N_WRITE setSpecialValueText)
    N_PROPERTY(QString, text, N_READ text)
    N_PROPERTY(bool, accelerated, N_READ isAccelerated N_WRITE setAccelerated)
    N_PROPERTY(QAbstractSpinBox::CorrectionMode, correctionMode, N_READ correctionMode N_WRITE setCorrectionMode)
    N_PROPERTY(bool, acceptableInput, N_READ hasAcceptableInput)
    N_PROPERTY(bool, keyboardTracking, N_READ keyboardTracking N_WRITE setKeyboardTracking)
    N_PROPERTY(bool, showGroupSeparator, N_READ isGroupSeparatorShown N_WRITE setGroupSeparatorShown)
    N_END_PROPERTY
};
#endif

#ifdef QDATETIMEEDIT_H
template <> class nwidget::MetaObject<QDateTimeEdit> : public MetaObject<QAbstractSpinBox>
{
    N_OBJECT(QDateTimeEdit, QAbstractSpinBox)

    N_BEGIN_PROPERTY
    N_PROPERTY(QDateTime, dateTime, N_READ dateTime N_WRITE setDateTime N_NOTIFY dateTimeChanged)
    N_PROPERTY(QDate, date, N_READ date N_WRITE setDate N_NOTIFY dateChanged)
    N_PROPERTY(QTime, time, N_READ time N_WRITE setTime N_NOTIFY timeChanged)
    N_PROPERTY(QDateTime,
               maximumDateTime,
               N_READ maximumDateTime N_WRITE setMaximumDateTime N_RESET clearMaximumDateTime)
    N_PROPERTY(QDateTime,
               minimumDateTime,
               N_READ minimumDateTime N_WRITE setMinimumDateTime N_RESET clearMinimumDateTime)
    N_PROPERTY(QDate, maximumDate, N_READ maximumDate N_WRITE setMaximumDate N_RESET clearMaximumDate)
    N_PROPERTY(QDate, minimumDate, N_READ minimumDate N_WRITE setMinimumDate N_RESET clearMinimumDate)
    N_PROPERTY(QTime, maximumTime, N_READ maximumTime N_WRITE setMaximumTime N_RESET clearMaximumTime)
    N_PROPERTY(QTime, minimumTime, N_READ minimumTime N_WRITE setMinimumTime N_RESET clearMinimumTime)
    N_PROPERTY(QDateTimeEdit::Section, currentSection, N_READ currentSection N_WRITE setCurrentSection)
    N_PROPERTY(QDateTimeEdit::Sections, displayedSections, N_READ displayedSections)
    N_PROPERTY(QString, displayFormat, N_READ displayFormat N_WRITE setDisplayFormat)
    N_PROPERTY(bool, calendarPopup, N_READ calendarPopup N_WRITE setCalendarPopup)
    N_PROPERTY(int, currentSectionIndex, N_READ currentSectionIndex N_WRITE setCurrentSectionIndex)
    N_PROPERTY(int, sectionCount, N_READ sectionCount)

    N_UNTIL(6, 7, N_PROPERTY(Qt::TimeSpec, timeSpec, N_READ timeSpec N_WRITE setTimeSpec))
    N_SINCE(6, 7, N_PROPERTY(QTimeZone, timeZone, N_READ timeZone N_WRITE setTimeZone))

    N_END_PROPERTY
};

template <> class nwidget::MetaObject<QDateEdit> : public MetaObject<QDateTimeEdit>
{
    N_OBJECT(QDateEdit, QDateTimeEdit)

    N_BEGIN_PROPERTY
    N_PROPERTY(QDate, date, N_READ date N_WRITE setDate N_NOTIFY userDateChanged)
    N_END_PROPERTY
};

template <> class nwidget::MetaObject<QTimeEdit> : public MetaObject<QDateTimeEdit>
{
    N_OBJECT(QTimeEdit, QDateTimeEdit)

    N_BEGIN_PROPERTY
    N_PROPERTY(QTime, time, N_READ time N_WRITE setTime N_NOTIFY userTimeChanged)
    N_END_PROPERTY
};
#endif

#ifdef QSPINBOX_H
template <> class nwidget::MetaObject<QSpinBox> : public MetaObject<QAbstractSpinBox>
{
    N_OBJECT(QSpinBox, QAbstractSpinBox)

    N_BEGIN_PROPERTY
    N_PROPERTY(QString, suffix, N_READ suffix N_WRITE setSuffix)
    N_PROPERTY(QString, prefix, N_READ prefix N_WRITE setPrefix)
    N_PROPERTY(QString, cleanText, N_READ cleanText)
    N_PROPERTY(int, minimum, N_READ minimum N_WRITE setMinimum)
    N_PROPERTY(int, maximum, N_READ maximum N_WRITE setMaximum)
    N_PROPERTY(int, singleStep, N_READ singleStep N_WRITE setSingleStep)
    N_PROPERTY(QSpinBox::StepType, stepType, N_READ stepType N_WRITE setStepType)
    N_PROPERTY(int, value, N_READ value N_WRITE setValue N_NOTIFY valueChanged)
    N_PROPERTY(int, displayIntegerBase, N_READ displayIntegerBase N_WRITE setDisplayIntegerBase)
    N_END_PROPERTY
};

template <> class nwidget::MetaObject<QDoubleSpinBox> : public MetaObject<QAbstractSpinBox>
{
    N_OBJECT(QDoubleSpinBox, QAbstractSpinBox)

    N_BEGIN_PROPERTY
    N_PROPERTY(QString, suffix, N_READ suffix N_WRITE setSuffix)
    N_PROPERTY(QString, prefix, N_READ prefix N_WRITE setPrefix)
    N_PROPERTY(QString, cleanText, N_READ cleanText)
    N_PROPERTY(int, decimals, N_READ decimals N_WRITE setDecimals)
    N_PROPERTY(double, minimum, N_READ minimum N_WRITE setMinimum)
    N_PROPERTY(double, maximum, N_READ maximum N_WRITE setMaximum)
    N_PROPERTY(double, singleStep, N_READ singleStep N_WRITE setSingleStep)
    N_PROPERTY(QSpinBox::StepType, stepType, N_READ stepType N_WRITE setStepType)
    N_PROPERTY(double, value, N_READ value N_WRITE setValue N_NOTIFY valueChanged)
    N_END_PROPERTY
};
#endif

#ifdef QCOMBOBOX_H
template <> class nwidget::MetaObject<QComboBox> : public MetaObject<QWidget>
{
    N_OBJECT(QComboBox, QWidget)

    N_BEGIN_PROPERTY
    N_PROPERTY(bool, editable, N_READ isEditable N_WRITE setEditable)
    N_PROPERTY(int, count, N_READ count)
    N_PROPERTY(QString, currentText, N_READ currentText N_WRITE setCurrentText N_NOTIFY currentTextChanged)
    N_PROPERTY(int, currentIndex, N_READ currentIndex N_WRITE setCurrentIndex N_NOTIFY currentIndexChanged)
    N_PROPERTY(QVariant, currentData, N_READ currentData)
    N_PROPERTY(int, maxVisibleItems, N_READ maxVisibleItems N_WRITE setMaxVisibleItems)
    N_PROPERTY(int, maxCount, N_READ maxCount N_WRITE setMaxCount)
    N_PROPERTY(QComboBox::InsertPolicy, insertPolicy, N_READ insertPolicy N_WRITE setInsertPolicy)
    N_PROPERTY(QComboBox::SizeAdjustPolicy, sizeAdjustPolicy, N_READ sizeAdjustPolicy N_WRITE setSizeAdjustPolicy)
    N_PROPERTY(int, minimumContentsLength, N_READ minimumContentsLength N_WRITE setMinimumContentsLength)
    N_PROPERTY(QSize, iconSize, N_READ iconSize N_WRITE setIconSize)
    N_PROPERTY(QString, placeholderText, N_READ placeholderText N_WRITE setPlaceholderText)
    N_PROPERTY(bool, duplicatesEnabled, N_READ duplicatesEnabled N_WRITE setDuplicatesEnabled)
    N_PROPERTY(bool, frame, N_READ hasFrame N_WRITE setFrame)
    N_PROPERTY(int, modelColumn, N_READ modelColumn N_WRITE setModelColumn)
    N_END_PROPERTY
};
#endif

#ifdef QGROUPBOX_H
template <> class nwidget::MetaObject<QGroupBox> : public MetaObject<QWidget>
{
    N_OBJECT(QGroupBox, QWidget)

    N_BEGIN_PROPERTY
    N_PROPERTY(QString, title, N_READ title N_WRITE setTitle)
    N_PROPERTY(Qt::Alignment, alignment, N_READ alignment N_WRITE setAlignment)
    N_PROPERTY(bool, flat, N_READ isFlat N_WRITE setFlat)
    N_PROPERTY(bool, checkable, N_READ isCheckable N_WRITE setCheckable)
    N_PROPERTY(bool, checked, N_READ isChecked N_WRITE setChecked N_NOTIFY toggled)
    N_END_PROPERTY
};
#endif

#ifdef QLABEL_H
template <> class nwidget::MetaObject<QLabel> : public MetaObject<QWidget>
{
    N_OBJECT(QLabel, QWidget)

    N_BEGIN_PROPERTY
    N_PROPERTY(QString, text, N_READ text N_WRITE setText)
    N_PROPERTY(Qt::TextFormat, textFormat, N_READ textFormat N_WRITE setTextFormat)
    N_PROPERTY(QPixmap, pixmap, N_READ pixmap N_WRITE setPixmap)
    N_PROPERTY(bool, scaledContents, N_READ hasScaledContents N_WRITE setScaledContents)
    N_PROPERTY(Qt::Alignment, alignment, N_READ alignment N_WRITE setAlignment)
    N_PROPERTY(bool, wordWrap, N_READ wordWrap N_WRITE setWordWrap)
    N_PROPERTY(int, margin, N_READ margin N_WRITE setMargin)
    N_PROPERTY(int, indent, N_READ indent N_WRITE setIndent)
    N_PROPERTY(bool, openExternalLinks, N_READ openExternalLinks N_WRITE setOpenExternalLinks)
    N_PROPERTY(Qt::TextInteractionFlags, InteractionFlags, N_READ textInteractionFlags N_WRITE setTextInteractionFlags)
    N_PROPERTY(bool, hasSelectedText, N_READ hasSelectedText)
    N_PROPERTY(QString, selectedText, N_READ selectedText)
    N_END_PROPERTY
};
#endif

#ifdef QLINEEDIT_H
template <> class nwidget::MetaObject<QLineEdit> : public MetaObject<QWidget>
{
    N_OBJECT(QLineEdit, QWidget)

    N_BEGIN_PROPERTY
    N_PROPERTY(QString, inputMask, N_READ inputMask N_WRITE setInputMask)
    N_PROPERTY(QString, text, N_READ text N_WRITE setText N_NOTIFY textChanged)
    N_PROPERTY(int, maxLength, N_READ maxLength N_WRITE setMaxLength)
    N_PROPERTY(bool, frame, N_READ hasFrame N_WRITE setFrame)
    N_PROPERTY(QLineEdit::EchoMode, echoMode, N_READ echoMode N_WRITE setEchoMode)
    N_PROPERTY(QString, displayText, N_READ displayText)
    N_PROPERTY(int, cursorPosition, N_READ cursorPosition N_WRITE setCursorPosition)
    N_PROPERTY(Qt::Alignment, alignment, N_READ alignment N_WRITE setAlignment)
    N_PROPERTY(bool, modified, N_READ isModified N_WRITE setModified)
    N_PROPERTY(bool, hasSelectedText, N_READ hasSelectedText)
    N_PROPERTY(QString, selectedText, N_READ selectedText)
    N_PROPERTY(bool, dragEnabled, N_READ dragEnabled N_WRITE setDragEnabled)
    N_PROPERTY(bool, readOnly, N_READ isReadOnly N_WRITE setReadOnly)
    N_PROPERTY(bool, undoAvailable, N_READ isUndoAvailable)
    N_PROPERTY(bool, redoAvailable, N_READ isRedoAvailable)
    N_PROPERTY(bool, acceptableInput, N_READ hasAcceptableInput)
    N_PROPERTY(QString, placeholderText, N_READ placeholderText N_WRITE setPlaceholderText)
    N_PROPERTY(Qt::CursorMoveStyle, cursorMoveStyle, N_READ cursorMoveStyle N_WRITE setCursorMoveStyle)
    N_PROPERTY(bool, clearButtonEnabled, N_READ isClearButtonEnabled N_WRITE setClearButtonEnabled)
    N_END_PROPERTY
};
#endif

#ifdef QMENU_H
template <> class nwidget::MetaObject<QMenu> : public MetaObject<QWidget>
{
    N_OBJECT(QMenu, QWidget)

    N_BEGIN_PROPERTY
    N_PROPERTY(bool, tearOffEnabled, N_READ isTearOffEnabled N_WRITE setTearOffEnabled)
    N_PROPERTY(QString, title, N_READ title N_WRITE setTitle)
    N_PROPERTY(QIcon, icon, N_READ icon N_WRITE setIcon)
    N_PROPERTY(bool, separatorsCollapsible, N_READ separatorsCollapsible N_WRITE setSeparatorsCollapsible)
    N_PROPERTY(bool, toolTipsVisible, N_READ toolTipsVisible N_WRITE setToolTipsVisible)
    N_END_PROPERTY
};
#endif

#ifdef QMENUBAR_H
template <> class nwidget::MetaObject<QMenuBar> : public MetaObject<QWidget>
{
    N_OBJECT(QMenuBar, QWidget)

    N_BEGIN_PROPERTY
    N_PROPERTY(bool, defaultUp, N_READ isDefaultUp N_WRITE setDefaultUp)
    N_PROPERTY(bool, nativeMenuBar, N_READ isNativeMenuBar N_WRITE setNativeMenuBar)
    N_END_PROPERTY
};
#endif

#ifdef QPROGRESSBAR_H
template <> class nwidget::MetaObject<QProgressBar> : public MetaObject<QWidget>
{
    N_OBJECT(QProgressBar, QWidget)

    N_BEGIN_PROPERTY
    N_PROPERTY(int, minimum, N_READ minimum N_WRITE setMinimum)
    N_PROPERTY(int, maximum, N_READ maximum N_WRITE setMaximum)
    N_PROPERTY(QString, text, N_READ text)
    N_PROPERTY(int, value, N_READ value N_WRITE setValue N_NOTIFY valueChanged)
    N_PROPERTY(Qt::Alignment, alignment, N_READ alignment N_WRITE setAlignment)
    N_PROPERTY(bool, textVisible, N_READ isTextVisible N_WRITE setTextVisible)
    N_PROPERTY(Qt::Orientation, orientation, N_READ orientation N_WRITE setOrientation)
    N_PROPERTY(bool, invertedAppearance, N_READ invertedAppearance N_WRITE setInvertedAppearance)
    N_PROPERTY(QProgressBar::Direction, textDirection, N_READ textDirection N_WRITE setTextDirection)
    N_PROPERTY(QString, format, N_READ format N_WRITE setFormat)
    N_END_PROPERTY
};
#endif

#ifdef QTABBAR_H
template <> class nwidget::MetaObject<QTabBar> : public MetaObject<QWidget>
{
    N_OBJECT(QTabBar, QWidget)

    N_BEGIN_PROPERTY
    N_PROPERTY(QTabBar::Shape, shape, N_READ shape N_WRITE setShape)
    N_PROPERTY(int, currentIndex, N_READ currentIndex N_WRITE setCurrentIndex N_NOTIFY currentChanged)
    N_PROPERTY(int, count, N_READ count)
    N_PROPERTY(bool, drawBase, N_READ drawBase N_WRITE setDrawBase)
    N_PROPERTY(QSize, iconSize, N_READ iconSize N_WRITE setIconSize)
    N_PROPERTY(Qt::TextElideMode, elideMode, N_READ elideMode N_WRITE setElideMode)
    N_PROPERTY(bool, usesScrollButtons, N_READ usesScrollButtons N_WRITE setUsesScrollButtons)
    N_PROPERTY(bool, tabsClosable, N_READ tabsClosable N_WRITE setTabsClosable)
    N_PROPERTY(QTabBar::SelectionBehavior,
               selectionBehaviorOnRemove,
               N_READ selectionBehaviorOnRemove N_WRITE setSelectionBehaviorOnRemove)
    N_PROPERTY(bool, expanding, N_READ expanding N_WRITE setExpanding)
    N_PROPERTY(bool, movable, N_READ isMovable N_WRITE setMovable)
    N_PROPERTY(bool, documentMode, N_READ documentMode N_WRITE setDocumentMode)
    N_PROPERTY(bool, autoHide, N_READ autoHide N_WRITE setAutoHide)
    N_PROPERTY(bool, changeCurrentOnDrag, N_READ changeCurrentOnDrag N_WRITE setChangeCurrentOnDrag)
    N_END_PROPERTY
};
#endif

#ifdef QTABWIDGET_H
template <> class nwidget::MetaObject<QTabWidget> : public MetaObject<QWidget>
{
    N_OBJECT(QTabWidget, QWidget)

    N_BEGIN_PROPERTY
    N_PROPERTY(QTabWidget::TabPosition, tabPosition, N_READ tabPosition N_WRITE setTabPosition)
    N_PROPERTY(QTabWidget::TabShape, tabShape, N_READ tabShape N_WRITE setTabShape)
    N_PROPERTY(bool, documentMode, N_READ documentMode N_WRITE setDocumentMode)
    N_PROPERTY(bool, usesScrollButtons, N_READ usesScrollButtons N_WRITE setUsesScrollButtons)
    N_PROPERTY(bool, tabsClosable, N_READ tabsClosable N_WRITE setTabsClosable)
    N_END_PROPERTY
};
#endif

#endif // N_WIDGET_META_OBJECTS_H
