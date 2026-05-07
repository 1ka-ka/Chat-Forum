#pragma once

#include <string>
#include <json/json.h>

class User
{
public:
    int64_t id = 0;
    std::string username;
    std::string passwordHash;
    std::string nickname;
    std::string avatarUrl;
    std::string createdAt;
    std::string updatedAt;

    Json::Value toJson() const;
    Json::Value toPublicJson(int postCount = 0) const;
    static User fromRow(const Json::Value &row);
};
