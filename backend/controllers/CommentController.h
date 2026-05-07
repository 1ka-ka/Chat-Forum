#pragma once

#include <drogon/HttpController.h>

class CommentController : public drogon::HttpController<CommentController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(CommentController::createComment, "/api/posts/{1}/comments", drogon::Post, drogon::Options);
    ADD_METHOD_TO(CommentController::getComments, "/api/posts/{1}/comments", drogon::Get, drogon::Options);
    METHOD_LIST_END

    void createComment(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                       int64_t postId);

    void getComments(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                     int64_t postId);
};
