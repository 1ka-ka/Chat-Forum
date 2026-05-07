#include "FileService.h"
#include "../utils/ResponseUtil.h"
#include "../utils/ValidationUtil.h"
#include <drogon/drogon.h>
#include <chrono>
#include <random>
#include <filesystem>

std::string FileService::saveUploadedFile(const drogon::HttpRequestPtr &req,
                                           const std::string &fieldName,
                                           const std::string &subDir,
                                           int64_t userId,
                                           size_t maxSize)
{
    auto &files = req->getFiles();
    for (auto &file : files)
    {
        if (file.getFieldName() == fieldName)
        {
            std::string filename = file.getFileName();
            if (!ValidationUtil::isValidImageFile(filename))
            {
                return "";
            }

            if (file.fileLength() > maxSize)
            {
                return "";
            }

            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                                 now.time_since_epoch())
                                 .count();

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(1000, 9999);
            int randomNum = dis(gen);

            std::string ext = filename.substr(filename.find_last_of('.'));
            std::string newFilename = std::to_string(userId) + "_" +
                                      std::to_string(timestamp) + "_" +
                                      std::to_string(randomNum) + ext;

            std::string uploadDir = "./uploads/" + subDir;
            std::filesystem::create_directories(uploadDir);

            std::string filePath = uploadDir + "/" + newFilename;
            file.saveAs(filePath);

            return "/uploads/" + subDir + "/" + newFilename;
        }
    }
    return "";
}
