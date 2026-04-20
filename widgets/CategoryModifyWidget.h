#ifndef CATEGORYMODIFYWIDGET_H
#define CATEGORYMODIFYWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QColor>

class CategoryModifyWidget : public QWidget {
    Q_OBJECT
public:
    explicit CategoryModifyWidget(QWidget *parent = nullptr);

signals:
    void categoriesChanged(); // 카테고리 변경 시 발생 (달력 갱신용)

private slots:
    void loadCategories();
    void handleAdd();
    void handleEdit();
    void handleDelete();
    void selectColor();
    void onItemSelected(QListWidgetItem *item);

private:
    QListWidget *m_listWidget;
    QLineEdit *m_nameInput;
    QPushButton *m_colorBtn;
    QPushButton *m_addBtn;
    QPushButton *m_editBtn;
    QPushButton *m_deleteBtn;
    
    QString m_selectedColor;
    int m_currentEditingId; // -1이면 추가 모드, 아니면 수정 모드
};

#endif // CATEGORYMODIFYWIDGET_H
