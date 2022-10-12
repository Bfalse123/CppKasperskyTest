#pragma once
#include <chrono>
#include <filesystem>
#include <iostream>
#include <mutex>

namespace Scan {
class Scanner {
   public:
    Scanner(const std::filesystem::path& path) : path_(path) {}
    void Scan();
    void PrintResults(std::ostream& out);

   private:
    void ScanFile(const std::filesystem::path& path);
    std::mutex all_mutex;
    std::filesystem::path path_;
    int64_t js_detected = 0;
    int64_t unix_detected = 0;
    int64_t macos_detected = 0;
    int64_t errors_detected = 0;
    int64_t files_processed = 0;
    std::chrono::seconds duration;
};

static std::string ScanDirectory(const std::filesystem::path& path) {
    Scanner scanner(path);
    scanner.Scan();
    std::ostringstream ss;
    scanner.PrintResults(ss);
    return ss.str();
}
}  // namespace Scan