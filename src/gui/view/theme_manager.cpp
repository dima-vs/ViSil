#include "theme_manager.h"
#include <QFile>
#include <QApplication>
#include <QLatin1String>

ThemeManager::ThemeManager()
{
    setTheme(ThemeType::Dark);
}

void ThemeManager::loadQssForTheme(ThemeType theme)
{
    QString filePath;
    if (theme == ThemeType::Dark)
        filePath = ":/styles/dark.qss";

    QFile file(filePath);
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    }
}

ThemeManager& ThemeManager::instance()
{
    static ThemeManager instance;
    return instance;
}

void ThemeManager::setTheme(ThemeType theme)
{
    m_currentTheme = theme;
    m_colors.clear();

    if (theme == ThemeType::Dark)
    {
        m_colors[GuiElementColor::MemBlock_SelectedRangeBorder] = QColor("#38BDF8");
        m_colors[GuiElementColor::MemBlock_SelectedCellBackground] = QColor("#1E385B");
        m_colors[GuiElementColor::MemBlock_EvenCellBackground] = QColor("#1A1D24");
        m_colors[GuiElementColor::MemBlock_OddCellBackground] = QColor("#1F232C");
        m_colors[GuiElementColor::MemBlock_CellBorder] = QColor("#2D313E");
        m_colors[GuiElementColor::MemBlock_SelectedCellBorder] = QColor("#2A4B7C");
        m_colors[GuiElementColor::MemBlock_CellText] = QColor("#ffffff");
    }

    loadQssForTheme(theme);
    emit themeChanged();
}

QColor ThemeManager::getColor(GuiElementColor element) const
{
    return m_colors.value(element, QColor(Qt::magenta));
}
