#pragma once

#include <string>
#include <functional>
#include <drogon/drogon.h>

class FileService
{
public:
    using Callback = std::function<void(const drogon::HttpResponsePtr &)>;

    void saveUploadFile(int64_t userId,
                        const std::string &filename,
                        const std::string &fileContent,
                        const std::string &uploadDir,
                        Callback &&callback);

    static bool isValidImageExtension(const std::string &filename);
};
