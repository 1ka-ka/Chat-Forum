#include "UserService.h"
#include "../models/User.h"
#include "../utils/ResponseUtil.h"
#include "../utils/JwtUtil.h"
#include "../utils/ValidationUtil.h"
#include <drogon/drogon.h>
#include <functional>
#include <string>
#include <iomanip>
#include <sstream>

void UserService::registerUser(const std::string &username,
                                const std::string &password,
                                const std::string &nickname,
                                Callback &&callback)
{
    if (!ValidationUtil::isValidUsername(username))
    {
        callback(ResponseUtil::badRequest("用户名格式不正确，需3-50位字母数字下划线"));
        return;
    }
    if (!ValidationUtil::isValidPassword(password))
    {
        callback(ResponseUtil::badRequest("密码长度需6-50位"));
        return;
    }
    if (!ValidationUtil::isValidNickname(nickname))
    {
        callback(ResponseUtil::badRequest("昵称格式不正确，需1-50位字符"));
        return;
    }

    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT id FROM users WHERE username = ?",
        [this, username, password, nickname, callback = std::move(callback)](const drogon::orm::Result &result) mutable {
            if (!result.empty())
            {
                callback(ResponseUtil::conflict("用户名已存在"));
                return;
            }

            std::string hashedPwd = hashPassword(password);
            auto db = drogon::app().getDbClient();
            db->execSqlAsync(
                "INSERT INTO users (username, password_hash, nickname) VALUES (?, ?, ?)",
                [this, username, nickname, callback = std::move(callback)](const drogon::orm::Result &result) mutable {
                    int64_t userId = result.insertId();
                    std::string token = JwtUtil::generateToken(userId, getJwtSecret(), getJwtExpiration());

                    Json::Value data;
                    data["id"] = userId;
                    data["username"] = username;
                    data["nickname"] = nickname;
                    data["token"] = token;
                    callback(ResponseUtil::success(data, "注册成功"));
                },
                [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
                    callback(ResponseUtil::serverError("注册失败"));
                },
                username, hashedPwd, nickname);
        },
        [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
            callback(ResponseUtil::serverError("查询失败"));
        },
        username);
}

void UserService::login(const std::string &username,
                         const std::string &password,
                         Callback &&callback)
{
    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT id, username, password_hash, nickname, avatar_url, created_at FROM users WHERE username = ?",
        [this, username, password, callback = std::move(callback)](const drogon::orm::Result &result) mutable {
            if (result.empty())
            {
                callback(ResponseUtil::badRequest("用户名或密码错误"));
                return;
            }

            auto row = result[0];
            std::string storedHash = row["password_hash"].c_str();
            if (!verifyPassword(password, storedHash))
            {
                callback(ResponseUtil::badRequest("用户名或密码错误"));
                return;
            }

            int64_t userId = row["id"].as<int64_t>();
            std::string token = JwtUtil::generateToken(userId, getJwtSecret(), getJwtExpiration());

            Json::Value data;
            data["id"] = userId;
            data["username"] = std::string(row["username"].c_str());
            data["nickname"] = std::string(row["nickname"].c_str());
            data["avatar_url"] = std::string(row["avatar_url"].c_str());
            data["token"] = token;
            callback(ResponseUtil::success(data, "登录成功"));
        },
        [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
            callback(ResponseUtil::serverError("登录失败"));
        },
        username);
}

void UserService::getProfile(int64_t userId, Callback &&callback)
{
    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT id, username, nickname, avatar_url, created_at FROM users WHERE id = ?",
        [callback = std::move(callback)](const drogon::orm::Result &result) mutable {
            if (result.empty())
            {
                callback(ResponseUtil::notFound("用户不存在"));
                return;
            }
            auto row = result[0];
            Json::Value data;
            data["id"] = row["id"].as<int64_t>();
            data["username"] = std::string(row["username"].c_str());
            data["nickname"] = std::string(row["nickname"].c_str());
            data["avatar_url"] = std::string(row["avatar_url"].c_str());
            data["created_at"] = std::string(row["created_at"].c_str());
            callback(ResponseUtil::success(data));
        },
        [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
            callback(ResponseUtil::serverError("查询失败"));
        },
        userId);
}

void UserService::getUserById(int64_t userId, Callback &&callback)
{
    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT u.id, u.nickname, u.avatar_url, u.created_at, "
        "(SELECT COUNT(*) FROM posts WHERE user_id = u.id) AS post_count "
        "FROM users u WHERE u.id = ?",
        [callback = std::move(callback)](const drogon::orm::Result &result) mutable {
            if (result.empty())
            {
                callback(ResponseUtil::notFound("用户不存在"));
                return;
            }
            auto row = result[0];
            Json::Value data;
            data["id"] = row["id"].as<int64_t>();
            data["nickname"] = std::string(row["nickname"].c_str());
            data["avatar_url"] = std::string(row["avatar_url"].c_str());
            data["post_count"] = row["post_count"].as<int>();
            data["created_at"] = std::string(row["created_at"].c_str());
            callback(ResponseUtil::success(data));
        },
        [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
            callback(ResponseUtil::serverError("查询失败"));
        },
        userId);
}

void UserService::updateProfile(int64_t userId,
                                 const std::string &nickname,
                                 const std::string &avatarUrl,
                                 Callback &&callback)
{
    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "UPDATE users SET nickname = ?, avatar_url = ? WHERE id = ?",
        [this, userId, callback = std::move(callback)](const drogon::orm::Result &result) mutable {
            getProfile(userId, std::move(callback));
        },
        [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
            callback(ResponseUtil::serverError("更新失败"));
        },
        nickname, avatarUrl, userId);
}

std::string UserService::hashPassword(const std::string &password)
{
    std::hash<std::string> hasher;
    size_t hash = hasher(password + "chatforum_salt_2025");
    std::stringstream ss;
    ss << std::hex << hash;
    std::string result = ss.str();
    while (result.length() < 16) {
        result = "0" + result;
    }
    return result;
}

bool UserService::verifyPassword(const std::string &password, const std::string &hash)
{
    return hashPassword(password) == hash;
}

std::string UserService::getJwtSecret() const
{
    auto &config = drogon::app().getCustomConfig();
    if (config.isMember("jwt") && config["jwt"].isMember("secret"))
    {
        return config["jwt"]["secret"].asString();
    }
    return "chatforum-secret-key-change-in-production-2025";
}

int UserService::getJwtExpiration() const
{
    auto &config = drogon::app().getCustomConfig();
    if (config.isMember("jwt") && config["jwt"].isMember("expiration_hours"))
    {
        return config["jwt"]["expiration_hours"].asInt();
    }
    return 24;
}
