#include "ai_sdk/realtime_client.hpp"
#include <nlohmann/json.hpp>

namespace ai_sdk {

RealtimeClient::RealtimeClient(const std::string& api_key, const std::string& model)
    : api_key_(api_key), model_(model), ws_client_(std::make_unique<WebSocketClient>()) {}

RealtimeClient::~RealtimeClient() = default;

void RealtimeClient::connect() {
    std::string url = "wss://api.openai.com/v1/realtime?model=" + model_;
    std::map<std::string, std::string> headers = {
        {"Authorization", "Bearer " + api_key_},
        {"OpenAI-Beta", "realtime=v1"}
    };

    ws_client_->setMessageCallback([this](const std::string& message) {
        if (event_callback_) {
            try {
                nlohmann::json j = nlohmann::json::parse(message);
                std::string event_type = j.value("type", "");
                event_callback_(event_type, message);
            } catch (...) {
                event_callback_("error", message);
            }
        }
    });

    ws_client_->connect(url, headers);
}

void RealtimeClient::disconnect() {
    ws_client_->close();
}

void RealtimeClient::sendEvent(const std::string& event_type, const std::string& data) {
    nlohmann::json j = nlohmann::json::parse(data);
    j["type"] = event_type;
    ws_client_->send(j.dump());
}

void RealtimeClient::setEventCallback(RealtimeEventCallback callback) {
    event_callback_ = callback;
}

} // namespace ai_sdk
