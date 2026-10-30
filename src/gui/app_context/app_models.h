#pragma once

#include "gui/model/memory_block_model.h"
#include "gui/model/memory_instruction_model.h"
#include "gui/model/memory_word_model.h"
#include "gui/model/register_file_model.h"
#include "gui/model/devices/uart_console_model.h"

#include <QAbstractItemModel>

namespace Gui
{

struct AppModels
{
    RegisterFileModel& registers;
    MemoryInstructionModel& instructions;
    MemoryWordModel& memoryWords;
    MemoryBlockModel& memoryBlocks;
    UartModel& uartModel;

    void refreshAll() const
    {
        refreshCpu();
        refreshMemory();
    }

    void refreshCpu() const
    {
        refreshModel(registers);
    }

    void refreshMemory() const
    {
        refreshModel(instructions);
        refreshModel(memoryWords);
        refreshModel(memoryBlocks);
    }

private:
    static void refreshModel(QAbstractItemModel& model)
    {
        if (model.rowCount() > 0 && model.columnCount() > 0)
        {
            QModelIndex topLeft = model.index(0, 0);
            QModelIndex bottomRight = model.index(model.rowCount() - 1, model.columnCount() - 1);

            model.dataChanged(topLeft, bottomRight);
        }
    }
};

} // namespace Gui
