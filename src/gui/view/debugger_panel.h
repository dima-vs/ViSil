#pragma once

#include <QWidget>
#include <QPushButton>
#include <QIcon>

namespace Gui
{

class DebuggerPanel : public QWidget
{
    Q_OBJECT

private:
    QPushButton *m_runBtn;
    QPushButton *m_pauseResumeBtn;
    QPushButton *m_stepIntoBtn;

    bool m_isPaused = false;

    QIcon m_pauseIcon;
    QIcon m_resumeIcon;

public:
    DebuggerPanel(QWidget *parent = nullptr);
    void initButtonAppearance();
    void initLayout();

private slots:
    void handlePauseResumeClicked();

signals:
    void runClicked();
    void pauseResumeClicked();
    void stepIntoClicked();
};

} // namespace Gui
