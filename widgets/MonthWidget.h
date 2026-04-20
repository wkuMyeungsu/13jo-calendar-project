#ifndef MONTHWIDGET_H
#define MONTHWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QDate>
#include <QVariantMap>
#include <QList>

class DayCell;

class MonthWidget : public QWidget {
    Q_OBJECT
public:
    explicit MonthWidget(QWidget* parent = nullptr);
    void updateMonth(int year, int month, const QList<QVariantMap>& schedules);

signals:
    void dayDoubleClicked(const QDate& date);
    void addRequested(const QDate& date);

private:
    QGridLayout*   m_gridLayout;
    QList<DayCell*> m_cells;
};

#endif // MONTHWIDGET_H
