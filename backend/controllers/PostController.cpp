#include "PostController.h"
#include "../services/PostService.h"
#include "../services/FileService.h"
#include "../utils/ResponseUtil.h"
#include <json/json.h>
#include <memory>

void PostController::createPost(const drogon::HttpRequestPtr &req,
                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    int64_t userId = req->getAttribute<int64_t>("userId");

    std::string title = std::string(req->getParameter("title"));
    std::string content = std::string(req->getParameter("content"));

    auto files = req->getFiles();
    if (files.empty())
    {
        PostService service;
        service.createPost(userId, title, content, "[]", std::move(callback));
        return;
    }

    auto savedPaths = std::make_shared<Json::Value>(Json::arrayValue);
    auto remaining = std::make_shared<int>(files.size());
    auto cb = std::make_shared<std::function<void(const drogon::HttpResponsePtr &)>>(std::move(callback));

    for (size_t i = 0; i < files.size(); ++i)
    {
        auto &file = files[i];
        std::string filename = file.getFileName();
        std::string fileContent = file.fileContent();

        FileService fileService;
        fileService.saveUploadFile(
            userId, filename, fileContent, "images",
            [userId, title, content, savedPaths, remaining, cb](
                const drogon::HttpResponsePtr &fileResp) mutable {
                auto json = fileResp->getJsonObject();
                if (json && (*json).isMember("data") && (*json)["data"].isMember("url"))
                {
                    savedPaths->append((*json)["data"]["url"].asString());
                }

                (*remaining)--;
                if (*remaining == 0)
                {
                    Json::StreamWriterBuilder builder;
                    std::string imageUrlsStr = Json::writeString(builder, *savedPaths);

                    PostService service;
                    service.createPost(userId, title, content, imageUrlsStr, std::move(*cb));
                }
            });
    }
}

void PostController::getPostList(const drogon::HttpRequestPtr &req,
                                  std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    int64_t currentUserId = req->getAttribute<int64_t>("userId");

    int page = std::stoi(req->getParameter("page", "1"));
    int pageSize = std::stoi(req->getParameter("page_size", "10"));
    std::string search = std::string(req->getParameter("search"));

    PostService service;
    service.getPostList(page, pageSize, currentUserId, search, std::move(callback));
}

void PostController::getPostById(const drogon::HttpRequestPtr &req,
                                  std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                  int64_t postId)
{
    int64_t currentUserId = req->getAttribute<int64_t>("userId");

    PostService service;
    service.getPostById(postId, currentUserId, std::move(callback));
}
