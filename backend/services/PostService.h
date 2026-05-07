#pragma once

#include <string>
#include <functional>
#include <json/json.h>
#include <drogon/HttpResponse.h>

class PostService
{
public:
    using Callback = std::function<void(const drogon::HttpResponsePtr &)>;

    void createPost(int64_t userId,
                    const std::string &title,
                    const std::string &content,
                    const std::vector<std::string> &imageUrls,
                    Callback &&callback);

    void getPostList(int page, int pageSize,
                     int64_t currentUserId,
                     Callback &&callback);

    void getPostById(int64_t postId,
                     int64_t currentUserId,
                     Callback &&callback);
};
