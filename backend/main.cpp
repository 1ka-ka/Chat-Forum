#include <drogon/drogon.h>
#include "controllers/UserController.h"
#include "controllers/PostController.h"
#include "controllers/CommentController.h"
#include "controllers/LikeController.h"
#include "controllers/ChatController.h"
#include "controllers/NotificationController.h"
#include "websocket/ChatWebSocketController.h"
#include "utils/AuthMiddleware.h"

int main(int argc, char *argv[]) {
    std::string configPath = "../config/config.json";
    if (argc > 1) {
        configPath = argv[1];
    }

    auto &app = drogon::app();
    app.loadConfigFile(configPath);

    app.registerMiddleware<AuthMiddleware>("auth");

    app.registerController<UserController>();
    app.registerController<PostController>();
    app.registerController<CommentController>();
    app.registerController<LikeController>();
    app.registerController<ChatController>();
    app.registerController<ChatWebSocketController>();
    app.registerController<NotificationController>();

    // 健康检查端点
    app.registerHandler(
        "/api/health",
        [](const drogon::HttpRequestPtr &req,
           std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
            Json::Value data;
            data["status"] = "ok";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(data);
            callback(resp);
        },
        {drogon::Get});

    app.registerPreRoutingAdvice(
        [](const drogon::HttpRequestPtr &req, drogon::AdviceCallback &&acb,
           drogon::AdviceChainCallback &&accb) {
            if (req->method() == drogon::Options) {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k204NoContent);
                resp->addHeader("Access-Control-Allow-Origin", "*");
                resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
                resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
                resp->addHeader("Access-Control-Max-Age", "86400");
                acb(resp);
                return;
            }
            accb();
        });

    app.registerPostHandlingAdvice(
        [](const drogon::HttpRequestPtr &req, const drogon::HttpResponsePtr &resp) {
            resp->addHeader("Access-Control-Allow-Origin", "*");
            resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
            resp->addHeader("Access-Control-Allow-Credentials", "true");
        });

    app.addListener("0.0.0.0", 8080);

    std::cout << "ChatForum server running on http://0.0.0.0:8080" << std::endl;

    app.run();
}
