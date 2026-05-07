#pragma once

#include <string>
#include <functional>
#include <json/json.h>
#include <drogon/HttpResponse.h>

class CommentService
{
public:
    using Callback = std::function<void(const drogon::HttpResponsePtr &)>;

    void createComment(int64_t postId,
                       int64_t userId,
                       const std::string &content,
                       Callback &&callback);

    void getCommentsByPostId(int64_t postId,
                             int page, int pageSize,
                             Callback &&callback);
};
