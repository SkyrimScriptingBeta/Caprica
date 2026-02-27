#include <common/CapricaReportingContext.h>

#include <algorithm>
#include <cassert>
#include <iostream>

#include <common/CapricaConfig.h>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace caprica {

void CapricaReportingContext::pushToErrorStream(std::string&& msg, bool isError) {
  if (!conf::Performance::performanceTestMode || isError) {
    std::cout.flush();
    std::cerr << msg << std::endl;
  }
}

void CapricaReportingContext::breakIfDebugging() {
#ifdef _WIN32
  if (IsDebuggerPresent())
    __debugbreak();
#endif
}

void CapricaReportingContext::exitIfErrors() {
  if (errorCount > 0) {
    pushToErrorStream(std::format("Compilation of '{}' failed; {} warnings and {} errors were encountered.", filename, warningCount, errorCount));
    throw std::runtime_error("");
  }
}

bool CapricaReportingContext::isWarningError(CapricaFileLocation /* location */, size_t warningNumber) const {
  // TODO: Support disabling warnings for specific sections of code.
  if (warningNumber >= 2000 && warningNumber <= 2200)
    return !conf::EngineLimits::ignoreLimits && conf::Warnings::warningsToIgnore.count(warningNumber) == 0;
  return conf::Warnings::treatWarningsAsErrors || conf::Warnings::warningsToHandleAsErrors.count(warningNumber);
}

bool CapricaReportingContext::isWarningEnabled(CapricaFileLocation /* location */, size_t warningNumber) const {
  // TODO: Support disabling warnings for specific sections of code.
  if (conf::Warnings::warningsToHandleAsErrors.count(warningNumber))
    return true;
  if (conf::Warnings::warningsToEnable.count(warningNumber))
    return true;
  if (conf::Warnings::warningsToIgnore.count(warningNumber))
    return false;
  if (warningNumber >= 2000 && warningNumber <= 2200)
    return !conf::EngineLimits::ignoreLimits;
  return !conf::Warnings::disableAllWarnings;
}

size_t CapricaReportingContext::getLocationLine(CapricaFileLocation location, size_t lastLineHint) {
  if (!lineOffsets.size())
    CapricaReportingContext::logicalFatal("Unable to locate line at offset {}.", location.startOffset);
  auto a = std::lower_bound(lineOffsets.begin(), lineOffsets.end(), location.startOffset);
  if (a == lineOffsets.end()) {
    if (lastLineHint != 0) {
      if (location.startOffset >= lineOffsets.at(lastLineHint - 1))
        return lastLineHint + 1;
      if (lastLineHint + 1 < lineOffsets.size()) {
        if (location.startOffset >= lineOffsets.at(lastLineHint - 1))
          return lastLineHint + 1;
      }
    }
    // TODO: Fix line offsets during parsing for reals, remove this hack
    // maybePushMessage(this, nullptr, "Warning:", 0, std::format("Unable to locate line at offset {}, using last known line {}...", location.startOffset, lineOffsets.size()), true);
    return lineOffsets.size();
    // CapricaReportingContext::logicalFatal("Unable to locate line at offset {}.", location.startOffset);
  }
  return std::distance(lineOffsets.begin(), a);
}

std::string CapricaReportingContext::formatLocation(CapricaFileLocation loc) {
  auto line = getLocationLine(loc);
  auto column = loc.startOffset - lineOffsets.at(line - 1) + 1;
  auto columnEnd = loc.endOffset - loc.startOffset + column;
  return std::format("{} ({}, {}:{})", filename, line, column, columnEnd);
}

void CapricaReportingContext::maybePushMessage(CapricaReportingContext* ctx,
                                               CapricaFileLocation* location,
                                               std::string_view msgType,
                                               size_t warningNumber,
                                               const std::string& msg,
                                               bool forceAsError) {
  // Try to format location, but don't let format failures hide the actual error
  std::string locationStr;
  if (ctx && location) {
    try {
      locationStr = ctx->formatLocation(*location);
    } catch (...) {
      locationStr = ctx->filename + " (unknown location)";
    }
  }

  if (warningNumber != 0) {
    if (ctx->isWarningEnabled(*location, warningNumber)) {
      if (ctx->isWarningError(*location, warningNumber)) {
        ctx->errorCount++;
        pushToErrorStream(std::format("{}: Error W{}: {}", locationStr, warningNumber, msg), true);
      } else {
        ctx->warningCount++;
        pushToErrorStream(std::format("{}: Warning W{}: {}", locationStr, warningNumber, msg));
      }
    }
  } else if (location != nullptr && ctx != nullptr) {
    pushToErrorStream(std::format("{}: {}: {}", locationStr, msgType, msg), forceAsError);
  } else {
    pushToErrorStream(std::format("{}: {}", msgType, msg), forceAsError);
  }
}

}
