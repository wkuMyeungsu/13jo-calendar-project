#include "SettingsWidget.h"
#include "UiCommon.h"
#include "CustomMessageBox.h"

SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent) {
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(StyleHelper::WIDGET_WIDTH + 2, 260 + UiConstants::TITLE_BAR_HEIGHT + 2);
    setWindowTitle("설정");

    // [New] 마스터 프레임 (모든 것을 감싸고 테두리/곡률 담당)
    m_mainFrame = new QFrame(this);
    m_mainFrame->setObjectName("mainFrame");
    m_mainFrame->setStyleSheet(StyleHelper::getDialogFrameStyle());

    // 최상위 레이아웃 (프레임에 1px 여백을 주어 곡률 안티앨리어싱 확보)
    QVBoxLayout* masterLayout = new QVBoxLayout(this);
    masterLayout->setContentsMargins(1, 1, 1, 1);
    masterLayout->addWidget(m_mainFrame);

    // 프레임 내부 레이아웃
    QVBoxLayout* frameLayout = new QVBoxLayout(m_mainFrame);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    frameLayout->setSpacing(0);

    m_titleBar = new CustomTitleBar(m_mainFrame);
    m_titleBar->setResizable(false);
    m_titleBar->showMinMaxButtons(false); // 최소화/최대화 숨김
    m_titleBar->setTitle(UiConstants::TITLE_SETTINGS);
    m_titleBar->applyTheme(StyleHelper::getBgColor(), StyleHelper::getTextColor(), "#DDD");
    frameLayout->addWidget(m_titleBar);

    m_contentWidget = new QWidget(m_mainFrame);
    m_contentWidget->setObjectName("container");
    m_contentWidget->setStyleSheet(StyleHelper::getDialogStyle());
    frameLayout->addWidget(m_contentWidget);

    // QSettings 초기화 (조직명, 앱이름을 기반으로 경로 자동 지정)
    QSettings settings("MyCompany", "CalendarProject");

    QVBoxLayout *mainLayout = new QVBoxLayout(m_contentWidget);
    mainLayout->setContentsMargins(UiConstants::CONTENT_MARGIN, UiConstants::CONTENT_MARGIN, UiConstants::CONTENT_MARGIN, UiConstants::CONTENT_MARGIN);
    mainLayout->setSpacing(UiConstants::LAYOUT_SPACING);

    QFormLayout *formLayout = new QFormLayout();

    formLayout->setVerticalSpacing(UiConstants::FORM_VERTICAL_SPACING);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // 테마 선택 콤보박스
    m_themeCombo = new QComboBox(m_contentWidget);
    m_themeCombo->addItem("기본 (White & Blue)", static_cast<int>(StyleHelper::Theme::Default));
    m_themeCombo->addItem("소프트 파스텔", static_cast<int>(StyleHelper::Theme::Soft));
    m_themeCombo->addItem("딥 블루", static_cast<int>(StyleHelper::Theme::DeepBlue));
    
    // 저장된 테마 불러오기 (기본값 Default)
    int savedTheme = settings.value("theme", static_cast<int>(StyleHelper::Theme::Default)).toInt();
    m_initialTheme = savedTheme; // 초기 상태 백업
    m_isApplied = false;
    StyleHelper::currentTheme = static_cast<StyleHelper::Theme>(savedTheme);
    m_themeCombo->setCurrentIndex(m_themeCombo->findData(savedTheme));
    m_themeCombo->setStyleSheet(StyleHelper::getCommonInputStyle());

    auto createLabel = [&](const QString& text) {
        QLabel* label = new QLabel(text, m_contentWidget);
        label->setStyleSheet(StyleHelper::getFormLabelStyle());
        return label;
    };

    formLayout->addRow(createLabel("테마 선택"), m_themeCombo);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();

    m_applyBtn = new QPushButton("설정 적용", m_contentWidget);
    m_applyBtn->setStyleSheet(StyleHelper::getBtnSaveStyle());

    m_resetBtn = new QPushButton("데이터 전체 초기화", m_contentWidget);
    m_resetBtn->setStyleSheet(StyleHelper::getBtnDeleteStyle());

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);
    btnLayout->addWidget(m_applyBtn, 2);
    btnLayout->addWidget(m_resetBtn, 1);
    
    mainLayout->addLayout(btnLayout);

    connect(m_applyBtn, &QPushButton::clicked, this, &SettingsWidget::applySettings);
    connect(m_resetBtn, &QPushButton::clicked, this, &SettingsWidget::handleReset);
    connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsWidget::previewTheme);
}

void SettingsWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void SettingsWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        m_titleBar->updateMaxIcon();
    }
    QWidget::changeEvent(event);
}

void SettingsWidget::closeEvent(QCloseEvent *event) {
    int currentThemeIdx = m_themeCombo->currentData().toInt();
    
    if (!m_isApplied && currentThemeIdx != m_initialTheme) {
        if (CustomMessageBox::question(this, "설정 취소", "변경한 설정이 적용되지 않았습니다.\n그대로 나가시겠습니까?")) {
            // 원복 후 종료
            StyleHelper::currentTheme = static_cast<StyleHelper::Theme>(m_initialTheme);
            emit settingsChanged();
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}

void SettingsWidget::handleCloseRequested() {
    this->close(); // closeEvent가 가로챔
}

void SettingsWidget::applySettings() {
    m_isApplied = true;
    
    // 1. 전역 변수 업데이트
    int selectedThemeIdx = m_themeCombo->currentData().toInt();
    StyleHelper::currentTheme = static_cast<StyleHelper::Theme>(selectedThemeIdx);
    
    // 2. QSettings를 사용하여 파일에 영구 저장
    QSettings settings("MyCompany", "CalendarProject");
    settings.setValue("theme", selectedThemeIdx);
    
    emit settingsChanged();
    CustomMessageBox::information(this, "알림", "설정이 저장되었습니다.");
    this->close();
}

void SettingsWidget::handleReset() {
    if (CustomMessageBox::question(this, "위험: 전체 초기화", 
        "모든 일정과 카테고리가 삭제됩니다.\n이 작업은 되돌릴 수 없습니다. 계속하시겠습니까?")) {

        if (DatabaseManager::instance().resetDatabase()) {
            CustomMessageBox::information(this, "완료", "데이터베이스가 초기화되었습니다.");
            emit settingsChanged(); // 메인 화면 갱신 유도
            this->close();
        } else {
            CustomMessageBox::warning(this, "오류", "데이터 초기화 중 문제가 발생했습니다.");
        }
    }
}

void SettingsWidget::previewTheme(int index) {
    // 1. 전역 테마 임시 변경
    StyleHelper::currentTheme = static_cast<StyleHelper::Theme>(m_themeCombo->itemData(index).toInt());
    
    // 2. 현재 창 스타일 즉시 갱신
    updateFormStyle();
    
    // 3. 메인 화면 실시간 반영
    emit settingsChanged();
}

void SettingsWidget::updateFormStyle() {
    m_mainFrame->setStyleSheet(StyleHelper::getDialogFrameStyle());
    m_contentWidget->setStyleSheet(StyleHelper::getDialogStyle());
    m_titleBar->applyTheme(StyleHelper::getBgColor(), StyleHelper::getTextColor(), "#DDD");
    
    // 라벨 스타일 갱신
    QList<QLabel*> labels = m_contentWidget->findChildren<QLabel*>();
    for (QLabel* label : labels) {
        label->setStyleSheet(StyleHelper::getFormLabelStyle());
    }

    m_themeCombo->setStyleSheet(StyleHelper::getCommonInputStyle());
    m_applyBtn->setStyleSheet(StyleHelper::getBtnSaveStyle());
    m_resetBtn->setStyleSheet(StyleHelper::getBtnDeleteStyle());
}
