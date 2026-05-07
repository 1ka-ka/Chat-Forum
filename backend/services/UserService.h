#pragma once

#include <string>
#include <functional>
#include <json/json.h>
#include <drogon/HttpResponse.h>

class UserService
{
public:
    using Callback = std::function<void(const drogon::HttpResponsePtr &)>;

    void registerUser(const std::string &username,
                      const std::string &password,
                      const std::string &nickname,
                      Callback &&callback);

    void login(const std::string &username,
               const std::string &password,
               Callback &&callback);

    void getProfile(int64_t userId,
                    Callback &&callback);

    void getUserById(int64_t userId,
                     Callback &&callback);

    void updateProfile(int64_t userId,
                       const std::string &nickname,
                       const std::string &avatarUrl,
                       Callback &&callback);

private:
    std::string hashPassword(const std::string &password);
    bool verifyPassword(const std::string &password, const std::string &hash);
    std::string getJwtSecret() const;
    int getJwtExpiration() const;
};
