#include "theme_manager.h"
#include <QFile>
#include <QApplication>
#include <QLatin1String>

namespace Gui
{

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
        m_colors[GuiElementColor::MemBlock_CellText] = QColor("#FFFFFF");

        m_colors[GuiElementColor::MemWord_SelectedRowBackground] = QColor("#1E385B");
        m_colors[GuiElementColor::MemWord_EvenRowBackground] = QColor("#1A1D24");
        m_colors[GuiElementColor::MemWord_OddRowBackground] = QColor("#1F232C");
        m_colors[GuiElementColor::MemWord_HeaderSectionBackground] = QColor("#1E222D");
        m_colors[GuiElementColor::MemWord_HeaderSectionText] = QColor("#A39BB4");
        m_colors[GuiElementColor::MemWord_HeaderBottomLine] = QColor("#3A005E");
        m_colors[GuiElementColor::MemWord_RawBytesCellText] = QColor("#FF1493");
        m_colors[GuiElementColor::MemWord_AddressCellText] = QColor("#52E562");
        m_colors[GuiElementColor::MemWord_NumericValCellText] = QColor("#5CE1E6");
        m_colors[GuiElementColor::MemWord_DefaultCellText] = QColor("#F3F4F6");

        m_colors[GuiElementColor::RegFile_CellBackground] = QColor("#14171D");
        m_colors[GuiElementColor::RegFile_PcBackground] = QColor("#0E1014");
        m_colors[GuiElementColor::RegFile_SelectedBorderStart] = QColor("#3B82F6");
        m_colors[GuiElementColor::RegFile_SelectedBorderMid] = QColor("#1D4ED8");
        m_colors[GuiElementColor::RegFile_SelectedBorderEnd] = QColor("#4C1D95");
        m_colors[GuiElementColor::RegFile_PcBorderStart] = QColor("#4F5666");
        m_colors[GuiElementColor::RegFile_PcBorderMid] = QColor("#2D333F");
        m_colors[GuiElementColor::RegFile_PcBorderEnd] = QColor("#1A1D24");
        m_colors[GuiElementColor::RegFile_NormalBorderStart] = QColor("#2C313C");
        m_colors[GuiElementColor::RegFile_NormalBorderMid] = QColor("#20242D");
        m_colors[GuiElementColor::RegFile_NormalBorderEnd] = QColor("#16191F");
        m_colors[GuiElementColor::RegFile_RegNameText] = QColor("#D4D4D4");
        m_colors[GuiElementColor::RegFile_ValueText] = QColor("#569CD6");
        QColor prefixColor = m_colors[GuiElementColor::RegFile_ValueText];
        prefixColor.setAlpha(100);
        m_colors[GuiElementColor::RegFile_ValuePrefixText] = prefixColor;
    }

    loadQssForTheme(theme);
    emit themeChanged();
}

QColor ThemeManager::getColor(GuiElementColor element) const
{
    return m_colors.value(element, QColor(Qt::magenta));
}

} // namespace Gui
