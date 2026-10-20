#include <cstdint>

namespace Gui
{

enum class RegisterFileRole
{
    RegisterNameRole = Qt::UserRole + 1,
    RegisterValueRole,
    ValuePrefixRole
};

enum class RegNumericFormat : uint8_t
{
    Hex,
    UnsignedDec,
    SignedDec
};

} // namespace Gui
