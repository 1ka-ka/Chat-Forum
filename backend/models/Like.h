#pragma once

#include <string>
#include <json/json.h>

class Like
{
public:
    int64_t id = 0;
    int64_t postId = 0;
    int64_t userId = 0;
    std::string createdAt;

    Json::Value toJson() const;
    static Like fromRow(const Json::Value &row);
};
