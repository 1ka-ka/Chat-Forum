#pragma once

#include <string>
#include <functional>
#include <drogon/drogon.h>

class CommentService
{
public:
    using Callback = std::function<void(const drogon::HttpResponsePtr &)>;

    void createComment(int64_t postId,
                       int64_t userId,
                       const std::string &content,
                       int64_t parentCommentId,
                       Callback &&callback);

    void getComments(int64_t postId, int page, int pageSize, Callback &&callback);
};
