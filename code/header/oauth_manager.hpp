#pragma once

#include "types.hpp"
#include <memory>
#include <string>
#include <chrono>

namespace ai_sdk {

class HttpClient;

class OAuthManager {
public:
    explicit OAuthManager(const OAuthConfig& config);
    ~OAuthManager();

    Result<std::string> getAccessToken();
    Result<void> refreshToken();

private:
    OAuthConfig config_;
    OAuthToken token_;
    std::unique_ptr<HttpClient> http_client_;

    bool isTokenExpired() const;
};

} // namespace ai_sdk
