#include "ScheduleEdit.h"
#include "../models/DatabaseManager.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDebug>

ScheduleInputWidget::ScheduleInputWidget(const QDate& initialDate, QWidget *parent) : QWidget(parent) {
    setFixedSize(400, 520);
    setWindowTitle("새 일정 추가");
    setStyleSheet("background-color: white;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 상단 제목
    QLabel *headerLabel = new QLabel("새 일정 등록", this);
    headerLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #333; margin-bottom: 5px;");
    mainLayout->addWidget(headerLabel);

    // 입력 필드 일괄 스타일 정의
    QString inputStyle = 
        "QLineEdit, QDateTimeEdit, QComboBox, QTextEdit {"
        "  border: 1px solid #DDD; border-radius: 6px; padding: 8px; padding-right: 30px; background: #FDFDFD; color: #333;"
        "}"
        "QLineEdit:focus, QDateTimeEdit:focus, QComboBox:focus, QTextEdit:focus {"
        "  border: 1px solid #4A90E2; background: white;"
        "}"
        "QTextEdit QScrollBar:vertical { border: none; background: transparent; width: 8px; margin: 2px 2px 2px 2px; }"
        "QTextEdit QScrollBar::handle:vertical { background: #E0E0E0; min-height: 20px; border-radius: 4px; }"
        "QTextEdit QScrollBar::handle:vertical:hover { background: #BDBDBD; }"
        "QTextEdit QScrollBar::add-line:vertical, QTextEdit QScrollBar::sub-line:vertical { border: none; background: none; height: 0px; }"
        "QTextEdit QScrollBar::add-page:vertical, QTextEdit QScrollBar::sub-page:vertical { background: none; }"
        "QComboBox::drop-down, QDateTimeEdit::drop-down {"
        "  subcontrol-origin: padding; subcontrol-position: top right; width: 24px; "
        "  border-left: 1px solid #EEE; border-top-right-radius: 6px; border-bottom-right-radius: 6px; background: #F5F5F5;"
        "}"
        "QComboBox::down-arrow, QDateTimeEdit::down-arrow {"
        "  width: 0; height: 0; border-left: 4px solid transparent; border-right: 4px solid transparent; border-top: 5px solid #888;"
        "  position: relative; top: 1px; right: 2px;"
        "}"
        "QComboBox::down-arrow:on, QDateTimeEdit::down-arrow:on, QComboBox::down-arrow:hover, QDateTimeEdit::down-arrow:hover {"
        "  border-top: 5px solid #4A90E2;"
        "}"
        /* 캘린더 팝업 스타일 수정 */
        "QCalendarWidget QWidget { background-color: white; }"
        "QCalendarWidget QToolButton { color: #333; font-weight: bold; background-color: transparent; border: none; padding: 5px; }"
        "QCalendarWidget QToolButton:hover { background-color: #EEE; }"
        "QCalendarWidget QMenu { background-color: white; color: #333; border: 1px solid #DDD; }"
        "QCalendarWidget QSpinBox { color: #333; background-color: white; border: 1px solid #DDD; padding-right: 15px; }"
        "QCalendarWidget QAbstractItemView:enabled { color: #333; selection-background-color: #4A90E2; selection-color: white; }";

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setVerticalSpacing(12);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // 입력 필드 초기화
    titleInput = new QLineEdit(this);
    titleInput->setPlaceholderText("일정 제목을 입력하세요");
    titleInput->setStyleSheet(inputStyle);

    categoryCombo = new QComboBox(this);
    categoryCombo->setStyleSheet(inputStyle);
    QList<QVariantMap> categories = DatabaseManager::instance().getCategories();
    for (const auto& cat : categories) {
        categoryCombo->addItem(cat["name"].toString(), cat["id"]);
    }

    m_selectedColor = "#4A90E2"; // 기본 색상
    colorBtn = new QPushButton("색상 선택", this);
    colorBtn->setCursor(Qt::PointingHandCursor);
    colorBtn->setFixedHeight(32);
    colorBtn->setStyleSheet(QString(
        "QPushButton { background-color: %1; color: white; font-weight: bold; border-radius: 6px; border: none; }"
        "QPushButton:hover { opacity: 0.9; }"
    ).arg(m_selectedColor));

    allDayCheck = new QCheckBox("하루 종일", this);
    allDayCheck->setStyleSheet("QCheckBox { color: #555; font-weight: bold; } QCheckBox::indicator { width: 18px; height: 18px; }");

    QDateTime startDateTime(initialDate, QTime::currentTime());
    startTimeEdit = new QDateTimeEdit(startDateTime, this);
    startTimeEdit->setCalendarPopup(true);
    startTimeEdit->setStyleSheet(inputStyle);

    endTimeEdit = new QDateTimeEdit(startDateTime.addSecs(3600), this);
    endTimeEdit->setCalendarPopup(true);
    endTimeEdit->setStyleSheet(inputStyle);

    contentInput = new QTextEdit(this);
    contentInput->setPlaceholderText("메모를 입력하세요...");
    contentInput->setMaximumHeight(100);
    contentInput->setStyleSheet(inputStyle);

    saveBtn = new QPushButton("일정 저장", this);
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #45a049; }"
    );

    // 라벨 스타일 적용
    auto createLabel = [&](const QString& text) {
        QLabel* label = new QLabel(text, this);
        label->setStyleSheet("color: #666; font-weight: bold; font-size: 13px;");
        return label;
    };

    formLayout->addRow(createLabel("제목"), titleInput);
    formLayout->addRow(createLabel("카테고리"), categoryCombo);
    formLayout->addRow(createLabel("색상"), colorBtn);
    formLayout->addRow("", allDayCheck);
    formLayout->addRow(createLabel("시작"), startTimeEdit);
    formLayout->addRow(createLabel("종료"), endTimeEdit);
    formLayout->addRow(createLabel("내용"), contentInput);

    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(saveBtn);

    connect(allDayCheck, &QCheckBox::toggled, this, &ScheduleInputWidget::toggleAllDay);
    connect(colorBtn, &QPushButton::clicked, this, &ScheduleInputWidget::selectColor);
    connect(saveBtn, &QPushButton::clicked, this, &ScheduleInputWidget::handleSave);
}

void ScheduleInputWidget::selectColor() {
    QColor color = QColorDialog::getColor(QColor(m_selectedColor), this, "Select Schedule Color");
    if (color.isValid()) {
        m_selectedColor = color.name();
        colorBtn->setStyleSheet(QString(
            "QPushButton { background-color: %1; color: white; font-weight: bold; border-radius: 6px; border: none; }"
            "QPushButton:hover { opacity: 0.9; }"
        ).arg(m_selectedColor));
    }
}

void ScheduleInputWidget::toggleAllDay(bool checked) {
    if (checked) {
        QDate current = startTimeEdit->date();
        startTimeEdit->setDateTime(QDateTime(current, QTime(0, 0, 0)));
        endTimeEdit->setDateTime(QDateTime(current, QTime(23, 59, 59)));
        startTimeEdit->setDisplayFormat("yyyy-MM-dd");
        endTimeEdit->setDisplayFormat("yyyy-MM-dd");
    } else {
        startTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
        endTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    }
}

void ScheduleInputWidget::handleSave() {
    QString title = titleInput->text();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "알림", "일정 제목을 입력해 주세요.");
        return;
    }

    int catId = categoryCombo->currentData().toInt();
    QDateTime start = startTimeEdit->dateTime();
    QDateTime end = endTimeEdit->dateTime();
    QString content = contentInput->toPlainText();

    bool success = DatabaseManager::instance().addSchedule(catId, title, content, start, end, m_selectedColor);

    if (success) {
        emit scheduleSaved();
        this->close();
    }
}
