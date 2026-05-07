#include "AuthMiddleware.h"
#include "JwtUtil.h"
#include "ResponseUtil.h"
#include <drogon/drogon.h>
#include <json/json.h>

void AuthMiddleware::invoke(const drogon::HttpRequestPtr &req,
                            drogon::MiddlewareNextCallback &&nextCb,
                            drogon::MiddlewareCallback &&mcb)
{
    std::string authHeader = std::string(req->getHeader("Authorization"));
    if (authHeader.empty() || authHeader.substr(0, 7) != "Bearer ")
    {
        auto resp = ResponseUtil::unauthorized();
        mcb(resp);
        return;
    }

    std::string token = authHeader.substr(7);
    std::string secret = getJwtSecret();

    if (!JwtUtil::verifyToken(token, secret))
    {
        auto resp = ResponseUtil::unauthorized();
        mcb(resp);
        return;
    }

    int64_t userId = JwtUtil::getUserIdFromToken(token, secret);
    if (userId <= 0)
    {
        auto resp = ResponseUtil::unauthorized();
        mcb(resp);
        return;
    }

    req->setAttribute("userId", userId);
    nextCb([mcb = std::move(mcb)](const drogon::HttpResponsePtr &resp) {
        mcb(resp);
    });
}

std::string AuthMiddleware::getJwtSecret() const
{
    auto &config = drogon::app().getCustomConfig();
    if (config.isMember("jwt") && config["jwt"].isMember("secret"))
    {
        return config["jwt"]["secret"].asString();
    }
    return "chatforum-secret-key-change-in-production-2025";
}
