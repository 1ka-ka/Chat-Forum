#pragma once

#include <string>
#include <drogon/HttpRequest.h>
#include <json/json.h>

namespace ResponseUtil
{
    drogon::HttpResponsePtr success(const Json::Value &data = Json::Value(), const std::string &message = "success");
    drogon::HttpResponsePtr success(const std::string &message = "success");
    drogon::HttpResponsePtr error(int code, const std::string &message);
    drogon::HttpResponsePtr badRequest(const std::string &message);
    drogon::HttpResponsePtr unauthorized(const std::string &message = "未认证或Token失效");
    drogon::HttpResponsePtr forbidden(const std::string &message = "无权限");
    drogon::HttpResponsePtr notFound(const std::string &message = "资源不存在");
    drogon::HttpResponsePtr conflict(const std::string &message);
    drogon::HttpResponsePtr serverError(const std::string &message = "服务器内部错误");
}
