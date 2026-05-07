#include "CommentController.h"
#include "../services/CommentService.h"
#include "../utils/ResponseUtil.h"
#include <json/json.h>

void CommentController::createComment(const drogon::HttpRequestPtr &req,
                                       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                       int64_t postId)
{
    int64_t userId = req->getAttribute<int64_t>("userId");

    auto json = req->getJsonObject();
    if (!json)
    {
        callback(ResponseUtil::badRequest("请求体格式错误"));
        return;
    }

    std::string content = (*json).get("content", "").asString();

    CommentService service;
    service.createComment(postId, userId, content, std::move(callback));
}

void CommentController::getComments(const drogon::HttpRequestPtr &req,
                                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                     int64_t postId)
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

    CommentService service;
    service.getCommentsByPostId(postId, page, pageSize, std::move(callback));
}
