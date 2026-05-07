#include "ChatService.h"
#include "../utils/ResponseUtil.h"
#include <drogon/drogon.h>

void ChatService::getConversations(int64_t userId, Callback &&callback)
{
    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT "
        "CASE WHEN m.sender_id = ? THEN m.receiver_id ELSE m.sender_id END AS other_user_id, "
        "u.nickname, u.avatar_url, "
        "m.content AS last_message, "
        "m.created_at AS last_time, "
        "(SELECT COUNT(*) FROM messages m2 "
        " WHERE m2.sender_id = other_user_id AND m2.receiver_id = ? AND m2.is_read = 0) AS unread_count "
        "FROM messages m "
        "INNER JOIN ("
        "  SELECT LEAST(sender_id, receiver_id) AS min_id, "
        "         GREATEST(sender_id, receiver_id) AS max_id, "
        "         MAX(id) AS max_msg_id "
        "  FROM messages "
        "  WHERE sender_id = ? OR receiver_id = ? "
        "  GROUP BY min_id, max_id"
        ") latest ON m.id = latest.max_msg_id "
        "LEFT JOIN users u ON u.id = CASE WHEN m.sender_id = ? THEN m.receiver_id ELSE m.sender_id END "
        "ORDER BY m.created_at DESC",
        [callback = std::move(callback)](const drogon::orm::Result &result) mutable {
            Json::Value items(Json::arrayValue);
            for (size_t i = 0; i < result.size(); ++i)
            {
                auto &r = result[i];
                Json::Value item;
                item["user_id"] = r["other_user_id"].as<int64_t>();
                item["nickname"] = std::string(r["nickname"].c_str());
                item["avatar_url"] = std::string(r["avatar_url"].c_str());
                item["last_message"] = std::string(r["last_message"].c_str());
                item["last_time"] = std::string(r["last_time"].c_str());
                item["unread_count"] = r["unread_count"].as<int>();
                items.append(item);
            }
            callback(ResponseUtil::success(items));
        },
        [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
            callback(ResponseUtil::serverError("查询会话列表失败"));
        },
        userId, userId, userId, userId, userId);
}

void ChatService::getMessages(int64_t userId, int64_t otherUserId,
                               int page, int pageSize,
                               Callback &&callback)
{
    if (page < 1) page = 1;
    if (pageSize < 1 || pageSize > 100) pageSize = 20;
    int offset = (page - 1) * pageSize;

    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT COUNT(*) AS total FROM messages "
        "WHERE (sender_id = ? AND receiver_id = ?) OR (sender_id = ? AND receiver_id = ?)",
        [this, userId, otherUserId, page, pageSize, callback = std::move(callback)](const drogon::orm::Result &countResult) mutable {
            int total = countResult[0]["total"].as<int>();
            int totalPages = (total + pageSize - 1) / pageSize;
            int offset = (page - 1) * pageSize;

            auto db = drogon::app().getDbClient();
            db->execSqlAsync(
                "SELECT id, sender_id, receiver_id, content, is_read, created_at "
                "FROM messages "
                "WHERE (sender_id = ? AND receiver_id = ?) OR (sender_id = ? AND receiver_id = ?) "
                "ORDER BY created_at DESC LIMIT ? OFFSET ?",
                [page, pageSize, total, totalPages, callback = std::move(callback)](const drogon::orm::Result &result) mutable {
                    Json::Value items(Json::arrayValue);
                    for (size_t i = 0; i < result.size(); ++i)
                    {
                        auto &r = result[i];
                        Json::Value item;
                        item["id"] = r["id"].as<int64_t>();
                        item["sender_id"] = r["sender_id"].as<int64_t>();
                        item["receiver_id"] = r["receiver_id"].as<int64_t>();
                        item["content"] = std::string(r["content"].c_str());
                        item["is_read"] = r["is_read"].as<int>();
                        item["created_at"] = std::string(r["created_at"].c_str());
                        items.append(item);
                    }
                    Json::Value data;
                    data["items"] = items;
                    data["total"] = total;
                    data["page"] = page;
                    data["page_size"] = pageSize;
                    data["total_pages"] = totalPages;
                    callback(ResponseUtil::success(data));
                },
                [callback = std::move(callback)](const drogon::orm::DrogonDbException &) mutable {
                    callback(ResponseUtil::serverError("查询消息失败"));
                },
                userId, otherUserId, otherUserId, userId, pageSize, offset);
        },
        [callback = std::move(callback)](const drogon::orm::DrogonDbException &) mutable {
            callback(ResponseUtil::serverError("查询消息数量失败"));
        },
        userId, otherUserId, otherUserId, userId);
}

void ChatService::markAsRead(int64_t userId, int64_t otherUserId, Callback &&callback)
{
    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "UPDATE messages SET is_read = 1 WHERE sender_id = ? AND receiver_id = ? AND is_read = 0",
        [callback = std::move(callback)](const drogon::orm::Result &) mutable {
            callback(ResponseUtil::success("已标记为已读"));
        },
        [callback = std::move(callback)](const drogon::orm::DrogonDbException &) mutable {
            callback(ResponseUtil::serverError("标记已读失败"));
        },
        otherUserId, userId);
}

void ChatService::saveMessage(int64_t senderId, int64_t receiverId,
                               const std::string &content,
                               std::function<void(int64_t)> &&onSuccess,
                               std::function<void()> &&onError)
{
    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "INSERT INTO messages (sender_id, receiver_id, content) VALUES (?, ?, ?)",
        [onSuccess = std::move(onSuccess)](const drogon::orm::Result &result) mutable {
            onSuccess(result.insertId());
        },
        [onError = std::move(onError)](const drogon::orm::DrogonDbException &) mutable {
            onError();
        },
        senderId, receiverId, content);
}
