#pragma once

#include <QObject>
#include <QMap>
#include <QColor>

namespace Gui
{

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum class ThemeType
    {
        Dark
    };

    enum class GuiElementColor
    {
        // Memory Block View
        MemBlock_SelectedRangeBorder,
        MemBlock_SelectedCellBackground,
        MemBlock_EvenCellBackground,
        MemBlock_OddCellBackground,
        MemBlock_CellBorder,
        MemBlock_SelectedCellBorder,
        MemBlock_CellText,

        // Memory Word View
        MemWord_SelectedRowBackground,
        MemWord_EvenRowBackground,
        MemWord_OddRowBackground,
        MemWord_HeaderSectionText,
        MemWord_HeaderSectionBackground,
        MemWord_HeaderBottomLine,
        MemWord_RawBytesCellText,
        MemWord_AddressCellText,
        MemWord_NumericValCellText,
        MemWord_DefaultCellText,

        // Register File View
        RegFile_CellBackground,
        RegFile_PcBackground,
        RegFile_SelectedBorderStart,
        RegFile_SelectedBorderMid,
        RegFile_SelectedBorderEnd,
        RegFile_PcBorderStart,
        RegFile_PcBorderMid,
        RegFile_PcBorderEnd,
        RegFile_NormalBorderStart,
        RegFile_NormalBorderMid,
        RegFile_NormalBorderEnd,
        RegFile_RegNameText,
        RegFile_ValuePrefixText,
        RegFile_ValueText,

        // Memory Instruction View
        MemInstr_InstrCellBackground,
        MemInstr_SelectedCellBackground,
        MemInstr_SidePanelCellBackground,
        MemInstr_AddrCodeSeparator,
        MemInstr_BreakPoint,
        MemInstr_BreakPointHover,
        MemInstr_AddressText,
        MemInstr_AsmTokenMnemonic,
        MemInstr_AsmTokenRegister,
        MemInstr_AsmTokenAddress,
        MemInstr_AsmTokenImmediate,
        MemInstr_AsmTokenPunctuation,
        MemInstr_AsmTokenIncorrect,
        MemInstr_AsmTokenDefault
    };

private:
    ThemeType m_currentTheme;
    QMap<GuiElementColor, QColor> m_colors;

    ThemeManager();
    void loadQssForTheme(ThemeType theme);
public:
    static ThemeManager& instance();

    void setTheme(ThemeType theme);
    QColor getColor(GuiElementColor element) const;
signals:
    void themeChanged();
};

} // namespace Gui
