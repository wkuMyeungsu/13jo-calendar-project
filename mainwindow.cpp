#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./models/DatabaseManager.h"
#include "./widgets/DayCell.h"

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

    if (!DatabaseManager::instance().initDatabase("calendar_data.db")) return;
    qDebug() << "Database Initialized";

    // 2. Create 테스트
    QDateTime start = QDateTime::currentDateTime();
    QDateTime end = start.addSecs(3600);
    DatabaseManager::instance().addSchedule("Test Schedule", "Description", start, end, "#FF5733");
    qDebug() << "Step 1: Data Added";
    printCurrentDbState();

    // 3. Update 테스트 (가장 최근 데이터의 ID 사용)
    QList<QVariantMap> list = DatabaseManager::instance().getSchedulesForDay(start.date());
    if (!list.isEmpty()) {
        int targetId = list.last()["id"].toInt();
        QDateTime newEnd = start.addSecs(7200);
        DatabaseManager::instance().updateSchedule(targetId, "Updated Title", "New Desc", start, newEnd, "#00FF00");
        qDebug() << "Step 2: Data Updated (ID:" << targetId << ")";
        printCurrentDbState();

        // 4. Delete 테스트
        DatabaseManager::instance().deleteSchedule(targetId);
        qDebug() << "Step 3: Data Deleted (ID:" << targetId << ")";
        printCurrentDbState();
        QWidget* centralWidget = new QWidget(this);
        QGridLayout* gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(0); // 셀 간 간격 제거

        QDate startDate(2026, 4, 1);

        int offset = startDate.dayOfWeek() - 1;
        QDate currentDisplayDate = startDate.addDays(-offset);

        for (int row = 0; row < 6; ++row) {
            for (int col = 0; col < 7; ++col) {
                DayCell* cell = new DayCell(this);
                cell->setDate(currentDisplayDate);

                // 이번 달이 아닌 날짜는 톤 다운 (QSS활용)
                if(currentDisplayDate.month() != 4) {
                    cell->setStyleSheet("background-color: #F5F5F5; color: #CCCCCC;");
                }

                gridLayout->addWidget(cell, row, col);
                currentDisplayDate = currentDisplayDate.addDays(1);
            }

            setCentralWidget(centralWidget);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
