#include "ColorPickerPopup.h"
#include "UiCommon.h"

ColorPickerPopup::ColorPickerPopup(QWidget *parent) : QWidget(parent) {
    // 팝업 설정: 테두리 없음, 항상 위, 배경 투명 (라운드 처리를 위함)
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setupUI();

    // 부드러운 그림자 효과 추가
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15);
    shadow->setXOffset(0);
    shadow->setYOffset(4);
    shadow->setColor(QColor(0, 0, 0, 60));
    setGraphicsEffect(shadow);
}

void ColorPickerPopup::setupUI() {
    // 내부 컨테이너 위젯 (여기에 실제 배경과 테두리를 입힘)
    QWidget *container = new QWidget(this);
    container->setStyleSheet(QString(
        "QWidget { background-color: %1; border: 1px solid #DDD; border-radius: 12px; }"
    ).arg(StyleHelper::getBgColor()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10); // 그림자 영역 확보
    mainLayout->addWidget(container);

    QGridLayout *gridLayout = new QGridLayout(container);
    gridLayout->setContentsMargins(12, 12, 12, 12);
    gridLayout->setSpacing(8);

    const int columns = 5;
    for (int i = 0; i < m_colors.size(); ++i) {
        QString color = m_colors[i];
        QPushButton *btn = new QPushButton(container);
        btn->setFixedSize(28, 28);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setProperty("colorValue", color); // 클릭 시 어떤 색상인지 식별용
        
        btn->setStyleSheet(QString(
            "QPushButton { background-color: %1; border-radius: 14px; border: 2px solid white; outline: none; }"
            "QPushButton:hover { border: 2px solid #999; }"
        ).arg(color));

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
