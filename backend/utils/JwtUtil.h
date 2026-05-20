#pragma once

#include <string>
#include <cstdint>

class JwtUtil {
public:
    static std::string generateToken(int64_t userId, const std::string &secret, int expirationHours);
    static bool verifyToken(const std::string &token, const std::string &secret);
    static int64_t getUserIdFromToken(const std::string &token, const std::string &secret);
};
