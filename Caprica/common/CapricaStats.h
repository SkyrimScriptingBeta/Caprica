#pragma once

#include <atomic>

namespace caprica {

struct CapricaStats final {
private:
  struct NopIncStruct final {
    size_t val;

    NopIncStruct& operator++(int) { return *this; }
    NopIncStruct& operator=(size_t) { return *this; }
  };

  // using counter_type = std::atomic<size_t>;
  using counter_type = size_t;
  // using counter_type = NopIncStruct;

public:
  static thread_local NopIncStruct peekedTokenCount;
  static thread_local NopIncStruct consumedTokenCount;
  static thread_local counter_type importedFileCount;
  static thread_local counter_type inputFileCount;
  static thread_local NopIncStruct lexedFilesCount;
  static thread_local NopIncStruct allocatedHeapCount;
  static thread_local NopIncStruct freedHeapCount;

  static void outputStats();
  static void outputImportedCount();
};

}
