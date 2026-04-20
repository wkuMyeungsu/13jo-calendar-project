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
#include <QResizeEvent>
#include <QWheelEvent>
#include <QEasingCurve>
#include <QPoint>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_yOffset(0)
{
    ui->setupUi(this);

    if (!DatabaseManager::instance().initDatabase("calendar_data.db")) {
        qDebug() << "Database initialization failed.";
    }

    // 컨테이너 설정
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
    int h = ui->centralwidget->height();
    
    m_container->resize(w, h * 3);
    for (int i = 0; i < 3; ++i) {
        m_months[i]->setGeometry(0, i * h, w, h);
    }
    // 현재 달이 중앙에 오도록 배치
    m_container->move(0, -h + m_yOffset);
    QMainWindow::resizeEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent* event) {
    if (m_animation->state() == QAbstractAnimation::Running) return;

    // 스크롤 감도 조정 및 즉시 반영
    m_yOffset += event->angleDelta().y() * 0.8; 
    
    int h = ui->centralwidget->height();
    
    if (m_yOffset > h) m_yOffset = h;
    if (m_yOffset < -h) m_yOffset = -h;

    m_container->move(0, -h + m_yOffset);

    m_scrollTimer->start(150);
    event->accept();
}

void MainWindow::finishScroll() {
    int h = ui->centralwidget->height();
    int threshold = h / 2;
    int targetY = -h;

    if (m_yOffset > threshold) {
        targetY = 0;
    } else if (m_yOffset < -threshold) {
        targetY = -2 * h;
    } else {
        targetY = -h;
    }

    m_animation->setStartValue(m_container->pos());
    m_animation->setEndValue(QPoint(0, targetY));
    
    connect(m_animation, &QPropertyAnimation::finished, [this, targetY, h]() {
        m_animation->disconnect(SIGNAL(finished()));
        if (targetY == 0) {
            m_currentMonth--;
            if (m_currentMonth < 1) { m_currentMonth = 12; m_currentYear--; }
        } else if (targetY == -2 * h) {
            m_currentMonth++;
            if (m_currentMonth > 12) { m_currentMonth = 1; m_currentYear++; }
        }
        m_yOffset = 0;
        updateCalendar();
    });
    
    m_animation->start();
}

void MainWindow::updateCalendar() {
    setWindowTitle(QString("%1년 %2월").arg(m_currentYear).arg(m_currentMonth));
    
    int h = ui->centralwidget->height();
    if (h <= 0) h = 600; // 초기화 전 방어 코드

    m_container->move(0, -h);

    QDate current(m_currentYear, m_currentMonth, 1);
    QDate prev = current.addMonths(-1);
    QDate next = current.addMonths(1);

    m_months[0]->updateMonth(prev.year(), prev.month(), DatabaseManager::instance().getSchedulesForMonth(prev.year(), prev.month()));
    m_months[1]->updateMonth(current.year(), current.month(), DatabaseManager::instance().getSchedulesForMonth(current.year(), current.month()));
    m_months[2]->updateMonth(next.year(), next.month(), DatabaseManager::instance().getSchedulesForMonth(next.year(), next.month()));
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
