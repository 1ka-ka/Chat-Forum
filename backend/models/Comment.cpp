#include "Comment.h"

Json::Value Comment::toJson() const
{
    Json::Value val;
    val["id"] = id;
    val["post_id"] = postId;
    val["content"] = content;
    val["user_id"] = userId;
    val["nickname"] = nickname;
    val["avatar_url"] = avatarUrl;
    val["created_at"] = createdAt;
    return val;
}

Comment Comment::fromRow(const Json::Value &row)
{
    Comment c;
    c.id = row.isMember("id") ? row["id"].asInt64() : 0;
    c.postId = row.isMember("post_id") ? row["post_id"].asInt64() : 0;
    c.userId = row.isMember("user_id") ? row["user_id"].asInt64() : 0;
    c.content = row.isMember("content") ? row["content"].asString() : "";
    c.createdAt = row.isMember("created_at") ? row["created_at"].asString() : "";
    c.nickname = row.isMember("nickname") ? row["nickname"].asString() : "";
    c.avatarUrl = row.isMember("avatar_url") ? row["avatar_url"].asString() : "";
    return c;
}
