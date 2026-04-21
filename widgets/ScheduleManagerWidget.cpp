#include "ScheduleManagerWidget.h"
#include "UiCommon.h"
#include "ScheduleEdit.h"
#include "SettingsWidget.h"

ScheduleManagerWidget::ScheduleManagerWidget(const QDate& date, QWidget *parent) : QWidget(parent), m_date(date) {
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(UiConstants::DIALOG_WIDGET_WIDTH + 2, StyleHelper::WIDGET_HEIGHT + UiConstants::TITLE_BAR_HEIGHT + 2);
    setWindowTitle(date.toString("yyyy-MM-dd") + " 일정 관리");

    // [New] 마스터 프레임 (모든 것을 감싸고 테두리/곡률 담당)
    QFrame* mainFrame = new QFrame(this);
    mainFrame->setObjectName("mainFrame");
    mainFrame->setStyleSheet(StyleHelper::getDialogFrameStyle());

    // 최상위 레이아웃 (프레임에 1px 여백을 주어 곡률 안티앨리어싱 확보)
    QVBoxLayout* masterLayout = new QVBoxLayout(this);
    masterLayout->setContentsMargins(1, 1, 1, 1);
    masterLayout->addWidget(mainFrame);

    // 프레임 내부 레이아웃
    QVBoxLayout* frameLayout = new QVBoxLayout(mainFrame);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    frameLayout->setSpacing(0);

    m_titleBar = new CustomTitleBar(mainFrame);
    m_titleBar->setResizable(false);
    m_titleBar->showMinMaxButtons(false); // 최소화/최대화 숨김
    m_titleBar->setTitle(UiConstants::TITLE_SCHEDULE_LIST);
    m_titleBar->applyTheme(StyleHelper::getBgColor(), StyleHelper::getTextColor(), "#DDD");
    frameLayout->addWidget(m_titleBar);

    m_contentWidget = new QWidget(mainFrame);
    m_contentWidget->setObjectName("container");
    m_contentWidget->setStyleSheet(StyleHelper::getDialogStyle());
    frameLayout->addWidget(m_contentWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_contentWidget);
    mainLayout->setContentsMargins(UiConstants::CONTENT_MARGIN, UiConstants::CONTENT_MARGIN, UiConstants::CONTENT_MARGIN, UiConstants::CONTENT_MARGIN);
    mainLayout->setSpacing(UiConstants::BTN_LAYOUT_SPACING);

    QScrollArea *scrollArea = new QScrollArea(m_contentWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(StyleHelper::getScrollbarStyle());

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent;");
    m_listLayout = new QVBoxLayout(scrollContent);
    m_listLayout->setContentsMargins(0, 0, 10, 0);
    m_listLayout->setSpacing(UiConstants::BTN_LAYOUT_SPACING);
    m_listLayout->setAlignment(Qt::AlignTop);
    
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    QPushButton *addBtn = new QPushButton("+ 새 일정 추가", m_contentWidget);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet(StyleHelper::getBtnSaveStyle());
    mainLayout->addWidget(addBtn);

    connect(addBtn, &QPushButton::clicked, this, &ScheduleManagerWidget::openAddWidget);

    refreshList();
}

void ScheduleManagerWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void ScheduleManagerWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        m_titleBar->updateMaxIcon();
    }
    QWidget::changeEvent(event);
}

void ScheduleManagerWidget::refreshList() {
    m_itemDataMap.clear();
    QLayoutItem *item;
    while ((item = m_listLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    auto schedules = DatabaseManager::instance().getSchedulesForDay(m_date);
    if (schedules.isEmpty()) {
        QLabel *emptyLabel = new QLabel("등록된 일정이 없습니다.", m_contentWidget);
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet(QString("color: %1; font-size: 14px; margin-top: 50px; background: transparent;").arg(UiConstants::COLOR_TEXT_LIGHT));
        m_listLayout->addWidget(emptyLabel);
        return;
    }

    for (const auto& s : schedules) {
        QWidget *itemWidget = new QWidget(m_contentWidget);
        itemWidget->setObjectName("itemWidget");
        itemWidget->setCursor(Qt::PointingHandCursor);
        itemWidget->setAttribute(Qt::WA_Hover);
        itemWidget->installEventFilter(this);
        
        QString colorStr = s.color.isEmpty() ? StyleHelper::getPrimaryColor() : s.color;
        itemWidget->setStyleSheet(StyleHelper::getItemBaseStyle(colorStr));

        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(15, 12, 15, 12);

        QVBoxLayout *textLayout = new QVBoxLayout();
        QLabel *title = new QLabel(s.title, itemWidget);
        title->setStyleSheet(QString("font-weight: bold; font-size: 14px; color: %1; border: none; background: transparent;").arg(StyleHelper::getTextColor()));

        QString startStr = s.start.toString("HH:mm");
        QString endStr   = s.end.toString("HH:mm");
        QString timeStr  = s.isAllDay ? "하루 종일" : QString("%1 ~ %2").arg(startStr, endStr);
        QLabel *time = new QLabel(timeStr, itemWidget);
        time->setStyleSheet(QString("color: %1; font-size: 12px; border: none; background: transparent;").arg(UiConstants::COLOR_TEXT_DIM));

        textLayout->addWidget(title);
        textLayout->addWidget(time);
        itemLayout->addLayout(textLayout, 2);

        QString content = s.content.simplified();
        if (!content.isEmpty()) {
            if (content.length() > 25) content = content.left(22) + "...";
            QLabel *noteLabel = new QLabel(content, itemWidget);
            noteLabel->setStyleSheet(QString("color: %1; font-size: %2px; font-style: italic; border: none; background: transparent; padding-left: 10px; padding-right: 5px;").arg(UiConstants::COLOR_TEXT_LIGHT).arg(UiConstants::FONT_SIZE_SMALL));
            noteLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            itemLayout->addWidget(noteLabel, 3);
        } else {
            itemLayout->addStretch(3);
        }

        m_itemDataMap[itemWidget] = s;
        m_listLayout->addWidget(itemWidget);
    }
}

bool ScheduleManagerWidget::eventFilter(QObject *obj, QEvent *event) {
    QWidget *widget = qobject_cast<QWidget*>(obj);
    if (!widget || !m_itemDataMap.contains(widget)) return QWidget::eventFilter(obj, event);

    QString colorStr = m_itemDataMap[widget].color;
    if (colorStr.isEmpty()) colorStr = StyleHelper::getPrimaryColor();

    if (event->type() == QEvent::MouseButtonPress) {
        openEditWidget(m_itemDataMap[widget]);
        return true;
    } 
    else if (event->type() == QEvent::HoverEnter) {
        widget->setStyleSheet(StyleHelper::getItemHoverStyle(colorStr));
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(widget);
        shadow->setBlurRadius(15); shadow->setYOffset(4); shadow->setColor(QColor(0, 0, 0, 40));
        widget->setGraphicsEffect(shadow);
    } 
    else if (event->type() == QEvent::HoverLeave) {
        widget->setStyleSheet(StyleHelper::getItemBaseStyle(colorStr));
        widget->setGraphicsEffect(nullptr);
    }

    return QWidget::eventFilter(obj, event);
}

void ScheduleManagerWidget::openAddWidget() {
    ScheduleEditDialog *inputWidget = new ScheduleEditDialog(m_date);
    inputWidget->setAttribute(Qt::WA_DeleteOnClose);
    inputWidget->setWindowModality(Qt::ApplicationModal);
    connect(inputWidget, &ScheduleEditDialog::scheduleSaved, this, [this]() {
        refreshList();
        emit dataChanged();
    });
    inputWidget->show();
}

void ScheduleManagerWidget::openEditWidget(const Schedule& data) {
    ScheduleEditDialog *modifyWidget = new ScheduleEditDialog(data);
    modifyWidget->setAttribute(Qt::WA_DeleteOnClose);
    modifyWidget->setWindowModality(Qt::ApplicationModal);
    connect(modifyWidget, &ScheduleEditDialog::scheduleSaved, this, [this]() {
        refreshList();
        emit dataChanged();
    });
    connect(modifyWidget, &ScheduleEditDialog::scheduleDeleted, this, [this]() {
        refreshList();
        emit dataChanged();
    });
    modifyWidget->show();
}
