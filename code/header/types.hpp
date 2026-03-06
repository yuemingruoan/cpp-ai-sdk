#pragma once

#include <string>
#include <exception>
#include <functional>
#include <expected>

namespace ai_sdk {

// Legacy exception types (kept for backward compatibility).
// New code should prefer SDKError + Result<T> instead of throwing.
class SDKException : public std::exception {
public:
    explicit SDKException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }
private:
    std::string message_;
};

class NetworkException : public SDKException {
public:
    explicit NetworkException(const std::string& message) : SDKException(message) {}
};

class APIException : public SDKException {
public:
    APIException(const std::string& message, int status_code)
        : SDKException(message), status_code_(status_code) {}
    int getStatusCode() const { return status_code_; }
private:
    int status_code_;
};

class ParseException : public SDKException {
public:
    explicit ParseException(const std::string& message) : SDKException(message) {}
};

// New non-throwing error model
enum class ErrorCategory {
    Network,
    Api,
    Parse,
    Unknown
};

struct SDKError {
    ErrorCategory category = ErrorCategory::Unknown;
    std::string message;
    int status_code = 0; // HTTP status for Api errors, 0 otherwise
};

template <typename T>
using Result = std::expected<T, SDKError>;

inline SDKError makeNetworkError(const std::string& message) {
    return SDKError{ErrorCategory::Network, message, 0};
}

inline SDKError makeApiError(const std::string& message, int status_code) {
    return SDKError{ErrorCategory::Api, message, status_code};
}

inline SDKError makeParseError(const std::string& message) {
    return SDKError{ErrorCategory::Parse, message, 0};
}

using StreamCallback = std::function<void(const std::string& chunk)>;

struct OAuthConfig {
    std::string client_id;
    std::string client_secret;
    std::string refresh_token;
    std::string access_token;
    int64_t expires_at = 0;
    std::string token_uri = "https://oauth2.googleapis.com/token";
};

struct OAuthToken {
    std::string access_token;
    int64_t expires_at = 0;
};

} // namespace ai_sdk
