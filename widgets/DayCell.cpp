#include "DayCell.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QGraphicsOpacityEffect>
#include <QDateTime>
#include <QColor>

DayCell::DayCell(QWidget* parent) 
    : QFrame(parent)
{
    setFrameStyle(QFrame::NoFrame);
    setMinimumSize(40, 40);
    m_stage = {2, 22, 11}; 

    // 메인 레이아웃
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // [1] Date Header 영역 (22px 고정)
    m_dateLabel = new QLabel(this);
    m_dateLabel->setFixedHeight(22);
    m_dateLabel->setStyleSheet("font-weight: bold; border: none;");
    mainLayout->addWidget(m_dateLabel, 0, Qt::AlignTop | Qt::AlignLeft);

    // [2] 상단 완충 여백 (Vertical Centering - 모든 셀에서 동일한 비율 유지)
    mainLayout->addStretch(1);

    // [3] 정보 블록 컨테이너 (고정 그리드 + 오버플로우 라벨)
    // 이 영역의 높이는 (maxSlots + 1) * slotHeight로 고정됨
    m_scheduleLayout = new QVBoxLayout();
    m_scheduleLayout->setSpacing(0);
    m_scheduleLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(m_scheduleLayout);

    // [4] 오버플로우 라벨 전용 구역 (공간 고정 핵심)
    m_moreLabel = new QLabel(this);
    m_moreLabel->setAlignment(Qt::AlignCenter);
    // 라벨이 숨겨져도 레이아웃 공간(Size)을 유지하도록 설정하여 수직 정렬 어긋남 방지
    QSizePolicy sp = m_moreLabel->sizePolicy();
    sp.setRetainSizeWhenHidden(true); 
    m_moreLabel->setSizePolicy(sp);
    m_moreLabel->hide();
    mainLayout->addWidget(m_moreLabel);

    // [5] 하단 완충 여백 (Vertical Centering)
    mainLayout->addStretch(1);

    // [6] 플러스 버튼 (Floating)
    m_plusButton = new QPushButton("+", this);
    m_plusButton->setFixedSize(24, 24);
    m_plusButton->setCursor(Qt::PointingHandCursor);
    m_plusButton->setStyleSheet(
        "QPushButton { border: none; background-color: #4A90E2; color: white; border-radius: 12px; font-size: 16px; font-weight: bold; }"
        "QPushButton:hover { background-color: #357ABD; }"
    );
    
    m_plusOpacity = new QGraphicsOpacityEffect(m_plusButton);
    m_plusOpacity->setOpacity(0.0);
    m_plusButton->setGraphicsEffect(m_plusOpacity);
    m_plusAnim = new QPropertyAnimation(m_plusOpacity, "opacity", this);

    connect(m_plusButton, &QPushButton::clicked, [this]() { emit addRequested(m_date); });
    setStyleSheet("DayCell { border: 0.5px solid #E0E0E0; background-color: white; }");
}

void DayCell::setStage(const SafeZoneStage& stage) {
    if (m_stage == stage) return; // Stage가 변하지 않았으면 재생성 생략 (깜빡임 방지)
    
    m_stage = stage;
    m_moreLabel->setFixedHeight(stage.slotHeight);
    m_moreLabel->setStyleSheet(QString("font-size: %1px; color: #777; font-weight: bold; padding: 0px 5px;").arg(stage.fontSize - 1));
    
    if (!m_currentSchedules.isEmpty()) {
        setSchedules(m_currentSchedules);
    }
}

void DayCell::setDate(const QDate& date) {
    m_date = date;
    m_dateLabel->setText(QString::number(date.day()));
    if (date == QDate::currentDate()) {
        m_dateLabel->setFixedWidth(26);
        m_dateLabel->setAlignment(Qt::AlignCenter);
        m_dateLabel->setStyleSheet("background-color: #4A90E2; color: white; border-radius: 13px; font-weight: bold; border: none; margin-left: 4px;");
    } else {
        m_dateLabel->setMinimumWidth(0); m_dateLabel->setMaximumWidth(QWIDGETSIZE_MAX);
        m_dateLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        QString color = (date.dayOfWeek() == 6) ? "#1565C0" : (date.dayOfWeek() == 7 ? "#D32F2F" : "#333");
        m_dateLabel->setStyleSheet(QString("font-weight: bold; color: %1; border: none; padding-left: 8px;").arg(color));
    }
}

static QLabel* createScheduleBar(const QVariantMap& data, const QDate& cellDate, const SafeZoneStage& stage, QWidget* parent) {
    QLabel* label = new QLabel(parent);
    label->setFixedHeight(stage.slotHeight);
    QDateTime startDT = QDateTime::fromString(data["start"].toString(), "yyyy-MM-dd HH:mm:ss");
    QDateTime endDT   = QDateTime::fromString(data["end"].toString(),   "yyyy-MM-dd HH:mm:ss");
    QDate startD = startDT.date(); QDate endD = endDT.date();
    bool isStartDay = (startD == cellDate), isEndDay = (endD == cellDate), isMultiDay = (startD != endD);
    
    QString title = data["title"].toString();
    if (isStartDay || cellDate.dayOfWeek() == 7 || cellDate.day() == 1) label->setText(" " + title);

    QColor baseColor(data["color"].toString());
    if (isMultiDay) {
        QString lr = isStartDay ? "4px" : "0px", rr = isEndDay ? "4px" : "0px";
        QString borderLeft = isStartDay ? QString("3px solid %1").arg(baseColor.name()) : "none";
        label->setStyleSheet(QString("background-color: rgba(%1, %2, %3, 50); color: #333; border-left: %4; border-top-left-radius: %5; border-bottom-left-radius: %5; border-top-right-radius: %6; border-bottom-right-radius: %6; font-size: %7px; font-weight: bold; margin: 1px 0px;")
            .arg(baseColor.red()).arg(baseColor.green()).arg(baseColor.blue()).arg(borderLeft).arg(lr).arg(rr).arg(stage.fontSize));
    } else {
        label->setText(" •" + label->text());
        label->setStyleSheet(QString("background-color: transparent; color: %1; font-size: %2px; font-weight: bold; margin: 1px 0px;").arg(baseColor.name()).arg(stage.fontSize));
    }
    label->setToolTip(QString("%1\n%2 ~ %3").arg(title, startDT.toString("MM/dd HH:mm"), endDT.toString("MM/dd HH:mm")));
    return label;
}

void DayCell::setSchedules(const QList<QVariantMap>& schedules) {
    m_currentSchedules = schedules;
    
    QLayoutItem* child;
    while ((child = m_scheduleLayout->takeAt(0)) != nullptr) { if (child->widget()) delete child->widget(); delete child; }

    int displayLimit = m_stage.maxSlots;
    int totalRealCount = 0;
    for (const auto& s : schedules) if (!s.isEmpty() && !s.contains("_is_bar_spacer")) totalRealCount++;

    // Strict Slotting: 무조건 m_stage.maxSlots 개수만큼 위젯을 채움
    for (int i = 0; i < displayLimit; ++i) {
        if (i < schedules.size()) {
            const QVariantMap& s = schedules[i];
            if (s.isEmpty() || s.contains("_is_bar_spacer")) {
                QWidget* sp = new QWidget(this); sp->setFixedHeight(m_stage.slotHeight);
                m_scheduleLayout->addWidget(sp);
            } else {
                m_scheduleLayout->addWidget(createScheduleBar(s, m_date, m_stage, this));
            }
        } else {
            QWidget* sp = new QWidget(this); sp->setFixedHeight(m_stage.slotHeight);
            m_scheduleLayout->addWidget(sp);
        }
    }

    int visibleRealInGrid = 0;
    for (int i = 0; i < displayLimit; ++i) {
        if (i < schedules.size() && !schedules[i].isEmpty() && !schedules[i].contains("_is_bar_spacer"))
            visibleRealInGrid++;
    }

    int hiddenRealCount = totalRealCount - visibleRealInGrid;

    // 오버플로우 라벨 업데이트: hide 시에도 공간을 유지하므로 수직 정렬이 깨지지 않음
    if (hiddenRealCount > 0) {
        m_moreLabel->setText(QString("+ %1 more").arg(hiddenRealCount));
        m_moreLabel->show();
    } else {
        m_moreLabel->hide();
    }
}

void DayCell::updatePlusButtonPos() {
    m_plusButton->move(width() - 28, height() - 28);
    m_plusButton->raise();
}

void DayCell::resizeEvent(QResizeEvent* e) {
    QFrame::resizeEvent(e);
    updatePlusButtonPos();
}

void DayCell::enterEvent(QEnterEvent* e) {
    m_plusButton->raise();
    m_plusAnim->stop();
    m_plusAnim->setStartValue(m_plusOpacity->opacity());
    m_plusAnim->setEndValue(1.0);
    m_plusAnim->setDuration(150);
    m_plusAnim->start();
    QFrame::enterEvent(e);
}

void DayCell::leaveEvent(QEvent* e) {
    m_plusAnim->stop();
    m_plusAnim->setStartValue(m_plusOpacity->opacity());
    m_plusAnim->setEndValue(0.0);
    m_plusAnim->setDuration(150);
    m_plusAnim->start();
    QFrame::leaveEvent(e);
}

void DayCell::mouseDoubleClickEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) emit dayDoubleClicked(m_date);
}
