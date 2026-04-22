#ifndef CATEGORYMODIFYWIDGET_H
#define CATEGORYMODIFYWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QColor>
#include "CustomTitleBar.h"

class CategoryModifyWidget : public QWidget {
    Q_OBJECT
public:
    explicit CategoryModifyWidget(QWidget *parent = nullptr);

signals:
    void categoriesChanged(); // 카테고리 변경 시 발생 (달력 갱신용)

protected:
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    void loadCategories();
    void handleAdd();
    void handleEdit();
    void handleDelete();
    void selectColor();
    void onItemSelected(QListWidgetItem *item);
    void validateButtons(); // 버튼 활성화 상태 업데이트 헬퍼

private:
    QListWidget *m_listWidget;
    QLineEdit *m_nameInput;
    QPushButton *m_colorBtn;
    QPushButton *m_addBtn;
    QPushButton *m_editBtn;
    QPushButton *m_deleteBtn;
    
    QString m_selectedColor;
    QString m_originalName;  // 수정 시 원래 이름 백업
    QString m_originalColor; // 수정 시 원래 색상 백업
    int m_currentEditingId; // -1이면 추가 모드, 아니면 수정 모드

    CustomTitleBar *m_titleBar;
    QWidget *m_contentWidget;
};

#endif // CATEGORYMODIFYWIDGET_H
