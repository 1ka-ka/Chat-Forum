#pragma once

#include <string>
#include <functional>
#include <json/json.h>
#include <drogon/HttpResponse.h>

class ChatService
{
public:
    using Callback = std::function<void(const drogon::HttpResponsePtr &)>;

    void getConversations(int64_t userId, Callback &&callback);
    void getMessages(int64_t userId, int64_t otherUserId,
                     int page, int pageSize,
                     Callback &&callback);
    void markAsRead(int64_t userId, int64_t otherUserId, Callback &&callback);
    void saveMessage(int64_t senderId, int64_t receiverId,
                     const std::string &content,
                     std::function<void(int64_t)> &&onSuccess,
                     std::function<void()> &&onError);
};
