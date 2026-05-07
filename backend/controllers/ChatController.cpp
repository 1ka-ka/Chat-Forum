#include "ChatController.h"
#include "../services/ChatService.h"

void ChatController::getConversations(const drogon::HttpRequestPtr &req,
                                       std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    int64_t userId = req->getAttribute<int64_t>("userId");

    ChatService service;
    service.getConversations(userId, std::move(callback));
}

void ChatController::getMessages(const drogon::HttpRequestPtr &req,
                                  std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                  int64_t otherUserId)
{
    int64_t userId = req->getAttribute<int64_t>("userId");

    int page = 1, pageSize = 20;
    std::string pageStr = std::string(req->getParameter("page"));
    std::string pageSizeStr = std::string(req->getParameter("page_size"));

    if (!pageStr.empty())
    {
        try { page = std::stoi(pageStr); } catch (...) {}
    }
    if (!pageSizeStr.empty())
    {
        try { pageSize = std::stoi(pageStr); } catch (...) {}
    }

    ChatService service;
    service.getMessages(userId, otherUserId, page, pageSize, std::move(callback));
}

void ChatController::markAsRead(const drogon::HttpRequestPtr &req,
                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                 int64_t otherUserId)
{
    int64_t userId = req->getAttribute<int64_t>("userId");

    ChatService service;
    service.markAsRead(userId, otherUserId, std::move(callback));
}
