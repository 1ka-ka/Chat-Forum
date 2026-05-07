#include "UserController.h"
#include "../services/UserService.h"
#include "../services/FileService.h"
#include "../utils/ResponseUtil.h"
#include <json/json.h>

void UserController::registerUser(const drogon::HttpRequestPtr &req,
                                   std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json)
    {
        callback(ResponseUtil::badRequest("请求体格式错误"));
        return;
    }

    std::string username = (*json).get("username", "").asString();
    std::string password = (*json).get("password", "").asString();
    std::string nickname = (*json).get("nickname", "").asString();

    UserService service;
    service.registerUser(username, password, nickname, std::move(callback));
}

void UserController::login(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json)
    {
        callback(ResponseUtil::badRequest("请求体格式错误"));
        return;
    }

    std::string username = (*json).get("username", "").asString();
    std::string password = (*json).get("password", "").asString();

    UserService service;
    service.login(username, password, std::move(callback));
}

void UserController::getProfile(const drogon::HttpRequestPtr &req,
                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    int64_t userId = req->getAttribute<int64_t>("userId");
    UserService service;
    service.getProfile(userId, std::move(callback));
}

void UserController::updateProfile(const drogon::HttpRequestPtr &req,
                                    std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    int64_t userId = req->getAttribute<int64_t>("userId");

    std::string nickname;
    std::string avatarUrl;

    auto json = req->getJsonObject();
    if (json)
    {
        nickname = (*json).get("nickname", "").asString();
    }

    FileService fileService;
    std::string uploadedAvatar = fileService.saveUploadedFile(req, "avatar", "avatars", userId, 2 * 1024 * 1024);

    if (!uploadedAvatar.empty())
    {
        avatarUrl = uploadedAvatar;
    }

    if (nickname.empty() && avatarUrl.empty())
    {
        callback(ResponseUtil::badRequest("请提供要更新的信息"));
        return;
    }

    UserService service;
    service.updateProfile(userId, nickname, avatarUrl, std::move(callback));
}

void UserController::getUserById(const drogon::HttpRequestPtr &req,
                                  std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                  int64_t userId)
{
    UserService service;
    service.getUserById(userId, std::move(callback));
}
