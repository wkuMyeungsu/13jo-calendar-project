#include "ScheduleModifyWidget.h"
#include "StyleHelper.h"
#include "../models/DatabaseManager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include "ColorPickerPopup.h"

// 리팩토링: 상수 정의
namespace {
    const int INPUT_HEIGHT = 32;
    const QString DATE_FORMAT_FULL = "yyyy-MM-dd HH:mm";
    const QString DATE_FORMAT_DAY = "yyyy-MM-dd";
    const QString DEFAULT_COLOR = "#4A90E2";
}

ScheduleModifyWidget::ScheduleModifyWidget(const QVariantMap& scheduleData, QWidget *parent) : QWidget(parent) {
    m_scheduleId = scheduleData["id"].toInt();
    m_selectedColor = scheduleData["color"].toString();
    if (m_selectedColor.isEmpty()) m_selectedColor = DEFAULT_COLOR;

    setFixedSize(StyleHelper::WIDGET_WIDTH, StyleHelper::WIDGET_HEIGHT);
    setWindowTitle("일정 수정/삭제");
    setStyleSheet(QString("background-color: %1;").arg(StyleHelper::getBgColor()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN);
    mainLayout->setSpacing(StyleHelper::LAYOUT_SPACING);

    QLabel *headerLabel = new QLabel("일정 상세 정보", this);
    headerLabel->setStyleSheet(StyleHelper::getHeaderStyle());
    mainLayout->addWidget(headerLabel);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setVerticalSpacing(12);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QString inputStyle = StyleHelper::getCommonInputStyle() + StyleHelper::getCalendarPopupStyle();

    titleInput = new QLineEdit(this);
    titleInput->setText(scheduleData["title"].toString());
    titleInput->setStyleSheet(inputStyle);

    categoryCombo = new QComboBox(this);
    categoryCombo->setStyleSheet(inputStyle);
    auto categories = DatabaseManager::instance().getCategories();
    for (const auto& cat : categories) {
        categoryCombo->addItem(cat["name"].toString(), cat["id"]);
    }
    int catIdx = categoryCombo->findData(scheduleData["category_id"]);
    if (catIdx != -1) categoryCombo->setCurrentIndex(catIdx);

    colorBtn = new QPushButton("색상 선택", this);
    colorBtn->setCursor(Qt::PointingHandCursor);
    colorBtn->setFixedHeight(INPUT_HEIGHT);
    colorBtn->setStyleSheet(QString("background-color: %1; color: white; font-weight: bold; border-radius: 6px; border: none;").arg(m_selectedColor));

    allDayCheck = new QCheckBox("하루 종일", this);
    allDayCheck->setStyleSheet(StyleHelper::getCheckboxStyle());
    if (scheduleData["all_day"].toInt() == 1) allDayCheck->setChecked(true);

    QDateTime start = QDateTime::fromString(scheduleData["start"].toString(), "yyyy-MM-dd HH:mm:ss");
    QDateTime end = QDateTime::fromString(scheduleData["end"].toString(), "yyyy-MM-dd HH:mm:ss");

    startTimeEdit = new QDateTimeEdit(start, this);
    startTimeEdit->setCalendarPopup(true);
    startTimeEdit->setStyleSheet(inputStyle);
    
    endTimeEdit = new QDateTimeEdit(end, this);
    endTimeEdit->setCalendarPopup(true);
    endTimeEdit->setStyleSheet(inputStyle);

    contentInput = new QTextEdit(this);
    contentInput->setPlainText(scheduleData["content"].toString());
    contentInput->setMaximumHeight(100);
    contentInput->setStyleSheet(inputStyle + StyleHelper::getScrollbarStyle());

    updateBtn = new QPushButton("수정 내용 저장", this);
    updateBtn->setCursor(Qt::PointingHandCursor);
    updateBtn->setStyleSheet(StyleHelper::getBtnModifyStyle());

    deleteBtn = new QPushButton("일정 삭제", this);
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setStyleSheet(StyleHelper::getBtnDeleteStyle());

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);
    btnLayout->addWidget(updateBtn, 2);
    btnLayout->addWidget(deleteBtn, 1);

    auto createFormLabel = [&](const QString& text) {
        QLabel* label = new QLabel(text, this);
        label->setStyleSheet(StyleHelper::getFormLabelStyle());
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
    mainLayout->addLayout(btnLayout);

    connect(allDayCheck, &QCheckBox::toggled, this, &ScheduleModifyWidget::toggleAllDay);
    connect(colorBtn, &QPushButton::clicked, this, &ScheduleModifyWidget::selectColor);
    connect(updateBtn, &QPushButton::clicked, this, &ScheduleModifyWidget::handleUpdate);
    connect(deleteBtn, &QPushButton::clicked, this, &ScheduleModifyWidget::handleDelete);

    toggleAllDay(allDayCheck->isChecked());
}

void ScheduleModifyWidget::selectColor() {
    ColorPickerPopup *popup = new ColorPickerPopup(this);
    popup->move(colorBtn->mapToGlobal(QPoint(0, colorBtn->height())));
    
    connect(popup, &ColorPickerPopup::colorSelected, this, [this](const QString& color) {
        m_selectedColor = color;
        colorBtn->setStyleSheet(QString("background-color: %1; color: white; font-weight: bold; border-radius: 6px; border: none;").arg(m_selectedColor));
    });
    
    popup->show();
}

void ScheduleModifyWidget::toggleAllDay(bool checked) {
    if (checked) {
        startTimeEdit->setDisplayFormat(DATE_FORMAT_DAY);
        endTimeEdit->setDisplayFormat(DATE_FORMAT_DAY);
    } else {
        startTimeEdit->setDisplayFormat(DATE_FORMAT_FULL);
        endTimeEdit->setDisplayFormat(DATE_FORMAT_FULL);
    }
}

void ScheduleModifyWidget::handleUpdate() {
    QString title = titleInput->text();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "알림", "일정 제목을 입력해 주세요.");
        return;
    }

    if (DatabaseManager::instance().updateSchedule(m_scheduleId, categoryCombo->currentData().toInt(), title, contentInput->toPlainText(), startTimeEdit->dateTime(), endTimeEdit->dateTime(), m_selectedColor)) {
        emit scheduleUpdated();
        this->close();
    }
}

void ScheduleModifyWidget::handleDelete() {
    if (QMessageBox::question(this, "삭제 확인", "정말 이 일정을 삭제하시겠습니까?") == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteSchedule(m_scheduleId)) {
            emit scheduleDeleted();
            this->close();
        }
    }
}
