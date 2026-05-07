#include "CommentService.h"
#include "../utils/ResponseUtil.h"
#include "../utils/ValidationUtil.h"
#include <drogon/drogon.h>

void CommentService::createComment(int64_t postId,
                                    int64_t userId,
                                    const std::string &content,
                                    Callback &&callback)
{
    if (!ValidationUtil::isValidCommentContent(content))
    {
        callback(ResponseUtil::badRequest("评论内容格式不正确，需1-1000位字符"));
        return;
    }

    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "INSERT INTO comments (post_id, user_id, content) VALUES (?, ?, ?)",
        [postId, userId, callback = std::move(callback)](const drogon::orm::Result &result) mutable {
            int64_t commentId = result.insertId();
            auto db = drogon::app().getDbClient();
            db->execSqlAsync(
                "UPDATE posts SET comment_count = comment_count + 1 WHERE id = ?",
                [commentId, callback = std::move(callback)](const drogon::orm::Result &) mutable {},
                [](const drogon::orm::DrogonDbException &e) {},
                postId);

            db->execSqlAsync(
                "SELECT c.id, c.post_id, c.content, c.user_id, c.created_at, "
                "u.nickname, u.avatar_url "
                "FROM comments c LEFT JOIN users u ON c.user_id = u.id "
                "WHERE c.id = ?",
                [callback = std::move(callback)](const drogon::orm::Result &result) mutable {
                    if (result.empty())
                    {
                        callback(ResponseUtil::serverError("评论创建失败"));
                        return;
                    }
                    auto &r = result[0];
                    Json::Value data;
                    data["id"] = r["id"].as<int64_t>();
                    data["post_id"] = r["post_id"].as<int64_t>();
                    data["content"] = std::string(r["content"].c_str());
                    data["user_id"] = r["user_id"].as<int64_t>();
                    data["nickname"] = std::string(r["nickname"].c_str());
                    data["avatar_url"] = std::string(r["avatar_url"].c_str());
                    data["created_at"] = std::string(r["created_at"].c_str());
                    callback(ResponseUtil::success(data, "评论成功"));
                },
                [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
                    callback(ResponseUtil::serverError("查询评论失败"));
                },
                commentId);
        },
        [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
            callback(ResponseUtil::serverError("评论失败"));
        },
        postId, userId, content);
}

void CommentService::getCommentsByPostId(int64_t postId,
                                          int page, int pageSize,
                                          Callback &&callback)
{
    if (page < 1) page = 1;
    if (pageSize < 1 || pageSize > 100) pageSize = 20;
    int offset = (page - 1) * pageSize;

    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT COUNT(*) AS total FROM comments WHERE post_id = ?",
        [this, postId, page, pageSize, callback = std::move(callback)](const drogon::orm::Result &countResult) mutable {
            int total = countResult[0]["total"].as<int>();
            int totalPages = (total + pageSize - 1) / pageSize;
            int offset = (page - 1) * pageSize;

            auto db = drogon::app().getDbClient();
            db->execSqlAsync(
                "SELECT c.id, c.post_id, c.content, c.user_id, c.created_at, "
                "u.nickname, u.avatar_url "
                "FROM comments c LEFT JOIN users u ON c.user_id = u.id "
                "WHERE c.post_id = ? ORDER BY c.created_at ASC LIMIT ? OFFSET ?",
                [page, pageSize, total, totalPages, callback = std::move(callback)](const drogon::orm::Result &result) mutable {
                    Json::Value items(Json::arrayValue);
                    for (size_t i = 0; i < result.size(); ++i)
                    {
                        auto &r = result[i];
                        Json::Value item;
                        item["id"] = r["id"].as<int64_t>();
                        item["post_id"] = r["post_id"].as<int64_t>();
                        item["content"] = std::string(r["content"].c_str());
                        item["user_id"] = r["user_id"].as<int64_t>();
                        item["nickname"] = std::string(r["nickname"].c_str());
                        item["avatar_url"] = std::string(r["avatar_url"].c_str());
                        item["created_at"] = std::string(r["created_at"].c_str());
                        items.append(item);
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
                    callback(ResponseUtil::serverError("查询评论失败"));
                },
                postId, pageSize, offset);
        },
        [callback = std::move(callback)](const drogon::orm::DrogonDbException &e) mutable {
            callback(ResponseUtil::serverError("查询评论数量失败"));
        },
        postId);
}
