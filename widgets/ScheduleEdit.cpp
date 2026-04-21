#include "ScheduleEdit.h"
#include "UiCommon.h"
#include "ColorPickerPopup.h"

ScheduleInputWidget::ScheduleInputWidget(const QDate& initialDate, QWidget *parent) : QWidget(parent) {
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
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

    // 타이틀바 생성 (프레임 내부에 배치)
    m_titleBar = new CustomTitleBar(mainFrame);
    m_titleBar->setResizable(false);
    frameLayout->addWidget(m_titleBar);
    
    // 메인 컨텐츠 영역
    m_contentWidget = new QWidget(mainFrame);
    m_contentWidget->setObjectName("container");
    m_contentWidget->setStyleSheet(StyleHelper::getDialogStyle());
    frameLayout->addWidget(m_contentWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_contentWidget);
    mainLayout->setContentsMargins(UiConstants::CONTENT_MARGIN, UiConstants::CONTENT_MARGIN, UiConstants::CONTENT_MARGIN, UiConstants::CONTENT_MARGIN);
    mainLayout->setSpacing(UiConstants::LAYOUT_SPACING);

    QLabel *headerLabel = new QLabel("새 일정 추가", m_contentWidget);
    headerLabel->setStyleSheet(StyleHelper::getHeaderStyle());
    mainLayout->addWidget(headerLabel);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setVerticalSpacing(UiConstants::FORM_VERTICAL_SPACING);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto createFormLabel = [this](const QString& text) {
        QLabel *lbl = new QLabel(text, m_contentWidget);
        lbl->setStyleSheet(StyleHelper::getFormLabelStyle());
        return lbl;
    };

    titleInput = new QLineEdit(m_contentWidget);
    titleInput->setPlaceholderText("일정 제목을 입력하세요");
    titleInput->setStyleSheet(StyleHelper::getCommonInputStyle());

    categoryCombo = new QComboBox(m_contentWidget);
    auto categories = DatabaseManager::instance().getCategories();
    for (const auto& cat : categories) {
        categoryCombo->addItem(cat.name, cat.id);
    }
    categoryCombo->setStyleSheet(StyleHelper::getCommonInputStyle());

    m_selectedColor = StyleHelper::getPrimaryColor();
    colorBtn = new QPushButton(m_contentWidget);
    colorBtn->setCursor(Qt::PointingHandCursor);
    colorBtn->setFixedSize(UiConstants::COLOR_BTN_SIZE, UiConstants::COLOR_BTN_SIZE);
    colorBtn->setStyleSheet(StyleHelper::getCircleButtonStyle(m_selectedColor, UiConstants::COLOR_BTN_SIZE));

    allDayCheck = new QCheckBox("하루 종일", m_contentWidget);
    allDayCheck->setStyleSheet(StyleHelper::getCheckboxStyle());

    startTimeEdit = new QDateTimeEdit(QDateTime(initialDate, QTime(9, 0)), m_contentWidget);
    startTimeEdit->setCalendarPopup(true);
    startTimeEdit->setStyleSheet(StyleHelper::getCommonInputStyle());

    endTimeEdit = new QDateTimeEdit(QDateTime(initialDate, QTime(10, 0)), m_contentWidget);
    endTimeEdit->setCalendarPopup(true);
    endTimeEdit->setStyleSheet(StyleHelper::getCommonInputStyle());

    contentInput = new QTextEdit(m_contentWidget);
    contentInput->setPlaceholderText("메모를 입력하세요");
    contentInput->setStyleSheet(StyleHelper::getCommonInputStyle());

    formLayout->addRow(createFormLabel("제목"), titleInput);
    formLayout->addRow(createFormLabel("카테고리"), categoryCombo);
    formLayout->addRow(createFormLabel("색상"), colorBtn);
    formLayout->addRow("", allDayCheck);
    formLayout->addRow(createFormLabel("시작"), startTimeEdit);
    formLayout->addRow(createFormLabel("종료"), endTimeEdit);
    formLayout->addRow(createFormLabel("메모"), contentInput);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(UiConstants::BTN_LAYOUT_SPACING);
    QPushButton *cancelBtn = new QPushButton("취소", m_contentWidget);
    cancelBtn->setStyleSheet(StyleHelper::getBtnModifyStyle());
    saveBtn = new QPushButton("저장", m_contentWidget);
    saveBtn->setStyleSheet(StyleHelper::getBtnSaveStyle());

    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(saveBtn);
    mainLayout->addLayout(btnLayout);

    connect(saveBtn, &QPushButton::clicked, this, &ScheduleInputWidget::handleSave);
    connect(cancelBtn, &QPushButton::clicked, this, &QWidget::close);
    connect(allDayCheck, &QCheckBox::toggled, this, &ScheduleInputWidget::toggleAllDay);
    connect(colorBtn, &QPushButton::clicked, this, &ScheduleInputWidget::selectColor);

    // 테마 동기화
    QString bgColor = StyleHelper::getBgColor();
    QString textColor = StyleHelper::getTextColor();
    QString borderColor = (StyleHelper::currentTheme == StyleHelper::Theme::Default) ? "#E0E0E0" : "#ADCCFB";
    m_titleBar->applyTheme(bgColor, textColor, borderColor);

    // 창 크기 설정 (타이틀바 포함)
    setFixedSize(UiConstants::DIALOG_WIDGET_WIDTH + 2, UiConstants::SCHEDULE_EDIT_HEIGHT + UiConstants::TITLE_BAR_HEIGHT + 2);
}

void ScheduleInputWidget::resizeEvent(QResizeEvent* event) {
    // 이제 레이아웃이 지오메트리를 관리하므로 수동 setGeometry는 불필요하거나 최소화됨
    QWidget::resizeEvent(event);
}

void ScheduleInputWidget::changeEvent(QEvent* event) {
    if (event->type() == QEvent::WindowStateChange) {
        m_titleBar->updateMaxIcon();
    }
    QWidget::changeEvent(event);
}

void ScheduleInputWidget::toggleAllDay(bool checked) {
    startTimeEdit->setDisplayFormat(checked ? "yyyy-MM-dd" : "yyyy-MM-dd HH:mm");
    endTimeEdit->setDisplayFormat(checked ? "yyyy-MM-dd" : "yyyy-MM-dd HH:mm");
}

void ScheduleInputWidget::selectColor() {
    ColorPickerPopup *picker = new ColorPickerPopup(this);
    QPoint pos = colorBtn->mapToGlobal(QPoint(0, colorBtn->height()));
    picker->move(pos);
    connect(picker, &ColorPickerPopup::colorSelected, this, [this](const QString& color) {
        m_selectedColor = color;
        colorBtn->setStyleSheet(StyleHelper::getCircleButtonStyle(m_selectedColor, UiConstants::COLOR_BTN_SIZE));
    });
    picker->show();
}

void ScheduleInputWidget::handleSave() {
    if (titleInput->text().isEmpty()) return;
    if (DatabaseManager::instance().addSchedule(categoryCombo->currentData().toInt(), titleInput->text(), contentInput->toPlainText(), startTimeEdit->dateTime(), endTimeEdit->dateTime(), m_selectedColor)) {
        emit scheduleSaved();
        this->close();
    }
}
