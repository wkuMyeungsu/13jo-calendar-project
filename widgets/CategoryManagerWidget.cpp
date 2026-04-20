#include "CategoryManagerWidget.h"
#include "../models/DatabaseManager.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QVariantMap>

CategoryManagerWidget::CategoryManagerWidget(QWidget *parent) : QWidget(parent) {
    setWindowTitle("카테고리 관리");
    setFixedSize(300, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    m_listWidget = new QListWidget(this);
    mainLayout->addWidget(new QLabel("카테고리 목록:", this));
    mainLayout->addWidget(m_listWidget);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    m_nameInput = new QLineEdit(this);
    m_nameInput->setPlaceholderText("새 카테고리 이름");
    
    m_selectedColor = "#4A90E2";
    m_colorBtn = new QPushButton(this);
    m_colorBtn->setFixedWidth(30);
    m_colorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid gray;").arg(m_selectedColor));

    QPushButton *addBtn = new QPushButton("추가", this);
    
    inputLayout->addWidget(m_nameInput);
    inputLayout->addWidget(m_colorBtn);
    inputLayout->addWidget(addBtn);
    mainLayout->addLayout(inputLayout);

    QPushButton *delBtn = new QPushButton("선택 항목 삭제", this);
    delBtn->setStyleSheet("background-color: #E24A4A; color: white;");
    mainLayout->addWidget(delBtn);

    connect(m_colorBtn, &QPushButton::clicked, this, &CategoryManagerWidget::pickColor);
    connect(addBtn, &QPushButton::clicked, this, &CategoryManagerWidget::addCategory);
    connect(delBtn, &QPushButton::clicked, this, &CategoryManagerWidget::deleteCategory);

    loadCategories();
}

void CategoryManagerWidget::loadCategories() {
    m_listWidget->clear();
    QList<QVariantMap> categories = DatabaseManager::instance().getCategories();
    for (const auto& cat : categories) {
        QListWidgetItem *item = new QListWidgetItem(cat["name"].toString(), m_listWidget);
        item->setData(Qt::UserRole, cat["id"]);
        item->setForeground(QColor(cat["color"].toString()));
    }
}

void CategoryManagerWidget::pickColor() {
    QColor color = QColorDialog::getColor(QColor(m_selectedColor), this, "색상 선택");
    if (color.isValid()) {
        m_selectedColor = color.name();
        m_colorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid gray;").arg(m_selectedColor));
    }
}

void CategoryManagerWidget::addCategory() {
    QString name = m_nameInput->text().trimmed();
    if (name.isEmpty()) return;

    if (DatabaseManager::instance().addCategory(name, m_selectedColor)) {
        m_nameInput->clear();
        loadCategories();
        emit categoriesChanged();
    }
}

void CategoryManagerWidget::deleteCategory() {
    QListWidgetItem *item = m_listWidget->currentItem();
    if (!item) return;

    int id = item->data(Qt::UserRole).toInt();
    if (QMessageBox::question(this, "삭제", "이 카테고리를 삭제하시겠습니까?\n(해당 카테고리의 일정은 미지정으로 변경됩니다.)") == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteCategory(id)) {
            loadCategories();
            emit categoriesChanged();
        }
    }
}
