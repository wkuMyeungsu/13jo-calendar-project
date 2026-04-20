#ifndef SCHEDULEINPUTWIDGET_H
#define SCHEDULEINPUTWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class ScheduleInputWidget : public QWidget {
    Q_OBJECT
public:
    explicit ScheduleInputWidget(const QDate& initialDate = QDate::currentDate(), QWidget *parent = nullptr);

signals:
    void scheduleSaved();

private slots:
    void handleSave();

private:
    QLineEdit *titleInput;
    QComboBox *categoryCombo;
    QDateTimeEdit *startTimeEdit;
    QDateTimeEdit *endTimeEdit;
    QPushButton *saveBtn;
};


#endif // SCHEDULEINPUTWIDGET_H
