#include <catch2/catch_test_macros.hpp>

#include <common/CapricaCompileFromSource.h>

TEST_CASE("Compile minimal Skyrim script from memory", "[compile]") {
  auto result = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"TestScript", "Scriptname TestScript"},
      {});
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
      {});
  REQUIRE(!result.success);
  REQUIRE(result.pex.empty());
}

TEST_CASE("Multiple sequential compilations work", "[compile]") {
  // Verify that reset() properly clears state between compilations.
  auto result1 = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"ScriptA", "Scriptname ScriptA"},
      {});
  REQUIRE(result1.success);

  auto result2 = caprica::compileFromSource(
      caprica::GameID::Skyrim,
      {"ScriptB", "Scriptname ScriptB"},
      {});
  REQUIRE(result2.success);

  // Different scripts should produce different output.
  REQUIRE(result1.pex != result2.pex);
}
