#include "common.h"
#include <QString>

enum class Column
{
    Address = 0,
    RawBytes,     // memory cell bytes
    NumericValue, // memory cell value in numeric format
    Count         // column values count
};

struct CellMetadata
{
    Column column;
    Core::Endianness endianness;
    QString prefix;
};

Q_DECLARE_METATYPE(CellMetadata)
