#include "Post.h"

Json::Value Post::toJson(bool truncateContent) const
{
    Json::Value val;
    val["id"] = id;
    val["title"] = title;
    std::string displayContent = content;
    if (truncateContent && displayContent.length() > 100)
    {
        displayContent = displayContent.substr(0, 100) + "...";
    }
    val["content"] = displayContent;
    Json::Value imgs(Json::arrayValue);
    for (const auto &url : imageUrls)
    {
        imgs.append(url);
    }
    val["image_urls"] = imgs;
    val["user_id"] = userId;
    val["nickname"] = nickname;
    val["avatar_url"] = avatarUrl;
    val["like_count"] = likeCount;
    val["comment_count"] = commentCount;
    val["is_liked"] = isLiked;
    val["created_at"] = createdAt;
    return val;
}

Post Post::fromRow(const Json::Value &row)
{
    Post p;
    p.id = row.isMember("id") ? row["id"].asInt64() : 0;
    p.userId = row.isMember("user_id") ? row["user_id"].asInt64() : 0;
    p.title = row.isMember("title") ? row["title"].asString() : "";
    p.content = row.isMember("content") ? row["content"].asString() : "";
    if (row.isMember("image_urls") && !row["image_urls"].isNull())
    {
        std::string imgsStr = row["image_urls"].asString();
        if (!imgsStr.empty())
        {
            Json::Value imgs;
            Json::CharReaderBuilder builder;
            std::istringstream stream(imgsStr);
            std::string errs;
            if (Json::parseFromStream(builder, stream, &imgs, &errs))
            {
                if (imgs.isArray())
                {
                    for (const auto &img : imgs)
                    {
                        p.imageUrls.push_back(img.asString());
                    }
                }
            }
        }
    }
    p.likeCount = row.isMember("like_count") ? row["like_count"].asInt() : 0;
    p.commentCount = row.isMember("comment_count") ? row["comment_count"].asInt() : 0;
    p.createdAt = row.isMember("created_at") ? row["created_at"].asString() : "";
    p.updatedAt = row.isMember("updated_at") ? row["updated_at"].asString() : "";
    p.nickname = row.isMember("nickname") ? row["nickname"].asString() : "";
    p.avatarUrl = row.isMember("avatar_url") ? row["avatar_url"].asString() : "";
    p.isLiked = row.isMember("is_liked") ? row["is_liked"].asBool() : false;
    return p;
}
