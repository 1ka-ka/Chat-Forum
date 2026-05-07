#include "Like.h"

Json::Value Like::toJson() const
{
    Json::Value val;
    val["id"] = id;
    val["post_id"] = postId;
    val["user_id"] = userId;
    val["created_at"] = createdAt;
    return val;
}

Like Like::fromRow(const Json::Value &row)
{
    Like l;
    l.id = row.isMember("id") ? row["id"].asInt64() : 0;
    l.postId = row.isMember("post_id") ? row["post_id"].asInt64() : 0;
    l.userId = row.isMember("user_id") ? row["user_id"].asInt64() : 0;
    l.createdAt = row.isMember("created_at") ? row["created_at"].asString() : "";
    return l;
}
