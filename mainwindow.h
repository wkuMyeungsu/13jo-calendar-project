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
#include <QSizeGrip>
#include "CustomTitleBar.h"
#include "models/Schedule.h"

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
    void changeEvent(QEvent* event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

public slots:
    void updateCalendar();
    void updateCategoryBar(); 
    void openCategoryManager(); 
    void toggleOverflow();    
    void setMiniMode(bool mini);
    void togglePinned();        
    void openSettingsWidget();

private slots:
    void handleDayDoubleClicked(const QDate& date);
    void handleDayAddRequested(const QDate& date);
    void finishScroll();
    void prevMonth();
    void nextMonth();
    void goToday();
    void slideMonth(int direction);

private:
    void updateLayoutPositions(); 
    void updateMiniModeStyle(); 
    void updateMainStyle(); // 일반 모드 테마 스타일 갱신

    int m_currentHeaderH;

    CustomTitleBar* m_titleBar;
    QSizeGrip*      m_sizeGrip;

    Ui::MainWindow *ui;

    QWidget*     m_headerBar;
    QLabel*      m_monthLabel;
    QPushButton* m_prevBtn;
    QPushButton* m_nextBtn;
    QPushButton* m_todayBtn;
    
    QHBoxLayout* m_categoryLayout;
    QMap<int, bool> m_categoryFilters;

    QWidget*      m_overflowWidget;
    QGridLayout*  m_overflowLayout;
    QPushButton*  m_moreBtn;
    bool          m_isExpanded;

    QWidget* m_container;
    MonthWidget* m_months[3];

    int m_currentYear;
    int m_currentMonth;
    int m_xOffset;
    QTimer* m_scrollTimer;
    QPropertyAnimation* m_animation;

    // 미니 모드 관련
    bool     m_isMiniMode;
    bool     m_isPinned;
    QWidget* m_miniWidget;
    QLabel*  m_miniDateLabel;
    QLabel*  m_miniTimeLabel;
    QPushButton* m_pinBtn;
    QVBoxLayout* m_miniScheduleLayout;
    QTimer*  m_realTimeTimer;
    QMap<QObject*, Schedule> m_miniItemDataMap;
};
#endif // MAINWINDOW_H
