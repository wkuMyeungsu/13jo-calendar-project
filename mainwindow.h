#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWheelEvent>
#include <QPropertyAnimation>
#include <QTimer>

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
    void finishScroll(); // 휠 중지 시 스냅 처리

private:
    Ui::MainWindow *ui;
    
    // 애니메이션 관련
    QWidget* m_container;
    MonthWidget* m_months[3]; // 0: 이전달, 1: 이번달, 2: 다음달
    
    int m_currentYear;
    int m_currentMonth;
    int m_yOffset;      // 현재 스크롤 오프셋
    QTimer* m_scrollTimer;
    QPropertyAnimation* m_animation;
};
#endif // MAINWINDOW_H
