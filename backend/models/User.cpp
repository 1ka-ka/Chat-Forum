#include "User.h"

Json::Value User::toJson() const
{
    Json::Value val;
    val["id"] = static_cast<Json::Int64>(_id);
    val["username"] = _username;
    val["nickname"] = _nickname;
    val["avatarUrl"] = _avatarUrl;
    val["createdAt"] = _createdAt;
    val["updatedAt"] = _updatedAt;
    return val;
}

User User::fromResult(const drogon::orm::Row& row)
{
    User user;
    user._id = row["id"].as<int64_t>();
    user._username = row["username"].as<std::string>();
    user._passwordHash = row["password_hash"].as<std::string>();
    user._nickname = row["nickname"].as<std::string>();
    user._avatarUrl = row["avatar_url"].as<std::string>();
    user._createdAt = row["created_at"].as<std::string>();
    user._updatedAt = row["updated_at"].as<std::string>();
    return user;
}
