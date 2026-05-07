#include "ValidationUtil.h"
#include <algorithm>

namespace ValidationUtil
{
    bool isValidUsername(const std::string &username)
    {
        if (username.length() < 3 || username.length() > 50)
            return false;
        for (char c : username)
        {
            if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_')
                return false;
        }
        return true;
    }

    bool isValidPassword(const std::string &password)
    {
        return password.length() >= 6 && password.length() <= 50;
    }

    bool isValidNickname(const std::string &nickname)
    {
        return !nickname.empty() && nickname.length() <= 50;
    }

    bool isValidImageFile(const std::string &filename)
    {
        std::string lower = filename;
        std::transform(lower.begin(), lower.end(), lower.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return lower.find(".jpg") != std::string::npos ||
               lower.find(".jpeg") != std::string::npos ||
               lower.find(".png") != std::string::npos ||
               lower.find(".gif") != std::string::npos;
    }

    bool isValidPostTitle(const std::string &title)
    {
        return !title.empty() && title.length() <= 200;
    }

    bool isValidPostContent(const std::string &content)
    {
        return !content.empty();
    }

    bool isValidCommentContent(const std::string &content)
    {
        return !content.empty() && content.length() <= 1000;
    }
}
