#pragma once

#include <cstdint>
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

// Compile a Papyrus script with all dependencies provided as strings.
// No filesystem access for source or output.
CompileOutput compileFromSource(
    GameID game,
    const SourceInput& script,
    const std::vector<SourceInput>& imports);

}
