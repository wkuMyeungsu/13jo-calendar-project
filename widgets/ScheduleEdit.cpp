#include "ScheduleEdit.h"
#include "StyleHelper.h"
#include "../models/DatabaseManager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDebug>
#include <QColorDialog>

ScheduleInputWidget::ScheduleInputWidget(const QDate& initialDate, QWidget *parent) : QWidget(parent) {
    setFixedSize(StyleHelper::WIDGET_WIDTH, 520);
    setWindowTitle("새 일정 추가");
    setStyleSheet(QString("background-color: %1;").arg(StyleHelper::getBgColor()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN);
    mainLayout->setSpacing(StyleHelper::LAYOUT_SPACING);

    QLabel *headerLabel = new QLabel("새 일정 등록", this);
    headerLabel->setStyleSheet(QString("font-size: 18px; font-weight: bold; color: %1; margin-bottom: 5px;").arg(StyleHelper::getTextColor()));
    mainLayout->addWidget(headerLabel);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setVerticalSpacing(12);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QString commonStyle = StyleHelper::getCommonInputStyle();

    titleInput = new QLineEdit(this);
    titleInput->setPlaceholderText("일정 제목을 입력하세요");
    titleInput->setStyleSheet(commonStyle);

    categoryCombo = new QComboBox(this);
    categoryCombo->setStyleSheet(commonStyle);
    auto categories = DatabaseManager::instance().getCategories();
    for (const auto& cat : categories) {
        categoryCombo->addItem(cat["name"].toString(), cat["id"]);
    }

    m_selectedColor = StyleHelper::getPrimaryColor();
    colorBtn = new QPushButton("색상 선택", this);
    colorBtn->setCursor(Qt::PointingHandCursor);
    colorBtn->setFixedHeight(32);
    colorBtn->setStyleSheet(QString("background-color: %1; color: white; font-weight: bold; border-radius: 6px; border: none;").arg(m_selectedColor));

    allDayCheck = new QCheckBox("하루 종일", this);
    allDayCheck->setStyleSheet(QString("QCheckBox { color: %1; font-weight: bold; } QCheckBox::indicator { width: 18px; height: 18px; }").arg(StyleHelper::getTextColor()));

    QDateTime startDateTime(initialDate, QTime::currentTime());
    startTimeEdit = new QDateTimeEdit(startDateTime, this);
    startTimeEdit->setCalendarPopup(true);
    startTimeEdit->setStyleSheet(commonStyle + StyleHelper::getCalendarPopupStyle());

    endTimeEdit = new QDateTimeEdit(startDateTime.addSecs(3600), this);
    endTimeEdit->setCalendarPopup(true);
    endTimeEdit->setStyleSheet(commonStyle + StyleHelper::getCalendarPopupStyle());

    contentInput = new QTextEdit(this);
    contentInput->setPlaceholderText("메모를 입력하세요...");
    contentInput->setMaximumHeight(100);
    contentInput->setStyleSheet(commonStyle + "QTextEdit {" + StyleHelper::getScrollbarStyle() + "}");

    saveBtn = new QPushButton("일정 저장", this);
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet(StyleHelper::getBtnSaveStyle());

    auto createFormLabel = [&](const QString& text) {
        QLabel* label = new QLabel(text, this);
        label->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 13px;").arg(StyleHelper::currentTheme == StyleHelper::Theme::Dark ? "#AAA" : "#666"));
        return label;
    };

    formLayout->addRow(createFormLabel("제목"), titleInput);
    formLayout->addRow(createFormLabel("카테고리"), categoryCombo);
    formLayout->addRow(createFormLabel("색상"), colorBtn);
    formLayout->addRow("", allDayCheck);
    formLayout->addRow(createFormLabel("시작"), startTimeEdit);
    formLayout->addRow(createFormLabel("종료"), endTimeEdit);
    formLayout->addRow(createFormLabel("내용"), contentInput);

    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(saveBtn);

    connect(allDayCheck, &QCheckBox::toggled, this, &ScheduleInputWidget::toggleAllDay);
    connect(colorBtn, &QPushButton::clicked, this, &ScheduleInputWidget::selectColor);
    connect(saveBtn, &QPushButton::clicked, this, &ScheduleInputWidget::handleSave);
}

void ScheduleInputWidget::selectColor() {
    QColor color = QColorDialog::getColor(QColor(m_selectedColor), this, "일정 색상 선택");
    if (color.isValid()) {
        m_selectedColor = color.name();
        colorBtn->setStyleSheet(QString("background-color: %1; color: white; font-weight: bold; border-radius: 6px; border: none;").arg(m_selectedColor));
    }
}

void ScheduleInputWidget::toggleAllDay(bool checked) {
    if (checked) {
        startTimeEdit->setDisplayFormat("yyyy-MM-dd");
        endTimeEdit->setDisplayFormat("yyyy-MM-dd");
    } else {
        startTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
        endTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    }
}

void ScheduleInputWidget::handleSave() {
    if (titleInput->text().isEmpty()) {
        QMessageBox::warning(this, "알림", "일정 제목을 입력해 주세요.");
        return;
    }
    if (DatabaseManager::instance().addSchedule(categoryCombo->currentData().toInt(), titleInput->text(), contentInput->toPlainText(), startTimeEdit->dateTime(), endTimeEdit->dateTime(), m_selectedColor)) {
        emit scheduleSaved();
        this->close();
    }
}
