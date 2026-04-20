#ifndef SCHEDULEINPUTWIDGET_H
#define SCHEDULEINPUTWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QTextEdit>
#include <QColorDialog>

class ScheduleInputWidget : public QWidget {
    Q_OBJECT
public:
    explicit ScheduleInputWidget(const QDate& initialDate = QDate::currentDate(), QWidget *parent = nullptr);

signals:
    void scheduleSaved();

private slots:
    void handleSave();
    void toggleAllDay(bool checked);
    void selectColor();

private:
    QLineEdit *titleInput;
    QComboBox *categoryCombo;
    QCheckBox *allDayCheck;
    QDateTimeEdit *startTimeEdit;
    QDateTimeEdit *endTimeEdit;
    QTextEdit *contentInput;
    QPushButton *colorBtn;
    QPushButton *saveBtn;
    QString m_selectedColor;
};


#endif // SCHEDULEINPUTWIDGET_H
