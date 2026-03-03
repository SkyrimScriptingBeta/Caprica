#include <catch2/catch_test_macros.hpp>

#include <map>
#include <string>

#include <common/CapricaCompileFromSource.h>

TEST_CASE("Compile minimal Skyrim script from memory", "[compile]") {
  auto result = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"TestScript", "Scriptname TestScript"},
      std::vector<caprica::SourceInput>{});
  REQUIRE(result.success);
  REQUIRE(!result.pex.empty());
}

TEST_CASE("Compile script with import from memory", "[compile]") {
  // MyBase is a simple base class provided as an import.
  std::string baseSource = R"(Scriptname MyBase
Function DoSomething()
EndFunction
)";

  std::string childSource = R"(Scriptname ChildScript extends MyBase
Function DoSomething()
  ; override
EndFunction
)";

  auto result = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"ChildScript", childSource},
      {{"MyBase", baseSource}});
  REQUIRE(result.success);
  REQUIRE(!result.pex.empty());
}

TEST_CASE("Compilation fails for missing import", "[compile]") {
  auto result = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"TestScript", "Scriptname TestScript extends NonExistent"},
      std::vector<caprica::SourceInput>{});
  REQUIRE(!result.success);
  REQUIRE(result.pex.empty());
}

TEST_CASE("Multiple sequential compilations work", "[compile]") {
  // Verify that reset() properly clears state between compilations.
  auto result1 = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"ScriptA", "Scriptname ScriptA"},
      std::vector<caprica::SourceInput>{});
  REQUIRE(result1.success);

  auto result2 = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"ScriptB", "Scriptname ScriptB"},
      std::vector<caprica::SourceInput>{});
  REQUIRE(result2.success);

  // Different scripts should produce different output.
  REQUIRE(result1.pex != result2.pex);
}

// ---------------------------------------------------------------------------
// ScriptProvider tests
// ---------------------------------------------------------------------------

TEST_CASE("ScriptProvider resolves extends dependency", "[provider]") {
  // Provider supplies the base class on demand.
  caprica::ScriptProvider provider = [](const std::string& name)
      -> std::optional<std::string> {
    if (name == "MyBase")
      return "Scriptname MyBase\nFunction DoSomething()\nEndFunction\n";
    return std::nullopt;
  };

  auto result = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"ChildScript", "Scriptname ChildScript extends MyBase\n"
                      "Function DoSomething()\nEndFunction\n"},
      provider);
  REQUIRE(result.success);
  REQUIRE(!result.pex.empty());
}

TEST_CASE("ScriptProvider resolves transitive dependencies", "[provider]") {
  // GrandChild extends Child extends GrandParent.
  // Provider supplies both Child and GrandParent lazily.
  caprica::ScriptProvider provider = [](const std::string& name)
      -> std::optional<std::string> {
    if (name == "ChildBase")
      return "Scriptname ChildBase extends GrandParent\n";
    if (name == "GrandParent")
      return "Scriptname GrandParent\n";
    return std::nullopt;
  };

  auto result = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"GrandChild", "Scriptname GrandChild extends ChildBase\n"},
      provider);
  REQUIRE(result.success);
  REQUIRE(!result.pex.empty());
}

TEST_CASE("ScriptProvider returns nullopt for unknown script", "[provider]") {
  // Provider can't supply the dependency — compilation should fail.
  caprica::ScriptProvider provider = [](const std::string&)
      -> std::optional<std::string> {
    return std::nullopt;
  };

  auto result = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"TestScript", "Scriptname TestScript extends NonExistent\n"},
      provider);
  REQUIRE(!result.success);
}

TEST_CASE("ScriptProvider not called when explicit import exists", "[provider]") {
  // If an explicit import is provided, the provider should NOT be called for it.
  bool providerCalled = false;
  caprica::ScriptProvider provider = [&](const std::string& name)
      -> std::optional<std::string> {
    if (name == "MyBase")
      providerCalled = true;
    return std::nullopt;
  };

  std::string baseSource = "Scriptname MyBase\nFunction DoSomething()\nEndFunction\n";
  std::string childSource = "Scriptname ChildScript extends MyBase\n"
                            "Function DoSomething()\nEndFunction\n";

  auto result = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"ChildScript", childSource},
      {{"MyBase", baseSource}},
      provider);
  REQUIRE(result.success);
  REQUIRE(!providerCalled);
}

TEST_CASE("ScriptProvider with explicit imports and lazy fallback", "[provider]") {
  // Explicit import for one dependency, provider for another.
  std::string middleSource = "Scriptname MiddleClass extends LazyBase\n";

  caprica::ScriptProvider provider = [](const std::string& name)
      -> std::optional<std::string> {
    if (name == "LazyBase")
      return "Scriptname LazyBase\n";
    return std::nullopt;
  };

  auto result = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"TopScript", "Scriptname TopScript extends MiddleClass\n"},
      {{"MiddleClass", middleSource}},
      provider);
  REQUIRE(result.success);
  REQUIRE(!result.pex.empty());
}

TEST_CASE("ScriptProvider handles circular dependency gracefully", "[provider]") {
  // ScriptA extends ScriptB, ScriptB extends ScriptA — should fail, not infinite loop.
  caprica::ScriptProvider provider = [](const std::string& name)
      -> std::optional<std::string> {
    if (name == "ScriptB")
      return "Scriptname ScriptB extends ScriptA\n";
    if (name == "ScriptA")
      return "Scriptname ScriptA extends ScriptB\n";
    return std::nullopt;
  };

  auto result = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"ScriptA", "Scriptname ScriptA extends ScriptB\n"},
      provider);
  // Should fail (circular), not hang.
  REQUIRE(!result.success);
}

TEST_CASE("ScriptProvider works across sequential compilations", "[provider]") {
  // Verify provider state is cleaned up between compilations.
  caprica::ScriptProvider provider = [](const std::string& name)
      -> std::optional<std::string> {
    if (name == "BaseA")
      return "Scriptname BaseA\n";
    if (name == "BaseB")
      return "Scriptname BaseB\n";
    return std::nullopt;
  };

  auto r1 = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"ScriptA", "Scriptname ScriptA extends BaseA\n"},
      provider);
  REQUIRE(r1.success);

  auto r2 = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"ScriptB", "Scriptname ScriptB extends BaseB\n"},
      provider);
  REQUIRE(r2.success);

  REQUIRE(r1.pex != r2.pex);
}

TEST_CASE("ScriptProvider is case-insensitive on lookup", "[provider]") {
  // Papyrus is case-insensitive. "mybase" should match even if provider checks "MyBase".
  caprica::ScriptProvider provider = [](const std::string& name)
      -> std::optional<std::string> {
    // Case-insensitive comparison — provider receives the name as written in source.
    // The compiler should call with whatever case the source uses.
    if (name == "MyBase" || name == "mybase" || name == "MYBASE")
      return "Scriptname MyBase\n";
    return std::nullopt;
  };

  auto result = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"TestScript", "Scriptname TestScript extends mybase\n"},
      provider);
  REQUIRE(result.success);
}
