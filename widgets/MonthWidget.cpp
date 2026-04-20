#include "MonthWidget.h"
#include "DayCell.h"
#include <QDateTime>
#include <QLayoutItem>

MonthWidget::MonthWidget(QWidget* parent) : QWidget(parent) {
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(0);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    
    // 행과 열의 확장 비율을 동일하게 설정 (격자 끊김 방지)
    for(int i=0; i<7; ++i) m_gridLayout->setColumnStretch(i, 1);
    for(int i=0; i<6; ++i) m_gridLayout->setRowStretch(i, 1);
}

void MonthWidget::updateMonth(int year, int month, const QList<QVariantMap>& schedules) {
    // 기존 아이템 제거
    QLayoutItem *item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    QDate firstDay(year, month, 1);
    int startCol = firstDay.dayOfWeek() % 7; 
    int daysInMonth = firstDay.daysInMonth();

    for (int day = 1; day <= daysInMonth; ++day) {
        DayCell* cell = new DayCell(this);
        QDate date(year, month, day);
        cell->setDate(date);
        
        QList<QVariantMap> daySchedules;
        for (const auto& s : schedules) {
            QDateTime start = QDateTime::fromString(s["start"].toString(), "yyyy-MM-dd HH:mm:ss");
            QDateTime end = QDateTime::fromString(s["end"].toString(), "yyyy-MM-dd HH:mm:ss");
            if (date >= start.date() && date <= end.date()) {
                daySchedules.append(s);
            }
        }
        cell->setSchedules(daySchedules);
        
        connect(cell, &DayCell::dayDoubleClicked, this, &MonthWidget::dayDoubleClicked);
        connect(cell, &DayCell::addRequested, this, &MonthWidget::addRequested);

        int row = (day + startCol - 1) / 7;
        int col = (day + startCol - 1) % 7;
        m_gridLayout->addWidget(cell, row, col);
    }
}
