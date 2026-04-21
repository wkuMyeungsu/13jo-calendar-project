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
    layout->setContentsMargins(10, 0, 0, 0);
    layout->setSpacing(0);

    // [1] 아이콘 설정 (Calendar.ico)
    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("titleLabel");
    
    // 여러 경로에서 아이콘 시도 (실행 환경 고려)
    QPixmap icon;
    QStringList paths = { "Calendar.ico", "../Calendar.ico", "../../Calendar.ico", QCoreApplication::applicationDirPath() + "/Calendar.ico" };
    for (const QString& path : paths) {
        if (icon.load(path)) break;
    }

    if (!icon.isNull()) {
        m_titleLabel->setPixmap(icon.scaled(18, 18, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // 아이콘 로드 실패 시 대체 이모지
        m_titleLabel->setText("  📅");
        m_titleLabel->setStyleSheet("font-size: 14px;");
    }
    layout->addWidget(m_titleLabel);

    layout->addStretch();

    // [2] 중앙 타이틀 설정 (레이아웃에 넣지 않고 별도 관리하여 절대 중앙 정렬)
    m_centerTitleLabel = new QLabel(this);
    m_centerTitleLabel->setObjectName("centerTitleLabel");
    m_centerTitleLabel->setAlignment(Qt::AlignCenter);
    m_centerTitleLabel->setAttribute(Qt::WA_TransparentForMouseEvents); // 클릭 이벤트 관통
    
    // 버튼들
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

void CustomTitleBar::setTitle(const QString& title) {
    m_centerTitleLabel->setText(title);
}

void CustomTitleBar::showMinMaxButtons(bool show) {
    m_minBtn->setVisible(show);
    m_maxBtn->setVisible(show);
}

void CustomTitleBar::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (m_centerTitleLabel) {
        m_centerTitleLabel->setFixedSize(width() / 2, height());
        m_centerTitleLabel->move(width() / 4, 0); // 가로 기준 25% 지점부터 50% 너비 차지 -> 절대 중앙
    }
}

void CustomTitleBar::applyTheme(const QString& bgColor, const QString& textColor, const QString& borderColor) {
    setStyleSheet(StyleHelper::getTitleBarStyle(bgColor, borderColor) + QString(
        "#titleLabel {"
        "  color: %1;"
        "  font-size: 11px;"
        "  font-weight: bold;"
        "  background: transparent;"
        "}"
        "#centerTitleLabel {"
        "  color: %1;"
        "  font-size: 13px;"
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
