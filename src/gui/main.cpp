// qt
#include <QApplication>

// core
#include "devices/ram.h"
#include "arch/rv32/cpu.h"
#include "arch/rv32/register_file.h"
#include "core/simulation_runner.h"
#include "core/execution_env_interface.h"
#include "arch/rv32/rv32_disassembler.h"

// models
#include "gui/app_context/app_models.h"
#include "gui/model/register_file_model.h"
#include "gui/model/memory_instruction_model.h"
#include "gui/model/memory_word_model.h"
#include "gui/model/memory_block_model.h"

// views
#include "gui/view/theme_manager.h"
#include "gui/view/mainwindow.h"

// controllers
#include "gui/controller/execution_flow_controller.h"
#include "gui/controller/file_controller.h"

class MockEnvironment : public Core::IExecutionEnvironment
{
public:
    bool handleEnvCall(Core::ICpu& cpu) override { return false; }
};

Q_DECLARE_METATYPE(Core::DisassembledInstr);
Q_DECLARE_METATYPE(Core::ExecutionStatus);
Q_DECLARE_METATYPE(Core::StopReason);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<Core::ExecutionStatus>();
    qRegisterMetaType<Core::StopReason>();

    // resources and theme
    Q_INIT_RESOURCE(resources_styles);
    Q_INIT_RESOURCE(resources_icons);
    Gui::ThemeManager::instance().setTheme(Gui::ThemeManager::ThemeType::Dark);

    // init backend
    Devices::Ram ram(1, 64 * 1024, Devices::RamUninitBehavior::ZeroFill, Core::Endianness::Little);
    RV32::RegisterFile regFile;
    RV32::Cpu cpu(regFile, ram);

    MockEnvironment mockEnv;
    Core::SimulationRunner simRunner(mockEnv, cpu);
    RV32::RV32Disassembler rv32Disasm;

    // init models
    Gui::RegisterFileModel regModel(cpu);

    Gui::MemoryInstructionModel instrModel(simRunner, ram, rv32Disasm);

    instrModel.inspectBlock(0x00000000, ram.getSize());

    Gui::MemoryWordModel wordModel(ram);
    wordModel.setAddressWidth(Core::BitWidth::Bits32);
    wordModel.setDisplayWordWidth(Core::BitWidth::Bits32);
    wordModel.inspectBlock(0x00000000, ram.getSize());

    Gui::MemoryBlockModel blockModel(ram);

    Gui::AppModels appModels {
        regModel,
        instrModel,
        wordModel,
        blockModel
    };

    // init views
    Gui::MainWindow mainWindow;

    mainWindow.setRegisterModel(&regModel);
    mainWindow.setInstructionModel(&instrModel);
    mainWindow.setMemoryWordModel(&wordModel);

    // init controllers
    Gui::ExecutionFlowController execController(cpu, simRunner, appModels, mainWindow);
    Gui::FileController fileController(appModels, mainWindow, ram);

    execController.changeState(Gui::ExecutionFlowController::State::Unloaded);
    QObject::connect(&fileController, &Gui::FileController::fileLoaded,
        &execController, &Gui::ExecutionFlowController::onFileLoaded);

    mainWindow.show();

    return a.exec();
}
