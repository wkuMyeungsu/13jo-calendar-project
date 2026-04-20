#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DatabaseManager.h"
#include "MonthWidget.h"
#include "ScheduleEdit.h"
#include "ScheduleManagerWidget.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDate>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QEasingCurve>
#include <QPoint>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_xOffset(0)
{
    ui->setupUi(this);

    if (!DatabaseManager::instance().initDatabase("calendar_data.db")) {
        qDebug() << "Database initialization failed.";
    }

    // ── 네비게이션 헤더 바 ──
    m_headerBar = new QWidget(ui->centralwidget);
    m_headerBar->setStyleSheet(
        "QWidget { background-color: white; border-bottom: 1px solid #E0E0E0; }"
    );

    m_prevBtn = new QPushButton("◀", m_headerBar);
    m_prevBtn->setCursor(Qt::PointingHandCursor);
    m_prevBtn->setStyleSheet(
        "QPushButton { border: none; font-size: 16px; color: #555; background: transparent; padding: 4px 10px; }"
        "QPushButton:hover { color: #4A90E2; }"
    );

    m_monthLabel = new QLabel(m_headerBar);
    m_monthLabel->setAlignment(Qt::AlignCenter);
    m_monthLabel->setStyleSheet("font-size: 17px; font-weight: bold; color: #222; border: none; background: transparent;");

    m_nextBtn = new QPushButton("▶", m_headerBar);
    m_nextBtn->setCursor(Qt::PointingHandCursor);
    m_nextBtn->setStyleSheet(
        "QPushButton { border: none; font-size: 16px; color: #555; background: transparent; padding: 4px 10px; }"
        "QPushButton:hover { color: #4A90E2; }"
    );

    m_todayBtn = new QPushButton("오늘", m_headerBar);
    m_todayBtn->setCursor(Qt::PointingHandCursor);
    m_todayBtn->setStyleSheet(
        "QPushButton { border: 1px solid #BDBDBD; border-radius: 4px; padding: 3px 12px; font-size: 12px; color: #333; background: white; }"
        "QPushButton:hover { border-color: #4A90E2; color: #4A90E2; }"
    );

    QHBoxLayout* headerLayout = new QHBoxLayout(m_headerBar);
    headerLayout->setContentsMargins(12, 0, 12, 0);
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

    // 타이머 및 애니메이션 설정
    m_scrollTimer = new QTimer(this);
    m_scrollTimer->setSingleShot(true);
    connect(m_scrollTimer, &QTimer::timeout, this, &MainWindow::finishScroll);

    m_animation = new QPropertyAnimation(m_container, "pos", this);
    m_animation->setDuration(300);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    // 초기 날짜
    QDate today = QDate::currentDate();
    m_currentYear = today.year();
    m_currentMonth = today.month();

    updateCalendar();
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    int w = ui->centralwidget->width();
    int h = ui->centralwidget->height() - kHeaderH;

    m_headerBar->setGeometry(0, 0, w, kHeaderH);
    m_container->resize(w * 3, h);
    for (int i = 0; i < 3; ++i) {
        m_months[i]->setGeometry(i * w, 0, w, h);
    }
    m_container->move(-w + m_xOffset, kHeaderH);
    QMainWindow::resizeEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent* event) {
    if (m_animation->state() == QAbstractAnimation::Running) return;

    // 위 스크롤(+) → 오른쪽(이전 달), 아래 스크롤(-) → 왼쪽(다음 달)
    m_xOffset += event->angleDelta().y() * 0.8;

    int w = ui->centralwidget->width();
    if (m_xOffset > w)  m_xOffset = w;
    if (m_xOffset < -w) m_xOffset = -w;

    m_container->move(-w + m_xOffset, kHeaderH);

    m_scrollTimer->start(150);
    event->accept();
}

void MainWindow::finishScroll() {
    int w = ui->centralwidget->width();
    int threshold = w / 2;

    int targetX;
    if (m_xOffset > threshold) {
        targetX = 0;        // 이전 달 (컨테이너 오른쪽 끝)
    } else if (m_xOffset < -threshold) {
        targetX = -2 * w;   // 다음 달 (컨테이너 왼쪽 끝)
    } else {
        targetX = -w;       // 현재 달 (중앙)
    }

    m_animation->setStartValue(m_container->pos());
    m_animation->setEndValue(QPoint(targetX, kHeaderH));

    connect(m_animation, &QPropertyAnimation::finished, [this, targetX, w]() {
        m_animation->disconnect(SIGNAL(finished()));
        if (targetX == 0) {
            m_currentMonth--;
            if (m_currentMonth < 1) { m_currentMonth = 12; m_currentYear--; }
        } else if (targetX == -2 * w) {
            m_currentMonth++;
            if (m_currentMonth > 12) { m_currentMonth = 1; m_currentYear++; }
        }
        m_xOffset = 0;
        updateCalendar();
    });

    m_animation->start();
}

void MainWindow::updateCalendar() {
    QString title = QString("%1년 %2월").arg(m_currentYear).arg(m_currentMonth);
    setWindowTitle(title);
    m_monthLabel->setText(title);

    int w = ui->centralwidget->width();
    int h = ui->centralwidget->height() - kHeaderH;
    if (w <= 0) w = 800;
    if (h <= 0) h = 600 - kHeaderH;

    m_container->resize(w * 3, h);
    for (int i = 0; i < 3; ++i)
        m_months[i]->setGeometry(i * w, 0, w, h);
    m_container->move(-w, kHeaderH);

    QDate current(m_currentYear, m_currentMonth, 1);
    QDate prev = current.addMonths(-1);
    QDate next = current.addMonths(1);

    m_months[0]->updateMonth(prev.year(), prev.month(), DatabaseManager::instance().getSchedulesForMonth(prev.year(), prev.month()));
    m_months[1]->updateMonth(current.year(), current.month(), DatabaseManager::instance().getSchedulesForMonth(current.year(), current.month()));
    m_months[2]->updateMonth(next.year(), next.month(), DatabaseManager::instance().getSchedulesForMonth(next.year(), next.month()));
}

void MainWindow::prevMonth() {
    if (m_animation->state() == QAbstractAnimation::Running) return;
    int w = ui->centralwidget->width();
    m_animation->setStartValue(m_container->pos());
    m_animation->setEndValue(QPoint(0, kHeaderH));
    connect(m_animation, &QPropertyAnimation::finished, [this]() {
        m_animation->disconnect(SIGNAL(finished()));
        m_currentMonth--;
        if (m_currentMonth < 1) { m_currentMonth = 12; m_currentYear--; }
        m_xOffset = 0;
        updateCalendar();
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
        m_currentMonth++;
        if (m_currentMonth > 12) { m_currentMonth = 1; m_currentYear++; }
        m_xOffset = 0;
        updateCalendar();
    });
    m_animation->start();
}

void MainWindow::goToday() {
    if (m_animation->state() == QAbstractAnimation::Running) return;
    QDate today = QDate::currentDate();
    m_currentYear = today.year();
    m_currentMonth = today.month();
    m_xOffset = 0;
    updateCalendar();
}

void MainWindow::handleDayDoubleClicked(const QDate& date) {
    qDebug() << "Day double clicked:" << date.toString();
    ScheduleManagerWidget *managerWidget = new ScheduleManagerWidget(date);
    managerWidget->setAttribute(Qt::WA_DeleteOnClose);
    managerWidget->setWindowModality(Qt::ApplicationModal);
    
    // 일정 저장/수정/삭제 시 달력 갱신 (데이터 변경 시 업데이트)
    connect(managerWidget, &ScheduleManagerWidget::dataChanged, [this]() {
        this->updateCalendar();
    });
    
    managerWidget->show();
}

void MainWindow::handleDayAddRequested(const QDate& date) {
    qDebug() << "Add requested for date:" << date.toString();
    ScheduleInputWidget *inputWidget = new ScheduleInputWidget(date);
    inputWidget->setAttribute(Qt::WA_DeleteOnClose);
    inputWidget->setWindowTitle(date.toString("yyyy-MM-dd") + " 일정 추가");
    inputWidget->setWindowModality(Qt::ApplicationModal);
    
    connect(inputWidget, &ScheduleInputWidget::scheduleSaved, this, &MainWindow::updateCalendar);
    
    inputWidget->show();
}

MainWindow::~MainWindow() {
    delete ui;
}
