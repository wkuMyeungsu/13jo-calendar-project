#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWheelEvent>
#include <QPropertyAnimation>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMap>

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

public slots:
    void updateCalendar();
    void updateCategoryBar(); // 카테고리 버튼 바 생성/갱신
    void openCategoryManager(); // 카테고리 편집기 열기

private slots:
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
    
    // 카테고리 필터링
    QHBoxLayout* m_categoryLayout;
    QMap<int, bool> m_categoryFilters;

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
