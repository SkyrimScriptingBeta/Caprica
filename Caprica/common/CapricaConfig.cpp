#include "CapricaConfig.h"
#include <common/CapricaConfig.h>
#include <common/FSUtils.h>
#include <filesystem>
namespace caprica { namespace conf {

// These should always be defaulted to false/empty, and their real
// default values set in the command line parsing.

namespace General {
  thread_local bool compileInParallel{ false };
  thread_local bool quietCompile{ false };
  thread_local bool recursive { false };
  thread_local std::filesystem::path outputDirectory;
  thread_local bool anonymizeOutput;
  thread_local std::vector<std::shared_ptr<IInputFile>> inputFiles;
  }

  namespace PCompiler {
  // pCompiler compatibility mode.
  thread_local bool pCompilerCompatibilityMode{false};
  thread_local bool all{false};
  thread_local bool norecurse{false};
}

namespace CodeGeneration {
  thread_local bool disableBetaCode{ false };
  thread_local bool disableDebugCode{ false };
  thread_local bool enableCKOptimizations{ false };
  thread_local bool enableOptimizations{ false };
  thread_local bool emitDebugInfo{ false };
}

namespace Debug {
  thread_local bool debugControlFlowGraph{ false };
  thread_local bool dumpPexAsm{ false };
}

namespace EngineLimits {
  thread_local bool ignoreLimits{ false };
  thread_local size_t maxArrayLength{ 0 };
  thread_local size_t maxFunctionsInEmptyStatePerObject{ 0 };
  thread_local size_t maxFunctionsPerState{ 0 };
  thread_local size_t maxGuardsPerObject{ 0 };
  thread_local size_t maxInitialValuesPerObject{ 0 };
  thread_local size_t maxNamedStatesPerObject{ 0 };
  thread_local size_t maxParametersPerFunction{ 0 };
  thread_local size_t maxPropertiesPerObject{ 0 };
  thread_local size_t maxStaticFunctionsPerObject{ 0 };
  thread_local size_t maxUserFlags{ 0 };
  thread_local size_t maxVariablesPerObject{ 0 };
}

namespace Papyrus {
thread_local GameID game { GameID::UNKNOWN };
  thread_local bool allowCompilerIdentifiers { false };
  thread_local bool allowDecompiledStructNameRefs{ false };
  thread_local bool allowNegativeLiteralAsBinaryOp{ false };
  thread_local bool enableLanguageExtensions{ false };
  thread_local bool ignorePropertyNameLocalConflicts{ false };
  thread_local bool allowImplicitNoneCastsToAnyType{ false };
  thread_local std::vector<ImportDir> importDirectories {};
  thread_local CapricaUserFlagsDefinition userFlagsDefinition{};
}

namespace Skyrim {
  thread_local bool skyrimAllowUnknownEventsOnNonNativeClass{ true };
  thread_local bool skyrimAllowObjectVariableShadowingParentProperty{ true };
  thread_local bool skyrimAllowLocalVariableShadowingParentProperty{ true };
  thread_local bool skyrimAllowLocalUseBeforeDeclaration{ true };
  thread_local bool skyrimAllowAssigningVoidMethodCallResult{ true };
}

namespace Performance {
  thread_local bool asyncFileRead{ false };
  thread_local bool asyncFileWrite{ false };
  thread_local bool dumpTiming{ false };
  thread_local bool performanceTestMode{ false };
  thread_local bool resolveSymlinks{ false };
}

namespace Warnings {
  thread_local bool disableAllWarnings{ false };
  thread_local bool treatWarningsAsErrors{ false };
  thread_local std::unordered_set<size_t> warningsToHandleAsErrors{ };
  thread_local std::unordered_set<size_t> warningsToIgnore{ };
  thread_local std::unordered_set<size_t> warningsToEnable{ };
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
