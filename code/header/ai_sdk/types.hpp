#pragma once

#include <string>
#include <exception>
#include <functional>

namespace ai_sdk {

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

using StreamCallback = std::function<void(const std::string& chunk)>;

} // namespace ai_sdk
