#include "User.h"

Json::Value User::toJson() const
{
    Json::Value val;
    val["id"] = id;
    val["username"] = username;
    val["nickname"] = nickname;
    val["avatar_url"] = avatarUrl;
    val["created_at"] = createdAt;
    return val;
}

Json::Value User::toPublicJson(int postCount) const
{
    Json::Value val;
    val["id"] = id;
    val["nickname"] = nickname;
    val["avatar_url"] = avatarUrl;
    val["post_count"] = postCount;
    val["created_at"] = createdAt;
    return val;
}

User User::fromRow(const Json::Value &row)
{
    User u;
    u.id = row.isMember("id") ? row["id"].asInt64() : 0;
    u.username = row.isMember("username") ? row["username"].asString() : "";
    u.passwordHash = row.isMember("password_hash") ? row["password_hash"].asString() : "";
    u.nickname = row.isMember("nickname") ? row["nickname"].asString() : "";
    u.avatarUrl = row.isMember("avatar_url") ? row["avatar_url"].asString() : "";
    u.createdAt = row.isMember("created_at") ? row["created_at"].asString() : "";
    u.updatedAt = row.isMember("updated_at") ? row["updated_at"].asString() : "";
    return u;
}
