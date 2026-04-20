#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWheelEvent>
#include <QPropertyAnimation>
#include <QTimer>
#include <QLabel>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MonthWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateCalendar();
    void handleDayDoubleClicked(const QDate& date);
    void handleDayAddRequested(const QDate& date);
    void finishScroll();
    void prevMonth();
    void nextMonth();
    void goToday();

private:
    static constexpr int kHeaderH = 52;

    Ui::MainWindow *ui;

    // 네비게이션 헤더
    QWidget*     m_headerBar;
    QLabel*      m_monthLabel;
    QPushButton* m_prevBtn;
    QPushButton* m_nextBtn;
    QPushButton* m_todayBtn;

    // 캘린더 컨테이너
    QWidget* m_container;
    MonthWidget* m_months[3];

    int m_currentYear;
    int m_currentMonth;
    int m_xOffset;
    QTimer* m_scrollTimer;
    QPropertyAnimation* m_animation;
};
#endif // MAINWINDOW_H
