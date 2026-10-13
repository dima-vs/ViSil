#include <QObject>
#include <QMap>
#include <QColor>

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
        MemBlock_CellText
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
