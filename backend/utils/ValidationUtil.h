#pragma once

#include <string>
#include <regex>

namespace ValidationUtil
{
    bool isValidUsername(const std::string &username);
    bool isValidPassword(const std::string &password);
    bool isValidNickname(const std::string &nickname);
    bool isValidImageFile(const std::string &filename);
    bool isValidPostTitle(const std::string &title);
    bool isValidPostContent(const std::string &content);
    bool isValidCommentContent(const std::string &content);
}
