#pragma once

#include <string>
#include <cstdint>
#include <optional>
#include <json/json.h>
#include <drogon/orm/Row.h>

class Comment
{
public:
    int64_t getId() const { return _id; }
    void setId(int64_t id) { _id = id; }

    int64_t getPostId() const { return _postId; }
    void setPostId(int64_t postId) { _postId = postId; }

    int64_t getUserId() const { return _userId; }
    void setUserId(int64_t userId) { _userId = userId; }

    const std::optional<int64_t>& getParentCommentId() const { return _parentCommentId; }
    void setParentCommentId(const std::optional<int64_t>& parentCommentId) { _parentCommentId = parentCommentId; }

    const std::string& getContent() const { return _content; }
    void setContent(const std::string& content) { _content = content; }

    const std::string& getCreatedAt() const { return _createdAt; }
    void setCreatedAt(const std::string& createdAt) { _createdAt = createdAt; }

    Json::Value toJson() const;

    static Comment fromResult(const drogon::orm::Row& row);

private:
    int64_t _id = 0;
    int64_t _postId = 0;
    int64_t _userId = 0;
    std::optional<int64_t> _parentCommentId;
    std::string _content;
    std::string _createdAt;
};
