#include "LikeService.h"
#include "../utils/ResponseUtil.h"
#include <drogon/drogon.h>

void LikeService::toggleLike(int64_t postId, int64_t userId, Callback &&callback)
{
    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "SELECT id FROM likes WHERE post_id = ? AND user_id = ?",
        [postId, userId, callback = std::move(callback)](const drogon::orm::Result &result) mutable {
            auto db = drogon::app().getDbClient();
            if (!result.empty())
            {
                int64_t likeId = result[0]["id"].as<int64_t>();
                db->execSqlAsync(
                    "DELETE FROM likes WHERE id = ?",
                    [postId, callback = std::move(callback)](const drogon::orm::Result &) mutable {
                        auto db2 = drogon::app().getDbClient();
                        db2->execSqlAsync(
                            "UPDATE posts SET like_count = GREATEST(like_count - 1, 0) WHERE id = ?",
                            [postId, callback = std::move(callback)](const drogon::orm::Result &) mutable {
                                auto db3 = drogon::app().getDbClient();
                                db3->execSqlAsync(
                                    "SELECT like_count FROM posts WHERE id = ?",
                                    [callback = std::move(callback)](const drogon::orm::Result &r) mutable {
                                        Json::Value data;
                                        data["is_liked"] = false;
                                        data["like_count"] = r.empty() ? 0 : r[0]["like_count"].as<int>();
                                        callback(ResponseUtil::success(data, "取消点赞成功"));
                                    },
                                    [callback = std::move(callback)](const drogon::orm::DrogonDbException &) mutable {
                                        callback(ResponseUtil::serverError("操作失败"));
                                    },
                                    postId);
                            },
                            [](const drogon::orm::DrogonDbException &) {},
                            postId);
                    },
                    [callback = std::move(callback)](const drogon::orm::DrogonDbException &) mutable {
                        callback(ResponseUtil::serverError("取消点赞失败"));
                    },
                    likeId);
            }
            else
            {
                db->execSqlAsync(
                    "INSERT INTO likes (post_id, user_id) VALUES (?, ?)",
                    [postId, callback = std::move(callback)](const drogon::orm::Result &) mutable {
                        auto db2 = drogon::app().getDbClient();
                        db2->execSqlAsync(
                            "UPDATE posts SET like_count = like_count + 1 WHERE id = ?",
                            [postId, callback = std::move(callback)](const drogon::orm::Result &) mutable {
                                auto db3 = drogon::app().getDbClient();
                                db3->execSqlAsync(
                                    "SELECT like_count FROM posts WHERE id = ?",
                                    [callback = std::move(callback)](const drogon::orm::Result &r) mutable {
                                        Json::Value data;
                                        data["is_liked"] = true;
                                        data["like_count"] = r.empty() ? 0 : r[0]["like_count"].as<int>();
                                        callback(ResponseUtil::success(data, "点赞成功"));
                                    },
                                    [callback = std::move(callback)](const drogon::orm::DrogonDbException &) mutable {
                                        callback(ResponseUtil::serverError("操作失败"));
                                    },
                                    postId);
                            },
                            [](const drogon::orm::DrogonDbException &) {},
                            postId);
                    },
                    [callback = std::move(callback)](const drogon::orm::DrogonDbException &) mutable {
                        callback(ResponseUtil::serverError("点赞失败"));
                    },
                    postId, userId);
            }
        },
        [callback = std::move(callback)](const drogon::orm::DrogonDbException &) mutable {
            callback(ResponseUtil::serverError("查询点赞状态失败"));
        },
        postId, userId);
}
