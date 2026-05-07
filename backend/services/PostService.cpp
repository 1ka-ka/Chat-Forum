#include "PostService.h"
#include "../models/Post.h"
#include "../utils/ResponseUtil.h"
#include "../utils/ValidationUtil.h"
#include <drogon/drogon.h>
#include <sstream>

void PostService::createPost(int64_t userId,
                              const std::string &title,
                              const std::string &content,
                              const std::vector<std::string> &imageUrls,
                              Callback &&callback)
{
    if (!ValidationUtil::isValidPostTitle(title))
    {
        callback(ResponseUtil::badRequest("标题格式不正确"));
        return;
    }
    if (!ValidationUtil::isValidPostContent(content))
    {
        callback(ResponseUtil::badRequest("内容不能为空"));
        return;
    }

    Json::Value imgs(Json::arrayValue);
    for (const auto &url : imageUrls)
    {
        imgs.append(url);
    }
    Json::StreamWriterBuilder builder;
    std::string imageUrlsJson = Json::writeString(builder, imgs);
    if (imageUrls.empty())
    {
        imageUrlsJson = "NULL";
    }

    auto dbClient = drogon::app().getDbClient();
    std::string sql = imageUrls.empty()
                          ? "INSERT INTO posts (user_id, title, content) VALUES (?, ?, ?)"
                          : "INSERT INTO posts (user_id, title, content, image_urls) VALUES (?, ?, ?, ?)";

    if (imageUrls.empty())
    {
        dbClient->execSqlAsync(
            "INSERT INTO posts (user_id, title, content) VALUES (?, ?, ?)",
            [this, userId, callback = std::move(callback)](const drogon::orm::Result &result) mutable {
                int64_t postId = result.insertId();
                getPostById(postId, userId, std::move(callback));
            },
            [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
                callback(ResponseUtil::serverError("发帖失败"));
            },
            userId, title, content);
    }
    else
    {
        dbClient->execSqlAsync(
            "INSERT INTO posts (user_id, title, content, image_urls) VALUES (?, ?, ?, ?)",
            [this, userId, callback = std::move(callback)](const drogon::orm::Result &result) mutable {
                int64_t postId = result.insertId();
                getPostById(postId, userId, std::move(callback));
            },
            [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
                callback(ResponseUtil::serverError("发帖失败"));
            },
            userId, title, content, imageUrlsJson);
    }
}

void PostService::getPostList(int page, int pageSize,
                               int64_t currentUserId,
                               Callback &&callback)
{
    if (page < 1) page = 1;
    if (pageSize < 1 || pageSize > 100) pageSize = 20;
    int offset = (page - 1) * pageSize;

    auto dbClient = drogon::app().getDbClient();

    std::string countSql = "SELECT COUNT(*) AS total FROM posts";
    std::string listSql = "SELECT p.id, p.user_id, p.title, p.content, p.image_urls, "
                           "p.like_count, p.comment_count, p.created_at, "
                           "u.nickname, u.avatar_url, "
                           "EXISTS(SELECT 1 FROM likes WHERE post_id = p.id AND user_id = ?) AS is_liked "
                           "FROM posts p "
                           "LEFT JOIN users u ON p.user_id = u.id "
                           "ORDER BY p.created_at DESC LIMIT ? OFFSET ?";

    dbClient->execSqlAsync(
        countSql,
        [this, listSql, currentUserId, page, pageSize, callback = std::move(callback)](const drogon::orm::Result &countResult) mutable {
            int total = countResult[0]["total"].as<int>();
            int totalPages = (total + pageSize - 1) / pageSize;

            auto db = drogon::app().getDbClient();
            db->execSqlAsync(
                listSql,
                [currentUserId, page, pageSize, total, totalPages, callback = std::move(callback)](const drogon::orm::Result &result) mutable {
                    Json::Value items(Json::arrayValue);
                    for (size_t i = 0; i < result.size(); ++i)
                    {
                        Json::Value row;
                        row["id"] = result[i]["id"].as<int64_t>();
                        row["user_id"] = result[i]["user_id"].as<int64_t>();
                        row["title"] = std::string(result[i]["title"].c_str());
                        std::string content = std::string(result[i]["content"].c_str());
                        if (content.length() > 100)
                            content = content.substr(0, 100) + "...";
                        row["content"] = content;
                        row["image_urls"] = result[i]["image_urls"].isNull()
                                                ? Json::Value(Json::arrayValue)
                                                : result[i]["image_urls"];
                        row["like_count"] = result[i]["like_count"].as<int>();
                        row["comment_count"] = result[i]["comment_count"].as<int>();
                        row["nickname"] = std::string(result[i]["nickname"].c_str());
                        row["avatar_url"] = std::string(result[i]["avatar_url"].c_str());
                        row["is_liked"] = result[i]["is_liked"].as<int>() > 0;
                        row["created_at"] = std::string(result[i]["created_at"].c_str());
                        items.append(row);
                    }

                    Json::Value data;
                    data["items"] = items;
                    data["total"] = total;
                    data["page"] = page;
                    data["page_size"] = pageSize;
                    data["total_pages"] = totalPages;
                    callback(ResponseUtil::success(data));
                },
                [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
                    callback(ResponseUtil::serverError("查询帖子列表失败"));
                },
                currentUserId, pageSize, offset);
        },
        [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
            callback(ResponseUtil::serverError("查询帖子数量失败"));
        });
}

void PostService::getPostById(int64_t postId, int64_t currentUserId, Callback &&callback)
{
    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT p.id, p.user_id, p.title, p.content, p.image_urls, "
        "p.like_count, p.comment_count, p.created_at, "
        "u.nickname, u.avatar_url, "
        "EXISTS(SELECT 1 FROM likes WHERE post_id = p.id AND user_id = ?) AS is_liked "
        "FROM posts p "
        "LEFT JOIN users u ON p.user_id = u.id "
        "WHERE p.id = ?",
        [callback = std::move(callback)](const drogon::orm::Result &result) mutable {
            if (result.empty())
            {
                callback(ResponseUtil::notFound("帖子不存在"));
                return;
            }
            auto &r = result[0];
            Json::Value data;
            data["id"] = r["id"].as<int64_t>();
            data["user_id"] = r["user_id"].as<int64_t>();
            data["title"] = std::string(r["title"].c_str());
            data["content"] = std::string(r["content"].c_str());
            data["image_urls"] = r["image_urls"].isNull()
                                     ? Json::Value(Json::arrayValue)
                                     : r["image_urls"];
            data["like_count"] = r["like_count"].as<int>();
            data["comment_count"] = r["comment_count"].as<int>();
            data["nickname"] = std::string(r["nickname"].c_str());
            data["avatar_url"] = std::string(r["avatar_url"].c_str());
            data["is_liked"] = r["is_liked"].as<int>() > 0;
            data["created_at"] = std::string(r["created_at"].c_str());
            callback(ResponseUtil::success(data));
        },
        [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
            callback(ResponseUtil::serverError("查询帖子失败"));
        },
        currentUserId, postId);
}
