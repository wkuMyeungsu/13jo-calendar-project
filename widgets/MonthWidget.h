#ifndef MONTHWIDGET_H
#define MONTHWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QDate>
#include <QList>
#include "UiCommon.h"
#include "../models/CalendarEngine.h"

class DayCell;

class MonthWidget : public QWidget {
    Q_OBJECT
public:
    explicit MonthWidget(QWidget* parent = nullptr);
    void updateMonth(int year, int month, const QList<Schedule>& schedules);
    void setStage(const SafeZoneStage& stage);

signals:
    void dayDoubleClicked(const QDate& date);
    void addRequested(const QDate& date);

private:
    QGridLayout*   m_gridLayout;
    QList<DayCell*> m_cells;
    SafeZoneStage  m_stage;
};

#endif // MONTHWIDGET_H
