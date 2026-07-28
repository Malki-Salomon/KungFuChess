#pragma once

#include "IUserRepository.h"

#include <mutex>
#include <string>
#include <unordered_map>

// In-process, in-memory account store - nothing survives a restart.
// Placeholder for this stage; SqliteUserRepository (real persistence)
// replaces it in a later stage without AuthService needing to change.
//
// Thread-safe: same precedent as PlayerDirectory - callers may be on the
// network I/O thread.
class InMemoryUserRepository : public IUserRepository
{
public:
    bool usernameExists(const std::string& username) override;
    bool createAccount(const std::string& username, const std::string& passwordHash) override;
    bool getPasswordHash(const std::string& username, std::string& outHash) override;
    int getRating(const std::string& username) override;
    void updateRating(const std::string& username, int newRating) override;

private:
    struct UserRecord
    {
        std::string passwordHash;
        int rating = 1200; // the one place this stage's starting rating is defined
    };

    mutable std::mutex m_mutex;
    std::unordered_map<std::string, UserRecord> m_users;
};
