#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "UiCommon.h"
#include "MonthWidget.h"
#include "ScheduleEdit.h"
#include "ScheduleManagerWidget.h"
#include "CategoryModifyWidget.h"
#include "SettingsWidget.h"

/*** 메인윈도우 ***/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentYear(QDate::currentDate().year())
    , m_currentMonth(QDate::currentDate().month())
    , m_xOffset(0)
    , m_currentHeaderH(kBaseHeaderH)
    , m_isExpanded(false)
    , m_isMiniMode(false)
    , m_isPinned(false)
{
    // 1. 저장된 테마 설정 불러오기 (UI 초기화 전 수행)
    QSettings settings("MyCompany", "CalendarProject");
    int savedTheme = settings.value("theme", static_cast<int>(StyleHelper::Theme::Default)).toInt();
    StyleHelper::currentTheme = static_cast<StyleHelper::Theme>(savedTheme);

    ui->setupUi(this);
    setMinimumSize(400, 500); // 최소 높이 강제 (겹침 방지)

    if (!DatabaseManager::instance().initDatabase("calendar_data.db")) {
        qDebug() << "Database initialization failed.";
    }

    // ── 일반 모드 UI 구성 ──
    m_headerBar = new QWidget(ui->centralwidget);
    m_headerBar->setStyleSheet("background-color: white; border-bottom: 1px solid #E0E0E0;");
    QHBoxLayout* headerLayout = new QHBoxLayout(m_headerBar);
    headerLayout->setContentsMargins(15, 0, 15, 0);
    headerLayout->setSpacing(0);

    // [좌측 영역] : 카테고리 필터
    QWidget* leftGroup = new QWidget(m_headerBar);
    m_categoryLayout = new QHBoxLayout(leftGroup);
    m_categoryLayout->setContentsMargins(0, 0, 0, 0);
    m_categoryLayout->setSpacing(6);
    m_categoryLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // [중앙 영역] : 월 표시 및 이동 버튼
    QWidget* centerGroup = new QWidget(m_headerBar);
    QHBoxLayout* centerLayout = new QHBoxLayout(centerGroup);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(8);
    centerLayout->setAlignment(Qt::AlignCenter);

    m_prevBtn = new QPushButton("◀", centerGroup);
    m_prevBtn->setFixedSize(30, 30);
    m_prevBtn->setCursor(Qt::PointingHandCursor);

    m_monthLabel = new QLabel(centerGroup);
    m_monthLabel->setAlignment(Qt::AlignCenter);

    m_nextBtn = new QPushButton("▶", centerGroup);
    m_nextBtn->setFixedSize(30, 30);
    m_nextBtn->setCursor(Qt::PointingHandCursor);

    centerLayout->addWidget(m_prevBtn);
    centerLayout->addWidget(m_monthLabel);
    centerLayout->addWidget(m_nextBtn);

    // [우측 영역] 오늘 버튼
    QWidget* rightGroup = new QWidget(m_headerBar);
    QHBoxLayout* rightLayout = new QHBoxLayout(rightGroup);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(8);
    rightLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_todayBtn = new QPushButton("오늘", rightGroup);
    m_todayBtn->setFixedSize(50, 28);
    m_todayBtn->setCursor(Qt::PointingHandCursor);

    QPushButton* mainSettingsBtn = new QPushButton("⚙", rightGroup);
    mainSettingsBtn->setFixedSize(32, 28);
    mainSettingsBtn->setCursor(Qt::PointingHandCursor);
    mainSettingsBtn->setObjectName("mainSettingsBtn"); 
    connect(mainSettingsBtn, &QPushButton::clicked, this, &MainWindow::openSettingsWidget);

    rightLayout->addWidget(m_todayBtn);
    rightLayout->addWidget(mainSettingsBtn);

    headerLayout->addWidget(leftGroup, 1);
    headerLayout->addWidget(centerGroup, 0);
    headerLayout->addWidget(rightGroup, 1);

    connect(m_prevBtn, &QPushButton::clicked, this, &MainWindow::prevMonth);
    connect(m_nextBtn, &QPushButton::clicked, this, &MainWindow::nextMonth);
    connect(m_todayBtn, &QPushButton::clicked, this, &MainWindow::goToday);

    m_overflowWidget = new QWidget(ui->centralwidget);
    m_overflowLayout = new QGridLayout(m_overflowWidget);
    m_overflowWidget->hide();

    m_moreBtn = new QPushButton("...", m_headerBar);
    m_moreBtn->setCheckable(true);
    m_moreBtn->setFixedSize(30, 22);
    connect(m_moreBtn, &QPushButton::clicked, this, &MainWindow::toggleOverflow);

    m_container = new QWidget(ui->centralwidget);
    for (int i = 0; i < 3; ++i) {
        m_months[i] = new MonthWidget(m_container);
        connect(m_months[i], &MonthWidget::dayDoubleClicked, this, &MainWindow::handleDayDoubleClicked);
        connect(m_months[i], &MonthWidget::addRequested, this, &MainWindow::handleDayAddRequested);
    }

    m_scrollTimer = new QTimer(this);
    m_scrollTimer->setSingleShot(true);
    connect(m_scrollTimer, &QTimer::timeout, this, &MainWindow::finishScroll);

    m_animation = new QPropertyAnimation(m_container, "pos", this);
    m_animation->setDuration(300);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    // ── 미니 모드 UI 구성 ──
    m_miniWidget = new QWidget(ui->centralwidget);
    QVBoxLayout* miniMainLayout = new QVBoxLayout(m_miniWidget);
    miniMainLayout->setContentsMargins(20, 20, 20, 20);
    miniMainLayout->setSpacing(10);

    QWidget* miniTopContainer = new QWidget(m_miniWidget);
    QHBoxLayout* miniTopLayout = new QHBoxLayout(miniTopContainer);
    miniTopLayout->setContentsMargins(0, 0, 0, 0);
    miniTopLayout->addStretch(1);
    m_miniTimeLabel = new QLabel(m_miniWidget);
    m_miniTimeLabel->setAlignment(Qt::AlignCenter);
    miniTopLayout->addWidget(m_miniTimeLabel);
    miniTopLayout->addStretch(1);
    m_pinBtn = new QPushButton("📌", m_miniWidget);
    m_pinBtn->setFixedSize(30, 30);
    m_pinBtn->setCheckable(true);
    connect(m_pinBtn, &QPushButton::clicked, this, &MainWindow::togglePinned);
    miniTopLayout->addWidget(m_pinBtn);
    miniMainLayout->addWidget(miniTopContainer);

    m_miniDateLabel = new QLabel(m_miniWidget);
    m_miniDateLabel->setAlignment(Qt::AlignCenter);
    miniMainLayout->addWidget(m_miniDateLabel);

    QScrollArea* miniScroll = new QScrollArea(m_miniWidget);
    miniScroll->setWidgetResizable(true);
    miniScroll->setFrameShape(QFrame::NoFrame);
    QWidget* miniScrollContent = new QWidget();
    miniScrollContent->setObjectName("miniScrollContent");
    m_miniScheduleLayout = new QVBoxLayout(miniScrollContent);
    m_miniScheduleLayout->setAlignment(Qt::AlignTop);
    m_miniScheduleLayout->setSpacing(8);
    miniScroll->setWidget(miniScrollContent);
    miniMainLayout->addWidget(miniScroll);

    QHBoxLayout* miniBottomLayout = new QHBoxLayout();
    miniBottomLayout->setSpacing(10);
    QPushButton* settingsBtn = new QPushButton("⚙ 설정", m_miniWidget);
    settingsBtn->setObjectName("miniSettingsBtn");
    connect(settingsBtn, &QPushButton::clicked, this, &MainWindow::openSettingsWidget);
    QPushButton* backToFull = new QPushButton("캘린더 보기", m_miniWidget);
    backToFull->setObjectName("miniBackBtn");
    connect(backToFull, &QPushButton::clicked, [this](){ 
        if (m_isPinned) togglePinned(); // 핀이 켜져 있다면 먼저 해제 (사이즈 고정 풀기)
        resize(800, 600); 
    });
    miniBottomLayout->addWidget(settingsBtn, 1);
    miniBottomLayout->addWidget(backToFull, 2);
    miniMainLayout->addLayout(miniBottomLayout);

    m_miniWidget->hide();

    m_realTimeTimer = new QTimer(this);
    connect(m_realTimeTimer, &QTimer::timeout, [this]() {
        if (m_isMiniMode) m_miniTimeLabel->setText(QTime::currentTime().toString("HH:mm:ss"));
    });
    m_realTimeTimer->start(1000);

    updateMainStyle(); 
    updateCategoryBar();
    updateCalendar();
}

void MainWindow::updateMainStyle() {
    QString bgColor = StyleHelper::getBgColor();
    QString textColor = StyleHelper::getTextColor();
    QString primary = StyleHelper::getPrimaryColor();
    bool isDark = (StyleHelper::currentTheme == StyleHelper::Theme::Dark);

    this->setStyleSheet(QString("QMainWindow { background-color: %1; }").arg(bgColor));
    ui->centralwidget->setStyleSheet(QString("#centralwidget { background-color: %1; }").arg(bgColor));
    m_container->setStyleSheet(QString("background-color: %1;").arg(bgColor));
    
    m_headerBar->setStyleSheet(QString("background-color: %1; border-bottom: 1px solid %2;").arg(bgColor, isDark ? "#333" : "#E0E0E0"));
    m_monthLabel->setStyleSheet(StyleHelper::getHeaderStyle());
    
    QString btnNavStyle = QString("QPushButton { border: none; font-size: 16px; color: %1; background: transparent; } QPushButton:hover { color: %2; }").arg(isDark ? "#AAA" : "#555", primary);
    m_prevBtn->setStyleSheet(btnNavStyle);
    m_nextBtn->setStyleSheet(btnNavStyle);

    m_todayBtn->setStyleSheet(QString("QPushButton { border: 1px solid %1; border-radius: 4px; font-size: 12px; color: %2; background: %3; } QPushButton:hover { border-color: %4; color: %4; }").arg(isDark ? "#444" : "#DDD", isDark ? "#AAA" : "#555", isDark ? "#2D2D2D" : "white", primary));

    m_overflowWidget->setStyleSheet(QString("background-color: %1; border-bottom: 1px solid %2;").arg(isDark ? "#252525" : "#F9F9F9", isDark ? "#333" : "#E0E0E0"));
    m_moreBtn->setStyleSheet(QString("QPushButton { border: 1px solid %1; border-radius: 4px; background: %2; color: %3; font-weight: bold; } QPushButton:checked { background: %4; color: white; border-color: %4; }").arg(isDark ? "#444" : "#BDBDBD", isDark ? "#2D2D2D" : "white", isDark ? "#AAA" : "#666", primary));

    updateMiniModeStyle();
    updateCategoryBar();
}

void MainWindow::updateMiniModeStyle() {
    if (!m_miniWidget) return;
    QString bgColor = StyleHelper::getBgColor();
    QString textColor = StyleHelper::getTextColor();
    QString primary = StyleHelper::getPrimaryColor();
    bool isDark = (StyleHelper::currentTheme == StyleHelper::Theme::Dark);

    m_miniWidget->setStyleSheet(QString("background-color: %1;").arg(bgColor));
    m_miniTimeLabel->setStyleSheet(QString("font-size: 32px; font-weight: bold; color: %1; font-family: 'Consolas', monospace; background: transparent;").arg(textColor));
    m_miniDateLabel->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1; background: transparent;").arg(primary));
    m_pinBtn->setStyleSheet(QString("QPushButton { border: none; font-size: 16px; background: transparent; } QPushButton:checked { background: %1; border-radius: 15px; }").arg(isDark ? "#444" : "#E3F2FD"));

    if (QPushButton* sBtn = m_miniWidget->findChild<QPushButton*>("miniSettingsBtn")) sBtn->setStyleSheet(StyleHelper::getBtnModifyStyle());
    if (QPushButton* bBtn = m_miniWidget->findChild<QPushButton*>("miniBackBtn")) bBtn->setStyleSheet(StyleHelper::getBtnSaveStyle());
}

void MainWindow::setMiniMode(bool mini) {
    if (m_isMiniMode == mini) return;
    m_isMiniMode = mini;

    if (m_isMiniMode) {
        updateMiniModeStyle();
        m_headerBar->hide(); m_overflowWidget->hide(); m_container->hide();
        m_miniWidget->show();
        QDate today = QDate::currentDate();
        m_miniDateLabel->setText(today.toString("yyyy년 MM월 dd일 dddd"));
        
        QLayoutItem *item;
        while ((item = m_miniScheduleLayout->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }

        auto schedules = DatabaseManager::instance().getSchedulesForDay(today);
        bool isDark = (StyleHelper::currentTheme == StyleHelper::Theme::Dark);
        
        if (schedules.isEmpty()) {
            QLabel* empty = new QLabel("오늘 일정이 없습니다.", m_miniWidget);
            empty->setStyleSheet(QString("color: #999; font-style: italic; padding: 20px; background: transparent;"));
            empty->setAlignment(Qt::AlignCenter);
            m_miniScheduleLayout->addWidget(empty);
        } else {
            for (const auto& s : schedules) {
                QWidget* itemWidget = new QWidget(m_miniWidget);
                itemWidget->setObjectName("itemWidget");
                itemWidget->setStyleSheet(StyleHelper::getItemBaseStyle(s["color"].toString().isEmpty() ? StyleHelper::getPrimaryColor() : s["color"].toString()));
                QHBoxLayout* itemLayout = new QHBoxLayout(itemWidget);
                itemLayout->setContentsMargins(15, 12, 15, 12); itemLayout->setSpacing(10);
                QLabel* titleLabel = new QLabel(s["title"].toString(), itemWidget);
                titleLabel->setStyleSheet(QString("font-weight: bold; font-size: 13px; color: %1; border: none; background: transparent;").arg(StyleHelper::getTextColor()));
                itemLayout->addWidget(titleLabel, 1);
                QString startStr = s["start"].toString().mid(11, 5);
                QString endStr = s["end"].toString().mid(11, 5);
                if (!((s["all_day"].toInt() == 1) || (startStr == "00:00" && endStr == "00:00"))) {
                    QLabel* timeLabel = new QLabel(QString("%1~%2").arg(startStr, endStr), itemWidget);
                    timeLabel->setStyleSheet(QString("color: %1; font-size: 11px; border: none; background: transparent;").arg(isDark ? "#AAA" : "#888"));
                    itemLayout->addWidget(timeLabel);
                }
                m_miniScheduleLayout->addWidget(itemWidget);
            }
        }
    } else {
        m_miniWidget->hide(); m_headerBar->show(); m_container->show();
        updateMainStyle();
        updateLayoutPositions();
    }
}

void MainWindow::openSettingsWidget() {
    SettingsWidget *settings = new SettingsWidget();
    settings->setAttribute(Qt::WA_DeleteOnClose);
    settings->setWindowModality(Qt::ApplicationModal);
    connect(settings, &SettingsWidget::settingsChanged, this, [this]() {
        updateMainStyle();
        updateCalendar();
    });
    settings->show();
}

void MainWindow::togglePinned() {
    m_isPinned = !m_isPinned;
    m_pinBtn->setChecked(m_isPinned);
    if (m_isPinned) this->setFixedSize(this->size());
    else { this->setMinimumSize(300, 400); this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX); }
    this->setWindowFlag(Qt::WindowStaysOnTopHint, m_isPinned);
    this->show();
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    if (event->size().width() < 600) setMiniMode(true);
    else setMiniMode(false);
    if (m_isMiniMode && m_miniWidget) m_miniWidget->setGeometry(ui->centralwidget->rect());
    else updateLayoutPositions();
    QMainWindow::resizeEvent(event);
}

void MainWindow::updateLayoutPositions() {
    if (m_isMiniMode) return;
    int w = ui->centralwidget->width(); if (w <= 0) return;
    m_headerBar->setGeometry(0, 0, w, kBaseHeaderH);
    m_headerBar->raise();
    if (m_isExpanded && m_moreBtn && !m_moreBtn->isHidden()) {
        m_overflowWidget->show();
        int btnCount = 0;
        for (int i = 0; i < m_overflowLayout->count(); ++i) if (m_overflowLayout->itemAt(i)->widget()) btnCount++;
        int overflowH = (btnCount > 0) ? 20 + (((btnCount + 4) / 5) * 22) + 10 : 0;
        m_overflowWidget->setFixedHeight(overflowH);
        m_overflowWidget->setGeometry(0, kBaseHeaderH, w, overflowH);
        m_overflowWidget->raise();
        m_currentHeaderH = kBaseHeaderH + overflowH;
    } else { m_overflowWidget->hide(); m_currentHeaderH = kBaseHeaderH; }
    int h = ui->centralwidget->height() - m_currentHeaderH;
    SafeZoneStage stage = getStageForHeight(h);
    
    m_container->resize(w * 3, h);
    for (int i = 0; i < 3; ++i) {
        m_months[i]->setStage(stage);
        m_months[i]->setGeometry(i * w, 0, w, h);
    }
    m_container->move(-w + m_xOffset, m_currentHeaderH);
    m_container->lower();
}

void MainWindow::updateCalendar() {
    m_monthLabel->setText(QString("%1년 %2월").arg(m_currentYear).arg(m_currentMonth));
    updateLayoutPositions();
    QDate current(m_currentYear, m_currentMonth, 1);
    QDate prev = current.addMonths(-1);
    QDate next = current.addMonths(1);
    auto getFiltered = [this](int y, int m) {
        QList<QVariantMap> raw = DatabaseManager::instance().getSchedulesForMonth(y, m);
        QList<QVariantMap> f;
        for (const auto& s : raw) if (m_categoryFilters.value(s["category_id"].toInt(), true)) f.append(s);
        return f;
    };
    m_months[0]->updateMonth(prev.year(), prev.month(), getFiltered(prev.year(), prev.month()));
    m_months[1]->updateMonth(current.year(), current.month(), getFiltered(current.year(), current.month()));
    m_months[2]->updateMonth(next.year(), next.month(), getFiltered(next.year(), next.month()));
}

void MainWindow::wheelEvent(QWheelEvent* event) {
    if (m_isMiniMode || m_animation->state() == QAbstractAnimation::Running) return;
    m_xOffset += event->angleDelta().y() * 1.5;
    int w = ui->centralwidget->width();
    if (m_xOffset > w) m_xOffset = w; if (m_xOffset < -w) m_xOffset = -w;
    m_container->move(-w + m_xOffset, m_currentHeaderH);
    m_scrollTimer->start(150);
    event->accept();
}

void MainWindow::finishScroll() {
    int w = ui->centralwidget->width();
    int dir = 0; if(m_xOffset > w/6) dir = -1; else if (m_xOffset < -w/6) dir = 1;
    slideMonth(dir);
}

void MainWindow::slideMonth(int direction) {
    if (m_animation->state() == QAbstractAnimation::Running) return;
    int w = ui->centralwidget->width();
    int targetX = (direction == -1) ? 0 : (direction == 1 ? -2 * w : -w);
    m_animation->stop(); m_animation->setStartValue(m_container->pos()); m_animation->setEndValue(QPoint(targetX, m_currentHeaderH));
    m_animation->disconnect(SIGNAL(finished()));
    connect(m_animation, &QPropertyAnimation::finished, [this, direction]() {
        if (direction != 0) {
            QDate date = QDate(m_currentYear, m_currentMonth, 1).addMonths(direction);
            m_currentYear = date.year(); m_currentMonth = date.month();
        }
        m_xOffset = 0; updateCalendar();
    });
    m_animation->start();
}

void MainWindow::prevMonth() { slideMonth(-1); }
void MainWindow::nextMonth() { slideMonth(1); }
void MainWindow::goToday() { QDate t = QDate::currentDate(); m_currentYear = t.year(); m_currentMonth = t.month(); m_xOffset = 0; updateCalendar(); }

void MainWindow::handleDayDoubleClicked(const QDate& date) {
    ScheduleManagerWidget *w = new ScheduleManagerWidget(date);
    w->setAttribute(Qt::WA_DeleteOnClose); w->setWindowModality(Qt::ApplicationModal);
    connect(w, &ScheduleManagerWidget::dataChanged, this, &MainWindow::updateCalendar);
    w->show();
}

void MainWindow::handleDayAddRequested(const QDate& date) {
    ScheduleInputWidget *w = new ScheduleInputWidget(date);
    w->setAttribute(Qt::WA_DeleteOnClose); w->setWindowModality(Qt::ApplicationModal);
    connect(w, &ScheduleInputWidget::scheduleSaved, this, &MainWindow::updateCalendar);
    w->show();
}

void MainWindow::openCategoryManager() {
    CategoryModifyWidget *w = new CategoryModifyWidget(); 
    w->setAttribute(Qt::WA_DeleteOnClose);
    connect(w, &CategoryModifyWidget::categoriesChanged, this, &MainWindow::updateCategoryBar);
    connect(w, &CategoryModifyWidget::categoriesChanged, this, &MainWindow::updateCalendar);
    w->show();
}

void MainWindow::updateCategoryBar() {
    QLayoutItem *item;
    while ((item = m_categoryLayout->takeAt(0)) != nullptr) { if (item->widget() && item->widget() != m_moreBtn) item->widget()->deleteLater(); delete item; }
    while ((item = m_overflowLayout->takeAt(0)) != nullptr) { if (item->widget()) item->widget()->deleteLater(); delete item; }
    auto categories = DatabaseManager::instance().getCategories();
    int count = 0;
    QString bgColor = StyleHelper::getBgColor();
    bool isDark = (StyleHelper::currentTheme == StyleHelper::Theme::Dark);
    for (const auto& cat : categories) {
        int id = cat["id"].toInt(); if (!m_categoryFilters.contains(id)) m_categoryFilters[id] = true;
        QPushButton* btn = new QPushButton(cat["name"].toString(), this);
        btn->setCheckable(true); btn->setChecked(m_categoryFilters[id]); btn->setFixedHeight(22);
        btn->setStyleSheet(QString("QPushButton { border: 1px solid %1; border-radius: 10px; padding: 0px 12px; font-size: 11px; color: %1; background: %2; font-weight: bold; } QPushButton:checked { background-color: %1; color: white; }").arg(cat["color"].toString(), bgColor));
        connect(btn, &QPushButton::toggled, [this, id](bool c) { m_categoryFilters[id] = c; updateCalendar(); });
        if (count < 3) m_categoryLayout->addWidget(btn);
        else m_overflowLayout->addWidget(btn, (count - 3) / 5, (count - 3) % 5, Qt::AlignLeft);
        count++;
    }
    if (count > 3) { m_categoryLayout->addWidget(m_moreBtn); m_moreBtn->show(); m_overflowWidget->setVisible(m_isExpanded); }
    else { m_moreBtn->hide(); m_isExpanded = false; m_overflowWidget->hide(); }
    QPushButton* addBtn = new QPushButton("+", this);
    addBtn->setFixedSize(22, 22); addBtn->setStyleSheet(QString("QPushButton { border: 1px solid %1; border-radius: 11px; background-color: %2; color: %3; font-size: 16px; font-weight: bold; } QPushButton:hover { background-color: %4; color: white; }").arg(isDark ? "#444" : "#E0E0E0", bgColor, isDark ? "#AAA" : "#757575", StyleHelper::getPrimaryColor()));
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::openCategoryManager);
    m_categoryLayout->addWidget(addBtn);
}

void MainWindow::toggleOverflow() {
    m_isExpanded = !m_isExpanded; m_moreBtn->setChecked(m_isExpanded); m_overflowWidget->setVisible(m_isExpanded);
    updateLayoutPositions(); updateCalendar();
}

MainWindow::~MainWindow() { delete ui; }
