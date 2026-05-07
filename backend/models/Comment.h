#pragma once

#include <string>
#include <json/json.h>

class Comment
{
public:
    int64_t id = 0;
    int64_t postId = 0;
    int64_t userId = 0;
    std::string content;
    std::string createdAt;

    std::string nickname;
    std::string avatarUrl;

    Json::Value toJson() const;
    static Comment fromRow(const Json::Value &row);
};
