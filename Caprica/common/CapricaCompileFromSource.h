#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include <common/GameID.h>

namespace caprica {

struct SourceInput {
  std::string name;    // script name, e.g. "Form"
  std::string source;  // the .psc source code
};

struct CompileOutput {
  bool success = false;
  std::vector<uint8_t> pex;  // compiled .pex bytes (empty on failure)
};

// Callback: given a script name, return its source. nullopt = not available.
using ScriptProvider = std::function<std::optional<std::string>(const std::string& script_name)>;

// Compile a Papyrus script with all dependencies provided as strings.
// No filesystem access for source or output.
CompileOutput compileFromSource(
    GameID game,
    const SourceInput& script,
    const std::vector<SourceInput>& imports);

// Compile with lazy imports via a ScriptProvider callback.
// When the compiler needs a script not already registered, it asks the provider.
CompileOutput compileFromSource(
    GameID game,
    const SourceInput& script,
    ScriptProvider provider);

// Compile with explicit imports and a lazy fallback provider.
// Explicit imports are registered first; the provider handles anything missing.
CompileOutput compileFromSource(
    GameID game,
    const SourceInput& script,
    const std::vector<SourceInput>& imports,
    ScriptProvider provider);

}
