#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DatabaseManager.h"
#include "MonthWidget.h"
#include "ScheduleEdit.h"
#include "ScheduleManagerWidget.h"
#include "CategoryModifyWidget.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDate>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QEasingCurve>
#include <QPoint>

/*** 메인윈도우 ***/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentYear(QDate::currentDate().year())    // 현재 연도 초기화
    , m_currentMonth(QDate::currentDate().month())  // 현재 월 초기화
    , m_xOffset(0)                                  // 드래그/휠 이동 거리 초기화
{
    ui->setupUi(this);

    // DB연결 초기화
    if (!DatabaseManager::instance().initDatabase("calendar_data.db")) {
        qDebug() << "Database initialization failed.";
    }

    /* 네비게이션 헤더 바 (상단 영역) */
    m_headerBar = new QWidget(ui->centralwidget);
    m_headerBar->setStyleSheet("QWidget { background-color: white; border-bottom: 1px solid #E0E0E0; }");

    // 전체 헤더 레이아웃
    QHBoxLayout* headerLayout = new QHBoxLayout(m_headerBar);
    headerLayout->setContentsMargins(15, 0, 15, 0);
    headerLayout->setSpacing(0);

    // [좌측 영역] : 카테고리 필터 버튼
    QWidget* leftGroup = new QWidget(m_headerBar);
    m_categoryLayout = new QHBoxLayout(leftGroup);
    m_categoryLayout->setContentsMargins(0, 0, 0, 0);
    m_categoryLayout->setSpacing(6);
    m_categoryLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // [중앙 영역] : 월 표시, 월 이동 버튼
    QWidget* centerGroup = new QWidget(m_headerBar);
    QHBoxLayout* centerLayout = new QHBoxLayout(centerGroup);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(10);
    centerLayout->setAlignment(Qt::AlignCenter);

    m_prevBtn = new QPushButton("◀", centerGroup);
    m_prevBtn->setCursor(Qt::PointingHandCursor);
    m_prevBtn->setFixedSize(30, 30);
    m_prevBtn->setStyleSheet("QPushButton { border: none; font-size: 16px; color: #555; background: transparent; } QPushButton:hover { color: #4A90E2; }");

    m_monthLabel = new QLabel(centerGroup);
    m_monthLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #222; border: none;");
    m_monthLabel->setAlignment(Qt::AlignCenter);

    m_nextBtn = new QPushButton("▶", centerGroup);
    m_nextBtn->setCursor(Qt::PointingHandCursor);
    m_nextBtn->setFixedSize(30, 30);
    m_nextBtn->setStyleSheet("QPushButton { border: none; font-size: 16px; color: #555; background: transparent; } QPushButton:hover { color: #4A90E2; }");

    centerLayout->addWidget(m_prevBtn);
    centerLayout->addWidget(m_monthLabel);
    centerLayout->addWidget(m_nextBtn);

    // [우측 영역] 오늘 버튼
    QWidget* rightGroup = new QWidget(m_headerBar);
    QHBoxLayout* rightLayout = new QHBoxLayout(rightGroup);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_todayBtn = new QPushButton("오늘", rightGroup);
    m_todayBtn->setCursor(Qt::PointingHandCursor);
    m_todayBtn->setStyleSheet(
        "QPushButton { border: 1px solid #BDBDBD; border-radius: 4px; padding: 4px 15px; font-size: 12px; color: #333; background: white; }"
        "QPushButton:hover { border-color: #4A90E2; color: #4A90E2; }"
    );
    rightLayout->addWidget(m_todayBtn);

    // 헤더 레이아웃에 좌/중/우 배치 (좌우에 동일한 Stretch를 주어 중앙 고정)
    headerLayout->addWidget(leftGroup, 1);
    headerLayout->addWidget(centerGroup, 0);
    headerLayout->addWidget(rightGroup, 1);

    // 네비게이션 버튼 시그널 연결
    connect(m_prevBtn,  &QPushButton::clicked, this, &MainWindow::prevMonth);
    connect(m_nextBtn,  &QPushButton::clicked, this, &MainWindow::nextMonth);
    connect(m_todayBtn, &QPushButton::clicked, this, &MainWindow::goToday);

    /* 캘린더 컨테이너 */
    // 무한 슬라이딩 구현을 위해 3개의 MonthWidget(이전달, 이번달, 다음달)을 사용
    m_container = new QWidget(ui->centralwidget);
    for (int i = 0; i < 3; ++i) {
        m_months[i] = new MonthWidget(m_container);
        // 시그널 연결 - 더블 클릭, 일정 추가 버튼
        connect(m_months[i], &MonthWidget::dayDoubleClicked, this, &MainWindow::handleDayDoubleClicked);
        connect(m_months[i], &MonthWidget::addRequested, this, &MainWindow::handleDayAddRequested);
    }

    // 휠 스크롤이 끝났음을 감지하는 타이머 (150ms 동안 입력 없으면 멈춘 것으로 판단)
    m_scrollTimer = new QTimer(this);
    m_scrollTimer->setSingleShot(true);
    connect(m_scrollTimer, &QTimer::timeout, this, &MainWindow::finishScroll);

    // 가로 슬라이딩 애니메이션 설정
    m_animation = new QPropertyAnimation(m_container, "pos", this);
    m_animation->setDuration(300);
    m_animation->setEasingCurve(QEasingCurve::OutCubic); // 부드럽게 멈추는 효과

    updateCategoryBar();    // 상단 카테고리 버튼 생성
    updateCalendar();       // 달력 데이터 및 위치 갱신
}

/*** updateCategoryBar() : 상단 카테고리 필터 바 갱신 ***/
void MainWindow::updateCategoryBar() {

    // 기존 버튼들 제거
    QLayoutItem *item;
    while ((item = m_categoryLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    // DB에서 모든 카테고리 조회
    QList<QVariantMap> categories = DatabaseManager::instance().getCategories();
    for (const auto& cat : categories) {
        int id = cat["id"].toInt();
        QString name = cat["name"].toString();
        QString color = cat["color"].toString();

        // 필터가 상태가 없으면 기본적으로 '켜짐' 상태로 초기화
        if (!m_categoryFilters.contains(id)) m_categoryFilters[id] = true;

        QPushButton* btn = new QPushButton(name, this);
        btn->setCheckable(true);
        btn->setChecked(m_categoryFilters[id]);

        // 카테고리 고유 색상을 배경 색상으로 적용
        btn->setStyleSheet(QString(
            "QPushButton { border: 1px solid %1; border-radius: 10px; padding: 2px 10px; font-size: 11px; color: %1; background: white; font-weight: bold; } "
            "QPushButton:checked { background-color: %1; color: white; }"
        ).arg(color));

        // 슬롯 연결 - 카테고리 필터 켜기/끄기
        connect(btn, &QPushButton::toggled, [this, id](bool checked) {
            m_categoryFilters[id] = checked;
            updateCalendar();// 필터 변경 후, 달력에 필터링 반영
        });
        m_categoryLayout->addWidget(btn);
    }


    // 카테고리 관리 버튼 추가
    QPushButton* addBtn = new QPushButton("+", this);
    addBtn->setFixedSize(22, 22);
    addBtn->setStyleSheet("QPushButton { border: 1px solid #ccc; border-radius: 11px; background: #f9f9f9; color: #666; font-weight: bold; font-size: 14px; } QPushButton:hover { background: #e9e9e9; color: #333; }");
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::openCategoryManager);
    m_categoryLayout->addWidget(addBtn);
}


/*** updateCalendar() : 달력 데이터 로드 및 위젯 위치 설정 ***/
void MainWindow::updateCalendar() {
    QString title = QString("%1년 %2월").arg(m_currentYear).arg(m_currentMonth);
    m_monthLabel->setText(title);

    int w = ui->centralwidget->width();
    int h = ui->centralwidget->height() - kHeaderH;
    if (w <= 0) w = 800;
    if (h <= 0) h = 600;

    // 컨테이너는 화면 너비의 3배 - 자식 위젯(이전달/이번달/다음달)을 가로로 배치
    m_container->resize(w * 3, h);
    for (int i = 0; i < 3; ++i) m_months[i]->setGeometry(i * w, 0, w, h);

    // 항상 중앙(이번달)이 보이도록 컨테이너 위치를 -w 로 조정
    m_container->move(-w, kHeaderH);

    QDate current(m_currentYear, m_currentMonth, 1);
    QDate prev = current.addMonths(-1);
    QDate next = current.addMonths(1);

    // 람다 함수 - DB에서 데이터를 가져온 후 현재 필터링된 카테고리만 걸러냄
    auto getFilteredSchedules = [this](int year, int month) {
        QList<QVariantMap> raw = DatabaseManager::instance().getSchedulesForMonth(year, month);
        QList<QVariantMap> filtered;
        for (const auto& s : raw) {
            if (m_categoryFilters.value(s["category_id"].toInt(), true)) filtered.append(s);
        }
        return filtered;
    };

    // 3개의 위젯에 각각 데이터 주입
    m_months[0]->updateMonth(prev.year(), prev.month(), getFilteredSchedules(prev.year(), prev.month()));
    m_months[1]->updateMonth(current.year(), current.month(), getFilteredSchedules(current.year(), current.month()));
    m_months[2]->updateMonth(next.year(), next.month(), getFilteredSchedules(next.year(), next.month()));
}

/*** resizeEvent(QResizeEvent* event) : 윈도우 크기 조절 시 달력 레이아웃 다시 계산 ***/
void MainWindow::resizeEvent(QResizeEvent* event) {
    int w = ui->centralwidget->width();
    int h = ui->centralwidget->height() - kHeaderH;
    m_headerBar->setGeometry(0, 0, w, kHeaderH);
    m_container->resize(w * 3, h);
    for (int i = 0; i < 3; ++i) m_months[i]->setGeometry(i * w, 0, w, h);
    m_container->move(-w + m_xOffset, kHeaderH);
    QMainWindow::resizeEvent(event);
}

/*** wheelEvent(QWheelEvent* event) : 마우스 휠을 돌릴 때 달력을 좌우로 밀기 ***/
void MainWindow::wheelEvent(QWheelEvent* event) {
    if (m_animation->state() == QAbstractAnimation::Running) return;

    // 휠 델타 값을 이용해 xOffset 계산 (가로 스크롤)
    m_xOffset += event->angleDelta().y() * 1.5;

    int w = ui->centralwidget->width();

    // 최대 이동 범위 제한 (이전달/다음달 범위를 넘지 않게)
    if (m_xOffset > w)  m_xOffset = w;
    if (m_xOffset < -w) m_xOffset = -w;

    m_container->move(-w + m_xOffset, kHeaderH);

    // 휠 동작 멈추면 finishScroll 실행
    m_scrollTimer->start(150);
    event->accept();
}

/*** finishScroll() : 자석처럼 딱 붙게 하는 스냅 동작 ***/
void MainWindow::finishScroll() {
    int w = ui->centralwidget->width();
    // 슬라이드 1/6 이상 밀었을 때 페이지 전환
    int threshold = w / 6;

    int targetX;
    if (m_xOffset > threshold) {
        targetX = 0;        // 이전달 위치로 슬라이딩
    } else if (m_xOffset < -threshold) {
        targetX = -2 * w;   // 다음달 위치로 슬라이딩
    } else {
        targetX = -w;       // threshold보다 덜 밀었을 경우 -> 제자리 이동
    }

    m_animation->setStartValue(m_container->pos());
    m_animation->setEndValue(QPoint(targetX, kHeaderH));

    // 애니메이션 종료 후 실제 연도/월 변수를 변경하고 위치 리셋
    connect(m_animation, &QPropertyAnimation::finished, [this, targetX, w]() {
        m_animation->disconnect(SIGNAL(finished()));
        if (targetX == 0) {
            m_currentMonth--;
            if (m_currentMonth < 1) {
                m_currentMonth = 12;
                m_currentYear--; }
        }
        else if (targetX == -2 * w) {
            m_currentMonth++;
            if (m_currentMonth > 12) {
                m_currentMonth = 1; m_currentYear++;
            }
        }
        m_xOffset = 0;
        updateCalendar(); // 데이터 갱신 및 컨테이너 위치(-w) 복구
    });
    m_animation->start();
}

void MainWindow::slideMonth(int direction) {
    // direction : -1(이전달), 0(제자리), 1(다음달)
    if (m_animation->state() == QAbstractAnimation::Running) return;

    int w = ui->centralwidget->width();
    int targetX;

    if(direction == -1)     targetX = 0;        // 왼쪽에서 오른쪽으로 밀기 (이전달)
    else if(direction == 1) targetX = -2 * w;   // 오른쪽에서 왼쪽으로 밀기 (다음달)
    else                    targetX = -w;       // 제자리로 복귀

    m_animation->stop();
    m_animation->setStartValue(m_container->pos());
    m_animation->setEndValue(QPoint(0, kHeaderH));

    disconnect(m_animation, &QPropertyAnimation::finished, nullptr, nullptr);
    connect(m_animation, &QPropertyAnimation::finished, [this, direction](){
        if(direction != 0) {
            // 실제 날짜 변경 로직
            QDate date(m_currentYear, m_currentMonth, 1);
            date = date.addMonths(direction);
            m_currentYear = date.year();
            m_currentMonth = date.month();
        }

        m_xOffset = 0;      // 오프셋 초기화
        updateCalendar();    // 달력 위치 및 데이터 갱신
    });

    m_animation->start();
}

/*** prevMonth() : 이전달 버튼 클릭시 ***/
void MainWindow::prevMonth() {
    if (m_animation->state() == QAbstractAnimation::Running) return;
    int w = ui->centralwidget->width();
    m_animation->setStartValue(m_container->pos());
    m_animation->setEndValue(QPoint(0, kHeaderH));
    connect(m_animation, &QPropertyAnimation::finished, [this]() {
        m_animation->disconnect(SIGNAL(finished()));
        m_currentMonth--;
        if (m_currentMonth < 1) {
            m_currentMonth = 12; m_currentYear--;
        }
        m_xOffset = 0; updateCalendar();
    });
    m_animation->start();
}

/*** nextMonth() : 다음달 버튼 클릭시 ***/
void MainWindow::nextMonth() {
    if (m_animation->state() == QAbstractAnimation::Running) return;
    int w = ui->centralwidget->width();
    m_animation->setStartValue(m_container->pos());
    m_animation->setEndValue(QPoint(-2 * w, kHeaderH));
    connect(m_animation, &QPropertyAnimation::finished, [this]() {
        m_animation->disconnect(SIGNAL(finished()));
        m_currentMonth++; if (m_currentMonth > 12) {
            m_currentMonth = 1; m_currentYear++;
        }
        m_xOffset = 0; updateCalendar();
    });
    m_animation->start();
}

/*** goToday() : 오늘 날짜로 이동 ***/
void MainWindow::goToday() {
    QDate today = QDate::currentDate();
    m_currentYear = today.year(); m_currentMonth = today.month();
    m_xOffset = 0; updateCalendar();
}

/*** handleDayDoubleClicked(const QDate& date) : 날짜 더블 클릭 시 일정 관리 창 표시 ***/
void MainWindow::handleDayDoubleClicked(const QDate& date) {
    ScheduleManagerWidget *managerWidget = new ScheduleManagerWidget(date);
    managerWidget->setAttribute(Qt::WA_DeleteOnClose);
    managerWidget->setWindowModality(Qt::ApplicationModal);
    connect(managerWidget, &ScheduleManagerWidget::dataChanged, [this]() {
        updateCategoryBar();
        updateCalendar();
    });
    managerWidget->show();
}

/*** handleDayAddRequested(const QDate& date) : 빈 날짜 영역 클릭 시 일정 추가 창 표시 ***/
void MainWindow::handleDayAddRequested(const QDate& date) {
    ScheduleInputWidget *inputWidget = new ScheduleInputWidget(date);
    inputWidget->setAttribute(Qt::WA_DeleteOnClose);
    inputWidget->setWindowModality(Qt::ApplicationModal);
    connect(inputWidget, &ScheduleInputWidget::scheduleSaved, this, &MainWindow::updateCalendar);
    inputWidget->show();
}

/*** openCategoryManager() : 카테고리 관리 다이얼로그 열기 ***/
void MainWindow::openCategoryManager() {
    CategoryModifyWidget *catModify = new CategoryModifyWidget(); 
    catModify->setAttribute(Qt::WA_DeleteOnClose);
    catModify->setWindowTitle("카테고리 설정");
    // 카테고리가 바뀌면 상단바와 달력을 모두 갱신
    connect(catModify, &CategoryModifyWidget::categoriesChanged, this, &MainWindow::updateCategoryBar);
    connect(catModify, &CategoryModifyWidget::categoriesChanged, this, &MainWindow::updateCalendar);
    catModify->show();
    catModify->raise();
    catModify->activateWindow();
}

MainWindow::~MainWindow() { delete ui; }
