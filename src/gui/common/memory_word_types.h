#pragma once

#include "common.h"
#include <QString>

namespace Gui
{

enum class MemWordColumn
{
    Address = 0,
    RawBytes,     // memory cell bytes
    NumericValue, // memory cell value in numeric format
    Count         // column values count
};

struct MemWordCellMetadata
{
    MemWordColumn column;
    Core::Endianness endianness;
    QString prefix;
};

Q_DECLARE_METATYPE(MemWordCellMetadata)

} // namespace Gui
