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

class DayCell : public QFrame {
    Q_OBJECT
public:
    explicit DayCell(QWidget* parent = nullptr);
    void setDate(const QDate& date);
    void setSchedules(const QList<QVariantMap>& schedules);

    signals:
    void dayDoubleClicked(const QDate& date);
    void addRequested(const QDate& date);

    protected:
    void enterEvent(QEnterEvent* e) override;
    void leaveEvent(QEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

    private:
    QDate m_date;
    QLabel* m_dateLabel;
    QPushButton* m_plusButton;
    QVBoxLayout* m_scheduleLayout;
    QGraphicsOpacityEffect* m_plusOpacity;
    QPropertyAnimation* m_plusAnim;
    QList<QVariantMap> m_currentSchedules; 
    bool m_wasTimeShown; 
    int m_lastMaxSchedules; // 이전에 표시했던 최대 일정 개수 저장
    };
#endif // DAYCELL_H
