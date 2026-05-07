#pragma once

#include <string>
#include <functional>
#include <json/json.h>
#include <drogon/HttpResponse.h>

class LikeService
{
public:
    using Callback = std::function<void(const drogon::HttpResponsePtr &)>;

    void toggleLike(int64_t postId,
                    int64_t userId,
                    Callback &&callback);
};
