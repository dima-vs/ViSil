#include "debugger_panel.h"
#include <QHBoxLayout>
#include <QSize>

namespace Gui
{

DebuggerPanel::DebuggerPanel(QWidget *parent) :
    QWidget(parent),
    m_runBtn(new QPushButton("")),
    m_pauseResumeBtn(new QPushButton("")),
    m_stepIntoBtn(new QPushButton(""))
{
    setObjectName("DebuggerPanel");

    initButtonAppearance();
    initLayout();

    connect(m_runBtn, &QPushButton::clicked, this, &DebuggerPanel::runClicked);
    connect(m_pauseResumeBtn, &QPushButton::clicked, this, &DebuggerPanel::handlePauseResumeClicked);
    connect(m_stepIntoBtn, &QPushButton::clicked, this, &DebuggerPanel::stepIntoClicked);
}

void DebuggerPanel::initLayout()
{
    QHBoxLayout *btnLayout = new QHBoxLayout();

    btnLayout->addStretch();
    btnLayout->addWidget(m_runBtn);
    btnLayout->addWidget(m_pauseResumeBtn);
    btnLayout->addWidget(m_stepIntoBtn);
    btnLayout->addStretch();

    setLayout(btnLayout);
}

void DebuggerPanel::initButtonAppearance()
{
    m_runBtn->setObjectName("runBtn");
    m_pauseResumeBtn->setObjectName("pauseBtn");
    m_stepIntoBtn->setObjectName("stepIntoBtn");

    QIcon runIcon(":/icons/debug-panel/run96.png");
    m_pauseIcon = QIcon(":/icons/debug-panel/pause96.png");
    m_resumeIcon = QIcon(":/icons/debug-panel/resume96.png");
    QIcon stepIntoIcon(":/icons/debug-panel/step-into100.png");
    const int btnSize = 38;
    const int btnIconSize = 25;

    m_runBtn->setIcon(runIcon);
    m_pauseResumeBtn->setIcon(m_pauseIcon);
    m_stepIntoBtn->setIcon(stepIntoIcon);

    m_runBtn->setFixedSize(btnSize, btnSize);
    m_pauseResumeBtn->setFixedSize(btnSize, btnSize);
    m_stepIntoBtn->setFixedSize(btnSize, btnSize);

    m_runBtn->setIconSize(QSize(btnIconSize, btnIconSize));
    m_pauseResumeBtn->setIconSize(QSize(btnIconSize, btnIconSize));
    m_stepIntoBtn->setIconSize(QSize(btnIconSize, btnIconSize));

    m_runBtn->setToolTip("Run");
    m_pauseResumeBtn->setToolTip("Pause");
    m_stepIntoBtn->setToolTip("Step into");
}

void DebuggerPanel::setRunEnabled(bool enabled)
{
    m_runBtn->setEnabled(enabled);
}

void DebuggerPanel::setStepIntoEnabled(bool enabled)
{
    m_stepIntoBtn->setEnabled(enabled);
}

void DebuggerPanel::setPauseResumeEnabled(bool enabled)
{
    m_pauseResumeBtn->setEnabled(enabled);
}

void DebuggerPanel::setPauseResumeMode(PauseResumeMode mode)
{
    m_currentMode = mode;
    if (mode == PauseResumeMode::Pause)
    {
        m_pauseResumeBtn->setIcon(m_pauseIcon);
        m_pauseResumeBtn->setToolTip("Pause");
    }
    else
    {
        m_pauseResumeBtn->setIcon(m_resumeIcon);
        m_pauseResumeBtn->setToolTip("Resume");
    }
}

void DebuggerPanel::handlePauseResumeClicked()
{
    emit pauseResumeClicked();
}

} // namespace Gui
