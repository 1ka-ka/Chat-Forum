#pragma once

#include <drogon/HttpController.h>

class LikeController : public drogon::HttpController<LikeController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(LikeController::toggleLike, "/api/posts/{1}/like", drogon::Post, drogon::Options);
    METHOD_LIST_END

    void toggleLike(const drogon::HttpRequestPtr &req,
                    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                    int64_t postId);
};
