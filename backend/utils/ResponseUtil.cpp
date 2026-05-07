#include "ResponseUtil.h"

namespace ResponseUtil
{
    drogon::HttpResponsePtr success(const Json::Value &data, const std::string &message)
    {
        Json::Value root;
        root["code"] = 200;
        root["message"] = message;
        root["data"] = data;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(root);
        resp->setStatusCode(drogon::k200OK);
        return resp;
    }

    drogon::HttpResponsePtr success(const std::string &message)
    {
        Json::Value root;
        root["code"] = 200;
        root["message"] = message;
        root["data"] = Json::nullValue;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(root);
        resp->setStatusCode(drogon::k200OK);
        return resp;
    }

    drogon::HttpResponsePtr error(int code, const std::string &message)
    {
        Json::Value root;
        root["code"] = code;
        root["message"] = message;
        root["data"] = Json::nullValue;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(root);
        switch (code)
        {
        case 400:
            resp->setStatusCode(drogon::k400BadRequest);
            break;
        case 401:
            resp->setStatusCode(drogon::k401Unauthorized);
            break;
        case 403:
            resp->setStatusCode(drogon::k403Forbidden);
            break;
        case 404:
            resp->setStatusCode(drogon::k404NotFound);
            break;
        case 409:
            resp->setStatusCode(drogon::k409Conflict);
            break;
        default:
            resp->setStatusCode(drogon::k500InternalServerError);
            break;
        }
        return resp;
    }

    drogon::HttpResponsePtr badRequest(const std::string &message)
    {
        return error(400, message);
    }

    drogon::HttpResponsePtr unauthorized(const std::string &message)
    {
        return error(401, message);
    }

    drogon::HttpResponsePtr forbidden(const std::string &message)
    {
        return error(403, message);
    }

    drogon::HttpResponsePtr notFound(const std::string &message)
    {
        return error(404, message);
    }

    drogon::HttpResponsePtr conflict(const std::string &message)
    {
        return error(409, message);
    }

    drogon::HttpResponsePtr serverError(const std::string &message)
    {
        return error(500, message);
    }
}
