#include "MonthWidget.h"
#include "DayCell.h"
#include <QDateTime>
#include <QLayoutItem>

static const char* kDayNames[] = { "일", "월", "화", "수", "목", "금", "토" };

MonthWidget::MonthWidget(QWidget* parent) : QWidget(parent) {
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(0);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < 7; ++i)
        m_gridLayout->setColumnStretch(i, 1);
    // row 0: 요일 헤더 (고정), row 1~6: DayCell
    m_gridLayout->setRowStretch(0, 0);
    for (int i = 1; i <= 6; ++i)
        m_gridLayout->setRowStretch(i, 1);

    // 요일 헤더 생성 (row 0, 재사용)
    for (int col = 0; col < 7; ++col) {
        QLabel* lbl = new QLabel(kDayNames[col], this);
        lbl->setAlignment(Qt::AlignCenter);
        QString color = (col == 0) ? "#D32F2F" : (col == 6) ? "#1565C0" : "#757575";
        lbl->setStyleSheet(QString(
            "font-size: 11px; font-weight: bold; color: %1; "
            "background: #FAFAFA; border-bottom: 1px solid #E0E0E0; "
            "padding: 4px 0px;"
        ).arg(color));
        m_gridLayout->addWidget(lbl, 0, col);
    }
}

void MonthWidget::updateMonth(int year, int month, const QList<QVariantMap>& schedules) {
    // 기존 DayCell만 삭제 (row 0 헤더는 유지)
    for (DayCell* cell : m_cells)
        delete cell;
    m_cells.clear();

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
            QDateTime end   = QDateTime::fromString(s["end"].toString(),   "yyyy-MM-dd HH:mm:ss");
            if (date >= start.date() && date <= end.date())
                daySchedules.append(s);
        }
        cell->setSchedules(daySchedules);

        connect(cell, &DayCell::dayDoubleClicked, this, &MonthWidget::dayDoubleClicked);
        connect(cell, &DayCell::addRequested,     this, &MonthWidget::addRequested);

        int row = (day + startCol - 1) / 7 + 1; // +1: 헤더 row 0 건너뜀
        int col = (day + startCol - 1) % 7;
        m_gridLayout->addWidget(cell, row, col);
        m_cells.append(cell);
    }
}
