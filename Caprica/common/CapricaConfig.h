#pragma once

#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

#include "GameID.h"
#include <common/CapricaUserFlagsDefinition.h>
#include <common/parser/PapyrusProject.h>
#include <common/FSUtils.h>
#include <filesystem>
#include <common/CapricaInputFile.h>
namespace caprica { namespace conf {

// Options that don't fit in any other category.
namespace General {
  // If true, when compiling multiple files, do so
  // in multiple threads.
  extern thread_local bool compileInParallel;
  // If true, only report failures, not progress.
  extern thread_local bool quietCompile;
  // If true, recurse into subdirectories when compiling.
  extern thread_local bool recursive;
  // self-explanatory
  extern thread_local std::filesystem::path outputDirectory;
  // If true, remove identifying information from the header.
  extern thread_local bool anonymizeOutput;
  // input files
  extern thread_local std::vector<std::shared_ptr<IInputFile>> inputFiles;
}

// options related to compatibility with PCompiler's CLI parsing and name resolution
namespace PCompiler {
    // pCompiler compatibility mode.
    extern thread_local bool pCompilerCompatibilityMode;
    extern thread_local bool all;
    extern thread_local bool norecurse;
}

// Options related to code generation.
namespace CodeGeneration {
  // If true, don't generate calls to BetaOnly functions.
  extern thread_local bool disableBetaCode;
  // If true, don't generate calls to DebugOnly functions.
  extern thread_local bool disableDebugCode;
  // Enable optimizations that are done regardless of if the -optimize
  // switch is passed to the CK compiler.
  extern thread_local bool enableCKOptimizations;
  // Enable optimizations normally enabled by the -optimize switch to the
  // CK compiler.
  extern thread_local bool enableOptimizations;
  // If true, emit debug info for the papyrus script.
  extern thread_local bool emitDebugInfo;
}

// Options related to debugging Caprica itself.
namespace Debug {
  // If true, output the control flow graph of every function in the
  // files being compiled to stdout.
  extern thread_local bool debugControlFlowGraph;
  // If true, dump the Asm representation of the Pex file generated
  // for the Papyrus scripts being compiled.
  extern thread_local bool dumpPexAsm;
}

// Limitations of the game engine, not of Caprica.
namespace EngineLimits {
  // If true, warn when the limits are exceeded, but allow compilation to continue anyways.
  extern thread_local bool ignoreLimits;
  // The maximum length of an array. 0 means no limit.
  extern thread_local size_t maxArrayLength;
  // The maximum number of functions in the empty state in a single object. 0 means no limit.
  extern thread_local size_t maxFunctionsInEmptyStatePerObject;
  // The maximum number of functions in a single state. 0 means no limit.
  extern thread_local size_t maxFunctionsPerState;
  // The maximum number of variables in a single object that can have initial values. 0 means no limit.
  extern thread_local size_t maxInitialValuesPerObject;
  // The maximum number of named states in a single object. 0 means no limit.
  extern thread_local size_t maxNamedStatesPerObject;
  // The maximum number of parameters to a single function. 0 means no limit.
  extern thread_local size_t maxParametersPerFunction;
  // The maximum number of properties in a single object. 0 means no limit.
  extern thread_local size_t maxPropertiesPerObject;
  // The maximum number of global functions allowed in a single object. 0 means no limit.
  extern thread_local size_t maxStaticFunctionsPerObject;
  // The maximum number of distinct user flags allowed. Composite flags do not count toward this limit.
  extern thread_local size_t maxUserFlags;
  // The maximum number of variables in a single object. 0 means no limit.
  extern thread_local size_t maxVariablesPerObject;
  // The maximum number of guards in a single object. 0 means no limit.
  extern thread_local size_t maxGuardsPerObject;
}

// Options directly related to the Papyrus language.
namespace Papyrus {
  // The game to compile for. Defaults to Starfield.
  extern thread_local GameID game;
  // If true, allow identifiers to be prefixed with '::', which are normally
  // reserved for compiler identifiers.
  extern thread_local bool allowCompilerIdentifiers;
  // Allow the parsing of references to structs as presented by
  // Champollion, where the script name is prepended to the struct
  // name and separated by a '#'.
  extern thread_local bool allowDecompiledStructNameRefs;
  // Allow a negative literal value to be interpreted as a binary operation.
  extern thread_local bool allowNegativeLiteralAsBinaryOp;
  // Enable Caprica extensions to the Papyrus language.
  extern thread_local bool enableLanguageExtensions;
  // Ignore Property name and local var/parameter conflicts within a function; otherwise emits a warning.
  extern thread_local bool ignorePropertyNameLocalConflicts;
  // Allow implicit casting of `None` to any type (by default, ints, floats, and event names are not allowed).
  extern thread_local bool allowImplicitNoneCastsToAnyType;
  // The directories to search in for imported types and
  // unknown types.
  extern thread_local std::vector<ImportDir> importDirectories;
  // The user flags definition.
  extern thread_local CapricaUserFlagsDefinition userFlagsDefinition;
}

// Skyrim-specific settings to emulate the behavior of the Skyrim PCompiler
namespace Skyrim {
  // Allows non-inherited events to be declared on non-native classes
  extern thread_local bool skyrimAllowUnknownEventsOnNonNativeClass;
  // Allows object variables to shadow parent class properties
  extern thread_local bool skyrimAllowObjectVariableShadowingParentProperty;
  // Allows local variables to shadow parent class properties
  extern thread_local bool skyrimAllowLocalVariableShadowingParentProperty;
  // Allows local variables to be used before they are declared and initialized
  extern thread_local bool skyrimAllowLocalUseBeforeDeclaration;
  // Allows void method call results to be assigned to Objects and Bools
  extern thread_local bool skyrimAllowAssigningVoidMethodCallResult;
}

// Options for tweaking the performance of Caprica.
namespace Performance {
  // If true, read files asyncronously in an attempt to pre-emptively
  // read them from disk. This results in worse performance on HDDs,
  // but better performance on SSDs, as they are actually able to read
  // multiple files at once.
  extern thread_local bool asyncFileRead;
  // If true, write files to disk on background threads, allowing
  // the main compile threads to keep working while waiting for the
  // disk to catch up.
  extern thread_local bool asyncFileWrite;
  // If true, output timing stats.
  extern thread_local bool dumpTiming;
  // If true, we pause and wait for all files to be read in before
  // compiling them, and we also don't write them out to disk.
  // This is done to increase the consistency of the test runs.
  extern thread_local bool performanceTestMode;
  // If true, resolve symlinks while building canonical
  // paths.
  extern thread_local bool resolveSymlinks;
}

// Options related to warnings.
namespace Warnings {
  // If true, disable warnings by default.
  extern thread_local bool disableAllWarnings;
  // If true, treat warnings as errors.
  extern thread_local bool treatWarningsAsErrors;
  // The set of warnings to treat as errors.
  extern thread_local std::unordered_set<size_t> warningsToHandleAsErrors;
  // The set of warnings to ignore.
  extern thread_local std::unordered_set<size_t> warningsToIgnore;
  // The set of warnings to enable.
  extern thread_local std::unordered_set<size_t> warningsToEnable;
}

// Reset all configuration to default values
void resetConfig();

}}
