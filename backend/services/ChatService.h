#pragma once

#include <string>
#include <functional>
#include <drogon/drogon.h>

class ChatService
{
public:
    using Callback = std::function<void(const drogon::HttpResponsePtr &)>;

    void saveMessage(int64_t senderId,
                     int64_t receiverId,
                     const std::string &content,
                     std::function<void(int64_t)> successCallback,
                     std::function<void()> errorCallback);

    void getConversations(int64_t userId, Callback &&callback);

    void getMessages(int64_t userId,
                     int64_t otherUserId,
                     int page,
                     int pageSize,
                     Callback &&callback);

    void markAsRead(int64_t userId, int64_t otherUserId, Callback &&callback);
};
