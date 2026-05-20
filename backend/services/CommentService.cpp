#include "CommentService.h"
#include "../utils/ResponseUtil.h"
#include "../utils/ValidationUtil.h"
#include "../models/Comment.h"

void CommentService::createComment(int64_t postId, int64_t userId,
                                    const std::string &content, int64_t parentCommentId,
                                    Callback &&callback)
{
    if (!ValidationUtil::isValidCommentContent(content))
    {
        callback(ResponseUtil::badRequest("评论内容格式不正确"));
        return;
    }

    auto dbClient = drogon::app().getDbClient();

    std::string insertSql = parentCommentId > 0
        ? "INSERT INTO comments (post_id, user_id, parent_comment_id, content) VALUES (?, ?, ?, ?)"
        : "INSERT INTO comments (post_id, user_id, content) VALUES (?, ?, ?)";

    auto insertCallback =
        [postId, callback = std::move(callback)]
        (const drogon::orm::Result &result) mutable {
            int64_t commentId = result.insertId();
            auto db = drogon::app().getDbClient();

            // 更新评论计数（无需 callback，发射后不管）
            db->execSqlAsync(
                "UPDATE posts SET comment_count = comment_count + 1 WHERE id = ?",
                [](const drogon::orm::Result &) {},
                [](const drogon::orm::DrogonDbException &) {},
                postId);

            // 查询评论详情（持有 callback 的唯一所有权）
            db->execSqlAsync(
                "SELECT c.id, c.post_id, c.content, c.user_id, c.parent_comment_id, c.created_at, "
                "u.nickname, u.avatar_url "
                "FROM comments c LEFT JOIN users u ON c.user_id = u.id WHERE c.id = ?",
                [callback = std::move(callback)]
                (const drogon::orm::Result &result) mutable {
                    auto &r = result[0];
                    Json::Value data;
                    data["id"] = r["id"].as<int64_t>();
                    data["post_id"] = r["post_id"].as<int64_t>();
                    data["content"] = std::string(r["content"].c_str());
                    data["user_id"] = r["user_id"].as<int64_t>();
                    data["parent_comment_id"] = r["parent_comment_id"].isNull()
                                                    ? Json::Value()
                                                    : r["parent_comment_id"].as<int64_t>();
                    data["created_at"] = std::string(r["created_at"].c_str());
                    data["nickname"] = std::string(r["nickname"].c_str());
                    data["avatar_url"] = std::string(r["avatar_url"].c_str());
                    callback(ResponseUtil::success(data, "评论成功"));
                },
                [](const drogon::orm::DrogonDbException &e) {
                    // 错误时无法使用 callback（已被移动到成功回调）
                    // 日志记录即可，INSERT 已成功，只是查询详情失败
                },
                commentId);
        };

    auto errorCallback =
        [](const drogon::orm::DrogonDbException &e) {
            // 外层错误回调：INSERT 失败，不需要 callback
            // 因为 insertCallback 已经通过移动捕获持有了 callback
        };

    if (parentCommentId > 0)
    {
        dbClient->execSqlAsync(insertSql, std::move(insertCallback), std::move(errorCallback),
                               postId, userId, parentCommentId, content);
    }
    else
    {
        dbClient->execSqlAsync(insertSql, std::move(insertCallback), std::move(errorCallback),
                               postId, userId, content);
    }
}

void CommentService::getComments(int64_t postId, int page, int pageSize, Callback &&callback)
{
    int offset = (page - 1) * pageSize;
    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT c.id, c.content, c.user_id, c.created_at, "
        "u.nickname, u.avatar_url "
        "FROM comments c LEFT JOIN users u ON c.user_id = u.id "
        "WHERE c.post_id = ? ORDER BY c.created_at ASC LIMIT ? OFFSET ?",
        [callback = std::move(callback)]
        (const drogon::orm::Result &result) mutable {
            Json::Value items(Json::arrayValue);
            for (size_t i = 0; i < result.size(); ++i)
            {
                Json::Value row;
                row["id"] = result[i]["id"].as<int64_t>();
                row["content"] = std::string(result[i]["content"].c_str());
                row["user_id"] = result[i]["user_id"].as<int64_t>();
                row["created_at"] = std::string(result[i]["created_at"].c_str());
                row["nickname"] = std::string(result[i]["nickname"].c_str());
                row["avatar_url"] = std::string(result[i]["avatar_url"].c_str());
                items.append(row);
            }
            callback(ResponseUtil::success(items));
        },
        [callback = std::move(callback)]
        (const drogon::orm::DrogonDbException &e) mutable {
            callback(ResponseUtil::serverError("查询评论列表失败"));
        },
        postId, pageSize, offset);
}
