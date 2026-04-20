#include "ScheduleEdit.h"
#include "../models/DatabaseManager.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

ScheduleInputWidget::ScheduleInputWidget(const QDate& initialDate, QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    // 입력 필드 초기화
    titleInput = new QLineEdit(this);
    titleInput->setPlaceholderText("일정 제목");

    categoryCombo = new QComboBox(this);
    categoryCombo->addItem("Work", 1);
    categoryCombo->addItem("Personal", 2);
    categoryCombo->addItem("Other", 3);

    // 전달받은 날짜와 현재 시간을 결합
    QDateTime startDateTime(initialDate, QTime::currentTime());
    startTimeEdit = new QDateTimeEdit(startDateTime, this);
    startTimeEdit->setCalendarPopup(true);

    endTimeEdit = new QDateTimeEdit(startDateTime.addSecs(3600), this);
    endTimeEdit->setCalendarPopup(true);

    saveBtn = new QPushButton("Save Schedule", this);
    saveBtn->setStyleSheet("background-color: #4A90E2; color: white; font-weight: bold; padding: 5px;");

    // 폼 레이아웃 배치
    formLayout->addRow("Title:", titleInput);
    formLayout->addRow("Category:", categoryCombo);
    formLayout->addRow("Start:", startTimeEdit);
    formLayout->addRow("End:", endTimeEdit);

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(saveBtn);

    connect(saveBtn, &QPushButton::clicked, this, &ScheduleInputWidget::handleSave);
}

void ScheduleInputWidget::handleSave() {
    QString title = titleInput->text();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a title.");
        return;
    }

    int catId = categoryCombo->currentData().toInt();
    QDateTime start = startTimeEdit->dateTime();
    QDateTime end = endTimeEdit->dateTime();

    // DB 저장 호출
    bool success = DatabaseManager::instance().addSchedule(catId, title, "", start, end, "#4A90E2");

    if (success) {
        titleInput->clear();
        emit scheduleSaved(); // 신호 발생
        qDebug() << "Schedule saved successfully.";
    }
}