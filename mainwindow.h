#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QGridLayout;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void updateCalendar(int year, int month);
    void handleDayDoubleClicked(const QDate& date);

private:
    Ui::MainWindow *ui;
    QGridLayout* m_calendarGrid;
    int m_currentYear;
    int m_currentMonth;
};
#endif // MAINWINDOW_H
