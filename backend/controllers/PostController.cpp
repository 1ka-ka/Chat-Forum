#include "PostController.h"
#include "../services/PostService.h"
#include "../services/FileService.h"
#include "../utils/ResponseUtil.h"
#include <json/json.h>
#include <drogon/drogon.h>

void PostController::createPost(const drogon::HttpRequestPtr &req,
                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    int64_t userId = req->getAttribute<int64_t>("userId");

    std::string title = std::string(req->getParameter("title"));
    std::string content = std::string(req->getParameter("content"));

    if (title.empty() || content.empty())
    {
        auto json = req->getJsonObject();
        if (json)
        {
            title = (*json).get("title", "").asString();
            content = (*json).get("content", "").asString();
        }
    }

    std::vector<std::string> imageUrls;
    FileService fileService;

    auto &files = req->getFiles();
    for (auto &file : files)
    {
        if (file.getFieldName() == "images")
        {
            std::string url = fileService.saveUploadedFile(req, "images", "images", userId, 5 * 1024 * 1024);
            if (!url.empty())
            {
                imageUrls.push_back(url);
            }
        }
    }

    PostService service;
    service.createPost(userId, title, content, imageUrls, std::move(callback));
}

void PostController::getPostList(const drogon::HttpRequestPtr &req,
                                  std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    int page = 1, pageSize = 20;
    std::string pageStr = std::string(req->getParameter("page"));
    std::string pageSizeStr = std::string(req->getParameter("page_size"));

    if (!pageStr.empty())
    {
        try { page = std::stoi(pageStr); } catch (...) {}
    }
    if (!pageSizeStr.empty())
    {
        try { pageSize = std::stoi(pageSizeStr); } catch (...) {}
    }

    int64_t currentUserId = 0;
    try
    {
        currentUserId = req->getAttribute<int64_t>("userId");
    }
    catch (...)
    {
        currentUserId = 0;
    }

    PostService service;
    service.getPostList(page, pageSize, currentUserId, std::move(callback));
}

void PostController::getPostById(const drogon::HttpRequestPtr &req,
                                  std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                  int64_t postId)
{
    int64_t currentUserId = 0;
    try
    {
        currentUserId = req->getAttribute<int64_t>("userId");
    }
    catch (...)
    {
        currentUserId = 0;
    }

    PostService service;
    service.getPostById(postId, currentUserId, std::move(callback));
}
