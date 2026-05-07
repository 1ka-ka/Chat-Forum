#pragma once

#include <string>
#include <json/json.h>

class Message
{
public:
    int64_t id = 0;
    int64_t senderId = 0;
    int64_t receiverId = 0;
    std::string content;
    int isRead = 0;
    std::string createdAt;

    Json::Value toJson() const;
    static Message fromRow(const Json::Value &row);
};
