#include "anthropic_client.hpp"
#include "http_client.hpp"
#include "context_manager.hpp"
#include <nlohmann/json.hpp>

namespace ai_sdk {

AnthropicClient::AnthropicClient(const std::string& api_key)
    : AnthropicClient(api_key, ClientConfig{}) {}

AnthropicClient::AnthropicClient(const std::string& api_key, bool auto_context)
    : api_key_(api_key), config_(), http_client_(std::make_unique<HttpClient>()) {
    config_.auto_context = auto_context;
    config_.default_model = "claude-3-opus-20240229";
    config_.base_url = "https://api.anthropic.com/v1";
    if (config_.auto_context) {
        context_ = std::make_unique<ContextManager>(config_.max_context_tokens);
    }
}

AnthropicClient::AnthropicClient(const std::string& api_key, const ClientConfig& config)
    : api_key_(api_key), config_(config), http_client_(std::make_unique<HttpClient>()) {
    if (config_.base_url == "https://api.openai.com/v1") {
        config_.base_url = "https://api.anthropic.com/v1";
    }
    if (config_.auto_context) {
        context_ = std::make_unique<ContextManager>(config_.max_context_tokens);
    }
}

AnthropicClient::~AnthropicClient() = default;

Result<std::string> AnthropicClient::callAPI(const std::string& model,
                                             const std::vector<Message>& messages,
                                             const std::string& system_prompt) {
    nlohmann::json json_body;
    json_body["model"] = model;
    json_body["max_tokens"] = 1024;
    json_body["messages"] = nlohmann::json::array();

    for (const auto& msg : messages) {
        json_body["messages"].push_back({{"role", msg.role}, {"content", msg.content}});
    }

    if (!system_prompt.empty()) {
        json_body["system"] = system_prompt;
    }

    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"x-api-key", api_key_},
        {"anthropic-version", "2023-06-01"}
    };

    auto response_result =
        http_client_->post(config_.base_url + "/messages", json_body.dump(), headers);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    try {
        nlohmann::json json_response = nlohmann::json::parse(response_result.value());
        if (json_response.contains("content") && json_response["content"].is_array() &&
            !json_response["content"].empty()) {
            return json_response["content"][0]["text"].get<std::string>();
        }
        return std::unexpected(
            makeParseError("Failed to parse response: missing content"));
    } catch (const std::exception& e) {
        return std::unexpected(
            makeParseError(std::string("Failed to parse response: ") + e.what()));
    }
}

Result<std::string> AnthropicClient::chat(const std::string& message) {
    return chat(config_.default_model, message);
}

Result<std::string> AnthropicClient::chat(const std::string& model,
                                          const std::string& message) {
    std::vector<Message> messages;
    if (config_.auto_context && context_) {
        messages = context_->getMessages();
    }
    messages.push_back({"user", message});

    auto response_result = callAPI(model, messages);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    if (config_.auto_context && context_) {
        context_->addMessage({"user", message});
        context_->addMessage({"assistant", response_result.value()});
    }

    return response_result;
}

Result<std::string> AnthropicClient::chat(const std::string& model,
                                          const std::string& message,
                                          const std::string& system_prompt) {
    std::vector<Message> messages;
    if (config_.auto_context && context_) {
        messages = context_->getMessages();
    }
    messages.push_back({"user", message});

    auto response_result = callAPI(model, messages, system_prompt);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    if (config_.auto_context && context_) {
        context_->addMessage({"user", message});
        context_->addMessage({"assistant", response_result.value()});
    }

    return response_result;
}

Result<std::string> AnthropicClient::chat(const std::string& model,
                                          const std::vector<Message>& messages) {
    return callAPI(model, messages);
}

std::future<Result<std::string>> AnthropicClient::chatAsync(const std::string& message) {
    return std::async(std::launch::async, [this, message]() {
        return chat(message);
    });
}

std::future<Result<std::string>> AnthropicClient::chatAsync(const std::string& model,
                                                            const std::string& message) {
    return std::async(std::launch::async, [this, model, message]() {
        return chat(model, message);
    });
}

Result<void> AnthropicClient::chatStream(const std::string& message,
                                         StreamCallback callback) {
    return chatStream(config_.default_model, message, callback);
}

Result<void> AnthropicClient::chatStream(const std::string& model,
                                         const std::string& message,
                                         StreamCallback callback) {
    std::vector<Message> messages;
    if (config_.auto_context && context_) {
        messages = context_->getMessages();
    }
    messages.push_back({"user", message});

    nlohmann::json json_body;
    json_body["model"] = model;
    json_body["max_tokens"] = 1024;
    json_body["stream"] = true;
    json_body["messages"] = nlohmann::json::array();

    for (const auto& msg : messages) {
        json_body["messages"].push_back({{"role", msg.role}, {"content", msg.content}});
    }

    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"x-api-key", api_key_},
        {"anthropic-version", "2023-06-01"}
    };

    return http_client_->postStream(
        config_.base_url + "/messages", json_body.dump(), headers, callback);
}

void AnthropicClient::clearContext() {
    if (context_) {
        context_->clearMessages();
    }
}

std::vector<Message> AnthropicClient::getContext() const {
    if (context_) {
        return context_->getMessages();
    }
    return {};
}

Result<std::string> AnthropicClient::createMessageBatch(const std::string& requests_file_path) {
    nlohmann::json json_body = {{"requests", requests_file_path}};
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"x-api-key", api_key_},
        {"anthropic-version", "2023-06-01"}
    };

    return http_client_->post(config_.base_url + "/messages/batches",
                              json_body.dump(),
                              headers);
}

Result<std::string> AnthropicClient::retrieveMessageBatch(const std::string& batch_id) {
    std::map<std::string, std::string> headers = {
        {"x-api-key", api_key_},
        {"anthropic-version", "2023-06-01"}
    };

    return http_client_->get(config_.base_url + "/messages/batches/" + batch_id, headers);
}

Result<std::string> AnthropicClient::cancelMessageBatch(const std::string& batch_id) {
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"x-api-key", api_key_},
        {"anthropic-version", "2023-06-01"}
    };

    return http_client_->post(config_.base_url + "/messages/batches/" + batch_id + "/cancel",
                              "",
                              headers);
}

} // namespace ai_sdk
