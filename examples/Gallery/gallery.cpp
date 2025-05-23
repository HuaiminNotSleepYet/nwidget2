#include "gallery.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QCommandLinkButton>
#include <QDateEdit>
#include <QDesktopServices>
#include <QDial>
#include <QDialogButtonBox>
#include <QFileSystemModel>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLibraryInfo>
#include <QLineEdit>
#include <QListView>
#include <QMenu>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScreen>
#include <QScrollBar>
#include <QShortcut>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QStyle>
#include <QStyleFactory>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTimer>
#include <QToolBox>
#include <QToolButton>
#include <QTreeView>
#include <QUrl>
#include <QWindow>

#include "nwidget/builders.h"

static QString className(const QObject* o) { return QString::fromUtf8(o->metaObject()->className()); }

static QString helpUrl(const QString& page)
{
    QString result;
    QTextStream(&result) << "https://doc.qt.io/qt-" << QT_VERSION_MAJOR << '/' << page << ".html";
    return result;
}

static QString helpUrl(const QWidget* w) { return helpUrl(className(w).toLower()); }

static void launchHelp(const QWidget* w) { QDesktopServices::openUrl(helpUrl(w)); }

static void launchModuleHelp() { QDesktopServices::openUrl(helpUrl(QLatin1String("qtwidgets-index"))); }

QTextStream& operator<<(QTextStream& str, const QRect& r)
{
    str << r.width() << 'x' << r.height() << Qt::forcesign << r.x() << r.y() << Qt::noforcesign;
    return str;
}

static QString highDpiScaleFactorRoundingPolicy()
{
    QString result;
    QDebug(&result) << QGuiApplication::highDpiScaleFactorRoundingPolicy();
    if (result.endsWith(QLatin1Char(')')))
        result.chop(1);
    const int lastSep = result.lastIndexOf(QLatin1String("::"));
    if (lastSep != -1)
        result.remove(0, lastSep + 2);
    return result;
}

Gallery::Gallery(QWidget* parent)
    : QDialog(parent)
{
    using namespace nwidget;

    QStringList   styleNames       = QStyleFactory::keys();
    const QString defaultStyleName = QApplication::style()->objectName();
    for (int i = 1, size = styleNames.size(); i < size; ++i) {
        if (defaultStyleName.compare(styleNames.at(i), Qt::CaseInsensitive) == 0) {
            styleNames.swapItemsAt(0, i);
            break;
        }
    }

    auto fileSystemModel = new QFileSystemModel(this);
    fileSystemModel->setRootPath(QDir::homePath());

    auto listModel = new QStandardItemModel(0, 1, this);
    listModel->appendRow(new QStandardItem(
        QIcon(QLatin1String(":/qt-project.org/styles/commonstyle/images/diropen-128.png")), tr("Directory")));
    listModel->appendRow(new QStandardItem(
        QIcon(QLatin1String(":/qt-project.org/styles/commonstyle/images/computer-32.png")), tr("Computer")));

    const QString plainText = tr("Twinkle, twinkle, little star,\n"
                                 "How I wonder what you are.\n"
                                 "Up above the world so high,\n"
                                 "Like a diamond in the sky.\n"
                                 "Twinkle, twinkle, little star,\n"
                                 "How I wonder what you are!\n");
    QString       richText  = QLatin1String("<html><head/><body><i>");
    for (const auto& line : QStringView{plainText}.split(QLatin1Char('\n')))
        richText += QString::fromLatin1("<center>%1</center>").arg(line);
    richText += QLatin1String("</i></body></html>");

    progressBar                     = new QProgressBar;
    systemInfoTextBrowser           = new QTextBrowser;
    auto disableWidgetsCheckBox     = new QCheckBox;
    auto buttonsGroupBox            = new QGroupBox;
    auto itemViewTabWidget          = new QTabWidget;
    auto simpleInputWidgetsGroupBox = new QGroupBox;
    auto textToolBox                = new QToolBox;

    setLayout(GridLayout{
        {0,
         0,
         1,
         2,
         HBoxLayout{
             Label(tr("Style:")),
             ComboBox(styleNames).onTextActivated(this, &Gallery::changeStyle),
             BoxLayout::Stretch(),
             Label(tr("Press F1 over a widget to see Documentation")),
             BoxLayout::Stretch(),
             CheckBox(disableWidgetsCheckBox).text(tr("Disable widgets")),
         }},
        {1,
         0,
         GroupBox(buttonsGroupBox)
             .title(tr("Buttons"))
             .layout(HBoxLayout{VBoxLayout{
                                    PushButton(tr("Default Push Button")).default_(true),
                                    PushButton(tr("Toggle Push Button")).checkable(true),
                                    PushButton(tr("Flat Push Button")).flat(true),
                                    HBoxLayout{
                                        ToolButton().text("Tool Button"),
                                        ToolButton()
                                            .text(tr("Menu Button"))
                                            .popupMode(QToolButton::InstantPopup)
                                            .menu(Menu{
                                                Action(tr("Option")),
                                                Menu::Separator(),
                                                Action(tr("Checkable Option")),
                                            }),
                                    },
                                    CommandLinkButton(tr("Command Link Button"), tr("Description")),
                                    BoxLayout::Stretch(),
                                },
                                VBoxLayout{
                                    RadioButton(tr("Radio button 1")).checked(true),
                                    RadioButton(tr("Radio button 2")),
                                    RadioButton(tr("Radio button 3")),
                                    CheckBox(tr("Tri-state check box")).tristate(true).checkState(Qt::PartiallyChecked),
                                    BoxLayout::Stretch(),
                                }})},
        {1,
         1,
         GroupBox(simpleInputWidgetsGroupBox)
             .title(tr("Simple Input Widgets"))
             .layout(GridLayout{
                 {0, 0, 1, 2, LineEdit("zhang").clearButtonEnabled(true).echoMode(QLineEdit::Password)},
                 {1, 0, 1, 2, SpinBox().value(50)},
                 {2, 0, 1, 2, DateTimeEdit().dateTime(QDateTime::currentDateTime())},
                 {3, 0, Slider(Qt::Horizontal).value(40)},
                 {4, 0, ScrollBar(Qt::Horizontal).value(60)},
                 {3, 1, 2, 1, Dial().value(30).notchesVisible(true)},
             })
             .checkable(true)
             .checked(true)},
        {2,
         0,
         TabWidget(itemViewTabWidget)
             .items({
                 {tr("Tree View"), TreeView().model(fileSystemModel)},
                 {tr("Table"), TableWidget().columnCount(10).rowCount(10)},
                 {tr("List"), ListView().model(listModel)},
                 {tr("Icon Mode List"), ListView().model(listModel).viewMode(QListView::IconMode)},
             })},
        {2,
         1,
         ToolBox(textToolBox)
             .items({
                 {tr("Text Edit"), TextEdit(richText)},
                 {tr("Plain Text Edit"), PlainTextEdit(plainText)},
                 {tr("Text Browser"), TextBrowser(systemInfoTextBrowser)},
             })},
        {3, 0, 1, 2, ProgressBar(progressBar).range(0, 10000)},
        {4,
         0,
         1,
         2,
         DialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Close)
             .onHelpRequested(this, launchModuleHelp)
             .onRejected(this, &QDialog::reject)},
    });

    connect(disableWidgetsCheckBox, &QCheckBox::toggled, buttonsGroupBox, &QWidget::setDisabled);
    connect(disableWidgetsCheckBox, &QCheckBox::toggled, itemViewTabWidget, &QWidget::setDisabled);
    connect(disableWidgetsCheckBox, &QCheckBox::toggled, simpleInputWidgetsGroupBox, &QWidget::setDisabled);
    connect(disableWidgetsCheckBox, &QCheckBox::toggled, textToolBox, &QWidget::setDisabled);

    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Gallery::advanceProgressBar);
    timer->start(1000);

    new QShortcut(QKeySequence::HelpContents, this, this, &Gallery::helpOnCurrentWidget);
}

void Gallery::setVisible(bool visible)
{
    QDialog::setVisible(visible);
    if (visible) {
        connect(windowHandle(), &QWindow::screenChanged, this, &Gallery::updateSystemInfo);
        updateSystemInfo();
    }
}

void Gallery::changeStyle(const QString& styleName) { QApplication::setStyle(QStyleFactory::create(styleName)); }

void Gallery::advanceProgressBar()
{
    int curVal = progressBar->value();
    int maxVal = progressBar->maximum();
    progressBar->setValue(curVal + (maxVal - curVal) / 100);
}

void Gallery::updateSystemInfo()
{
    QString     systemInfo;
    QTextStream str(&systemInfo);
    str << "<html><head/><body><h3>Build</h3><p>" << QLibraryInfo::build() << "</p>"
        << "<h3>Operating System</h3><p>" << QSysInfo::prettyProductName() << "</p>"
        << "<h3>Screens</h3><p>High DPI scale factor rounding policy: " << highDpiScaleFactorRoundingPolicy()
        << "</p><ol>";
    const auto screens = QGuiApplication::screens();
    for (auto screen : screens) {
        const bool current = screen == this->screen();
        str << "<li>";
        if (current)
            str << "<i>";
        str << '"' << screen->name() << "\" " << screen->geometry() << ", " << screen->logicalDotsPerInchX()
            << "DPI, DPR=" << screen->devicePixelRatio();
        if (current)
            str << "</i>";
        str << "</li>";
    }
    str << "</ol></body></html>";
    systemInfoTextBrowser->setHtml(systemInfo);
}

void Gallery::helpOnCurrentWidget()
{
    for (auto w = QApplication::widgetAt(QCursor::pos(screen())); w; w = w->parentWidget()) {
        launchHelp(w);
    }
}
