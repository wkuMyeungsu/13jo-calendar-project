#ifndef CATEGORYFILTERWIDGET_H
#define CATEGORYFILTERWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QMap>

class CategoryFilterWidget : public QWidget {
    Q_OBJECT
public:
    explicit CategoryFilterWidget(QWidget *parent = nullptr);
    void refresh();
    void updateStyle();
    bool isFilterActive(int categoryId) const;

signals:
    void filterChanged();

private slots:
    void toggleOverflow();
    void openCategoryManager();

private:
    QHBoxLayout* m_mainLayout;
    QGridLayout* m_overflowLayout;
    QWidget*     m_overflowWidget;
    QPushButton* m_moreBtn;
    QPushButton* m_addBtn;
    
    QMap<int, bool> m_filters;
    bool m_isExpanded;
};

#endif // CATEGORYFILTERWIDGET_H
