#include "PasswordHasher.h"

#include <functional>
#include <sstream>

namespace PasswordHasher
{
    namespace
    {
        // Not a secret - just a fixed value mixed in so this placeholder
        // isn't literally std::hash(password) verbatim. Provides zero real
        // security; see the header comment.
        const std::string kPlaceholderPepper = "kfc-stage4a-placeholder-pepper-not-secure";
    }

    bool hash(const std::string& plainPassword, std::string& outHash)
    {
        std::size_t hashed = std::hash<std::string>{}(plainPassword + kPlaceholderPepper);

        std::ostringstream out;
        out << hashed;
        outHash = out.str();
        return true;
    }

    bool verify(const std::string& plainPassword, const std::string& storedHash)
    {
        std::string recomputed;
        if (!hash(plainPassword, recomputed))
            return false;

        return recomputed == storedHash;
    }
}
