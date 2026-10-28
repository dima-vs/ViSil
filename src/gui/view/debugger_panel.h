#pragma once

#include <QWidget>
#include <QPushButton>
#include <QIcon>

namespace Gui
{

class DebuggerPanel : public QWidget
{
    Q_OBJECT

public:
    enum class PauseResumeMode
    {
        Pause,
        Resume
    };
private:
    QPushButton *m_runBtn;
    QPushButton *m_pauseResumeBtn;
    QPushButton *m_stepIntoBtn;

    bool m_isPaused = false;
    PauseResumeMode m_currentMode = PauseResumeMode::Pause;

    QIcon m_pauseIcon;
    QIcon m_resumeIcon;

    void initButtonAppearance();
    void initLayout();
public:
    DebuggerPanel(QWidget *parent = nullptr);

    void setRunEnabled(bool enabled);
    void setStepIntoEnabled(bool enabled);
    void setPauseResumeEnabled(bool enabled);

    void setPauseResumeMode(PauseResumeMode mode);
    PauseResumeMode getPauseResumeMode() const { return m_currentMode; }

private slots:
    void handlePauseResumeClicked();

signals:
    void runClicked();
    void pauseResumeClicked();
    void stepIntoClicked();
};

} // namespace Gui
