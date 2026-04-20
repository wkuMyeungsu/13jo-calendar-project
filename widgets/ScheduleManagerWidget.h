#ifndef SCHEDULEMANAGERWIDGET_H
#define SCHEDULEMANAGERWIDGET_H

#include <QWidget>
#include <QDate>
#include <QVBoxLayout>
#include <QList>
#include <QVariantMap>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>

class ScheduleManagerWidget : public QWidget {
    Q_OBJECT
public:
    explicit ScheduleManagerWidget(const QDate& date, QWidget *parent = nullptr);

signals:
    void dataChanged();

private slots:
    void refreshList();
    void openAddWidget();
    void openEditWidget(const QVariantMap& data);
    void openCategoryManager();

private:
    QDate m_date;
    QVBoxLayout *m_listLayout;
    QLabel *m_titleLabel;
};

#endif // SCHEDULEMANAGERWIDGET_H
