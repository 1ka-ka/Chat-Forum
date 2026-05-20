#include "ValidationUtil.h"
#include <algorithm>

bool ValidationUtil::isAlphanumericOrUnderscore(const std::string &str) {
    return std::all_of(str.begin(), str.end(), [](char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    });
}

bool ValidationUtil::isValidUsername(const std::string &username) {
    if (username.length() < 3 || username.length() > 50) return false;
    return isAlphanumericOrUnderscore(username);
}

bool ValidationUtil::isValidPassword(const std::string &password) {
    return password.length() >= 6 && password.length() <= 50;
}

bool ValidationUtil::isValidNickname(const std::string &nickname) {
    return !nickname.empty() && nickname.length() <= 50;
}

bool ValidationUtil::isValidImageExt(const std::string &filename) {
    static const std::vector<std::string> allowed = {".jpg", ".jpeg", ".png", ".gif"};
    std::string lower = filename;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    for (const auto &ext : allowed) {
        if (lower.size() >= ext.size() &&
            lower.compare(lower.size() - ext.size(), ext.size(), ext) == 0) {
            return true;
        }
    }
    return false;
}

bool ValidationUtil::isValidTitle(const std::string &title) {
    return !title.empty() && title.length() <= 200;
}

bool ValidationUtil::isValidCommentContent(const std::string &content) {
    return !content.empty() && content.length() <= 1000;
}
