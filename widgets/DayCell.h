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

private:
    QDate m_date;
    QLabel* m_dateLabel;
    QPushButton* m_plusButton; // QLabel에서 QPushButton으로 변경
    QVBoxLayout* m_scheduleLayout;
    QGraphicsOpacityEffect* m_plusOpacity;
    QPropertyAnimation* m_plusAnim;
};
#endif // DAYCELL_H
