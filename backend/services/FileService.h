#pragma once

#include <string>
#include <functional>
#include <drogon/HttpResponse.h>

class FileService
{
public:
    using Callback = std::function<void(const drogon::HttpResponsePtr &)>;

    std::string saveUploadedFile(const drogon::HttpRequestPtr &req,
                                 const std::string &fieldName,
                                 const std::string &subDir,
                                 int64_t userId,
                                 size_t maxSize);
};
