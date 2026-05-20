#pragma once

#include <functional>
#include <drogon/drogon.h>

class LikeService
{
public:
    using Callback = std::function<void(const drogon::HttpResponsePtr &)>;

    void toggleLike(int64_t postId, int64_t userId, Callback &&callback);

    void getLikeUsers(int64_t postId, Callback &&callback);
};
