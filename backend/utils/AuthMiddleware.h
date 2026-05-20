#pragma once

#include <drogon/drogon.h>

class AuthMiddleware : public drogon::HttpMiddleware<AuthMiddleware> {
public:
    void invoke(const drogon::HttpRequestPtr &req,
                drogon::MiddlewareNextCallback &&nextCb,
                drogon::MiddlewareCallback &&mcb) override;

private:
    std::string getJwtSecret() const;
};
