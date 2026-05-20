#include "ConfigUtil.h"
#include <drogon/drogon.h>

std::string ConfigUtil::getJwtSecret() {
    auto &config = drogon::app().getCustomConfig();
    if (config.isMember("jwt") && config["jwt"].isMember("secret")) {
        return config["jwt"]["secret"].asString();
    }
    return "chatforum-secret-key-change-in-production-2025";
}

int ConfigUtil::getJwtExpiration() {
    auto &config = drogon::app().getCustomConfig();
    if (config.isMember("jwt") && config["jwt"].isMember("expiration_hours")) {
        return config["jwt"]["expiration_hours"].asInt();
    }
    return 24;
}
