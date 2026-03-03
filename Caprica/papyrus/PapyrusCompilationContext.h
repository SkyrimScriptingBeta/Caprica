#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include <common/CapricaJobManager.h>
#include <common/CaselessStringComparer.h>
#include <common/FSUtils.h>
#include <common/identifier_ref.h>

#include <papyrus/PapyrusScript.h>

namespace caprica { namespace papyrus {

struct PapyrusCompilationNode final {
  enum class NodeType {
    Unknown,

    PapyrusCompile,
    PapyrusImport,

    PasCompile,
    PexDissassembly,

    PasReflection,
    PexReflection,
  };

  std::string_view baseName;

  // When true, FileWriteJob captures .pex output to capturedOutput instead of writing to disk.
  bool outputToMemory = false;
  // Populated by FileWriteJob when outputToMemory is true.
  std::vector<uint8_t> capturedOutput;

  // Create a node with source already in memory. FileReadJob will be a no-op.
  static PapyrusCompilationNode* createFromSource(
      CapricaJobManager* mgr,
      NodeType type,
      const std::string& scriptName,
      std::string source);

  PapyrusCompilationNode() = delete;
  PapyrusCompilationNode(CapricaJobManager* mgr,
                         NodeType compileType,
                         std::string&& sourcePath,
                         std::string&& baseOutputDir,
                         std::string&& absolutePath,
                         time_t lastMod,
                         size_t fileSize,
                         bool strictNS)
      : reportedName(std::move(sourcePath)),
        baseOutputDirectory(std::move(baseOutputDir)),
        sourceFilePath(std::move(absolutePath)),
        lastModTime(lastMod),
        filesize(fileSize),
        reportingContext(reportedName),
        jobManager(mgr),
        type(compileType),
        strictNS(strictNS) {

    // set the output directory
    if (reportedName.find_last_of("\\/") != std::string::npos)
      outputDirectory = baseOutputDirectory + FSUtils::SEP + std::string(FSUtils::parentPathAsRef(reportedName));
    else
      outputDirectory = baseOutputDirectory;

    baseName = FSUtils::basenameAsRef(sourceFilePath);
    // TODO: fix Imports hack
    if (type == NodeType::PapyrusImport)
      reportingContext.m_QuietWarnings = true;
    jobManager->queueJob(&readJob);
  }

  ~PapyrusCompilationNode() {
    if (loadedScript)
      delete loadedScript;
    if (pexFile)
      delete pexFile->alloc;
    if (resolvedObject)
      delete resolvedObject;
    if (resolutionContext)
      delete resolutionContext;
  }

  void awaitRead();

  std::string awaitPreParse();

  PapyrusScript *awaitParse();

  PapyrusObject *awaitPreSemantic();

  PapyrusObject *awaitSemantic();

  void queueCompile();
  void awaitWrite();

  NodeType getType() const;

private:
  struct BaseJob : public CapricaJob {
    BaseJob(PapyrusCompilationNode* par) : parent(par) { }

  protected:
    PapyrusCompilationNode* parent;
  };

  NodeType type;
  bool sourceProvided = false;
  size_t filesize;
  time_t lastModTime;
  std::string reportedName;
  std::string objectName;
  bool isPexFile = false;
  bool strictNS = false;
  std::string baseOutputDirectory;
  std::string outputDirectory;
  std::string sourceFilePath;
  std::string_view readFileData {};
  std::string ownedReadFileData {};
  pex::PexWriter* pexWriter { nullptr };
  PapyrusScript* loadedScript { nullptr };
  pex::PexFile* pexFile { nullptr };
  PapyrusObject* resolvedObject { nullptr };
  CapricaReportingContext reportingContext;
  PapyrusResolutionContext* resolutionContext { nullptr };
  CapricaJobManager* jobManager;

  struct FileReadJob final : public BaseJob {
    using BaseJob::BaseJob;
    virtual void run() override;
  } readJob { this };

  struct FilePreParseJob final : public BaseJob {
    using BaseJob::BaseJob;
    virtual void run() override;
  } preParseJob{this};

  struct FileParseJob final : public BaseJob {
    using BaseJob::BaseJob;
    virtual void run() override;
  } parseJob{this};

  struct FilePreSemanticJob final : public BaseJob {
    using BaseJob::BaseJob;

    virtual void run() override;
  } preSemanticJob{this};

  struct FileSemanticJob final : public BaseJob {
    using BaseJob::BaseJob;
    virtual void run() override;
  } semanticJob { this };
  struct FileCompileJob final : public BaseJob {
    using BaseJob::BaseJob;
    virtual void run() override;
  } compileJob { this };
  struct FileWriteJob final : public BaseJob {
    using BaseJob::BaseJob;
    virtual void run() override;
  } writeJob { this };
};

struct PapyrusCompilationContext final {
  static void awaitRead();

  static void doCompile(CapricaJobManager *jobManager);

  static void pushNamespaceFullContents(const std::string &namespaceName,
                                        caseless_unordered_identifier_ref_map<PapyrusCompilationNode *> &&map);

  static bool tryFindType(const identifier_ref &baseNamespace,
                          const identifier_ref &typeName,
                          PapyrusCompilationNode **retNode,
                          identifier_ref *retStructName);

  static void RenameImports(CapricaJobManager *jobManager);

  // Set/clear a lazy script provider for on-demand import resolution.
  // When tryFindType() fails to find a script in the namespace, it calls the provider.
  static void setScriptProvider(
      std::function<std::optional<std::string>(const std::string&)> provider,
      CapricaJobManager* jobMgr);
  static void clearScriptProvider();

  // Reset all compilation state for a fresh compilation
  static void reset();
};

}}
