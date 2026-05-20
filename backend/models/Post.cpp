#include "Post.h"

Json::Value Post::toJson() const
{
    Json::Value val;
    val["id"] = static_cast<Json::Int64>(_id);
    val["userId"] = static_cast<Json::Int64>(_userId);
    val["title"] = _title;
    val["content"] = _content;

    if (_imageUrls.has_value())
    {
        Json::Value images;
        Json::CharReaderBuilder builder;
        std::istringstream stream(_imageUrls.value());
        std::string errors;
        if (Json::parseFromStream(builder, stream, &images, &errors))
        {
            val["imageUrls"] = images;
        }
        else
        {
            val["imageUrls"] = Json::arrayValue;
        }
    }
    else
    {
        val["imageUrls"] = Json::arrayValue;
    }

    val["likeCount"] = _likeCount;
    val["commentCount"] = _commentCount;
    val["createdAt"] = _createdAt;
    val["updatedAt"] = _updatedAt;
    return val;
}

Post Post::fromResult(const drogon::orm::Row& row)
{
    Post post;
    post._id = row["id"].as<int64_t>();
    post._userId = row["user_id"].as<int64_t>();
    post._title = row["title"].as<std::string>();
    post._content = row["content"].as<std::string>();

    if (!row["image_urls"].isNull())
    {
        post._imageUrls = row["image_urls"].as<std::string>();
    }

    post._likeCount = row["like_count"].as<int>();
    post._commentCount = row["comment_count"].as<int>();
    post._createdAt = row["created_at"].as<std::string>();
    post._updatedAt = row["updated_at"].as<std::string>();
    return post;
}
