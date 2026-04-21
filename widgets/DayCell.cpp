#include "DayCell.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDateTime>
#include <QColor>
#include <QGraphicsOpacityEffect>

DayCell::DayCell(QWidget* parent) 
    : QFrame(parent)
{
    setFrameStyle(QFrame::NoFrame);
    setMinimumSize(UiConstants::CELL_MIN_SIZE, UiConstants::CELL_MIN_SIZE);
    m_stage = {2, 22, 11}; 

    // 메인 레이아웃
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // [1] Date Header 영역
    m_dateLabel = new QLabel(this);
    m_dateLabel->setFixedHeight(UiConstants::DATE_HEADER_HEIGHT);
    m_dateLabel->setStyleSheet("font-weight: bold; border: none; background: transparent;");
    mainLayout->addWidget(m_dateLabel, 0, Qt::AlignTop | Qt::AlignLeft);

    // [2] 상단 완충 여백
    mainLayout->addStretch(1);

    // [3] 정보 블록 컨테이너
    m_scheduleLayout = new QVBoxLayout();
    m_scheduleLayout->setSpacing(0);
    m_scheduleLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(m_scheduleLayout);

    // [4] 오버플로우 라벨
    m_moreLabel = new QLabel(this);
    m_moreLabel->setAlignment(Qt::AlignCenter);
    QSizePolicy sp = m_moreLabel->sizePolicy();
    sp.setRetainSizeWhenHidden(true); 
    m_moreLabel->setSizePolicy(sp);
    m_moreLabel->hide();
    mainLayout->addWidget(m_moreLabel);

    // [5] 하단 완충 여백
    mainLayout->addStretch(1);

    // [6] 호버 힌트 라벨 (+) - 생성 시에는 기본 속성만 설정
    m_hoverHintLabel = new QLabel("+", this);
    m_hoverHintLabel->setAlignment(Qt::AlignCenter);
    m_hoverHintLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_hoverHintLabel->setFixedSize(40, 40);
    
    m_hintOpacity = new QGraphicsOpacityEffect(m_hoverHintLabel);
    m_hintOpacity->setOpacity(0.0);
    m_hoverHintLabel->setGraphicsEffect(m_hintOpacity);
    
    m_hintAnim = new QPropertyAnimation(m_hintOpacity, "opacity", this);
    m_hintAnim->setDuration(150);

    updateStyle();
}

void DayCell::updateStyle() {
    QString primary = StyleHelper::getPrimaryColor();
    QString border = QString("0.5px solid %1").arg(UiConstants::COLOR_BORDER_DEFAULT);
    QString bgColor = "white";

    // 1. 선택 및 오늘 하이라이트 경계선 결정
    if (m_isSelected) {
        border = QString("2px solid %1").arg(primary);
    } else if (m_isToday) {
        border = "2px solid #FF9800";
    }

    // 2. 호버 배경색 결정
    if (m_isHovered) {
        bgColor = StyleHelper::getDayCellHoverBg();
    }

    // 3. 메인 컨테이너 스타일 적용
    setStyleSheet(QString("DayCell { border: %1; background-color: %2; }").arg(border, bgColor));

    // 4. [+] 기호 색상 테마 동기화 (핵심 수정 사항)
    m_hoverHintLabel->setStyleSheet(
        QString("font-size: 32px; font-weight: bold; color: %1; background: transparent;").arg(primary)
    );
}

void DayCell::setSelected(bool select) {
    if (m_isSelected != select) {
        m_isSelected = select;
        updateStyle();
    }
}

void DayCell::setStage(const SafeZoneStage& stage) {
    if (m_stage == stage) return;
    m_stage = stage;
    m_moreLabel->setFixedHeight(stage.slotHeight);
    m_moreLabel->setStyleSheet(QString("font-size: %1px; color: %2; font-weight: bold; padding: 0px 5px;").arg(stage.fontSize - 1).arg(UiConstants::COLOR_TEXT_DIM));
    if (!m_currentSchedules.isEmpty()) setSchedules(m_currentSchedules);
}

void DayCell::setDate(const QDate& date) {
    m_date = date;
    m_isToday = (date == QDate::currentDate());
    m_dateLabel->setText(QString::number(date.day()));
    
    if (m_isToday) {
        m_dateLabel->setMinimumWidth(0); m_dateLabel->setMaximumWidth(QWIDGETSIZE_MAX);
        m_dateLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_dateLabel->setStyleSheet("font-weight: bold; color: #FF9800; border: none; background: transparent; padding-left: 8px;");
    } else {
        m_dateLabel->setMinimumWidth(0); m_dateLabel->setMaximumWidth(QWIDGETSIZE_MAX);
        m_dateLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        QString color = (date.dayOfWeek() == 6) ? UiConstants::COLOR_SAT : (date.dayOfWeek() == 7 ? UiConstants::COLOR_SUN : "#333");
        m_dateLabel->setStyleSheet(QString("font-weight: bold; color: %1; border: none; background: transparent; padding-left: 8px;").arg(color));
    }
    updateStyle();
}

static QLabel* createScheduleBar(const QVariantMap& data, const QDate& cellDate, const SafeZoneStage& stage, QWidget* parent) {
    QLabel* label = new QLabel(parent);
    label->setFixedHeight(stage.slotHeight);
    QDateTime startDT = QDateTime::fromString(data["start"].toString(), "yyyy-MM-dd HH:mm:ss");
    QDateTime endDT   = QDateTime::fromString(data["end"].toString(),   "yyyy-MM-dd HH:mm:ss");
    QDate startD = startDT.date(); QDate endD = endDT.date();
    bool isStartDay = (startD == cellDate), isEndDay = (endD == cellDate), isMultiDay = (startD != endD);
    bool isAllDay = (data["all_day"].toInt() == 1);
    
    QString title = data["title"].toString();
    if (isStartDay || cellDate.dayOfWeek() == 7 || cellDate.day() == 1) label->setText(" " + title);

    QColor baseColor(data["color"].toString());
    if (isMultiDay) {
        QString lr = isStartDay ? "4px" : "0px", rr = isEndDay ? "4px" : "0px";
        QString borderLeft = isStartDay ? QString("3px solid %1").arg(baseColor.name()) : "none";
        label->setStyleSheet(QString("background-color: rgba(%1, %2, %3, 50); color: #333; border-left: %4; border-top-left-radius: %5; border-bottom-left-radius: %5; border-top-right-radius: %6; border-bottom-right-radius: %6; font-size: %7px; font-weight: bold; margin: 1px 0px;")
            .arg(baseColor.red()).arg(baseColor.green()).arg(baseColor.blue()).arg(borderLeft).arg(lr).arg(rr).arg(stage.fontSize));
    } else if (isAllDay) {
        label->setStyleSheet(StyleHelper::getAllDayBarStyle(baseColor.name()));
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
    while ((child = m_scheduleLayout->takeAt(0)) != nullptr) { 
        if (child->widget()) child->widget()->deleteLater(); 
        delete child; 
    }

    int displayLimit = m_stage.maxSlots;
    for (int i = 0; i < displayLimit; ++i) {
        if (i < schedules.size()) {
            const QVariantMap& s = schedules[i];
            if (s.isEmpty() || s.contains("_is_bar_spacer")) {
                m_scheduleLayout->addSpacing(m_stage.slotHeight);
            } else {
                m_scheduleLayout->addWidget(createScheduleBar(s, m_date, m_stage, this));
            }
        } else {
            m_scheduleLayout->addSpacing(m_stage.slotHeight);
        }
    }

    int totalRealCount = 0;
    for (const auto& s : schedules) if (!s.isEmpty() && !s.contains("_is_bar_spacer")) totalRealCount++;
    int visibleRealInGrid = 0;
    for (int i = 0; i < displayLimit; ++i) {
        if (i < schedules.size() && !schedules[i].isEmpty() && !schedules[i].contains("_is_bar_spacer"))
            visibleRealInGrid++;
    }
    int hiddenRealCount = totalRealCount - visibleRealInGrid;
    if (hiddenRealCount > 0) {
        m_moreLabel->setText(QString("+ %1 more").arg(hiddenRealCount));
        m_moreLabel->show();
    } else {
        m_moreLabel->hide();
    }
}

void DayCell::updateHoverHintPos() {
    m_hoverHintLabel->move((width() - m_hoverHintLabel->width()) / 2, (height() - m_hoverHintLabel->height()) / 2);
}

void DayCell::resizeEvent(QResizeEvent* e) {
    QFrame::resizeEvent(e);
    updateHoverHintPos();
}

void DayCell::enterEvent(QEnterEvent* e) {
    m_isHovered = true;
    updateStyle();

    bool hasSchedules = false;
    for (const auto& s : m_currentSchedules) {
        if (!s.isEmpty() && !s.contains("_is_bar_spacer")) {
            hasSchedules = true; break;
        }
    }

    if (!hasSchedules) {
        m_hoverHintLabel->raise();
        m_hintAnim->stop();
        m_hintAnim->setStartValue(m_hintOpacity->opacity());
        m_hintAnim->setEndValue(1.0);
        m_hintAnim->start();
    }

    QFrame::enterEvent(e);
}

void DayCell::leaveEvent(QEvent* e) {
    m_isHovered = false;
    updateStyle();

    m_hintAnim->stop();
    m_hintAnim->setStartValue(m_hintOpacity->opacity());
    m_hintAnim->setEndValue(0.0);
    m_hintAnim->start();

    QFrame::leaveEvent(e);
}

void DayCell::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) {
        emit dayClicked(m_date);
    }
    QFrame::mousePressEvent(e);
}

void DayCell::mouseDoubleClickEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) {
        bool hasSchedules = false;
        for (const auto& s : m_currentSchedules) {
            if (!s.isEmpty() && !s.contains("_is_bar_spacer")) {
                hasSchedules = true; break;
            }
        }
        
        if (!hasSchedules) {
            emit addRequested(m_date);
        } else {
            emit dayDoubleClicked(m_date);
        }
    }
}
