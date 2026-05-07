#pragma once

#include <drogon/HttpRequest.h>
#include <drogon/HttpMiddleware.h>
#include <string>

class AuthMiddleware : public drogon::HttpMiddleware<AuthMiddleware>
{
public:
    AuthMiddleware() = default;

    void invoke(const drogon::HttpRequestPtr &req,
                drogon::MiddlewareNextCallback &&nextCb,
                drogon::MiddlewareCallback &&mcb) override;

private:
    std::string getJwtSecret() const;
};
