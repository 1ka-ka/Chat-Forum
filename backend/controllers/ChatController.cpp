#include "ChatController.h"
#include "../services/ChatService.h"
#include "../utils/ResponseUtil.h"
#include <memory>

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

    int page = std::stoi(req->getParameter("page", "1"));
    int pageSize = std::stoi(req->getParameter("page_size", "20"));

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

void ChatController::sendMessage(const drogon::HttpRequestPtr &req,
                                  std::function<void(const drogon::HttpResponsePtr &)> &&callback)
{
    int64_t userId = req->getAttribute<int64_t>("userId");

    auto json = req->getJsonObject();
    if (!json)
    {
        callback(ResponseUtil::badRequest("请求体格式错误"));
        return;
    }

    int64_t receiverId = (*json).get("receiver_id", 0).asInt64();
    std::string content = (*json).get("content", "").asString();

    auto cb = std::make_shared<std::function<void(const drogon::HttpResponsePtr &)>>(std::move(callback));

    ChatService service;
    service.saveMessage(
        userId, receiverId, content,
        [cb](int64_t messageId) mutable {
            Json::Value data;
            data["message_id"] = messageId;
            (*cb)(ResponseUtil::success(data, "发送成功"));
        },
        [cb]() mutable {
            (*cb)(ResponseUtil::serverError("消息发送失败"));
        });
}
