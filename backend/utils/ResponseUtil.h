#pragma once

#include <drogon/drogon.h>
#include <string>

class ResponseUtil {
public:
    static drogon::HttpResponsePtr success(const Json::Value &data = Json::Value(), const std::string &message = "操作成功");
    static drogon::HttpResponsePtr badRequest(const std::string &message);
    static drogon::HttpResponsePtr unauthorized(const std::string &message = "未授权");
    static drogon::HttpResponsePtr notFound(const std::string &message);
    static drogon::HttpResponsePtr conflict(const std::string &message);
    static drogon::HttpResponsePtr serverError(const std::string &message);
};
