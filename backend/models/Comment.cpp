#include "Comment.h"

Json::Value Comment::toJson() const
{
    Json::Value val;
    val["id"] = static_cast<Json::Int64>(_id);
    val["postId"] = static_cast<Json::Int64>(_postId);
    val["userId"] = static_cast<Json::Int64>(_userId);

    if (_parentCommentId.has_value())
    {
        val["parentCommentId"] = static_cast<Json::Int64>(_parentCommentId.value());
    }
    else
    {
        val["parentCommentId"] = Json::nullValue;
    }

    val["content"] = _content;
    val["createdAt"] = _createdAt;
    return val;
}

Comment Comment::fromResult(const drogon::orm::Row& row)
{
    Comment comment;
    comment._id = row["id"].as<int64_t>();
    comment._postId = row["post_id"].as<int64_t>();
    comment._userId = row["user_id"].as<int64_t>();

    if (!row["parent_comment_id"].isNull())
    {
        comment._parentCommentId = row["parent_comment_id"].as<int64_t>();
    }

    comment._content = row["content"].as<std::string>();
    comment._createdAt = row["created_at"].as<std::string>();
    return comment;
}
