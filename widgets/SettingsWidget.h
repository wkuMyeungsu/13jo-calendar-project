#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>

class SettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget *parent = nullptr);

signals:
    void settingsChanged(); // 설정 변경 시 발생 (테마 갱신 등용)

private slots:
    void applySettings();
    void previewTheme(int index); // 테마 미리보기 슬롯
    void handleReset();           // 데이터 초기화 슬롯

private:
    void updateFormStyle(); // 폼 전체 스타일 갱신 헬퍼
    QComboBox *m_themeCombo;
    QSpinBox  *m_deadlineSpin;
    QPushButton *m_applyBtn;
    QPushButton *m_resetBtn;
};

#endif // SETTINGSWIDGET_H
