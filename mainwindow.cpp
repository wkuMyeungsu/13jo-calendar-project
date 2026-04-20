#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DatabaseManager.h"
#include "DayCell.h"
#include "ScheduleEdit.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDate>
#include <QGridLayout>



void printCurrentDbState() {
    qDebug() << "Current DB State:";
    // 오늘 날짜 기준으로 전체 조회 (테스트 편의상 getSchedulesForDay 활용)
    QList<QVariantMap> allData = DatabaseManager::instance().getSchedulesForDay(QDate::currentDate());

    if (allData.isEmpty()) {
        qDebug() << "Empty";
    } else {
        for (const auto& row : allData) {
            qDebug() << "ID:" << row["id"].toInt()
            << "| Title:" << row["title"].toString()
            << "| Start:" << row["start"].toString()
            << "| End:" << row["end"].toString()
            << "| Color:" << row["color"].toString();
        }
    }
    qDebug() << "---------------------------------------";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. DB 초기화
    if (!DatabaseManager::instance().initDatabase("calendar_data.db")) {
        qDebug() << "Database initialization failed.";
    }

    // 2. 레이아웃 설정
    m_calendarGrid = new QGridLayout(ui->centralwidget);
    ui->centralwidget->setLayout(m_calendarGrid);

    // 3. 현재 날짜 기준으로 달력 초기화
    QDate today = QDate::currentDate();
    m_currentYear = today.year();
    m_currentMonth = today.month();

    updateCalendar(m_currentYear, m_currentMonth);
}

void MainWindow::updateCalendar(int year, int month) {
    // 기존 그리드 아이템 제거
    QLayoutItem *item;
    while ((item = m_calendarGrid->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // 해당 월의 일정 가져오기
    QList<QVariantMap> monthSchedules = DatabaseManager::instance().getSchedulesForMonth(year, month);

    QDate firstDay(year, month, 1);
    int startCol = firstDay.dayOfWeek() % 7; 

    int daysInMonth = firstDay.daysInMonth();

    for (int day = 1; day <= daysInMonth; ++day) {
        DayCell* cell = new DayCell(this);
        QDate date(year, month, day);
        cell->setDate(date);
        
        // 해당 날짜의 일정 필터링
        QList<QVariantMap> daySchedules;
        for (const auto& s : monthSchedules) {
            QDateTime start = QDateTime::fromString(s["start"].toString(), "yyyy-MM-dd HH:mm:ss");
            if (start.date() == date) {
                daySchedules.append(s);
            }
        }
        cell->setSchedules(daySchedules);
        
        connect(cell, &DayCell::dayDoubleClicked, this, &MainWindow::handleDayDoubleClicked);

        int row = (day + startCol - 1) / 7;
        int col = (day + startCol - 1) % 7;
        m_calendarGrid->addWidget(cell, row, col);
    }
}

void MainWindow::handleDayDoubleClicked(const QDate& date) {
    qDebug() << "Day double clicked:" << date.toString();
    ScheduleInputWidget *inputWidget = new ScheduleInputWidget(date);
    inputWidget->setAttribute(Qt::WA_DeleteOnClose);
    inputWidget->setWindowTitle(date.toString("yyyy-MM-dd") + " 일정 추가");
    inputWidget->setWindowModality(Qt::ApplicationModal);
    
    // 일정 저장 시 달력 갱신 (추후 필요시)
    connect(inputWidget, &ScheduleInputWidget::scheduleSaved, [this]() {
        this->updateCalendar(m_currentYear, m_currentMonth);
    });
    
    inputWidget->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
