#ifndef DAYCELL_H
#define DAYCELL_H

#include <QFrame>
#include <QLabel>
#include <QDate>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QVBoxLayout>
#include <QVariantMap>
#include <QPushButton>
#include "UiCommon.h"

class DayCell : public QFrame {
    Q_OBJECT
public:
    explicit DayCell(QWidget* parent = nullptr);
    void setDate(const QDate& date);
    void setSchedules(const QList<QVariantMap>& schedules);
    void setStage(const SafeZoneStage& stage);
    void setSelected(bool select); // 선택 상태 설정
    const QDate& date() const { return m_date; }

    signals:
    void dayClicked(const QDate& date); // 클릭 시그널 추가
    void dayDoubleClicked(const QDate& date);
    void addRequested(const QDate& date);

    protected:
    void enterEvent(QEnterEvent* e) override;
    void leaveEvent(QEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override; // 마우스 클릭 추가
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

    private:
    void updateStyle(); // 스타일 갱신 전용 메서드
    void updateHoverHintPos(); // 호버 힌트 위치 갱신

    QDate m_date;
    bool  m_isToday = false;
    bool  m_isHovered = false;
    bool  m_isSelected = false; // 선택 여부
    QLabel* m_dateLabel;
    QLabel* m_moreLabel; 
    QLabel* m_hoverHintLabel; // 호버 시 나타나는 + 힌트
    QVBoxLayout* m_scheduleLayout;
    QGraphicsOpacityEffect* m_hintOpacity;
    QPropertyAnimation* m_hintAnim;
    QList<QVariantMap> m_currentSchedules; 
    
    SafeZoneStage m_stage; 
    };
#endif // DAYCELL_H
