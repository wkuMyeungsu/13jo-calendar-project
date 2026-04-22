#include "CategoryFilterWidget.h"
#include <QVBoxLayout>
#include "StyleHelper.h"
#include "DatabaseManager.h"
#include "CategoryModifyWidget.h"

CategoryFilterWidget::CategoryFilterWidget(QWidget *parent) 
    : QWidget(parent), m_isExpanded(false) {
    
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // Main horizontal bar
    QWidget* mainBar = new QWidget(this);
    m_mainLayout = new QHBoxLayout(mainBar);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(6);
    m_mainLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    outerLayout->addWidget(mainBar);

    // Overflow grid panel
    m_overflowWidget = new QWidget(this);
    m_overflowLayout = new QGridLayout(m_overflowWidget);
    m_overflowLayout->setContentsMargins(10, 10, 10, 10);
    m_overflowLayout->setSpacing(8);
    m_overflowWidget->hide();
    outerLayout->addWidget(m_overflowWidget);

    m_moreBtn = new QPushButton("...", this);
    m_moreBtn->setCheckable(true);
    m_moreBtn->setFixedSize(30, 22);
    m_moreBtn->setCursor(Qt::PointingHandCursor);
    connect(m_moreBtn, &QPushButton::clicked, this, &CategoryFilterWidget::toggleOverflow);

    m_addBtn = new QPushButton("+", this);
    m_addBtn->setFixedSize(22, 22);
    m_addBtn->setCursor(Qt::PointingHandCursor);
    connect(m_addBtn, &QPushButton::clicked, this, &CategoryFilterWidget::openCategoryManager);

    refresh();
}

void CategoryFilterWidget::refresh() {
    // Clear existing
    QLayoutItem *item;
    while ((item = m_mainLayout->takeAt(0)) != nullptr) {
        if (item->widget() && item->widget() != m_moreBtn && item->widget() != m_addBtn) 
            item->widget()->deleteLater();
        delete item;
    }
    while ((item = m_overflowLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    auto categories = DatabaseManager::instance().getCategories();
    int count = 0;

    for (const auto& cat : categories) {
        if (!m_filters.contains(cat.id)) m_filters[cat.id] = true;

        QPushButton* btn = new QPushButton(cat.name, this);
        btn->setCheckable(true);
        btn->setChecked(m_filters[cat.id]);
        btn->setFixedHeight(24);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(StyleHelper::getCategoryPillStyle(cat.color, m_filters[cat.id]));

        connect(btn, &QPushButton::toggled, this, [this, cat, btn](bool checked) {
            m_filters[cat.id] = checked;
            btn->setStyleSheet(StyleHelper::getCategoryPillStyle(cat.color, checked));
            emit filterChanged();
        });

        if (count < 3) m_mainLayout->addWidget(btn);
        else m_overflowLayout->addWidget(btn, (count - 3) / 5, (count - 3) % 5, Qt::AlignLeft | Qt::AlignTop);
        count++;
    }

    m_overflowLayout->setColumnStretch(5, 1);

    if (count > 3) {
        m_mainLayout->addWidget(m_moreBtn);
        m_moreBtn->show();
        m_overflowWidget->setVisible(m_isExpanded);
    } else {
        m_moreBtn->hide();
        m_isExpanded = false;
        m_overflowWidget->hide();
    }
    m_mainLayout->addWidget(m_addBtn);
    
    updateStyle();
}

void CategoryFilterWidget::updateStyle() {
    m_addBtn->setStyleSheet(StyleHelper::getAddButtonStyle());
    m_moreBtn->setStyleSheet(StyleHelper::getMoreButtonStyle());
    m_overflowWidget->setStyleSheet(StyleHelper::getOverflowPanelStyle());
}

bool CategoryFilterWidget::isFilterActive(int categoryId) const {
    return m_filters.value(categoryId, true);
}

void CategoryFilterWidget::toggleOverflow() {
    m_isExpanded = !m_isExpanded;
    m_moreBtn->setChecked(m_isExpanded);
    m_overflowWidget->setVisible(m_isExpanded);
    emit filterChanged(); // Layout update for parent
}

void CategoryFilterWidget::openCategoryManager() {
    CategoryModifyWidget *w = new CategoryModifyWidget();
    w->setAttribute(Qt::WA_DeleteOnClose);
    connect(w, &CategoryModifyWidget::categoriesChanged, this, &CategoryFilterWidget::refresh);
    connect(w, &CategoryModifyWidget::categoriesChanged, this, &CategoryFilterWidget::filterChanged);
    w->show();
}
