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
    // 카테고리 로드
    QList<QVariantMap> categories = DatabaseManager::instance().getCategories();
    for (const auto& cat : categories) {
        categoryCombo->addItem(cat["name"].toString(), cat["id"]);
    }

    m_selectedColor = "#4A90E2"; // 기본 색상
    colorBtn = new QPushButton("Pick Color", this);
    colorBtn->setStyleSheet(QString("background-color: %1; color: white; font-weight: bold;").arg(m_selectedColor));

    allDayCheck = new QCheckBox("All Day", this);

    // 전달받은 날짜와 현재 시간을 결합
    QDateTime startDateTime(initialDate, QTime::currentTime());
    startTimeEdit = new QDateTimeEdit(startDateTime, this);
    startTimeEdit->setCalendarPopup(true);

    endTimeEdit = new QDateTimeEdit(startDateTime.addSecs(3600), this);
    endTimeEdit->setCalendarPopup(true);

    contentInput = new QTextEdit(this);
    contentInput->setPlaceholderText("상세 내용을 입력하세요...");
    contentInput->setMaximumHeight(100);

    saveBtn = new QPushButton("Save Schedule", this);
    saveBtn->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 5px;");

    // 폼 레이아웃 배치
    formLayout->addRow("Title:", titleInput);
    formLayout->addRow("Category:", categoryCombo);
    formLayout->addRow("Color:", colorBtn);
    formLayout->addRow("", allDayCheck);
    formLayout->addRow("Start:", startTimeEdit);
    formLayout->addRow("End:", endTimeEdit);
    formLayout->addRow("Content:", contentInput);

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(saveBtn);

    connect(allDayCheck, &QCheckBox::toggled, this, &ScheduleInputWidget::toggleAllDay);
    connect(colorBtn, &QPushButton::clicked, this, &ScheduleInputWidget::selectColor);
    connect(saveBtn, &QPushButton::clicked, this, &ScheduleInputWidget::handleSave);
}

void ScheduleInputWidget::selectColor() {
    QColor color = QColorDialog::getColor(QColor(m_selectedColor), this, "Select Schedule Color");
    if (color.isValid()) {
        m_selectedColor = color.name();
        colorBtn->setStyleSheet(QString("background-color: %1; color: white; font-weight: bold;").arg(m_selectedColor));
    }
}

void ScheduleInputWidget::toggleAllDay(bool checked) {
    if (checked) {
        // 하루종일 체크 시 시간을 00:00 ~ 23:59로 고정
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

void ScheduleInputWidget::handleSave() {
    QString title = titleInput->text();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a title.");
        return;
    }

    int catId = categoryCombo->currentData().toInt();
    QDateTime start = startTimeEdit->dateTime();
    QDateTime end = endTimeEdit->dateTime();
    QString content = contentInput->toPlainText();

    // DB 저장 호출 (선택한 색상 반영)
    bool success = DatabaseManager::instance().addSchedule(catId, title, content, start, end, m_selectedColor);

    if (success) {
        titleInput->clear();
        contentInput->clear();
        emit scheduleSaved(); // 신호 발생
        qDebug() << "Schedule saved successfully.";
        this->close();
    }
}
