#pragma once

#include <string>
#include <vector>
#include <json/json.h>

class Post
{
public:
    int64_t id = 0;
    int64_t userId = 0;
    std::string title;
    std::string content;
    std::vector<std::string> imageUrls;
    int likeCount = 0;
    int commentCount = 0;
    std::string createdAt;
    std::string updatedAt;

    std::string nickname;
    std::string avatarUrl;
    bool isLiked = false;

    Json::Value toJson(bool truncateContent = false) const;
    static Post fromRow(const Json::Value &row);
};
