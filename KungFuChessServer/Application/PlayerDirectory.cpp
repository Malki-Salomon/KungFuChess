#include "PlayerDirectory.h"

void PlayerDirectory::set(SessionId id, const std::string& username)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_usernames[id] = username;
}

std::string PlayerDirectory::usernameOf(SessionId id) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_usernames.find(id);
    return it == m_usernames.end() ? std::string() : it->second;
}

void PlayerDirectory::release(SessionId id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_usernames.erase(id);
}
