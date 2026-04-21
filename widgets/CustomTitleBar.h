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
    void setTitle(const QString& title);
    void showMinMaxButtons(bool show); // 최소화/최대화 버튼 표시 제어
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
    void resizeEvent(QResizeEvent* event) override; // 절대 중앙 정렬을 위한 리사이즈 이벤트

private:
    QLabel*      m_titleLabel;
    QLabel*      m_centerTitleLabel; // 중앙 타이틀용
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
