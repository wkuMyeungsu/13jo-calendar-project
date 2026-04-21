#include "CustomTitleBar.h"
#include "UiConstants.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include "StyleHelper.h"

CustomTitleBar::CustomTitleBar(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(UiConstants::TITLE_BAR_HEIGHT);
    setObjectName("customTitleBar");
    setAttribute(Qt::WA_StyledBackground, true);
    setMouseTracking(true);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(14, 0, 0, 0);
    layout->setSpacing(0);

    m_titleLabel = new QLabel("  캘린더", this);
    m_titleLabel->setObjectName("titleLabel");

    layout->addWidget(m_titleLabel);
    layout->addStretch();

    auto makeBtn = [this](const QString& text, const QString& name) -> QPushButton* {
        auto* btn = new QPushButton(text, this);
        btn->setObjectName(name);
        btn->setFixedSize(46, UiConstants::TITLE_BAR_HEIGHT);
        btn->setCursor(Qt::ArrowCursor);
        btn->setFlat(true);
        return btn;
    };

    m_minBtn   = makeBtn("—", "minBtn");
    m_maxBtn   = makeBtn("□", "maxBtn");
    m_closeBtn = makeBtn("✕", "closeBtn");

    m_minBtn->installEventFilter(this);
    m_maxBtn->installEventFilter(this);
    m_closeBtn->installEventFilter(this);

    layout->addWidget(m_minBtn);
    layout->addWidget(m_maxBtn);
    layout->addWidget(m_closeBtn);

    connect(m_minBtn, &QPushButton::clicked, this, [this] {
        window()->showMinimized();
    });
    connect(m_maxBtn, &QPushButton::clicked, this, [this] {
        auto* w = window();
        if (w->isMaximized()) w->showNormal(); else w->showMaximized();
        updateMaxIcon();
    });
    connect(m_closeBtn, &QPushButton::clicked, this, [this] {
        window()->close();
    });
}

void CustomTitleBar::updateMaxIcon() {
    m_maxBtn->setText(window()->isMaximized() ? "❐" : "□");
}

bool CustomTitleBar::eventFilter(QObject* watched, QEvent* event) {
    auto* btn = qobject_cast<QPushButton*>(watched);
    if (!btn || window()->isMaximized()) return QWidget::eventFilter(watched, event);

    if (m_isResizable && event->type() == QEvent::MouseMove) {
        auto* me = static_cast<QMouseEvent*>(event);
        QPoint titleBarPos = btn->mapToParent(me->position().toPoint());
        if (titleBarPos.x() > width() - RESIZE_MARGIN && titleBarPos.y() < RESIZE_MARGIN) {
            btn->setCursor(Qt::SizeBDiagCursor);
        } else {
            btn->setCursor(Qt::ArrowCursor);
        }
    } else if (m_isResizable && event->type() == QEvent::MouseButtonPress) {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) {
            QPoint titleBarPos = btn->mapToParent(me->position().toPoint());
            if (titleBarPos.x() > width() - RESIZE_MARGIN && titleBarPos.y() < RESIZE_MARGIN) {
                m_resizing = true;
                m_resizeStartPos = me->globalPosition().toPoint();
                m_resizeStartGeometry = window()->geometry();
                return true; // 버튼 클릭 이벤트를 무시하고 리사이즈 시작
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void CustomTitleBar::applyTheme(const QString& bgColor, const QString& textColor, const QString& borderColor) {
    setStyleSheet(StyleHelper::getTitleBarStyle(bgColor, borderColor) + QString(
        "#titleLabel {"
        "  color: %1;"
        "  font-size: 11px;"
        "  font-weight: bold;"
        "  background: transparent;"
        "}"
        "#minBtn, #maxBtn {"
        "  color: %1;"
        "  background: transparent;"
        "  border: none;"
        "  font-size: 13px;"
        "}"
        "#minBtn:hover, #maxBtn:hover {"
        "  background-color: rgba(128, 128, 128, 0.18);"
        "}"
        "#closeBtn {"
        "  color: %1;"
        "  background: transparent;"
        "  border: none;"
        "  font-size: 12px;"
        "}"
        "#closeBtn:hover {"
        "  background-color: #E81123;"
        "  color: white;"
        "}"
    ).arg(textColor));
}

void CustomTitleBar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (m_isResizable && event->position().x() > width() - RESIZE_MARGIN && event->position().y() < RESIZE_MARGIN && !window()->isMaximized()) {
            m_resizing = true;
            m_resizeStartPos = event->globalPosition().toPoint();
            m_resizeStartGeometry = window()->geometry();
        } else {
            m_dragging = true;
            m_dragStartPos = event->globalPosition().toPoint() - window()->frameGeometry().topLeft();
        }
    }
    QWidget::mousePressEvent(event);
}

void CustomTitleBar::mouseMoveEvent(QMouseEvent* event) {
    if (m_resizing) {
        QPoint diff = event->globalPosition().toPoint() - m_resizeStartPos;
        QRect newGeom = m_resizeStartGeometry;

        // 우상단 리사이즈: 오른쪽 경계(Right)와 상단 경계(Top)를 독립적으로 계산
        int targetRight = m_resizeStartGeometry.right() + diff.x();
        int targetTop = m_resizeStartGeometry.top() + diff.y();

        // 너비 최소 크기 제한
        if (targetRight - m_resizeStartGeometry.left() < window()->minimumWidth()) {
            targetRight = m_resizeStartGeometry.left() + window()->minimumWidth();
        }
        
        // 높이 최소 크기 제한 (상단을 움직이므로 하단 고정 상태에서 계산)
        if (m_resizeStartGeometry.bottom() - targetTop < window()->minimumHeight()) {
            targetTop = m_resizeStartGeometry.bottom() - window()->minimumHeight();
        }

        newGeom.setRight(targetRight);
        newGeom.setTop(targetTop);
        
        window()->setGeometry(newGeom);
        return;
    }

    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint globalPos = event->globalPosition().toPoint();

        if (window()->isMaximized()) {
            double ratio = (double)event->position().x() / width();
            window()->showNormal();
            updateMaxIcon();
            int anchorX = static_cast<int>(ratio * window()->width());
            m_dragStartPos = QPoint(anchorX, event->position().y());
        }

        window()->move(globalPos - m_dragStartPos);
        return;
    }

    // 마우스 커서 변경 로직 (버튼이 눌리지 않았을 때)
    if (m_isResizable && !window()->isMaximized() && event->position().x() > width() - RESIZE_MARGIN && event->position().y() < RESIZE_MARGIN) {
        setCursor(Qt::SizeBDiagCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }

    QWidget::mouseMoveEvent(event);
}

void CustomTitleBar::mouseReleaseEvent(QMouseEvent* event) {
    m_dragging = false;
    m_resizing = false;
    QWidget::mouseReleaseEvent(event);
}

void CustomTitleBar::leaveEvent(QEvent* event) {
    setCursor(Qt::ArrowCursor);
    QWidget::leaveEvent(event);
}

void CustomTitleBar::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        auto* w = window();
        if (w->isMaximized()) w->showNormal(); else w->showMaximized();
        updateMaxIcon();
    }
    QWidget::mouseDoubleClickEvent(event);
}
