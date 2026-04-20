#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DatabaseManager.h"
#include "MonthWidget.h"
#include "ScheduleEdit.h"
#include "ScheduleManagerWidget.h"
#include "CategoryModifyWidget.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDate>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QEasingCurve>
#include <QPoint>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentYear(QDate::currentDate().year())
    , m_currentMonth(QDate::currentDate().month())
    , m_xOffset(0)
{
    ui->setupUi(this);

    if (!DatabaseManager::instance().initDatabase("calendar_data.db")) {
        qDebug() << "Database initialization failed.";
    }

    // ── 네비게이션 헤더 바 ──
    m_headerBar = new QWidget(ui->centralwidget);
    m_headerBar->setStyleSheet("QWidget { background-color: white; border-bottom: 1px solid #E0E0E0; }");

    // 카테고리 토글 레이아웃 (좌측)
    m_categoryLayout = new QHBoxLayout();
    m_categoryLayout->setContentsMargins(10, 0, 0, 0);
    m_categoryLayout->setSpacing(6);

    m_prevBtn = new QPushButton("◀", m_headerBar);
    m_monthLabel = new QLabel(m_headerBar);
    m_monthLabel->setStyleSheet("font-size: 17px; font-weight: bold; color: #222; border: none;");
    m_nextBtn = new QPushButton("▶", m_headerBar);
    m_todayBtn = new QPushButton("오늘", m_headerBar);

    QHBoxLayout* headerLayout = new QHBoxLayout(m_headerBar);
    headerLayout->setContentsMargins(12, 0, 12, 0);
    
    headerLayout->addLayout(m_categoryLayout); // 좌측에 배치
    headerLayout->addStretch();
    headerLayout->addWidget(m_prevBtn);
    headerLayout->addWidget(m_monthLabel);
    headerLayout->addWidget(m_nextBtn);
    headerLayout->addStretch();
    headerLayout->addWidget(m_todayBtn);

    connect(m_prevBtn,  &QPushButton::clicked, this, &MainWindow::prevMonth);
    connect(m_nextBtn,  &QPushButton::clicked, this, &MainWindow::nextMonth);
    connect(m_todayBtn, &QPushButton::clicked, this, &MainWindow::goToday);

    // ── 캘린더 컨테이너 ──
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

    updateCategoryBar();
    updateCalendar();
}

void MainWindow::updateCategoryBar() {
    // 기존 버튼 제거
    QLayoutItem *item;
    while ((item = m_categoryLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    QList<QVariantMap> categories = DatabaseManager::instance().getCategories();
    for (const auto& cat : categories) {
        int id = cat["id"].toInt();
        QString name = cat["name"].toString();
        QString color = cat["color"].toString();

        if (!m_categoryFilters.contains(id)) m_categoryFilters[id] = true;

        QPushButton* btn = new QPushButton(name, m_headerBar);
        btn->setCheckable(true);
        btn->setChecked(m_categoryFilters[id]);
        btn->setStyleSheet(QString(
            "QPushButton { border: 1px solid %1; border-radius: 10px; padding: 2px 10px; font-size: 11px; color: %1; background: white; font-weight: bold; } "
            "QPushButton:checked { background-color: %1; color: white; }"
        ).arg(color));

        connect(btn, &QPushButton::toggled, [this, id](bool checked) {
            m_categoryFilters[id] = checked;
            updateCalendar();
        });

        m_categoryLayout->addWidget(btn);
    }

    // 카테고리 설정용 (+) 버튼 추가
    QPushButton* addBtn = new QPushButton("+", m_headerBar);
    addBtn->setFixedSize(22, 22);
    addBtn->setStyleSheet(
        "QPushButton { border: 1px solid #ccc; border-radius: 11px; background: #f9f9f9; color: #666; font-weight: bold; font-size: 14px; } "
        "QPushButton:hover { background: #e9e9e9; color: #333; }"
    );
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::openCategoryManager);
    m_categoryLayout->addWidget(addBtn);
}

void MainWindow::openCategoryManager() {
    CategoryModifyWidget *catModify = new CategoryModifyWidget(); 
    catModify->setAttribute(Qt::WA_DeleteOnClose);
    catModify->setWindowTitle("카테고리 설정");
    
    // 카테고리 변경 시 메인 윈도우 즉시 갱신
    connect(catModify, &CategoryModifyWidget::categoriesChanged, this, &MainWindow::updateCategoryBar);
    connect(catModify, &CategoryModifyWidget::categoriesChanged, this, &MainWindow::updateCalendar);
    
    catModify->show();
    catModify->raise();
    catModify->activateWindow();
}

void MainWindow::updateCalendar() {
    QString title = QString("%1년 %2월").arg(m_currentYear).arg(m_currentMonth);
    m_monthLabel->setText(title);

    int w = ui->centralwidget->width();
    int h = ui->centralwidget->height() - kHeaderH;
    if (w <= 0) w = 800;
    if (h <= 0) h = 600;

    m_container->resize(w * 3, h);
    for (int i = 0; i < 3; ++i) m_months[i]->setGeometry(i * w, 0, w, h);
    m_container->move(-w, kHeaderH);

    QDate current(m_currentYear, m_currentMonth, 1);
    QDate prev = current.addMonths(-1);
    QDate next = current.addMonths(1);

    auto getFilteredSchedules = [this](int year, int month) {
        QList<QVariantMap> raw = DatabaseManager::instance().getSchedulesForMonth(year, month);
        QList<QVariantMap> filtered;
        for (const auto& s : raw) {
            if (m_categoryFilters.value(s["category_id"].toInt(), true)) {
                filtered.append(s);
            }
        }
        return filtered;
    };

    m_months[0]->updateMonth(prev.year(), prev.month(), getFilteredSchedules(prev.year(), prev.month()));
    m_months[1]->updateMonth(current.year(), current.month(), getFilteredSchedules(current.year(), current.month()));
    m_months[2]->updateMonth(next.year(), next.month(), getFilteredSchedules(next.year(), next.month()));
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    int w = ui->centralwidget->width();
    int h = ui->centralwidget->height() - kHeaderH;
    m_headerBar->setGeometry(0, 0, w, kHeaderH);
    m_container->resize(w * 3, h);
    for (int i = 0; i < 3; ++i) m_months[i]->setGeometry(i * w, 0, w, h);
    m_container->move(-w + m_xOffset, kHeaderH);
    QMainWindow::resizeEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent* event) {
    if (m_animation->state() == QAbstractAnimation::Running) return;
    m_xOffset += event->angleDelta().y() * 0.8;
    int w = ui->centralwidget->width();
    if (m_xOffset > w) m_xOffset = w;
    if (m_xOffset < -w) m_xOffset = -w;
    m_container->move(-w + m_xOffset, kHeaderH);
    m_scrollTimer->start(150);
    event->accept();
}

void MainWindow::finishScroll() {
    int w = ui->centralwidget->width();
    int targetX = (m_xOffset > w/2) ? 0 : (m_xOffset < -w/2) ? -2*w : -w;
    m_animation->setStartValue(m_container->pos());
    m_animation->setEndValue(QPoint(targetX, kHeaderH));
    connect(m_animation, &QPropertyAnimation::finished, [this, targetX, w]() {
        m_animation->disconnect(SIGNAL(finished()));
        if (targetX == 0) { m_currentMonth--; if (m_currentMonth < 1) { m_currentMonth = 12; m_currentYear--; } }
        else if (targetX == -2 * w) { m_currentMonth++; if (m_currentMonth > 12) { m_currentMonth = 1; m_currentYear++; } }
        m_xOffset = 0;
        updateCalendar();
    });
    m_animation->start();
}

void MainWindow::prevMonth() {
    if (m_animation->state() == QAbstractAnimation::Running) return;
    int w = ui->centralwidget->width();
    m_animation->setStartValue(m_container->pos());
    m_animation->setEndValue(QPoint(0, kHeaderH));
    connect(m_animation, &QPropertyAnimation::finished, [this]() {
        m_animation->disconnect(SIGNAL(finished()));
        m_currentMonth--; if (m_currentMonth < 1) { m_currentMonth = 12; m_currentYear--; }
        m_xOffset = 0; updateCalendar();
    });
    m_animation->start();
}

void MainWindow::nextMonth() {
    if (m_animation->state() == QAbstractAnimation::Running) return;
    int w = ui->centralwidget->width();
    m_animation->setStartValue(m_container->pos());
    m_animation->setEndValue(QPoint(-2 * w, kHeaderH));
    connect(m_animation, &QPropertyAnimation::finished, [this]() {
        m_animation->disconnect(SIGNAL(finished()));
        m_currentMonth++; if (m_currentMonth > 12) { m_currentMonth = 1; m_currentYear++; }
        m_xOffset = 0; updateCalendar();
    });
    m_animation->start();
}

void MainWindow::goToday() {
    QDate today = QDate::currentDate();
    m_currentYear = today.year(); m_currentMonth = today.month();
    m_xOffset = 0; updateCalendar();
}

void MainWindow::handleDayDoubleClicked(const QDate& date) {
    ScheduleManagerWidget *managerWidget = new ScheduleManagerWidget(date);
    managerWidget->setAttribute(Qt::WA_DeleteOnClose);
    managerWidget->setWindowModality(Qt::ApplicationModal);
    connect(managerWidget, &ScheduleManagerWidget::dataChanged, [this]() {
        updateCategoryBar();
        updateCalendar();
    });
    managerWidget->show();
}

void MainWindow::handleDayAddRequested(const QDate& date) {
    ScheduleInputWidget *inputWidget = new ScheduleInputWidget(date);
    inputWidget->setAttribute(Qt::WA_DeleteOnClose);
    inputWidget->setWindowModality(Qt::ApplicationModal);
    connect(inputWidget, &ScheduleInputWidget::scheduleSaved, this, &MainWindow::updateCalendar);
    inputWidget->show();
}

MainWindow::~MainWindow() { delete ui; }
