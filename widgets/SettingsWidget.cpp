#include "SettingsWidget.h"
#include "StyleHelper.h"
#include "DatabaseManager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>

SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent) {
    setWindowTitle("설정");
    setFixedSize(StyleHelper::WIDGET_WIDTH, 300);
    setStyleSheet(QString("background-color: %1;").arg(StyleHelper::getBgColor()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN);
    mainLayout->setSpacing(StyleHelper::LAYOUT_SPACING);

    QLabel *headerLabel = new QLabel("시스템 설정", this);
    headerLabel->setStyleSheet(StyleHelper::getHeaderStyle());
    mainLayout->addWidget(headerLabel);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // 테마 선택 콤보박스
    m_themeCombo = new QComboBox(this);
    m_themeCombo->addItem("기본 (White & Blue)", static_cast<int>(StyleHelper::Theme::Default));
    m_themeCombo->addItem("다크 모드", static_cast<int>(StyleHelper::Theme::Dark));
    m_themeCombo->addItem("소프트 파스텔", static_cast<int>(StyleHelper::Theme::Soft));
    m_themeCombo->addItem("딥 블루", static_cast<int>(StyleHelper::Theme::DeepBlue));
    
    // 현재 테마 인덱스 설정
    m_themeCombo->setCurrentIndex(static_cast<int>(StyleHelper::currentTheme));
    m_themeCombo->setStyleSheet(StyleHelper::getCommonInputStyle());

    // 데드라인 시간 설정
    m_deadlineSpin = new QSpinBox(this);
    m_deadlineSpin->setRange(1, 1440); // 1분 ~ 24시간
    m_deadlineSpin->setSuffix(" 분");
    m_deadlineSpin->setStyleSheet(StyleHelper::getCommonInputStyle());
    // 현재는 하드코딩된 값을 가져오지만 나중에 MainWindow에서 가져오도록 수정 가능
    m_deadlineSpin->setValue(60); 

    auto createLabel = [&](const QString& text) {
        QLabel* label = new QLabel(text, this);
        label->setStyleSheet(StyleHelper::getFormLabelStyle());
        return label;
    };

    formLayout->addRow(createLabel("테마 선택"), m_themeCombo);
    formLayout->addRow(createLabel("데드라인 알림"), m_deadlineSpin);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();

    m_applyBtn = new QPushButton("설정 적용", this);
    m_applyBtn->setStyleSheet(StyleHelper::getBtnSaveStyle());

    m_resetBtn = new QPushButton("데이터 전체 초기화", this);
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

void SettingsWidget::handleReset() {
    auto reply = QMessageBox::critical(this, "위험: 전체 초기화", 
        "모든 일정과 카테고리가 삭제됩니다.\n이 작업은 되돌릴 수 없습니다. 계속하시겠습니까?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (DatabaseManager::instance().resetDatabase()) {
            QMessageBox::information(this, "완료", "데이터베이스가 초기화되었습니다.");
            emit settingsChanged(); // 메인 화면 갱신 유도
            this->close();
        } else {
            QMessageBox::warning(this, "오류", "데이터 초기화 중 문제가 발생했습니다.");
        }
    }
}

void SettingsWidget::previewTheme(int index) {
    // 1. 전역 테마 임시 변경
    StyleHelper::currentTheme = static_cast<StyleHelper::Theme>(m_themeCombo->itemData(index).toInt());
    
    // 2. 현재 창 스타일 즉시 갱신
    updateFormStyle();
}

void SettingsWidget::updateFormStyle() {
    this->setStyleSheet(QString("background-color: %1;").arg(StyleHelper::getBgColor()));
    
    // 헤더 및 라벨 스타일 갱신 (자식 위젯들을 찾아서 적용)
    QList<QLabel*> labels = this->findChildren<QLabel*>();
    for (QLabel* label : labels) {
        if (label->text() == "시스템 설정") {
            label->setStyleSheet(StyleHelper::getHeaderStyle());
        } else {
            label->setStyleSheet(StyleHelper::getFormLabelStyle());
        }
    }

    m_themeCombo->setStyleSheet(StyleHelper::getCommonInputStyle());
    m_deadlineSpin->setStyleSheet(StyleHelper::getCommonInputStyle());
    m_applyBtn->setStyleSheet(StyleHelper::getBtnSaveStyle());
    m_resetBtn->setStyleSheet(StyleHelper::getBtnDeleteStyle());
}

void SettingsWidget::applySettings() {
    // 테마 변경 확정
    StyleHelper::currentTheme = static_cast<StyleHelper::Theme>(m_themeCombo->currentData().toInt());
    
    emit settingsChanged();
    QMessageBox::information(this, "알림", "설정이 적용되었습니다.");
    this->close();
}
