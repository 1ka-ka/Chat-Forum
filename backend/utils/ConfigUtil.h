#pragma once

#include <string>

class ConfigUtil {
public:
    static std::string getJwtSecret();
    static int getJwtExpiration();
};
