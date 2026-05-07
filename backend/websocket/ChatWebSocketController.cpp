#include "ChatWebSocketController.h"
#include "../services/ChatService.h"
#include "../utils/JwtUtil.h"
#include <json/json.h>
#include <drogon/drogon.h>

std::unordered_map<int64_t, drogon::WebSocketConnectionPtr> ChatWebSocketController::_connections;
std::shared_mutex ChatWebSocketController::_mutex;

void ChatWebSocketController::handleNewConnection(const drogon::HttpRequestPtr &req,
                                                    const drogon::WebSocketConnectionPtr &conn)
{
    int64_t userId = authenticateConnection(req);
    if (userId <= 0)
    {
        conn->forceClose();
        return;
    }

    conn->setContext(std::make_shared<int64_t>(userId));

    {
        std::unique_lock<std::shared_mutex> lock(_mutex);
        _connections[userId] = conn;
    }

    broadcastOnlineStatus(userId, true);

    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT DISTINCT sender_id FROM messages WHERE receiver_id = ? AND is_read = 0 GROUP BY sender_id",
        [this](const drogon::orm::Result &result) {},
        [](const drogon::orm::DrogonDbException &) {},
        userId);
}

void ChatWebSocketController::handleNewMessage(const drogon::WebSocketConnectionPtr &conn,
                                                 std::string &&message)
{
    auto ctx = conn->getContext<int64_t>();
    if (!ctx)
    {
        return;
    }
    int64_t senderId = *ctx;

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::istringstream stream(message);
    std::string errs;
    if (!Json::parseFromStream(builder, stream, &root, &errs))
    {
        return;
    }

    std::string type = root.get("type", "").asString();

    if (type == "ping")
    {
        Json::Value pong;
        pong["type"] = "pong";
        Json::StreamWriterBuilder wb;
        conn->send(Json::writeString(wb, pong));
        return;
    }

    if (type == "message")
    {
        int64_t receiverId = root.get("receiver_id", 0).asInt64();
        std::string content = root.get("content", "").asString();

        if (receiverId <= 0 || content.empty())
        {
            return;
        }

        ChatService chatService;
        chatService.saveMessage(
            senderId, receiverId, content,
            [this, senderId, receiverId, content](int64_t msgId) {
                auto dbClient = drogon::app().getDbClient();
                dbClient->execSqlAsync(
                    "SELECT u.nickname, u.avatar_url FROM users u WHERE u.id = ?",
                    [this, senderId, receiverId, content, msgId](const drogon::orm::Result &result) {
                        std::string nickname = "未知用户";
                        std::string avatarUrl = "";
                        if (!result.empty())
                        {
                            nickname = std::string(result[0]["nickname"].c_str());
                            avatarUrl = std::string(result[0]["avatar_url"].c_str());
                        }

                        Json::Value msg;
                        msg["type"] = "message";
                        msg["sender_id"] = senderId;
                        msg["sender_nickname"] = nickname;
                        msg["sender_avatar"] = avatarUrl;
                        msg["content"] = content;
                        msg["message_id"] = msgId;

                        auto now = std::chrono::system_clock::now();
                        auto time_t_now = std::chrono::system_clock::to_time_t(now);
                        std::stringstream ss;
                        ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
                        msg["created_at"] = ss.str();

                        Json::StreamWriterBuilder wb;
                        std::string msgStr = Json::writeString(wb, msg);
                        sendMessageToUser(receiverId, msgStr);
                        sendMessageToUser(senderId, msgStr);
                    },
                    [](const drogon::orm::DrogonDbException &) {},
                    senderId);
            },
            []() {});
    }
}

void ChatWebSocketController::handleConnectionClosed(const drogon::WebSocketConnectionPtr &conn)
{
    auto ctx = conn->getContext<int64_t>();
    if (!ctx)
    {
        return;
    }
    int64_t userId = *ctx;

    {
        std::unique_lock<std::shared_mutex> lock(_mutex);
        _connections.erase(userId);
    }

    broadcastOnlineStatus(userId, false);
}

int64_t ChatWebSocketController::authenticateConnection(const drogon::HttpRequestPtr &req)
{
    std::string token = std::string(req->getParameter("token"));
    if (token.empty())
    {
        std::string authHeader = std::string(req->getHeader("Authorization"));
        if (!authHeader.empty() && authHeader.substr(0, 7) == "Bearer ")
        {
            token = authHeader.substr(7);
        }
    }

    if (token.empty())
    {
        return -1;
    }

    std::string secret = getJwtSecret();
    if (!JwtUtil::verifyToken(token, secret))
    {
        return -1;
    }

    return JwtUtil::getUserIdFromToken(token, secret);
}

void ChatWebSocketController::broadcastOnlineStatus(int64_t userId, bool online)
{
    Json::Value msg;
    msg["type"] = "online_status";
    msg["user_id"] = userId;
    msg["online"] = online;

    Json::StreamWriterBuilder wb;
    std::string msgStr = Json::writeString(wb, msg);

    std::shared_lock<std::shared_mutex> lock(_mutex);
    for (auto &[id, conn] : _connections)
    {
        if (id != userId)
        {
            conn->send(msgStr);
        }
    }
}

void ChatWebSocketController::sendMessageToUser(int64_t userId, const std::string &message)
{
    std::shared_lock<std::shared_mutex> lock(_mutex);
    auto it = _connections.find(userId);
    if (it != _connections.end())
    {
        it->second->send(message);
    }
}

std::string ChatWebSocketController::getJwtSecret() const
{
    auto &config = drogon::app().getCustomConfig();
    if (config.isMember("jwt") && config["jwt"].isMember("secret"))
    {
        return config["jwt"]["secret"].asString();
    }
    return "chatforum-secret-key-change-in-production-2025";
}
