#pragma once

#include <drogon/drogon.h>
#include <unordered_map>
#include <shared_mutex>
#include <cstdint>

class ChatWebSocketController : public drogon::WebSocketController<ChatWebSocketController> {
public:
    WS_PATH_LIST_BEGIN
    WS_PATH_ADD("/ws/chat", drogon::Get);
    WS_PATH_LIST_END

    void handleNewConnection(const drogon::HttpRequestPtr &req,
                             const drogon::WebSocketConnectionPtr &conn) override;
    void handleNewMessage(const drogon::WebSocketConnectionPtr &conn,
                          std::string &&message) override;
    void handleConnectionClosed(const drogon::WebSocketConnectionPtr &conn) override;

private:
    static std::unordered_map<int64_t, drogon::WebSocketConnectionPtr> _connections;
    static std::shared_mutex _mutex;

    void broadcastOnlineStatus(int64_t userId, bool online);
    void sendMessageToUser(int64_t userId, const std::string &message);
    int64_t authenticateConnection(const drogon::HttpRequestPtr &req);
};
