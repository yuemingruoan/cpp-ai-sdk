#include "websocket_client.hpp"
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXNetSystem.h>

namespace ai_sdk {

class WebSocketClient::Impl {
public:
    ix::WebSocket ws;
    WebSocketMessageCallback message_callback;
    WebSocketErrorCallback error_callback;

    Impl() {
        ix::initNetSystem();
    }
};

WebSocketClient::WebSocketClient() : impl_(std::make_unique<Impl>()) {}
WebSocketClient::~WebSocketClient() = default;

void WebSocketClient::connect(const std::string& url, const std::map<std::string, std::string>& headers) {
    impl_->ws.setUrl(url);

    ix::WebSocketHttpHeaders ws_headers;
    for (const auto& [key, value] : headers) {
        ws_headers[key] = value;
    }
    impl_->ws.setExtraHeaders(ws_headers);

    impl_->ws.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Message) {
            if (impl_->message_callback) {
                impl_->message_callback(msg->str);
            }
        } else if (msg->type == ix::WebSocketMessageType::Error) {
            if (impl_->error_callback) {
                impl_->error_callback(msg->errorInfo.reason);
            }
        }
    });

    impl_->ws.start();
}

void WebSocketClient::send(const std::string& message) {
    impl_->ws.send(message);
}

void WebSocketClient::close() {
    impl_->ws.stop();
}

void WebSocketClient::setMessageCallback(WebSocketMessageCallback callback) {
    impl_->message_callback = callback;
}

void WebSocketClient::setErrorCallback(WebSocketErrorCallback callback) {
    impl_->error_callback = callback;
}

} // namespace ai_sdk
