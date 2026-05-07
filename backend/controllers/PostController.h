#pragma once

#include <drogon/HttpController.h>

class PostController : public drogon::HttpController<PostController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(PostController::createPost, "/api/posts", drogon::Post, drogon::Options);
    ADD_METHOD_TO(PostController::getPostList, "/api/posts", drogon::Get, drogon::Options);
    ADD_METHOD_TO(PostController::getPostById, "/api/posts/{1}", drogon::Get, drogon::Options);
    METHOD_LIST_END

    void createPost(const drogon::HttpRequestPtr &req,
                    std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void getPostList(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void getPostById(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                     int64_t postId);
};
