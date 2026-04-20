#include "ScheduleModifyWidget.h"
#include "../models/DatabaseManager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDebug>

ScheduleModifyWidget::ScheduleModifyWidget(const QVariantMap& scheduleData, QWidget *parent) : QWidget(parent) {
    m_scheduleId = scheduleData["id"].toInt();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    titleInput = new QLineEdit(this);
    titleInput->setText(scheduleData["title"].toString());

    categoryCombo = new QComboBox(this);
    categoryCombo->addItem("Work", 1);
    categoryCombo->addItem("Personal", 2);
    categoryCombo->addItem("Other", 3);
    // 현재 카테고리 설정
    int catIdx = categoryCombo->findData(scheduleData["category_id"]);
    if (catIdx != -1) categoryCombo->setCurrentIndex(catIdx);

    allDayCheck = new QCheckBox("All Day", this);

    QDateTime start = QDateTime::fromString(scheduleData["start"].toString(), "yyyy-MM-dd HH:mm:ss");
    QDateTime end = QDateTime::fromString(scheduleData["end"].toString(), "yyyy-MM-dd HH:mm:ss");

    startTimeEdit = new QDateTimeEdit(start, this);
    startTimeEdit->setCalendarPopup(true);
    endTimeEdit = new QDateTimeEdit(end, this);
    endTimeEdit->setCalendarPopup(true);

    contentInput = new QTextEdit(this);
    contentInput->setPlainText(scheduleData["content"].toString());
    contentInput->setMaximumHeight(100);

    updateBtn = new QPushButton("Update", this);
    updateBtn->setStyleSheet("background-color: #4A90E2; color: white; font-weight: bold; padding: 5px;");

    deleteBtn = new QPushButton("Delete", this);
    deleteBtn->setStyleSheet("background-color: #E24A4A; color: white; font-weight: bold; padding: 5px;");

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(updateBtn);
    btnLayout->addWidget(deleteBtn);

    formLayout->addRow("Title:", titleInput);
    formLayout->addRow("Category:", categoryCombo);
    formLayout->addRow("", allDayCheck);
    formLayout->addRow("Start:", startTimeEdit);
    formLayout->addRow("End:", endTimeEdit);
    formLayout->addRow("Content:", contentInput);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(btnLayout);

    connect(allDayCheck, &QCheckBox::toggled, this, &ScheduleModifyWidget::toggleAllDay);
    connect(updateBtn, &QPushButton::clicked, this, &ScheduleModifyWidget::handleUpdate);
    connect(deleteBtn, &QPushButton::clicked, this, &ScheduleModifyWidget::handleDelete);
}

void ScheduleModifyWidget::toggleAllDay(bool checked) {
    if (checked) {
        QDate current = startTimeEdit->date();
        startTimeEdit->setDateTime(QDateTime(current, QTime(0, 0, 0)));
        endTimeEdit->setDateTime(QDateTime(current, QTime(23, 59, 59)));
        startTimeEdit->setDisplayFormat("yyyy-MM-dd");
        endTimeEdit->setDisplayFormat("yyyy-MM-dd");
    } else {
        startTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
        endTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    }
}

void ScheduleModifyWidget::handleUpdate() {
    QString title = titleInput->text();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a title.");
        return;
    }

    int catId = categoryCombo->currentData().toInt();
    QDateTime start = startTimeEdit->dateTime();
    QDateTime end = endTimeEdit->dateTime();
    QString content = contentInput->toPlainText();

    bool success = DatabaseManager::instance().updateSchedule(m_scheduleId, catId, title, content, start, end, "#4A90E2");

    if (success) {
        emit scheduleUpdated();
        this->close();
    }
}

void ScheduleModifyWidget::handleDelete() {
    if (QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this schedule?") == QMessageBox::Yes) {
        bool success = DatabaseManager::instance().deleteSchedule(m_scheduleId);
        if (success) {
            emit scheduleDeleted();
            this->close();
        }
    }
}
