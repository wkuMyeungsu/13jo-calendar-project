#include "ColorPickerPopup.h"
#include "UiCommon.h"

ColorPickerPopup::ColorPickerPopup(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setupUI();
}

void ColorPickerPopup::setupUI() {
    // 내부 컨테이너 위젯 (실제 배경과 테두리)
    QWidget *container = new QWidget(this);
    container->setObjectName("container");
    container->setStyleSheet(StyleHelper::getPopupStyle());

    // 최상위 레이아웃: 여백을 0으로 설정하여 투명한 부분이 남지 않도록 함
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); 
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(container);

    // 실제 버튼들이 들어갈 그리드 레이아웃
    QGridLayout *gridLayout = new QGridLayout(container);
    gridLayout->setContentsMargins(UiConstants::LAYOUT_SPACING, UiConstants::LAYOUT_SPACING, UiConstants::LAYOUT_SPACING, UiConstants::LAYOUT_SPACING);
    gridLayout->setSpacing(UiConstants::BTN_LAYOUT_SPACING);

    const int columns = 5;
    for (int i = 0; i < m_colors.size(); ++i) {
        QString color = m_colors[i];
        QPushButton *btn = new QPushButton(container);
        btn->setFixedSize(28, 28);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setProperty("colorValue", color);
        
        btn->setStyleSheet(StyleHelper::getCircleButtonStyle(color, 28));

        connect(btn, &QPushButton::clicked, this, &ColorPickerPopup::handleColorClick);
        gridLayout->addWidget(btn, i / columns, i % columns);
    }
}

void ColorPickerPopup::handleColorClick() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        emit colorSelected(btn->property("colorValue").toString());
    }
    this->close();
}
