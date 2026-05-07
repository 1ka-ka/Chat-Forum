#include "Message.h"

Json::Value Message::toJson() const
{
    Json::Value val;
    val["id"] = id;
    val["sender_id"] = senderId;
    val["receiver_id"] = receiverId;
    val["content"] = content;
    val["is_read"] = isRead;
    val["created_at"] = createdAt;
    return val;
}

Message Message::fromRow(const Json::Value &row)
{
    Message m;
    m.id = row.isMember("id") ? row["id"].asInt64() : 0;
    m.senderId = row.isMember("sender_id") ? row["sender_id"].asInt64() : 0;
    m.receiverId = row.isMember("receiver_id") ? row["receiver_id"].asInt64() : 0;
    m.content = row.isMember("content") ? row["content"].asString() : "";
    m.isRead = row.isMember("is_read") ? row["is_read"].asInt() : 0;
    m.createdAt = row.isMember("created_at") ? row["created_at"].asString() : "";
    return m;
}
