#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./models/DatabaseManager.h"
#include "./widgets/DayCell.h"
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
        return;
    }
    qDebug() << "Database Initialized";

    // 2. CREATE 테스트 (카테고리 ID: 1 부여)
    QDateTime start = QDateTime::currentDateTime();
    QDateTime end = start.addSecs(3600); // 1시간 일정
    DatabaseManager::instance().addSchedule(1, "Test Schedule", "Description", start, end, "#FF5733");
    qDebug() << "Step 1: Data Added (Category 1)";
    printCurrentDbState();

    // 3. UPDATE 테스트 (가장 최근 데이터의 ID 사용)
    QList<QVariantMap> list = DatabaseManager::instance().getSchedulesForDay(start.date());
    if (!list.isEmpty()) {
        int targetId = list.last()["id"].toInt();
        QDateTime newEnd = start.addSecs(7200); // 2시간으로 연장

        // 카테고리 ID를 2로 변경하며 업데이트
        DatabaseManager::instance().updateSchedule(targetId, 2, "Updated Title", "New Desc", start, newEnd, "#00FF00");
        qDebug() << "Step 2: Data Updated (ID:" << targetId << " -> Category 2)";
        printCurrentDbState();

        // 4. DELETE 테스트
        // DatabaseManager::instance().deleteSchedule(targetId);
        // qDebug() << "Step 3: Data Deleted (ID:" << targetId << ")";
        // printCurrentDbState();
    }

    //인풋 창 테스트
    ScheduleInputWidget *inputWidget = new ScheduleInputWidget();
    inputWidget->setAttribute(Qt::WA_DeleteOnClose); // 창을 닫으면 메모리에서 자동 해제
    inputWidget->setWindowTitle("일정 추가");
    inputWidget->setWindowModality(Qt::ApplicationModal); // 이 창이 떠 있는 동안 메인창 클릭 불가

    connect(inputWidget, &ScheduleInputWidget::scheduleSaved, this, &printCurrentDbState);
    // 3. 화면에 표시
    inputWidget->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
