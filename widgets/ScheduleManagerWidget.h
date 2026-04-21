#ifndef SCHEDULEMANAGERWIDGET_H
#define SCHEDULEMANAGERWIDGET_H

#include <QWidget>
#include <QDate>
#include <QVBoxLayout>
#include <QList>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include "CustomTitleBar.h"
#include "../models/Schedule.h"

class ScheduleManagerWidget : public QWidget {
    Q_OBJECT
public:
    explicit ScheduleManagerWidget(const QDate& date, QWidget *parent = nullptr);

signals:
    void dataChanged();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    void refreshList();
    void openAddWidget();
    void openEditWidget(const Schedule& data);

private:
    QDate m_date;
    QVBoxLayout *m_listLayout;
    QLabel *m_titleLabel;
    QMap<QObject*, Schedule> m_itemDataMap;

    CustomTitleBar *m_titleBar;
    QWidget *m_contentWidget;
};

#endif // SCHEDULEMANAGERWIDGET_H
