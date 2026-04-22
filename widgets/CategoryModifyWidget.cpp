#include "CategoryModifyWidget.h"
#include "UiCommon.h"
#include "ColorPickerPopup.h"
#include "CustomMessageBox.h"

CategoryModifyWidget::CategoryModifyWidget(QWidget *parent) : QWidget(parent), m_currentEditingId(-1) {
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(StyleHelper::WIDGET_WIDTH + 2, StyleHelper::WIDGET_HEIGHT + UiConstants::TITLE_BAR_HEIGHT + 2);
    setWindowTitle("카테고리 설정");

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
    m_titleBar->setTitle(UiConstants::TITLE_CATEGORY_MGMT);
    m_titleBar->applyTheme(StyleHelper::getBgColor(), StyleHelper::getTextColor(), "#DDD");
    frameLayout->addWidget(m_titleBar);

    m_contentWidget = new QWidget(mainFrame);
    m_contentWidget->setObjectName("container");
    m_contentWidget->setStyleSheet(StyleHelper::getDialogStyle());
    frameLayout->addWidget(m_contentWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_contentWidget);
    mainLayout->setContentsMargins(StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN);
    mainLayout->setSpacing(StyleHelper::LAYOUT_SPACING);

    // 2. 카테고리 목록 섹션
    QLabel *listLabel = new QLabel("현재 카테고리 (클릭 시 수정)", m_contentWidget);

    listLabel->setStyleSheet(StyleHelper::getFormLabelStyle());
    mainLayout->addWidget(listLabel);

    m_listWidget = new QListWidget(m_contentWidget);
    m_listWidget->setSpacing(8);
    m_listWidget->setStyleSheet(StyleHelper::getListWidgetStyle() + StyleHelper::getScrollbarStyle());
    mainLayout->addWidget(m_listWidget);

    // 3. 입력 필드 섹션
    QString inputStyle = StyleHelper::getCommonInputStyle();

    QHBoxLayout *editLayout = new QHBoxLayout();
    editLayout->setSpacing(10);

    m_nameInput = new QLineEdit(m_contentWidget);
    m_nameInput->setPlaceholderText("카테고리 이름을 입력하세요");
    m_nameInput->setStyleSheet(inputStyle);
    
    m_selectedColor = "#4A90E2";
    m_colorBtn = new QPushButton(m_contentWidget);
    m_colorBtn->setCursor(Qt::PointingHandCursor);
    m_colorBtn->setFixedSize(UiConstants::COLOR_BTN_SIZE, UiConstants::COLOR_BTN_SIZE);
    m_colorBtn->setStyleSheet(StyleHelper::getCircleButtonStyle(m_selectedColor, UiConstants::COLOR_BTN_SIZE));
    m_colorBtn->setEnabled(true); // 초기 활성화 (새 카테고리용 색상 선택 가능)

    editLayout->addWidget(m_nameInput, 1);
    editLayout->addWidget(m_colorBtn);
    mainLayout->addLayout(editLayout);

    // 4. 버튼 섹션
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);

    m_addBtn = new QPushButton("신규 추가", m_contentWidget);
    m_addBtn->setCursor(Qt::PointingHandCursor);
    m_addBtn->setStyleSheet(StyleHelper::getBtnSaveStyle());
    
    m_editBtn = new QPushButton("변경 저장", m_contentWidget);
    m_editBtn->setCursor(Qt::PointingHandCursor);
    m_editBtn->setStyleSheet(StyleHelper::getBtnModifyStyle());
    m_editBtn->setEnabled(false);

    m_deleteBtn = new QPushButton("삭제", m_contentWidget);
    m_deleteBtn->setCursor(Qt::PointingHandCursor);
    m_deleteBtn->setStyleSheet(StyleHelper::getBtnDeleteStyle());
    m_deleteBtn->setEnabled(false);

    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_editBtn);
    btnLayout->addWidget(m_deleteBtn);
    mainLayout->addLayout(btnLayout);

    // 시그널 연결
    connect(m_listWidget, &QListWidget::itemClicked, this, &CategoryModifyWidget::onItemSelected);
    connect(m_listWidget, &QListWidget::itemSelectionChanged, this, [this]() {
        if (m_listWidget->selectedItems().isEmpty()) loadCategories();
    });
    connect(m_nameInput, &QLineEdit::textChanged, this, &CategoryModifyWidget::validateButtons);
    connect(m_colorBtn, &QPushButton::clicked, this, &CategoryModifyWidget::selectColor);
    connect(m_addBtn, &QPushButton::clicked, this, &CategoryModifyWidget::handleAdd);
    connect(m_editBtn, &QPushButton::clicked, this, &CategoryModifyWidget::handleEdit);
    connect(m_deleteBtn, &QPushButton::clicked, this, &CategoryModifyWidget::handleDelete);

    loadCategories();
}

void CategoryModifyWidget::validateButtons() {
    QString currentName = m_nameInput->text().trimmed();
    bool isNameEmpty = currentName.isEmpty();
    
    if (m_currentEditingId != -1) {
        // [수정 모드]
        // 1. 변경사항이 있어야 함 (이름 또는 색상)
        // 2. 이름이 비어있지 않아야 함
        bool isChanged = (currentName != m_originalName) || (m_selectedColor != m_originalColor);
        m_editBtn->setEnabled(isChanged && !isNameEmpty);
        m_addBtn->setEnabled(false); 
    } else {
        // [추가 모드]
        // 이름이 비어있지 않을 때만 활성화
        m_addBtn->setEnabled(!isNameEmpty);
        m_editBtn->setEnabled(false);
    }
}

void CategoryModifyWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void CategoryModifyWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        m_titleBar->updateMaxIcon();
    }
    QWidget::changeEvent(event);
}

void CategoryModifyWidget::loadCategories() {
    m_listWidget->blockSignals(true);
    m_listWidget->clearSelection();
    m_listWidget->blockSignals(false);

    m_listWidget->clear();
    auto categories = DatabaseManager::instance().getCategories();
    for (const auto& cat : categories) {
        QListWidgetItem *item = new QListWidgetItem(cat.name, m_listWidget);
        item->setData(Qt::UserRole, cat.id);
        item->setData(Qt::UserRole + 1, cat.color);

        item->setForeground(QColor(cat.color));
        QFont font = item->font();
        font.setBold(true);
        font.setPointSize(10);
        item->setFont(font);
    }
    
    // 상태 초기화
    m_currentEditingId = -1;
    m_originalName.clear();
    m_originalColor.clear();
    m_nameInput->clear();
    m_selectedColor = "#4A90E2";
    m_colorBtn->setStyleSheet(StyleHelper::getCircleButtonStyle(m_selectedColor, UiConstants::COLOR_BTN_SIZE));
    
    m_colorBtn->setEnabled(true);  // [변경] 초기 상태에서도 색상 선택 가능
    m_addBtn->setEnabled(false);   // 이름 입력 전까지 비활성화
    m_editBtn->setEnabled(false);
    m_deleteBtn->setEnabled(false);
}

void CategoryModifyWidget::selectColor() {
    ColorPickerPopup *popup = new ColorPickerPopup(this);
    popup->move(m_colorBtn->mapToGlobal(QPoint(0, m_colorBtn->height())));

    connect(popup, &ColorPickerPopup::colorSelected, this, [this](const QString& color) {
        m_selectedColor = color;
        m_colorBtn->setStyleSheet(StyleHelper::getCircleButtonStyle(m_selectedColor, UiConstants::COLOR_BTN_SIZE));
        validateButtons();
    });

    popup->show();
}

void CategoryModifyWidget::onItemSelected(QListWidgetItem *item) {
    if (!item) return;
    
    m_currentEditingId = item->data(Qt::UserRole).toInt();
    m_originalName = item->text();
    m_originalColor = item->data(Qt::UserRole + 1).toString();
    
    m_nameInput->setText(m_originalName);
    m_selectedColor = m_originalColor;
    m_colorBtn->setStyleSheet(StyleHelper::getCircleButtonStyle(m_selectedColor, UiConstants::COLOR_BTN_SIZE));
    
    m_colorBtn->setEnabled(true); // 아이템 선택 시 활성화
    m_addBtn->setEnabled(false);  // 수정 모드이므로 비활성화
    m_editBtn->setEnabled(false); // 아직 변경 전이므로 비활성화
    m_deleteBtn->setEnabled(true);
}

void CategoryModifyWidget::handleAdd() {
    QString name = m_nameInput->text().trimmed();
    if (name.isEmpty()) {
        CustomMessageBox::warning(this, "알림", "카테고리 이름을 입력해 주세요.");
        return;
    }

    // 중복 체크
    auto categories = DatabaseManager::instance().getCategories();
    for (const auto& cat : categories) {
        if (cat.name == name) {
            CustomMessageBox::warning(this, "중복 오류", "이미 존재하는 카테고리 이름입니다.");
            return;
        }
    }

    if (DatabaseManager::instance().addCategory(name, m_selectedColor)) {
        loadCategories();
        emit categoriesChanged();
    }
}

void CategoryModifyWidget::handleEdit() {
    if (m_currentEditingId == -1) return;
    
    QString name = m_nameInput->text().trimmed();
    if (name.isEmpty()) {
        CustomMessageBox::warning(this, "알림", "카테고리 이름을 입력해 주세요.");
        return;
    }

    // 중복 체크 (자신 제외)
    auto categories = DatabaseManager::instance().getCategories();
    for (const auto& cat : categories) {
        if (cat.id != m_currentEditingId && cat.name == name) {
            CustomMessageBox::warning(this, "중복 오류", "이미 존재하는 카테고리 이름입니다.");
            return;
        }
    }

    if (DatabaseManager::instance().updateCategory(m_currentEditingId, name, m_selectedColor)) {
        loadCategories();
        emit categoriesChanged();
    }
}

void CategoryModifyWidget::handleDelete() {
    if (m_currentEditingId == -1) return;

    if (CustomMessageBox::question(this, "삭제 확인", "이 카테고리를 삭제하시겠습니까?\n(해당 카테고리의 일정은 미지정으로 변경됩니다.)")) {
        if (DatabaseManager::instance().deleteCategory(m_currentEditingId)) {
            loadCategories();
            emit categoriesChanged();
        }
    }
}
