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
#include <QColorDialog>

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
    void selectColor();

private:
    int m_scheduleId;
    QLineEdit *titleInput;
    QComboBox *categoryCombo;
    QCheckBox *allDayCheck;
    QDateTimeEdit *startTimeEdit;
    QDateTimeEdit *endTimeEdit;
    QTextEdit *contentInput;
    QPushButton *colorBtn;
    QPushButton *updateBtn;
    QPushButton *deleteBtn;
    QString m_selectedColor;
};

#endif // SCHEDULEMODIFYWIDGET_H
