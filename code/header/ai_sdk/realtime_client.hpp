#pragma once

#include "ai_sdk/websocket_client.hpp"
#include <string>
#include <functional>
#include <memory>

namespace ai_sdk {

using RealtimeEventCallback = std::function<void(const std::string& event_type, const std::string& data)>;

class RealtimeClient {
public:
    explicit RealtimeClient(const std::string& api_key, const std::string& model = "gpt-4o-realtime-preview-2024-12-17");
    ~RealtimeClient();

    void connect();
    void disconnect();
    void sendEvent(const std::string& event_type, const std::string& data);
    void setEventCallback(RealtimeEventCallback callback);

private:
    std::string api_key_;
    std::string model_;
    std::unique_ptr<WebSocketClient> ws_client_;
    RealtimeEventCallback event_callback_;
};

} // namespace ai_sdk
