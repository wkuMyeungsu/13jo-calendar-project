#include "CategoryModifyWidget.h"
#include "../models/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QColorDialog>
#include <QMessageBox>
#include <QVariantMap>

CategoryModifyWidget::CategoryModifyWidget(QWidget *parent) : QWidget(parent), m_currentEditingId(-1) {
    setWindowTitle("카테고리 설정");
    setFixedSize(400, 550);
    setStyleSheet("background-color: white;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 1. 상단 제목
    QLabel *headerLabel = new QLabel("카테고리 관리", this);
    headerLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #333; margin-bottom: 5px;");
    mainLayout->addWidget(headerLabel);

    // 2. 카테고리 목록 섹션
    QLabel *listLabel = new QLabel("현재 카테고리 (클릭 시 수정)", this);
    listLabel->setStyleSheet("color: #666; font-weight: bold; font-size: 13px;");
    mainLayout->addWidget(listLabel);

    m_listWidget = new QListWidget(this);
    m_listWidget->setSpacing(8);
    m_listWidget->setStyleSheet(
        "QListWidget { border: 1px solid #EEE; border-radius: 8px; background: #FAFAFA; padding: 5px; }"
        "QListWidget::item { background: white; border: 1px solid #EEE; border-radius: 6px; padding: 10px; margin-bottom: 2px; }"
        "QListWidget::item:selected { background: #F0F7FF; border: 1px solid #4A90E2; color: #333; }"
        "QListWidget::item:hover { background: #F5F9FF; border: 1px solid #D0E5FF; }"
        "QScrollBar:vertical { border: none; background: transparent; width: 8px; margin: 0px; }"
        "QScrollBar::handle:vertical { background: #E0E0E0; min-height: 20px; border-radius: 4px; }"
        "QScrollBar::handle:vertical:hover { background: #BDBDBD; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
    );
    mainLayout->addWidget(m_listWidget);

    // 3. 입력 필드 섹션
    QString inputStyle = 
        "QLineEdit { border: 1px solid #DDD; border-radius: 6px; padding: 8px; background: #FDFDFD; color: #333; }"
        "QLineEdit:focus { border: 1px solid #4A90E2; background: white; }";

    QHBoxLayout *editLayout = new QHBoxLayout();
    editLayout->setSpacing(10);

    m_nameInput = new QLineEdit(this);
    m_nameInput->setPlaceholderText("카테고리 이름을 입력하세요");
    m_nameInput->setStyleSheet(inputStyle);
    
    m_selectedColor = "#4A90E2";
    m_colorBtn = new QPushButton(this);
    m_colorBtn->setCursor(Qt::PointingHandCursor);
    m_colorBtn->setFixedSize(36, 36);
    m_colorBtn->setStyleSheet(QString("background-color: %1; border-radius: 18px; border: 2px solid white; outline: 1px solid #DDD;").arg(m_selectedColor));

    editLayout->addWidget(m_nameInput, 1);
    editLayout->addWidget(m_colorBtn);
    mainLayout->addLayout(editLayout);

    // 4. 버튼 섹션
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);

    m_addBtn = new QPushButton("신규 추가", this);
    m_addBtn->setCursor(Qt::PointingHandCursor);
    m_addBtn->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #45a049; }"
    );
    
    m_editBtn = new QPushButton("변경 저장", this);
    m_editBtn->setCursor(Qt::PointingHandCursor);
    m_editBtn->setStyleSheet(
        "QPushButton { background-color: #4A90E2; color: white; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #357ABD; }"
        "QPushButton:disabled { background-color: #E0E0E0; color: #AAA; }"
    );
    m_editBtn->setEnabled(false);

    m_deleteBtn = new QPushButton("삭제", this);
    m_deleteBtn->setCursor(Qt::PointingHandCursor);
    m_deleteBtn->setStyleSheet(
        "QPushButton { background-color: #F8D7DA; color: #721C24; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 6px; border: 1px solid #F5C6CB; }"
        "QPushButton:hover { background-color: #E24A4A; color: white; border-color: #E24A4A; }"
        "QPushButton:disabled { background-color: #F9F9F9; color: #DDD; border-color: #EEE; }"
    );
    m_deleteBtn->setEnabled(false);

    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_editBtn);
    btnLayout->addWidget(m_deleteBtn);
    mainLayout->addLayout(btnLayout);

    // 시그널 연결
    connect(m_listWidget, &QListWidget::itemClicked, this, &CategoryModifyWidget::onItemSelected);
    connect(m_colorBtn, &QPushButton::clicked, this, &CategoryModifyWidget::selectColor);
    connect(m_addBtn, &QPushButton::clicked, this, &CategoryModifyWidget::handleAdd);
    connect(m_editBtn, &QPushButton::clicked, this, &CategoryModifyWidget::handleEdit);
    connect(m_deleteBtn, &QPushButton::clicked, this, &CategoryModifyWidget::handleDelete);

    loadCategories();
}

void CategoryModifyWidget::loadCategories() {
    m_listWidget->clear();
    QList<QVariantMap> categories = DatabaseManager::instance().getCategories();
    for (const auto& cat : categories) {
        QListWidgetItem *item = new QListWidgetItem(cat["name"].toString(), m_listWidget);
        item->setData(Qt::UserRole, cat["id"]);
        item->setData(Qt::UserRole + 1, cat["color"].toString());
        
        // 텍스트 색상 및 폰트 설정
        item->setForeground(QColor(cat["color"].toString()));
        QFont font = item->font();
        font.setBold(true);
        font.setPointSize(10);
        item->setFont(font);
    }
    
    // 상태 초기화
    m_currentEditingId = -1;
    m_nameInput->clear();
    m_editBtn->setEnabled(false);
    m_deleteBtn->setEnabled(false);
}

void CategoryModifyWidget::selectColor() {
    QColor color = QColorDialog::getColor(QColor(m_selectedColor), this, "카테고리 색상 선택");
    if (color.isValid()) {
        m_selectedColor = color.name();
        m_colorBtn->setStyleSheet(QString("background-color: %1; border-radius: 18px; border: 2px solid white; outline: 1px solid #DDD;").arg(m_selectedColor));
    }
}

void CategoryModifyWidget::onItemSelected(QListWidgetItem *item) {
    if (!item) return;
    
    m_currentEditingId = item->data(Qt::UserRole).toInt();
    m_nameInput->setText(item->text());
    m_selectedColor = item->data(Qt::UserRole + 1).toString();
    m_colorBtn->setStyleSheet(QString("background-color: %1; border-radius: 18px; border: 2px solid white; outline: 1px solid #DDD;").arg(m_selectedColor));
    
    m_editBtn->setEnabled(true);
    m_deleteBtn->setEnabled(true);
}

void CategoryModifyWidget::handleAdd() {
    QString name = m_nameInput->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "알림", "카테고리 이름을 입력해 주세요.");
        return;
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
        QMessageBox::warning(this, "알림", "카테고리 이름을 입력해 주세요.");
        return;
    }

    if (DatabaseManager::instance().updateCategory(m_currentEditingId, name, m_selectedColor)) {
        loadCategories();
        emit categoriesChanged();
    }
}

void CategoryModifyWidget::handleDelete() {
    if (m_currentEditingId == -1) return;

    if (QMessageBox::question(this, "삭제 확인", "이 카테고리를 삭제하시겠습니까?\n(해당 카테고리의 일정은 미지정으로 변경됩니다.)") == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteCategory(m_currentEditingId)) {
            loadCategories();
            emit categoriesChanged();
        }
    }
}
