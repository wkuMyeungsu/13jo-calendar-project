#ifndef DAYCELL_H
#define DAYCELL_H

#include <QFrame>
#include <QLabel>
#include <QDate>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class DayCell : public QFrame {
    Q_OBJECT
public:
    explicit DayCell(QWidget* parent = nullptr);
    void setDate(const QDate& date);

protected:
    void enterEvent(QEnterEvent* e) override;
    void leaveEvent(QEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;

private:
    QDate m_date;
    QLabel* m_dateLabel;
    QLabel* m_plusHint;
    QGraphicsOpacityEffect* m_plusOpacity;
    QPropertyAnimation* m_plusAnim;
};
#endif // DAYCELL_H
