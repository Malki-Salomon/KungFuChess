#include "doctest.h"
#include "Logger.h"

#include <chrono>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace
{
    // Logger::init() is a process-wide singleton (subsequent calls are
    // no-ops - see its header) - every test case here uses this same
    // path so it doesn't matter which one "wins" the actual open().
    constexpr const char* kTestLogPath = "logger_unit_test.log";

    std::string readWholeFile(const std::string& path)
    {
        std::ifstream in(path);
        std::ostringstream out;
        out << in.rdbuf();
        return out.str();
    }

    // Unique per call so assertions can find just this test's own lines,
    // regardless of whatever earlier content (from earlier test cases,
    // or earlier runs of this same binary - the file is append-only)
    // already sits in the file.
    std::string uniqueMarker(const char* label)
    {
        return std::string(label) + "_" +
            std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    }
}

TEST_SUITE("Logger")
{
    TEST_CASE("A logged line is actually appended to the file, with its level and message text")
    {
        Logger::init(kTestLogPath);

        std::string marker = uniqueMarker("APPEND_TEST");
        Logger::warn(marker + " hello from the append test");

        std::string contents = readWholeFile(kTestLogPath);
        CHECK(contents.find("[WARN]") != std::string::npos);
        CHECK(contents.find(marker + " hello from the append test") != std::string::npos);
    }

    TEST_CASE("Concurrent logging from multiple threads produces no garbled or interleaved lines")
    {
        Logger::init(kTestLogPath);

        std::string marker = uniqueMarker("CONCURRENCY_TEST");
        constexpr int kThreadCount = 8;
        constexpr int kMessagesPerThread = 50;

        std::vector<std::thread> threads;
        for (int t = 0; t < kThreadCount; ++t)
        {
            threads.emplace_back([t, marker]
            {
                for (int m = 0; m < kMessagesPerThread; ++m)
                {
                    Logger::info(marker + " " + std::to_string(t) + "-" + std::to_string(m));
                }
            });
        }
        for (auto& th : threads)
            th.join();

        std::ifstream in(kTestLogPath);
        std::string line;
        std::set<std::string> seen;
        int matchingLines = 0;

        while (std::getline(in, line))
        {
            size_t pos = line.find(marker);
            if (pos == std::string::npos)
                continue;

            ++matchingLines;

            // An intact line ends in exactly "<thread>-<message>" and
            // nothing else - a write that got interleaved/garbled by a
            // concurrent write from another thread would break this
            // clean pattern (extra bytes tacked on, or a truncated/
            // merged value), which the uniqueness check below catches.
            std::string suffix = line.substr(pos + marker.size() + 1);
            CHECK_MESSAGE(seen.insert(suffix).second, "duplicate or garbled suffix: " << suffix);
        }

        CHECK(matchingLines == kThreadCount * kMessagesPerThread);
        CHECK(seen.size() == static_cast<size_t>(kThreadCount * kMessagesPerThread));
    }
}
