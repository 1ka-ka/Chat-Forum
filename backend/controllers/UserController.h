#pragma once

#include <drogon/drogon.h>

class UserController : public drogon::HttpController<UserController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(UserController::registerUser, "/api/auth/register", drogon::Post, drogon::Options);
    ADD_METHOD_TO(UserController::login, "/api/auth/login", drogon::Post, drogon::Options);
    ADD_METHOD_TO(UserController::getProfile, "/api/user/profile", drogon::Get, drogon::Options, "auth");
    ADD_METHOD_TO(UserController::updateProfile, "/api/user/profile", drogon::Put, drogon::Options, "auth");
    ADD_METHOD_TO(UserController::getUserById, "/api/user/{1}", drogon::Get, drogon::Options);
    METHOD_LIST_END

    void registerUser(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void login(const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void getProfile(const drogon::HttpRequestPtr &req,
                    std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void updateProfile(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void getUserById(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                     int64_t userId);
};
