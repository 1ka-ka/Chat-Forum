#include <drogon/drogon.h>
#include <json/json.h>
#include <iostream>
#include <filesystem>
#include "utils/AuthMiddleware.h"
#include "controllers/UserController.h"
#include "controllers/PostController.h"
#include "controllers/CommentController.h"
#include "controllers/LikeController.h"
#include "controllers/ChatController.h"
#include "websocket/ChatWebSocketController.h"

int main(int argc, char *argv[])
{
    std::string configPath = "./config/config.json";
    if (argc > 1)
    {
        configPath = argv[1];
    }

    std::filesystem::create_directories("./uploads/images");
    std::filesystem::create_directories("./uploads/avatars");
    std::filesystem::create_directories("./logs");

    auto &app = drogon::app();

    app.loadConfigFile(configPath);

    app.registerMiddleware<AuthMiddleware>("auth");

    app.registerController<UserController>();
    app.registerController<PostController>();
    app.registerController<CommentController>();
    app.registerController<LikeController>();
    app.registerController<ChatController>();
    app.registerController<ChatWebSocketController>();

    app.addListener("0.0.0.0", 8080);

    app.setDocumentRoot("./frontend/dist");

    app.enableSession(false);

    app.registerPostHandlingAdvice(
        [](const drogon::HttpRequestPtr &req, const drogon::HttpResponsePtr &resp) {
            resp->addHeader("Access-Control-Allow-Origin", "*");
            resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
            resp->addHeader("Access-Control-Allow-Credentials", "true");
        });

    app.registerPreRoutingAdvice(
        [](const drogon::HttpRequestPtr &req, drogon::AdviceCallback &&acb, drogon::AdviceChainCallback &&accb) {
            if (req->method() == drogon::Options)
            {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k204NoContent);
                resp->addHeader("Access-Control-Allow-Origin", "*");
                resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
                resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
                resp->addHeader("Access-Control-Allow-Credentials", "true");
                resp->addHeader("Access-Control-Max-Age", "86400");
                acb(resp);
                return;
            }
            accb();
        });

    std::cout << "========================================" << std::endl;
    std::cout << "  ChatForum Server Starting..." << std::endl;
    std::cout << "  Config: " << configPath << std::endl;
    std::cout << "  Listening on: http://0.0.0.0:8080" << std::endl;
    std::cout << "========================================" << std::endl;

    app.run();

    return 0;
}
