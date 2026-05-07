#pragma once

#include <string>
#include <jwt-cpp/jwt.h>

namespace JwtUtil
{
    std::string generateToken(int64_t userId, const std::string &secret, int expirationHours);
    bool verifyToken(const std::string &token, const std::string &secret);
    int64_t getUserIdFromToken(const std::string &token, const std::string &secret);
}
