#include "scan.h"

#include <chrono>
#include <fstream>
#include <future>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Scan {

enum SuspiciousTypes {
    JS = 0,
    Unix = 1,
    MacOS = 2
};

const std::unordered_map<SuspiciousTypes, std::string> SUSPICIOUS_STRINGS = {
    {SuspiciousTypes::JS, "<script>evil_script()</script>"},
    {SuspiciousTypes::Unix, "rm -rf ~/Documents"},
    {SuspiciousTypes::MacOS, "system(\"launchctl load /Library/LaunchAgents/com.malware.agent\")"}};

void Scanner::ScanFile(const std::filesystem::path& path) {
    try {
        std::ifstream input(path);
        if (!input.is_open()) throw "error";
        std::string s;
        while (std::getline(input, s)) {
            if (path.extension() == ".js" && s.find(SUSPICIOUS_STRINGS.at(SuspiciousTypes::JS)) != std::string::npos) {
                std::lock_guard<std::mutex> l(all_mutex);
                ++js_detected;
                break;
            } else if (s.find(SUSPICIOUS_STRINGS.at(SuspiciousTypes::Unix)) != std::string::npos) {
                std::lock_guard<std::mutex> l(all_mutex);
                ++unix_detected;
                break;
            } else if (s.find(SUSPICIOUS_STRINGS.at(SuspiciousTypes::MacOS)) != std::string::npos) {
                std::lock_guard<std::mutex> l(all_mutex);
                ++macos_detected;
                break;
            }
        }
    } catch (...) {
        std::lock_guard<std::mutex> l(all_mutex);
        ++errors_detected;
    }
    std::lock_guard<std::mutex> l(all_mutex);
    ++files_processed;
}

void Scanner::Scan() {
    using namespace std::filesystem;
    using namespace std::chrono_literals;
    std::chrono::time_point beginning = std::chrono::system_clock::now();
    if (exists(path_)) {
        std::vector<std::future<void>> futures;
        for (const auto& file : directory_iterator(path_)) {
            const std::filesystem::path& path = file.path();
            futures.push_back(std::async(
                [this, path]() {
                    ScanFile(path);
                }));
        }
        for (auto& f : futures) {
            f.get();
        }
    }
    else {
        std::cerr << "No such directory\n";
        ++errors_detected;
    }
    duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - beginning);
}

std::tuple<int64_t, int64_t, int64_t> ParseDuration(std::chrono::seconds duration) {
    int64_t hours = (duration / (60 * 60)).count();
    duration %= 60;
    int64_t minutes = (duration / 60).count();
    duration %= 60;
    int64_t seconds = duration.count();
    return {hours, minutes, seconds};
}

void Scanner::PrintResults(std::ostream& out) {
    auto [hours, minutes, seconds] = ParseDuration(duration);
    out << "====== Scan result ======\n";
    out << "Processed files: " << files_processed << "\n";
    out << "JS detects: " << js_detected << "\n";
    out << "Unix detects: " << unix_detected << "\n";
    out << "macOS detects: " << macos_detected << "\n";
    out << "Errors: " << errors_detected << "\n";
    out << "Exection time: " << hours << ":" << minutes << ":" << seconds << "\n";
    out << "=========================\n";
}

}  // namespace Scan
