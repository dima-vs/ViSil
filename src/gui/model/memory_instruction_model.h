#pragma once

#include <QObject>
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>

#include <cstdint>
#include "bus_device_interface.h"
#include "disassembler_interface.h"
#include "simulation_runner.h"

namespace Gui
{

class MemoryInstructionModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    const Core::SimulationRunner &m_simRunner;
    const Core::IBusDevice &m_device;
    const Core::IDisassembler &m_disassembler;

    uint64_t m_currentPc = 0;
    int m_wordWidth = 4;
    uint64_t m_blockBaseAddress = 0;
    uint64_t m_blockSize = 0;

    void replaceIllegalTokensWithBytes(
        Core::DisassembledInstr& instr,
        uint64_t rawBytes) const;
public:
    explicit MemoryInstructionModel(
        const Core::SimulationRunner &simRunner,
        const Core::IBusDevice &device,
        const Core::IDisassembler &disassembler,
        QObject *parent = nullptr
        );

    void setCurrentPc(uint64_t pc);
    uint64_t getCurrentPc() const { return m_currentPc; }
    int getRowByAddress(uint64_t address) const;

    void inspectBlock(uint64_t baseAddress, uint64_t blockSize);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
};

} // namespace Gui
