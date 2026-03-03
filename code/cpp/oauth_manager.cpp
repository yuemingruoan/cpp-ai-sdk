#include "oauth_manager.hpp"
#include "http_client.hpp"
#include <nlohmann/json.hpp>
#include <chrono>

namespace ai_sdk {

OAuthManager::OAuthManager(const OAuthConfig& config)
    : config_(config), http_client_(std::make_unique<HttpClient>()) {
    if (!config_.access_token.empty()) {
        token_.access_token = config_.access_token;
        token_.expires_at = config_.expires_at;
    }
    if (isTokenExpired() && !config_.refresh_token.empty()) {
        refreshToken();
    }
}

OAuthManager::~OAuthManager() = default;

std::string OAuthManager::getAccessToken() {
    if (isTokenExpired() && !config_.refresh_token.empty()) {
        refreshToken();
    }
    return token_.access_token;
}

void OAuthManager::refreshToken() {
    std::string body = "client_id=" + config_.client_id +
                      "&client_secret=" + config_.client_secret +
                      "&refresh_token=" + config_.refresh_token +
                      "&grant_type=refresh_token";

    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/x-www-form-urlencoded"}
    };

    std::string response = http_client_->post(config_.token_uri, body, headers);
    nlohmann::json json_response = nlohmann::json::parse(response);

    token_.access_token = json_response["access_token"].get<std::string>();
    int expires_in = json_response["expires_in"].get<int>();

    auto now = std::chrono::system_clock::now();
    auto expires_at = now + std::chrono::seconds(expires_in - 60);
    token_.expires_at = std::chrono::duration_cast<std::chrono::seconds>(
        expires_at.time_since_epoch()).count();
}

bool OAuthManager::isTokenExpired() const {
    if (token_.access_token.empty()) return true;

    auto now = std::chrono::system_clock::now();
    int64_t now_seconds = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();

    return now_seconds >= token_.expires_at;
}

} // namespace ai_sdk
