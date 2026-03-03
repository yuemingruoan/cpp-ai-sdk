#include "ai_sdk/openai_client.hpp"
#include "ai_sdk/http_client.hpp"
#include "ai_sdk/context_manager.hpp"
#include "ai_sdk/extended_models.hpp"
#include <nlohmann/json.hpp>

namespace ai_sdk {

OpenAIClient::OpenAIClient(const std::string& api_key)
    : OpenAIClient(api_key, ClientConfig{}) {}

OpenAIClient::OpenAIClient(const std::string& api_key, bool auto_context)
    : api_key_(api_key), config_(), http_client_(std::make_unique<HttpClient>()) {
    config_.auto_context = auto_context;
    if (config_.auto_context) {
        context_ = std::make_unique<ContextManager>(config_.max_context_tokens);
    }
}

OpenAIClient::OpenAIClient(const std::string& api_key, const ClientConfig& config)
    : api_key_(api_key), config_(config), http_client_(std::make_unique<HttpClient>()) {
    if (config_.auto_context) {
        context_ = std::make_unique<ContextManager>(config_.max_context_tokens);
    }
}

OpenAIClient::~OpenAIClient() = default;

std::string OpenAIClient::callAPI(const std::string& model,
                                   const std::vector<Message>& messages,
                                   std::optional<float> temperature) {
    ChatRequest request;
    request.model = model;
    request.messages = messages;
    request.temperature = temperature;

    nlohmann::json json_body = request;
    std::string body = json_body.dump();

    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + api_key_}
    };

    std::string response = http_client_->post(config_.base_url + "/chat/completions", body, headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        ChatResponse chat_response = json_response;
        return chat_response.choices[0].message.content;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

std::string OpenAIClient::chat(const std::string& message) {
    return chat(config_.default_model, message, config_.default_temperature);
}

std::string OpenAIClient::chat(const std::string& model, const std::string& message) {
    return chat(model, message, config_.default_temperature);
}

std::string OpenAIClient::chat(const std::string& model, const std::string& message, float temperature) {
    std::vector<Message> messages;

    if (config_.auto_context && context_) {
        messages = context_->getMessages();
    }

    messages.push_back({"user", message});
    std::string response = callAPI(model, messages, temperature);

    if (config_.auto_context && context_) {
        context_->addMessage({"user", message});
        context_->addMessage({"assistant", response});
    }

    return response;
}

std::string OpenAIClient::chat(const std::string& model, const std::vector<Message>& messages) {
    return callAPI(model, messages, config_.default_temperature);
}

std::future<std::string> OpenAIClient::chatAsync(const std::string& message) {
    return std::async(std::launch::async, [this, message]() {
        return chat(message);
    });
}

std::future<std::string> OpenAIClient::chatAsync(const std::string& model, const std::string& message) {
    return std::async(std::launch::async, [this, model, message]() {
        return chat(model, message);
    });
}

void OpenAIClient::chatStream(const std::string& message, StreamCallback callback) {
    chatStream(config_.default_model, message, callback);
}

void OpenAIClient::chatStream(const std::string& model, const std::string& message, StreamCallback callback) {
    std::vector<Message> messages;
    if (config_.auto_context && context_) {
        messages = context_->getMessages();
    }
    messages.push_back({"user", message});

    ChatRequest request;
    request.model = model;
    request.messages = messages;
    request.stream = true;

    nlohmann::json json_body = request;
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + api_key_}
    };

    http_client_->postStream(config_.base_url + "/chat/completions", json_body.dump(), headers, callback);
}

ChatResponse OpenAIClient::chatCompletion(const ChatRequest& request) {
    nlohmann::json json_body = request;
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + api_key_}
    };

    std::string response = http_client_->post(config_.base_url + "/chat/completions", json_body.dump(), headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

std::future<ChatResponse> OpenAIClient::chatCompletionAsync(const ChatRequest& request) {
    return std::async(std::launch::async, [this, request]() {
        return chatCompletion(request);
    });
}

void OpenAIClient::clearContext() {
    if (context_) {
        context_->clearMessages();
    }
}

std::vector<Message> OpenAIClient::getContext() const {
    if (context_) {
        return context_->getMessages();
    }
    return {};
}

EmbeddingResponse OpenAIClient::createEmbedding(const EmbeddingRequest& request) {
    nlohmann::json json_body = request;
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + api_key_}
    };

    std::string response = http_client_->post(config_.base_url + "/embeddings", json_body.dump(), headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

EmbeddingResponse OpenAIClient::createEmbedding(const std::string& model, const std::vector<std::string>& input) {
    EmbeddingRequest request;
    request.model = model;
    request.input = input;
    return createEmbedding(request);
}

ModelsResponse OpenAIClient::listModels() {
    std::map<std::string, std::string> headers = {
        {"Authorization", "Bearer " + api_key_}
    };

    std::string response = http_client_->post(config_.base_url + "/models", "", headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

Model OpenAIClient::retrieveModel(const std::string& model_id) {
    std::map<std::string, std::string> headers = {
        {"Authorization", "Bearer " + api_key_}
    };

    std::string response = http_client_->post(config_.base_url + "/models/" + model_id, "", headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        Model model;
        json_response.at("id").get_to(model.id);
        json_response.at("object").get_to(model.object);
        json_response.at("created").get_to(model.created);
        json_response.at("owned_by").get_to(model.owned_by);
        return model;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

ModerationResponse OpenAIClient::createModeration(const ModerationRequest& request) {
    nlohmann::json json_body = request;
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + api_key_}
    };

    std::string response = http_client_->post(config_.base_url + "/moderations", json_body.dump(), headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

ModerationResponse OpenAIClient::createModeration(const std::string& input) {
    ModerationRequest request;
    request.input = input;
    return createModeration(request);
}

ImageResponse OpenAIClient::createImage(const ImageGenerationRequest& request) {
    nlohmann::json json_body = request;
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + api_key_}
    };

    std::string response = http_client_->post(config_.base_url + "/images/generations", json_body.dump(), headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

ImageResponse OpenAIClient::createImage(const std::string& prompt) {
    ImageGenerationRequest request;
    request.prompt = prompt;
    return createImage(request);
}

} // namespace ai_sdk
