#include "CustomMessageBox.h"
#include <QPainter>
#include <QPainterPath>

CustomMessageBox::CustomMessageBox(QWidget* parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    m_primaryColor = StyleHelper::getPrimaryColor();
    initUi();
    applyStyles();
}

void CustomMessageBox::initUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(1, 1, 1, 1);
    mainLayout->setSpacing(0);

    // [1] 커스텀 타이틀바
    m_titleBar = new CustomTitleBar(this);
    m_titleBar->showMinMaxButtons(false);
    m_titleBar->setResizable(false);
    mainLayout->addWidget(m_titleBar);

    // [2] 컨텐츠 영역
    QWidget* contentWidget = new QWidget(this);
    contentWidget->setObjectName("contentWidget");
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(25, 30, 25, 25);
    contentLayout->setSpacing(20);

    QHBoxLayout* msgLayout = new QHBoxLayout();
    msgLayout->setSpacing(15);

    m_iconLabel = new QLabel(contentWidget);
    m_iconLabel->setFixedSize(48, 48);
    msgLayout->addWidget(m_iconLabel, 0, Qt::AlignTop);

    m_textLabel = new QLabel(contentWidget);
    m_textLabel->setWordWrap(true);
    m_textLabel->setStyleSheet("font-size: 14px; color: #333; line-height: 1.4;");
    msgLayout->addWidget(m_textLabel, 1, Qt::AlignVCenter);
    
    contentLayout->addLayout(msgLayout);

    // [3] 버튼 영역
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setContentsMargins(0, 10, 0, 0);
    m_buttonLayout->addStretch();
    contentLayout->addLayout(m_buttonLayout);

    mainLayout->addWidget(contentWidget);
    
    setFixedWidth(400); // 고정 폭 (필요 시 조절 가능)
}

void CustomMessageBox::applyStyles() {
    QString primary = StyleHelper::getPrimaryColor();
    QString bg = StyleHelper::getBgColor();
    QString text = StyleHelper::getTextColor();
    QString border = (StyleHelper::currentTheme == StyleHelper::Theme::DeepBlue ? "#ADCCFB" : "#DDD");

    QString qss = QString(
        "QWidget#contentWidget { background-color: %1; border-bottom-left-radius: 12px; border-bottom-right-radius: 12px; }"
        "QLabel { color: %2; }"
        "QPushButton { "
        "   padding: 8px 20px; border-radius: 6px; font-weight: bold; font-size: 13px;"
        "   background-color: #EEE; color: #444; border: 1px solid #DDD;"
        "}"
        "QPushButton:hover { background-color: #E0E0E0; }"
        "QPushButton#primaryBtn { background-color: %3; color: white; border: none; }"
        "QPushButton#primaryBtn:hover { background-color: %4; }"
    ).arg(bg, text, primary, QColor(primary).lighter(110).name());
    
    setStyleSheet(qss);
    
    // 타이틀바 테마 적용
    m_titleBar->applyTheme(bg, text, border);
}

void CustomMessageBox::setMessage(const QString& title, const QString& text, IconType iconType) {
    m_titleBar->setTitle(title);
    m_textLabel->setText(text);
    m_textLabel->setStyleSheet(QString("font-size: 14px; color: %1; line-height: 1.4; background: transparent;").arg(StyleHelper::getTextColor()));

    // 아이콘 설정 (간단한 원형 스타일)
    QString iconChar;
    QString iconColor;
    
    switch (iconType) {
        case Information: iconChar = "i"; iconColor = StyleHelper::getPrimaryColor(); break;
        case Warning:     iconChar = "!"; iconColor = "#FFB300"; break;
        case Critical:    iconChar = "X"; iconColor = "#E53935"; break;
        case Question:    iconChar = "?"; iconColor = "#43A047"; break;
    }

    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setStyleSheet(QString(
        "background-color: %1; color: white; font-size: 24px; font-weight: bold; border-radius: 24px;"
    ).arg(iconColor));
    m_iconLabel->setText(iconChar);
}

void CustomMessageBox::setButtons(QDialogButtonBox::StandardButtons buttons) {
    // 기존 버튼 제거
    QLayoutItem* item;
    while ((item = m_buttonLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
    m_buttonLayout->addStretch();

    auto addButton = [&](const QString& text, bool isPrimary, int result) {
        QPushButton* btn = new QPushButton(text, this);
        if (isPrimary) btn->setObjectName("primaryBtn");
        connect(btn, &QPushButton::clicked, this, [this, result]() { done(result); });
        m_buttonLayout->addWidget(btn);
    };

    if (buttons & QDialogButtonBox::Yes) addButton("예", true, QDialog::Accepted);
    if (buttons & QDialogButtonBox::No)  addButton("아니오", false, QDialog::Rejected);
    if (buttons & QDialogButtonBox::Ok)  addButton("확인", true, QDialog::Accepted);
    if (buttons & QDialogButtonBox::Cancel) addButton("취소", false, QDialog::Rejected);
}

// Static Helpers
bool CustomMessageBox::information(QWidget* parent, const QString& title, const QString& text) {
    CustomMessageBox box(parent);
    box.setMessage(title, text, Information);
    box.setButtons(QDialogButtonBox::Ok);
    return box.exec() == QDialog::Accepted;
}

bool CustomMessageBox::warning(QWidget* parent, const QString& title, const QString& text) {
    CustomMessageBox box(parent);
    box.setMessage(title, text, Warning);
    box.setButtons(QDialogButtonBox::Ok);
    return box.exec() == QDialog::Accepted;
}

bool CustomMessageBox::critical(QWidget* parent, const QString& title, const QString& text) {
    CustomMessageBox box(parent);
    box.setMessage(title, text, Critical);
    box.setButtons(QDialogButtonBox::Ok);
    return box.exec() == QDialog::Accepted;
}

bool CustomMessageBox::question(QWidget* parent, const QString& title, const QString& text) {
    CustomMessageBox box(parent);
    box.setMessage(title, text, Question);
    box.setButtons(QDialogButtonBox::Yes | QDialogButtonBox::No);
    return box.exec() == QDialog::Accepted;
}

void CustomMessageBox::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor bgColor(StyleHelper::getBgColor());
    QColor borderColor = (StyleHelper::currentTheme == StyleHelper::Theme::DeepBlue ? QColor("#ADCCFB") : QColor(0, 0, 0, 30));

    QPainterPath path;
    path.addRoundedRect(rect().adjusted(1,1,-1,-1), 12, 12);
    
    painter.fillPath(path, bgColor);
    
    // 외곽선
    painter.setPen(QPen(borderColor, 1));
    painter.drawPath(path);
}
