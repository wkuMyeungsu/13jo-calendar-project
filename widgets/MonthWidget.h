#ifndef MONTHWIDGET_H
#define MONTHWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QDate>
#include <QVariantMap>

class MonthWidget : public QWidget {
    Q_OBJECT
public:
    explicit MonthWidget(QWidget* parent = nullptr);
    void updateMonth(int year, int month, const QList<QVariantMap>& schedules);

signals:
    void dayDoubleClicked(const QDate& date);

private:
    QGridLayout* m_gridLayout;
};

#endif // MONTHWIDGET_H
