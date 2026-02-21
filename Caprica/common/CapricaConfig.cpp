#include "CapricaConfig.h"
#include <common/CapricaConfig.h>
#include <common/FSUtils.h>
#include <filesystem>
namespace caprica { namespace conf {

// These should always be defaulted to false/empty, and their real
// default values set in the command line parsing.

namespace General {
  bool compileInParallel{ false };
  bool quietCompile{ false };
  bool recursive { false };
  std::filesystem::path outputDirectory;
  bool anonymizeOutput;
  std::vector<std::shared_ptr<IInputFile>> inputFiles;
  }

  namespace PCompiler {
  // pCompiler compatibility mode.
  bool pCompilerCompatibilityMode{false};
  bool all{false};
  bool norecurse{false};
}

namespace CodeGeneration {
  bool disableBetaCode{ false };
  bool disableDebugCode{ false };
  bool enableCKOptimizations{ false };
  bool enableOptimizations{ false };
  bool emitDebugInfo{ false };
}

namespace Debug {
  bool debugControlFlowGraph{ false };
  bool dumpPexAsm{ false };
}

namespace EngineLimits {
  bool ignoreLimits{ false };
  size_t maxArrayLength{ 0 };
  size_t maxFunctionsInEmptyStatePerObject{ 0 };
  size_t maxFunctionsPerState{ 0 };
  size_t maxGuardsPerObject{ 0 };
  size_t maxInitialValuesPerObject{ 0 };
  size_t maxNamedStatesPerObject{ 0 };
  size_t maxParametersPerFunction{ 0 };
  size_t maxPropertiesPerObject{ 0 };
  size_t maxStaticFunctionsPerObject{ 0 };
  size_t maxUserFlags{ 0 };
  size_t maxVariablesPerObject{ 0 };
}

namespace Papyrus {
GameID game { GameID::UNKNOWN };
  bool allowCompilerIdentifiers { false };
  bool allowDecompiledStructNameRefs{ false };
  bool allowNegativeLiteralAsBinaryOp{ false };
  bool enableLanguageExtensions{ false };
  bool ignorePropertyNameLocalConflicts{ false };
  bool allowImplicitNoneCastsToAnyType{ false };
  std::vector<ImportDir> importDirectories {};
  CapricaUserFlagsDefinition userFlagsDefinition{};
}

namespace Skyrim {
  bool skyrimAllowUnknownEventsOnNonNativeClass{ true };
  bool skyrimAllowObjectVariableShadowingParentProperty{ true };
  bool skyrimAllowLocalVariableShadowingParentProperty{ true };
  bool skyrimAllowLocalUseBeforeDeclaration{ true };
  bool skyrimAllowAssigningVoidMethodCallResult{ true };
}

namespace Performance {
  bool asyncFileRead{ false };
  bool asyncFileWrite{ false };
  bool dumpTiming{ false };
  bool performanceTestMode{ false };
  bool resolveSymlinks{ false };
}

namespace Warnings {
  bool disableAllWarnings{ false };
  bool treatWarningsAsErrors{ false };
  std::unordered_set<size_t> warningsToHandleAsErrors{ };
  std::unordered_set<size_t> warningsToIgnore{ };
  std::unordered_set<size_t> warningsToEnable{ };
}

void resetConfig() {
    // General
    General::compileInParallel = false;
    General::quietCompile = false;
    General::recursive = false;
    General::outputDirectory.clear();
    General::anonymizeOutput = false;
    General::inputFiles.clear();

    // PCompiler
    PCompiler::pCompilerCompatibilityMode = false;
    PCompiler::all = false;
    PCompiler::norecurse = false;

    // CodeGeneration
    CodeGeneration::disableBetaCode = false;
    CodeGeneration::disableDebugCode = false;
    CodeGeneration::enableCKOptimizations = false;
    CodeGeneration::enableOptimizations = false;
    CodeGeneration::emitDebugInfo = false;

    // Debug
    Debug::debugControlFlowGraph = false;
    Debug::dumpPexAsm = false;

    // EngineLimits
    EngineLimits::ignoreLimits = false;
    EngineLimits::maxArrayLength = 0;
    EngineLimits::maxFunctionsInEmptyStatePerObject = 0;
    EngineLimits::maxFunctionsPerState = 0;
    EngineLimits::maxGuardsPerObject = 0;
    EngineLimits::maxInitialValuesPerObject = 0;
    EngineLimits::maxNamedStatesPerObject = 0;
    EngineLimits::maxParametersPerFunction = 0;
    EngineLimits::maxPropertiesPerObject = 0;
    EngineLimits::maxStaticFunctionsPerObject = 0;
    EngineLimits::maxUserFlags = 0;
    EngineLimits::maxVariablesPerObject = 0;

    // Papyrus
    Papyrus::game = GameID::UNKNOWN;
    Papyrus::allowCompilerIdentifiers = false;
    Papyrus::allowDecompiledStructNameRefs = false;
    Papyrus::allowNegativeLiteralAsBinaryOp = false;
    Papyrus::enableLanguageExtensions = false;
    Papyrus::ignorePropertyNameLocalConflicts = false;
    Papyrus::allowImplicitNoneCastsToAnyType = false;
    Papyrus::importDirectories.clear();
    Papyrus::userFlagsDefinition.clear();

    // Skyrim
    Skyrim::skyrimAllowUnknownEventsOnNonNativeClass = true;
    Skyrim::skyrimAllowObjectVariableShadowingParentProperty = true;
    Skyrim::skyrimAllowLocalVariableShadowingParentProperty = true;
    Skyrim::skyrimAllowLocalUseBeforeDeclaration = true;
    Skyrim::skyrimAllowAssigningVoidMethodCallResult = true;

    // Performance
    Performance::asyncFileRead = false;
    Performance::asyncFileWrite = false;
    Performance::dumpTiming = false;
    Performance::performanceTestMode = false;
    Performance::resolveSymlinks = false;

    // Warnings
    Warnings::disableAllWarnings = false;
    Warnings::treatWarningsAsErrors = false;
    Warnings::warningsToHandleAsErrors.clear();
    Warnings::warningsToIgnore.clear();
    Warnings::warningsToEnable.clear();
}

}}
