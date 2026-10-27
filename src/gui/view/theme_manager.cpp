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
        // Memory Block View
        m_colors[GuiElementColor::MemBlock_SelectedRangeBorder] = QColor("#38BDF8");
        m_colors[GuiElementColor::MemBlock_SelectedCellBackground] = QColor("#1E283D");
        m_colors[GuiElementColor::MemBlock_EvenCellBackground] = QColor("#13161F");
        m_colors[GuiElementColor::MemBlock_OddCellBackground] = QColor("#161A24");
        m_colors[GuiElementColor::MemBlock_CellBorder] = QColor("#232838");
        m_colors[GuiElementColor::MemBlock_SelectedCellBorder] = QColor("#38BDF8");
        m_colors[GuiElementColor::MemBlock_CellText] = QColor("#E2E8F0");

        // Memory Word View
        m_colors[GuiElementColor::MemWord_SelectedRowBackground] = QColor("#1E283D");
        m_colors[GuiElementColor::MemWord_EvenRowBackground] = QColor("#13161F");
        m_colors[GuiElementColor::MemWord_OddRowBackground] = QColor("#161A24");
        m_colors[GuiElementColor::MemWord_HeaderSectionBackground] = QColor("#0D0F14");
        m_colors[GuiElementColor::MemWord_HeaderSectionText] = QColor("#8A91A8");
        m_colors[GuiElementColor::MemWord_HeaderBottomLine] = QColor("#232838");
        m_colors[GuiElementColor::MemWord_RawBytesCellText] = QColor("#F472B6");
        m_colors[GuiElementColor::MemWord_AddressCellText] = QColor("#4ADE80");
        m_colors[GuiElementColor::MemWord_NumericValCellText] = QColor("#38BDF8");
        m_colors[GuiElementColor::MemWord_DefaultCellText] = QColor("#E2E8F0");

        // Register File View
        m_colors[GuiElementColor::RegFile_CellBackground] = QColor("#161922");
        m_colors[GuiElementColor::RegFile_PcBackground] = QColor("#1E2438");

        m_colors[GuiElementColor::RegFile_SelectedBorderStart] = QColor("#38BDF8");
        m_colors[GuiElementColor::RegFile_SelectedBorderMid] = QColor("#0284C7");
        m_colors[GuiElementColor::RegFile_SelectedBorderEnd] = QColor("#0369A1");

        m_colors[GuiElementColor::RegFile_PcBorderStart] = QColor("#4E5775");
        m_colors[GuiElementColor::RegFile_PcBorderMid] = QColor("#3B4259");
        m_colors[GuiElementColor::RegFile_PcBorderEnd] = QColor("#232838");

        m_colors[GuiElementColor::RegFile_NormalBorderStart] = QColor("#232838");
        m_colors[GuiElementColor::RegFile_NormalBorderMid] = QColor("#232838");
        m_colors[GuiElementColor::RegFile_NormalBorderEnd] = QColor("#232838");

        m_colors[GuiElementColor::RegFile_RegNameText] = QColor("#8A91A8");
        m_colors[GuiElementColor::RegFile_ValueText] = QColor("#60A5FA");
        QColor prefixColor = QColor("#60A5FA");
        prefixColor.setAlpha(120);
        m_colors[GuiElementColor::RegFile_ValuePrefixText] = prefixColor;

        // Memory Instruction View
        m_colors[GuiElementColor::MemInstr_InstrCellBackground] = QColor("#13161F");
        m_colors[GuiElementColor::MemInstr_CurrentPcCellBackground] = QColor("#2D2D22");
        m_colors[GuiElementColor::MemInstr_SelectedCellBackground] = QColor("#1E283D");
        m_colors[GuiElementColor::MemInstr_SidePanelCellBackground] = QColor("#0D0F14");
        m_colors[GuiElementColor::MemInstr_AddrCodeSeparator] = QColor("#232838");
        m_colors[GuiElementColor::MemInstr_BreakPoint] = QColor("#F43F5E");
        m_colors[GuiElementColor::MemInstr_BreakPointHover] = QColor("#64F43F5E");
        m_colors[GuiElementColor::MemInstr_AddressText] = QColor("#555D73");

        m_colors[GuiElementColor::MemInstr_AsmTokenMnemonic] = QColor("#FBBF24");
        m_colors[GuiElementColor::MemInstr_AsmTokenRegister] = QColor("#60A5FA");
        m_colors[GuiElementColor::MemInstr_AsmTokenAddress] = QColor("#F472B6");
        m_colors[GuiElementColor::MemInstr_AsmTokenImmediate] = QColor("#2DD4BF");
        m_colors[GuiElementColor::MemInstr_AsmTokenPunctuation] = QColor("#64748B");
        m_colors[GuiElementColor::MemInstr_AsmTokenIncorrect] = QColor("#8A91A8");
        m_colors[GuiElementColor::MemInstr_AsmTokenDefault] = QColor("#E2E8F0");
    }

    loadQssForTheme(theme);
    emit themeChanged();
}

QColor ThemeManager::getColor(GuiElementColor element) const
{
    return m_colors.value(element, QColor(Qt::magenta));
}

} // namespace Gui
