#pragma once

#include <string>
#include <functional>
#include <memory>

namespace ai_sdk {

using WebSocketMessageCallback = std::function<void(const std::string& message)>;
using WebSocketErrorCallback = std::function<void(const std::string& error)>;

class WebSocketClient {
public:
    WebSocketClient();
    ~WebSocketClient();

    void connect(const std::string& url, const std::map<std::string, std::string>& headers);
    void send(const std::string& message);
    void close();

    void setMessageCallback(WebSocketMessageCallback callback);
    void setErrorCallback(WebSocketErrorCallback callback);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace ai_sdk
