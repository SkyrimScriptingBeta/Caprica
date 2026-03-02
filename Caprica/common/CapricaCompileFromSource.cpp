#include <common/CapricaCompileFromSource.h>

#include <common/CapricaConfig.h>
#include <common/CapricaJobManager.h>
#include <common/CapricaReportingContext.h>
#include <common/CapricaUserFlagsDefinition.h>
#include <common/CaselessStringComparer.h>
#include <common/FakeScripts.h>
#include <common/FSUtils.h>
#include <common/identifier_ref.h>

#include <papyrus/PapyrusCompilationContext.h>

namespace caprica {

CompileOutput compileFromSource(
    GameID game,
    const SourceInput& script,
    const std::vector<SourceInput>& imports) {

  // 1. Reset all state from any prior compilation.
  papyrus::PapyrusCompilationContext::reset();
  conf::resetConfig();

  // 2. Configure for the target game.
  conf::Papyrus::game = game;
  conf::General::compileInParallel = false;
  conf::General::quietCompile = true;

  if (game == GameID::Skyrim) {
    conf::Skyrim::skyrimAllowUnknownEventsOnNonNativeClass = true;
    conf::Skyrim::skyrimAllowObjectVariableShadowingParentProperty = true;
    conf::Skyrim::skyrimAllowLocalVariableShadowingParentProperty = true;
    conf::Skyrim::skyrimAllowLocalUseBeforeDeclaration = true;
    conf::Skyrim::skyrimAllowAssigningVoidMethodCallResult = true;
  }

  // 2b. Register user flags for the target game.
  {
    using VL = CapricaUserFlagsDefinition::ValidLocations;
    CapricaReportingContext flagsCtx{"<built-in-flags>"};

    if (game == GameID::Skyrim) {
      // Hidden (bit 0) — valid on Script, Property
      CapricaUserFlagsDefinition::UserFlag hidden(CapricaFileLocation{});
      hidden.name = "Hidden";
      hidden.bitIndex = 0;
      hidden.validLocations = VL::Script | VL::Property;
      conf::Papyrus::userFlagsDefinition.registerUserFlag(flagsCtx, hidden);

      // Conditional (bit 1) — valid on Script, Variable
      CapricaUserFlagsDefinition::UserFlag conditional(CapricaFileLocation{});
      conditional.name = "Conditional";
      conditional.bitIndex = 1;
      conditional.validLocations = VL::Script | VL::Variable;
      conf::Papyrus::userFlagsDefinition.registerUserFlag(flagsCtx, conditional);
    }
  }

  // 3. Create a single-threaded job manager.
  CapricaJobManager jobManager;

  // 4. Register fake scripts (e.g. __ScriptObject for Skyrim).
  if (game == GameID::Skyrim) {
    static const char* FAKE_SKYRIM_SCRIPTS[] = {
      "fake://skyrim/__ScriptObject.psc",
      "fake://skyrim/DLC1SCWispWallScript.psc",
    };
    caseless_unordered_identifier_ref_map<papyrus::PapyrusCompilationNode*> fakeMap;
    for (const auto& fakeScript : FAKE_SKYRIM_SCRIPTS) {
      auto basename = FSUtils::filenameAsRef(fakeScript);
      auto node = new papyrus::PapyrusCompilationNode(
          &jobManager,
          papyrus::PapyrusCompilationNode::NodeType::PapyrusImport,
          std::string(basename),
          std::string(""),
          std::string(fakeScript),
          0,
          FakeScripts::getSizeOfFakeScript(fakeScript, game),
          false);
      fakeMap.emplace(identifier_ref(node->baseName), node);
    }
    papyrus::PapyrusCompilationContext::pushNamespaceFullContents("", std::move(fakeMap));
  }

  // 5. Add imports from memory.
  if (!imports.empty()) {
    caseless_unordered_identifier_ref_map<papyrus::PapyrusCompilationNode*> importMap;
    for (const auto& imp : imports) {
      auto node = papyrus::PapyrusCompilationNode::createFromSource(
          &jobManager,
          papyrus::PapyrusCompilationNode::NodeType::PapyrusImport,
          imp.name, imp.source);
      importMap.emplace(identifier_ref(node->baseName), node);
    }
    papyrus::PapyrusCompilationContext::pushNamespaceFullContents("", std::move(importMap));
  }

  // 6. Add the script to compile (from memory, output to memory).
  auto compileNode = papyrus::PapyrusCompilationNode::createFromSource(
      &jobManager,
      papyrus::PapyrusCompilationNode::NodeType::PapyrusCompile,
      script.name, script.source);
  compileNode->outputToMemory = true;

  {
    caseless_unordered_identifier_ref_map<papyrus::PapyrusCompilationNode*> scriptMap;
    scriptMap.emplace(identifier_ref(compileNode->baseName), compileNode);
    papyrus::PapyrusCompilationContext::pushNamespaceFullContents("", std::move(scriptMap));
  }

  // 7. Compile.
  CompileOutput out;
  try {
    papyrus::PapyrusCompilationContext::doCompile(&jobManager);
  } catch (const std::runtime_error&) {
    out.success = false;
    return out;
  }

  // 8. Collect results.
  out.pex = std::move(compileNode->capturedOutput);
  out.success = !out.pex.empty();
  return out;
}

}
