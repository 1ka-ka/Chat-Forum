#pragma once

#include <string>
#include <cstdint>
#include <json/json.h>
#include <drogon/orm/Row.h>

class User
{
public:
    int64_t getId() const { return _id; }
    void setId(int64_t id) { _id = id; }

    const std::string& getUsername() const { return _username; }
    void setUsername(const std::string& username) { _username = username; }

    const std::string& getPasswordHash() const { return _passwordHash; }
    void setPasswordHash(const std::string& passwordHash) { _passwordHash = passwordHash; }

    const std::string& getNickname() const { return _nickname; }
    void setNickname(const std::string& nickname) { _nickname = nickname; }

    const std::string& getAvatarUrl() const { return _avatarUrl; }
    void setAvatarUrl(const std::string& avatarUrl) { _avatarUrl = avatarUrl; }

    const std::string& getCreatedAt() const { return _createdAt; }
    void setCreatedAt(const std::string& createdAt) { _createdAt = createdAt; }

    const std::string& getUpdatedAt() const { return _updatedAt; }
    void setUpdatedAt(const std::string& updatedAt) { _updatedAt = updatedAt; }

    Json::Value toJson() const;

    static User fromResult(const drogon::orm::Row& row);

private:
    int64_t _id = 0;
    std::string _username;
    std::string _passwordHash;
    std::string _nickname;
    std::string _avatarUrl;
    std::string _createdAt;
    std::string _updatedAt;
};
