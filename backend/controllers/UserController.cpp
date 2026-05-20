#include "UserController.h"
#include "../services/UserService.h"
#include "../services/FileService.h"
#include "../utils/ResponseUtil.h"

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

    std::string nickname = std::string(req->getParameter("nickname"));
    std::string avatarUrl;

    auto files = req->getFiles();
    if (!files.empty())
    {
        auto &file = files[0];
        std::string filename = file.getFileName();
        std::string fileContent = file.fileContent();

        FileService fileService;
        fileService.saveUploadFile(
            userId, filename, fileContent, "avatars",
            [this, userId, nickname, callback = std::move(callback)](
                const drogon::HttpResponsePtr &fileResp) mutable {
                auto json = fileResp->getJsonObject();
                std::string savedPath;
                if (json && (*json).isMember("data") && (*json)["data"].isMember("url"))
                {
                    savedPath = (*json)["data"]["url"].asString();
                }

                UserService service;
                service.updateProfile(userId, nickname, savedPath, std::move(callback));
            });
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
