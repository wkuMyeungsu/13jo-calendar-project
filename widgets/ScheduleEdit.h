#ifndef SCHEDULEEDITDIALOG_H
#define SCHEDULEEDITDIALOG_H

#include <QWidget>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QTextEdit>
#include <QColorDialog>
#include "CustomTitleBar.h"
#include "../models/Schedule.h"

class ScheduleEditDialog : public QWidget {
    Q_OBJECT
public:
    // 모드 정의: 추가 또는 수정
    enum class Mode { Add, Edit };

    // 신규 추가용 생성자
    explicit ScheduleEditDialog(const QDate& initialDate = QDate::currentDate(), QWidget *parent = nullptr);
    // 기존 수정용 생성자
    explicit ScheduleEditDialog(const Schedule& scheduleData, QWidget *parent = nullptr);

signals:
    void scheduleSaved();   // 추가/수정 완료 시
    void scheduleDeleted(); // 삭제 완료 시

protected:
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void handleSave();
    void handleDelete();
    void toggleAllDay(bool checked);
    void selectColor();

private:
    void initUi();
    void setupMode(Mode mode, const Schedule& data = Schedule());

    Mode m_mode;
    int m_scheduleId = -1;
    QString m_selectedColor;

    // UI 요소
    QLineEdit *titleInput;
    QComboBox *categoryCombo;
    QCheckBox *allDayCheck;
    QDateTimeEdit *startTimeEdit;
    QDateTimeEdit *endTimeEdit;
    QTextEdit *contentInput;
    QPushButton *colorBtn;
    QPushButton *saveBtn;
    QPushButton *deleteBtn;

    CustomTitleBar* m_titleBar;
    QWidget*        m_contentWidget;
};

#endif // SCHEDULEEDITDIALOG_H
