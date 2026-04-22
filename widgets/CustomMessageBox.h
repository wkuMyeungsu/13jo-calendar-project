#ifndef CUSTOMMESSAGEBOX_H
#define CUSTOMMESSAGEBOX_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "CustomTitleBar.h"
#include "UiCommon.h"

class CustomMessageBox : public QDialog {
    Q_OBJECT

public:
    enum IconType { Information, Warning, Critical, Question };

    explicit CustomMessageBox(QWidget* parent = nullptr);
    
    void setMessage(const QString& title, const QString& text, IconType iconType = Information);
    void setButtons(QDialogButtonBox::StandardButtons buttons);

    static bool information(QWidget* parent, const QString& title, const QString& text);
    static bool warning(QWidget* parent, const QString& title, const QString& text);
    static bool critical(QWidget* parent, const QString& title, const QString& text);
    static bool question(QWidget* parent, const QString& title, const QString& text);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void initUi();
    void applyStyles();

    CustomTitleBar* m_titleBar;
    QLabel*         m_iconLabel;
    QLabel*         m_textLabel;
    QHBoxLayout*    m_buttonLayout;
    
    QString m_primaryColor;
};

#endif // CUSTOMMESSAGEBOX_H
