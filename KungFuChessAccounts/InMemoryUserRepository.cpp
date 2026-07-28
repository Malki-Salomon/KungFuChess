#include "InMemoryUserRepository.h"

bool InMemoryUserRepository::usernameExists(const std::string& username)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_users.find(username) != m_users.end();
}

bool InMemoryUserRepository::createAccount(const std::string& username, const std::string& passwordHash)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_users.find(username) != m_users.end())
        return false;

    UserRecord record;
    record.passwordHash = passwordHash;
    m_users[username] = record;
    return true;
}

bool InMemoryUserRepository::getPasswordHash(const std::string& username, std::string& outHash)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_users.find(username);
    if (it == m_users.end())
        return false;

    outHash = it->second.passwordHash;
    return true;
}

int InMemoryUserRepository::getRating(const std::string& username)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_users.find(username);
    return it == m_users.end() ? 0 : it->second.rating;
}

void InMemoryUserRepository::updateRating(const std::string& username, int newRating)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_users.find(username);
    if (it != m_users.end())
        it->second.rating = newRating;
}
