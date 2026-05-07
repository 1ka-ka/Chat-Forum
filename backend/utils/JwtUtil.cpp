#include "JwtUtil.h"
#include <chrono>

namespace JwtUtil
{
    std::string generateToken(int64_t userId, const std::string &secret, int expirationHours)
    {
        auto now = std::chrono::system_clock::now();
        auto exp = now + std::chrono::hours(expirationHours);

        return jwt::create()
            .set_issuer("chatforum")
            .set_type("JWT")
            .set_payload_claim("user_id", jwt::claim(std::to_string(userId)))
            .set_issued_at(now)
            .set_expires_at(exp)
            .sign(jwt::algorithm::hs256{secret});
    }

    bool verifyToken(const std::string &token, const std::string &secret)
    {
        try
        {
            auto decoded = jwt::decode(token);
            auto verifier = jwt::verify()
                                .allow_algorithm(jwt::algorithm::hs256{secret})
                                .with_issuer("chatforum");
            verifier.verify(decoded);
            return true;
        }
        catch (const std::exception &)
        {
            return false;
        }
    }

    int64_t getUserIdFromToken(const std::string &token, const std::string &secret)
    {
        try
        {
            auto decoded = jwt::decode(token);
            auto verifier = jwt::verify()
                                .allow_algorithm(jwt::algorithm::hs256{secret})
                                .with_issuer("chatforum");
            verifier.verify(decoded);
            auto userIdStr = decoded.get_payload_claim("user_id").as_string();
            return std::stoll(userIdStr);
        }
        catch (const std::exception &)
        {
            return -1;
        }
    }
}
