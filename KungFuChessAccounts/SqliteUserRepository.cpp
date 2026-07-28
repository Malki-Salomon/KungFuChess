#include "SqliteUserRepository.h"

#include <sqlite3.h>

#include <stdexcept>

namespace
{
    constexpr int kStartingRating = 1200; // the one place this stage's starting rating is defined - see InMemoryUserRepository for the same constant in the other implementation

    void execOrThrow(sqlite3* db, const char* sql)
    {
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK)
        {
            std::string message = errMsg ? errMsg : "unknown sqlite error";
            sqlite3_free(errMsg);
            throw std::runtime_error("SqliteUserRepository: " + message);
        }
    }
}

SqliteUserRepository::SqliteUserRepository(const std::string& dbPath)
{
    if (sqlite3_open(dbPath.c_str(), &m_db) != SQLITE_OK)
    {
        std::string message = m_db ? sqlite3_errmsg(m_db) : "could not open database";
        sqlite3_close(m_db);
        m_db = nullptr;
        throw std::runtime_error("SqliteUserRepository: failed to open '" + dbPath + "': " + message);
    }

    execOrThrow(m_db,
        "CREATE TABLE IF NOT EXISTS users ("
        "  username TEXT PRIMARY KEY,"
        "  password_hash TEXT NOT NULL,"
        "  rating INTEGER NOT NULL"
        ");");
}

SqliteUserRepository::~SqliteUserRepository()
{
    if (m_db)
        sqlite3_close(m_db);
}

bool SqliteUserRepository::usernameExists(const std::string& username)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, "SELECT 1 FROM users WHERE username = ?;", -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    bool exists = sqlite3_step(stmt) == SQLITE_ROW;
    sqlite3_finalize(stmt);
    return exists;
}

bool SqliteUserRepository::createAccount(const std::string& username, const std::string& passwordHash)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, "INSERT INTO users (username, password_hash, rating) VALUES (?, ?, ?);", -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, kStartingRating);

    // Fails (not SQLITE_DONE) if username already exists - the PRIMARY
    // KEY constraint on username enforces uniqueness at the DB level, so
    // there's no separate existence check needed here.
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool SqliteUserRepository::getPasswordHash(const std::string& username, std::string& outHash)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, "SELECT password_hash FROM users WHERE username = ?;", -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    bool found = sqlite3_step(stmt) == SQLITE_ROW;
    if (found)
        outHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

    sqlite3_finalize(stmt);
    return found;
}

int SqliteUserRepository::getRating(const std::string& username)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, "SELECT rating FROM users WHERE username = ?;", -1, &stmt, nullptr) != SQLITE_OK)
        return 0;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    int rating = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        rating = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    return rating;
}

void SqliteUserRepository::updateRating(const std::string& username, int newRating)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, "UPDATE users SET rating = ? WHERE username = ?;", -1, &stmt, nullptr) != SQLITE_OK)
        return;

    sqlite3_bind_int(stmt, 1, newRating);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}
