#pragma once

#include <string>
#include <vector>

class ValidationUtil {
public:
    static bool isValidUsername(const std::string &username);
    static bool isValidPassword(const std::string &password);
    static bool isValidNickname(const std::string &nickname);
    static bool isValidImageExt(const std::string &filename);
    static bool isValidTitle(const std::string &title);
    static bool isValidCommentContent(const std::string &content);

private:
    static bool isAlphanumericOrUnderscore(const std::string &str);
};
