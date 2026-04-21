#include "ScheduleModifyWidget.h"
#include "UiCommon.h"
#include "ColorPickerPopup.h"

// 리팩토링: 상수 정의
namespace {
    const int INPUT_HEIGHT = 32;
    const QString DATE_FORMAT_FULL = "yyyy-MM-dd HH:mm";
    const QString DATE_FORMAT_DAY = "yyyy-MM-dd";
    const QString DEFAULT_COLOR = "#4A90E2";
}

ScheduleModifyWidget::ScheduleModifyWidget(const Schedule& scheduleData, QWidget *parent) : QWidget(parent) {
    m_scheduleId    = scheduleData.id;
    m_selectedColor = scheduleData.color.isEmpty() ? DEFAULT_COLOR : scheduleData.color;

    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(StyleHelper::WIDGET_WIDTH + 2, StyleHelper::WIDGET_HEIGHT + UiConstants::TITLE_BAR_HEIGHT + 2);
    setWindowTitle("일정 수정/삭제");

    // [New] 마스터 프레임 (모든 것을 감싸고 테두리/곡률 담당)
    QFrame* mainFrame = new QFrame(this);
    mainFrame->setObjectName("mainFrame");
    mainFrame->setStyleSheet(StyleHelper::getDialogFrameStyle());

    // 최상위 레이아웃 (프레임에 1px 여백을 주어 곡률 안티앨리어싱 확보)
    QVBoxLayout* masterLayout = new QVBoxLayout(this);
    masterLayout->setContentsMargins(1, 1, 1, 1);
    masterLayout->addWidget(mainFrame);

    // 프레임 내부 레이아웃
    QVBoxLayout* frameLayout = new QVBoxLayout(mainFrame);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    frameLayout->setSpacing(0);

    m_titleBar = new CustomTitleBar(mainFrame);
    m_titleBar->setResizable(false);
    m_titleBar->applyTheme(StyleHelper::getBgColor(), StyleHelper::getTextColor(), "#DDD");
    frameLayout->addWidget(m_titleBar);

    m_contentWidget = new QWidget(mainFrame);
    m_contentWidget->setObjectName("container");
    m_contentWidget->setStyleSheet(StyleHelper::getDialogStyle());
    frameLayout->addWidget(m_contentWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_contentWidget);
    mainLayout->setContentsMargins(StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN);
    mainLayout->setSpacing(StyleHelper::LAYOUT_SPACING);

    QLabel *headerLabel = new QLabel("일정 상세 정보", m_contentWidget);
    headerLabel->setStyleSheet(StyleHelper::getHeaderStyle());
    mainLayout->addWidget(headerLabel);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setVerticalSpacing(12);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QString inputStyle = StyleHelper::getCommonInputStyle() + StyleHelper::getCalendarPopupStyle();

    titleInput = new QLineEdit(m_contentWidget);
    titleInput->setText(scheduleData.title);
    titleInput->setStyleSheet(inputStyle);

    categoryCombo = new QComboBox(m_contentWidget);
    categoryCombo->setStyleSheet(inputStyle);
    auto categories = DatabaseManager::instance().getCategories();
    for (const auto& cat : categories) {
        categoryCombo->addItem(cat.name, cat.id);
    }
    int catIdx = categoryCombo->findData(scheduleData.categoryId);
    if (catIdx != -1) categoryCombo->setCurrentIndex(catIdx);

    colorBtn = new QPushButton("색상 선택", m_contentWidget);
    colorBtn->setCursor(Qt::PointingHandCursor);
    colorBtn->setFixedHeight(INPUT_HEIGHT);
    colorBtn->setStyleSheet(QString("background-color: %1; color: white; font-weight: bold; border-radius: 6px; border: none;").arg(m_selectedColor));

    allDayCheck = new QCheckBox("하루 종일", m_contentWidget);
    allDayCheck->setStyleSheet(StyleHelper::getCheckboxStyle());

    startTimeEdit = new QDateTimeEdit(scheduleData.start, m_contentWidget);
    startTimeEdit->setCalendarPopup(true);
    startTimeEdit->setStyleSheet(inputStyle);

    endTimeEdit = new QDateTimeEdit(scheduleData.end, m_contentWidget);
    endTimeEdit->setCalendarPopup(true);
    endTimeEdit->setStyleSheet(inputStyle);

    contentInput = new QTextEdit(m_contentWidget);
    contentInput->setPlainText(scheduleData.content);
    contentInput->setMaximumHeight(100);
    contentInput->setStyleSheet(inputStyle + StyleHelper::getScrollbarStyle());

    updateBtn = new QPushButton("수정 내용 저장", m_contentWidget);
    updateBtn->setCursor(Qt::PointingHandCursor);
    updateBtn->setStyleSheet(StyleHelper::getBtnModifyStyle());

    deleteBtn = new QPushButton("일정 삭제", m_contentWidget);
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setStyleSheet(StyleHelper::getBtnDeleteStyle());

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);
    btnLayout->addWidget(updateBtn, 2);
    btnLayout->addWidget(deleteBtn, 1);

    auto createFormLabel = [&](const QString& text) {
        QLabel* label = new QLabel(text, m_contentWidget);
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

void ScheduleModifyWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void ScheduleModifyWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        m_titleBar->updateMaxIcon();
    }
    QWidget::changeEvent(event);
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
