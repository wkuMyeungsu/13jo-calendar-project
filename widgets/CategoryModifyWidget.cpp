#include "CategoryModifyWidget.h"
#include "../models/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QColorDialog>
#include <QMessageBox>
#include <QVariantMap>

CategoryModifyWidget::CategoryModifyWidget(QWidget *parent) : QWidget(parent), m_currentEditingId(-1) {
    setWindowTitle("카테고리 수정 및 관리");
    setFixedSize(350, 450);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 1. 카테고리 목록 섹션
    mainLayout->addWidget(new QLabel("카테고리 목록 (클릭 시 수정 가능):", this));
    m_listWidget = new QListWidget(this);
    m_listWidget->setStyleSheet("QListWidget::item { height: 40px; padding: 5px; }");
    mainLayout->addWidget(m_listWidget);

    // 2. 입력/편집 섹션
    mainLayout->addSpacing(10);
    mainLayout->addWidget(new QLabel("카테고리 설정:", this));
    
    QHBoxLayout *editLayout = new QHBoxLayout();
    m_nameInput = new QLineEdit(this);
    m_nameInput->setPlaceholderText("카테고리 이름");
    
    m_selectedColor = "#4A90E2"; // 기본 색상
    m_colorBtn = new QPushButton(this);
    m_colorBtn->setFixedWidth(30);
    m_colorBtn->setFixedHeight(30);
    m_colorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #ccc;").arg(m_selectedColor));

    editLayout->addWidget(m_nameInput);
    editLayout->addWidget(m_colorBtn);
    mainLayout->addLayout(editLayout);

    // 3. 버튼 섹션 (추가/수정/삭제)
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_addBtn = new QPushButton("신규 추가", this);
    m_addBtn->setStyleSheet("background-color: #4CAF50; color: white; padding: 5px; font-weight: bold;");
    
    m_editBtn = new QPushButton("변경 저장", this);
    m_editBtn->setStyleSheet("background-color: #4A90E2; color: white; padding: 5px; font-weight: bold;");
    m_editBtn->setEnabled(false); // 선택한 항목이 있을 때만 활성화

    m_deleteBtn = new QPushButton("삭제", this);
    m_deleteBtn->setStyleSheet("background-color: #E24A4A; color: white; padding: 5px; font-weight: bold;");
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
        
        // 아이템의 아이콘 대신 텍스트 색상으로 색깔 표시
        item->setForeground(QColor(cat["color"].toString()));
        item->setFont(QFont("Segoe UI", 10, QFont::Bold));
    }
    
    // 상태 초기화
    m_currentEditingId = -1;
    m_nameInput->clear();
    m_editBtn->setEnabled(false);
    m_deleteBtn->setEnabled(false);
}

void CategoryModifyWidget::selectColor() {
    QColor color = QColorDialog::getColor(QColor(m_selectedColor), this, "색상 선택");
    if (color.isValid()) {
        m_selectedColor = color.name();
        m_colorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #ccc;").arg(m_selectedColor));
    }
}

void CategoryModifyWidget::onItemSelected(QListWidgetItem *item) {
    if (!item) return;
    
    m_currentEditingId = item->data(Qt::UserRole).toInt();
    m_nameInput->setText(item->text());
    m_selectedColor = item->data(Qt::UserRole + 1).toString();
    m_colorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #ccc;").arg(m_selectedColor));
    
    m_editBtn->setEnabled(true);
    m_deleteBtn->setEnabled(true);
}

void CategoryModifyWidget::handleAdd() {
    QString name = m_nameInput->text().trimmed();
    if (name.isEmpty()) return;

    if (DatabaseManager::instance().addCategory(name, m_selectedColor)) {
        loadCategories();
        emit categoriesChanged();
    }
}

void CategoryModifyWidget::handleEdit() {
    if (m_currentEditingId == -1) return;
    
    QString name = m_nameInput->text().trimmed();
    if (name.isEmpty()) return;

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
