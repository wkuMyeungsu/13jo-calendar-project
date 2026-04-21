#ifndef COLORPICKERPOPUP_H
#define COLORPICKERPOPUP_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QList>
#include <QString>

class ColorPickerPopup : public QWidget {
    Q_OBJECT
public:
    explicit ColorPickerPopup(QWidget *parent = nullptr);

signals:
    void colorSelected(const QString& color); // 색상 선택 시 발생

private slots:
    void handleColorClick();

private:
    void setupUI();
    const QList<QString> m_colors = {
        "#EF5350", "#EC407A", "#AB47BC", "#7E57C2", "#5C6BC0", 
        "#42A5F5", "#29B6F6", "#26C6DA", "#26A69A", "#66BB6A", 
        "#D4E157", "#FFEE58", "#FFCA28", "#FFA726", "#FF7043",
        "#8D6E63", "#BDBDBD", "#78909C", "#333333", "#4A90E2"
    };
};

#endif // COLORPICKERPOPUP_H
