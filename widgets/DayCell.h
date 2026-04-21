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

    signals:
    void dayDoubleClicked(const QDate& date);
    void addRequested(const QDate& date);

    protected:
    void enterEvent(QEnterEvent* e) override;
    void leaveEvent(QEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

    private:
    void updatePlusButtonPos();

    QDate m_date;
    QLabel* m_dateLabel;
    QLabel* m_moreLabel; // 추가됨
    QPushButton* m_plusButton;
    QVBoxLayout* m_scheduleLayout;
    QGraphicsOpacityEffect* m_plusOpacity;
    QPropertyAnimation* m_plusAnim;
    QList<QVariantMap> m_currentSchedules; 
    
    SafeZoneStage m_stage; // 6단계 세이프 존 상태
    };
#endif // DAYCELL_H
