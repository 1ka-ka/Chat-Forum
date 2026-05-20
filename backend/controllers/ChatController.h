#pragma once

#include <drogon/drogon.h>

class ChatController : public drogon::HttpController<ChatController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ChatController::getConversations, "/api/chat/conversations", drogon::Get, drogon::Options, "auth");
    ADD_METHOD_TO(ChatController::getMessages, "/api/chat/messages/{1}", drogon::Get, drogon::Options, "auth");
    ADD_METHOD_TO(ChatController::markAsRead, "/api/chat/read/{1}", drogon::Put, drogon::Options, "auth");
    ADD_METHOD_TO(ChatController::sendMessage, "/api/chat/send", drogon::Post, drogon::Options, "auth");
    METHOD_LIST_END

    void getConversations(const drogon::HttpRequestPtr &req,
                          std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void getMessages(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                     int64_t otherUserId);

    void markAsRead(const drogon::HttpRequestPtr &req,
                    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                    int64_t otherUserId);

    void sendMessage(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};
