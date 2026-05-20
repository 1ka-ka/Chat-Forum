#include "AuthMiddleware.h"
#include "JwtUtil.h"
#include "ResponseUtil.h"
#include "ConfigUtil.h"

void AuthMiddleware::invoke(const drogon::HttpRequestPtr &req,
                            drogon::MiddlewareNextCallback &&nextCb,
                            drogon::MiddlewareCallback &&mcb) {
    std::string authHeader = std::string(req->getHeader("Authorization"));
    if (authHeader.empty() || authHeader.substr(0, 7) != "Bearer ") {
        mcb(ResponseUtil::unauthorized());
        return;
    }

    std::string token = authHeader.substr(7);
    std::string secret = getJwtSecret();
    if (!JwtUtil::verifyToken(token, secret)) {
        mcb(ResponseUtil::unauthorized());
        return;
    }

    int64_t userId = JwtUtil::getUserIdFromToken(token, secret);
    if (userId <= 0) {
        mcb(ResponseUtil::unauthorized());
        return;
    }

    req->setAttribute("userId", userId);
    nextCb([mcb = std::move(mcb)](const drogon::HttpResponsePtr &resp) {
        mcb(resp);
    });
}

std::string AuthMiddleware::getJwtSecret() const {
    return ConfigUtil::getJwtSecret();
}
