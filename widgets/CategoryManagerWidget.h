#ifndef CATEGORYMANAGERWIDGET_H
#define CATEGORYMANAGERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QColorDialog>

class CategoryManagerWidget : public QWidget {
    Q_OBJECT
public:
    explicit CategoryManagerWidget(QWidget *parent = nullptr);

signals:
    void categoriesChanged();

private slots:
    void loadCategories();
    void addCategory();
    void deleteCategory();
    void pickColor();

private:
    QListWidget *m_listWidget;
    QLineEdit *m_nameInput;
    QPushButton *m_colorBtn;
    QString m_selectedColor;
};

#endif // CATEGORYMANAGERWIDGET_H
