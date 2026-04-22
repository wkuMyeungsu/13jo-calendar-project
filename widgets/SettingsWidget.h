#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include "CustomTitleBar.h"

class SettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget *parent = nullptr);

signals:
    void settingsChanged(); // 설정 변경 시 발생 (테마 갱신 등용)

protected:
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void applySettings();
    void previewTheme(int index); // 테마 미리보기 슬롯
    void handleReset();           // 데이터 초기화 슬롯
    void handleCloseRequested();   // X 버튼 클릭 시 호출

private:
    void updateFormStyle(); // 폼 전체 스타일 갱신 헬퍼
    QComboBox *m_themeCombo;
    QPushButton *m_applyBtn;
    QPushButton *m_resetBtn;

    CustomTitleBar *m_titleBar;
    QFrame *m_mainFrame;
    QWidget *m_contentWidget;

    int m_initialTheme; // 처음 열렸을 때의 테마 저장
    bool m_isApplied = false; // 적용 버튼을 눌렀는지 여부
};

#endif // SETTINGSWIDGET_H
