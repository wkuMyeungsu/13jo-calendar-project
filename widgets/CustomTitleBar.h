#ifndef CUSTOMTITLEBAR_H
#define CUSTOMTITLEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPoint>

class CustomTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit CustomTitleBar(QWidget* parent = nullptr);
    void applyTheme(const QString& bgColor, const QString& textColor, const QString& borderColor);
    void updateMaxIcon();
    bool eventFilter(QObject* watched, QEvent* event) override;
    void setResizable(bool resizable) { m_isResizable = resizable; }

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QLabel*      m_titleLabel;
    QPushButton* m_minBtn;
    QPushButton* m_maxBtn;
    QPushButton* m_closeBtn;

    bool   m_dragging = false;
    QPoint m_dragStartPos;

    bool   m_isResizable = true; // 기본값 true로 설정 (MainWindow에서 사용하므로)
    bool   m_resizing = false;
    QPoint m_resizeStartPos;
    QRect  m_resizeStartGeometry;
    static const int RESIZE_MARGIN = 10;
};

#endif // CUSTOMTITLEBAR_H
