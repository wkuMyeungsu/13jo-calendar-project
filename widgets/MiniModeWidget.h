#ifndef MINIMODEWIDGET_H
#define MINIMODEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QMap>
#include "../models/Schedule.h"

class MiniModeWidget : public QWidget {
    Q_OBJECT
public:
    explicit MiniModeWidget(QWidget *parent = nullptr);
    void refresh();
    void updateStyle();

signals:
    void backToNormal();
    void settingsRequested();
    void scheduleChanged();
    void pinRequested(bool pinned);

public:
    void setPinned(bool pinned);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void updateDateTime();

private:
    QLabel*      m_dateLabel;
    QLabel*      m_timeLabel;
    QVBoxLayout* m_scheduleLayout;
    QTimer*      m_realTimeTimer;
    QMap<QObject*, Schedule> m_itemDataMap;

    QWidget*     m_contentWidget;
};

#endif // MINIMODEWIDGET_H
