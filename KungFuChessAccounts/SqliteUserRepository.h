#pragma once

#include "IUserRepository.h"

#include <mutex>
#include <string>

struct sqlite3; // opaque - only SqliteUserRepository.cpp includes sqlite3.h

// Real, file-backed persistence for accounts. This is the ONLY place in
// the entire codebase that includes any SQLite header or calls any
// sqlite3_* function - nothing outside this class (not even this header)
// should ever need to know SQLite exists. IUserRepository is the real
// contract; InMemoryUserRepository (stage 4a, still used for fast unit
// tests that don't need a real database) and this class are both just
// implementations of it.
//
// The table/column layout is this class's own private implementation
// detail, not part of the IUserRepository contract.
//
// Thread-safe: same precedent as InMemoryUserRepository/PlayerDirectory.
class SqliteUserRepository : public IUserRepository
{
public:
    // dbPath is the .db file location - not hardcoded here; the caller
    // decides where it lives. Throws std::runtime_error if the file can't
    // be opened or the schema can't be created.
    explicit SqliteUserRepository(const std::string& dbPath);
    ~SqliteUserRepository() override;

    SqliteUserRepository(const SqliteUserRepository&) = delete;
    SqliteUserRepository& operator=(const SqliteUserRepository&) = delete;

    bool usernameExists(const std::string& username) override;
    bool createAccount(const std::string& username, const std::string& passwordHash) override;
    bool getPasswordHash(const std::string& username, std::string& outHash) override;
    int getRating(const std::string& username) override;
    void updateRating(const std::string& username, int newRating) override;

private:
    mutable std::mutex m_mutex;
    sqlite3* m_db = nullptr;
};
