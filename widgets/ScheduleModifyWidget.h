#ifndef SCHEDULEMODIFYWIDGET_H
#define SCHEDULEMODIFYWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QTextEdit>
#include <QVariantMap>

class ScheduleModifyWidget : public QWidget {
    Q_OBJECT
public:
    explicit ScheduleModifyWidget(const QVariantMap& scheduleData, QWidget *parent = nullptr);

signals:
    void scheduleUpdated();
    void scheduleDeleted();

private slots:
    void handleUpdate();
    void handleDelete();
    void toggleAllDay(bool checked);

private:
    int m_scheduleId;
    QLineEdit *titleInput;
    QComboBox *categoryCombo;
    QCheckBox *allDayCheck;
    QDateTimeEdit *startTimeEdit;
    QDateTimeEdit *endTimeEdit;
    QTextEdit *contentInput;
    QPushButton *updateBtn;
    QPushButton *deleteBtn;
};

#endif // SCHEDULEMODIFYWIDGET_H
