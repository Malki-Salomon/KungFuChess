#pragma once

#include <string>

// Small, shared, thread-safe logger used by both KungFuChessServer and
// KungFuChessShellClient - a namespace-style API (not a class instance
// threaded through every constructor) so it's callable from anywhere
// after one init() call at the very start of main(), the same way
// std::cout is.
//
// Format: one line per entry, "[timestamp] [LEVEL] message", plain text
// - no structured/JSON logging (that belongs to the later Docker/
// Kubernetes-era Observability work, not here). Every call writes to
// both the log file and stdout, so existing console-watching workflows
// still see everything - this adds durable persistence alongside the
// console, it doesn't replace it.
//
// No log rotation or size limits - a single ever-growing file is fine
// at this project's current scale; revisit if that ever becomes a real
// problem.
namespace Logger
{
    enum class Level { Info, Warn, Error };

    // Must be called once, before any log()/info()/warn()/error() call -
    // opens the log file (a fixed relative path next to the executable,
    // e.g. "server.log"/"client.log" - same "no config system yet"
    // placeholder status as kUserDbPath/kListenPort elsewhere). Safe to
    // call more than once - subsequent calls are no-ops, same precedent
    // as PasswordHasher::init().
    void init(const std::string& logFilePath);

    void log(Level level, const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
}
