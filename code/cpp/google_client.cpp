#include "google_client.hpp"
#include "http_client.hpp"
#include "context_manager.hpp"
#include "websocket_client.hpp"
#include <nlohmann/json.hpp>

namespace ai_sdk {

GoogleClient::GoogleClient(const std::string& api_key)
    : GoogleClient(api_key, GoogleClientConfig{}) {}

GoogleClient::GoogleClient(const std::string& api_key, bool auto_context)
    : api_key_(api_key), config_(), http_client_(std::make_unique<HttpClient>()) {
    config_.auto_context = auto_context;
    if (config_.auto_context) {
        context_ = std::make_unique<ContextManager>(config_.max_context_tokens);
    }
}

GoogleClient::GoogleClient(const std::string& api_key, const GoogleClientConfig& config)
    : api_key_(api_key), config_(config), http_client_(std::make_unique<HttpClient>()) {
    if (config_.auto_context) {
        context_ = std::make_unique<ContextManager>(config_.max_context_tokens);
    }
}

GoogleClient::~GoogleClient() = default;

std::vector<GeminiContent> GoogleClient::messagesToContents(const std::vector<Message>& messages) {
    std::vector<GeminiContent> contents;
    for (const auto& msg : messages) {
        GeminiContent content;
        content.role = (msg.role == "assistant") ? "model" : msg.role;
        GeminiPart part;
        part.text = msg.content;
        content.parts.push_back(part);
        contents.push_back(content);
    }
    return contents;
}

std::vector<Message> GoogleClient::contentsToMessages(const std::vector<GeminiContent>& contents) {
    std::vector<Message> messages;
    for (const auto& content : contents) {
        Message msg;
        msg.role = (content.role == "model") ? "assistant" : content.role;
        if (!content.parts.empty() && content.parts[0].text) {
            msg.content = *content.parts[0].text;
        }
        messages.push_back(msg);
    }
    return messages;
}

void GoogleClient::clearContext() {
    if (context_) {
        context_->clearMessages();
    }
}

std::vector<Message> GoogleClient::getContext() const {
    if (context_) {
        return context_->getMessages();
    }
    return {};
}

std::string GoogleClient::callAPI(const std::string& model, const std::vector<Message>& messages, std::optional<float> temperature) {
    auto contents = messagesToContents(messages);

    GeminiRequest request;
    request.contents = contents;
    if (temperature) {
        GenerationConfig gen_config;
        gen_config.temperature = temperature;
        request.generation_config = gen_config;
    }

    nlohmann::json json_body = request;
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"x-goog-api-key", api_key_}
    };

    std::string url = config_.base_url + "/models/" + model + ":generateContent";
    std::string response = http_client_->post(url, json_body.dump(), headers);

    nlohmann::json json_response = nlohmann::json::parse(response);
    GeminiResponse gemini_response = json_response;

    if (!gemini_response.candidates.empty() && !gemini_response.candidates[0].content.parts.empty()) {
        if (gemini_response.candidates[0].content.parts[0].text) {
            return *gemini_response.candidates[0].content.parts[0].text;
        }
    }
    return "";
}

std::string GoogleClient::chat(const std::string& message) {
    return chat(config_.default_model, message, config_.default_temperature);
}

std::string GoogleClient::chat(const std::string& model, const std::string& message) {
    return chat(model, message, config_.default_temperature);
}

std::string GoogleClient::chat(const std::string& model, const std::string& message, float temperature) {
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

std::future<std::string> GoogleClient::chatAsync(const std::string& message) {
    return std::async(std::launch::async, [this, message]() { return chat(message); });
}

std::future<std::string> GoogleClient::chatAsync(const std::string& model, const std::string& message) {
    return std::async(std::launch::async, [this, model, message]() { return chat(model, message); });
}

GeminiResponse GoogleClient::generateContent(const std::string& model, const std::vector<GeminiContent>& contents) {
    GeminiRequest request;
    request.contents = contents;

    nlohmann::json json_body = request;
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"x-goog-api-key", api_key_}
    };

    std::string url = config_.base_url + "/models/" + model + ":generateContent";
    std::string response = http_client_->post(url, json_body.dump(), headers);

    nlohmann::json json_response = nlohmann::json::parse(response);
    return json_response;
}

GeminiResponse GoogleClient::generateContent(const std::string& model, const std::vector<GeminiContent>& contents, const GenerationConfig& config) {
    GeminiRequest request;
    request.contents = contents;
    request.generation_config = config;

    nlohmann::json json_body = request;
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"x-goog-api-key", api_key_}
    };

    std::string url = config_.base_url + "/models/" + model + ":generateContent";
    std::string response = http_client_->post(url, json_body.dump(), headers);

    nlohmann::json json_response = nlohmann::json::parse(response);
    return json_response;
}

void GoogleClient::chatStream(const std::string& message, StreamCallback callback) {
    chatStream(config_.default_model, message, callback);
}

void GoogleClient::chatStream(const std::string& model, const std::string& message, StreamCallback callback) {
    std::vector<Message> messages;
    if (config_.auto_context && context_) {
        messages = context_->getMessages();
    }
    messages.push_back({"user", message});

    auto contents = messagesToContents(messages);
    streamGenerateContent(model, contents, callback);
}

void GoogleClient::streamGenerateContent(const std::string& model, const std::vector<GeminiContent>& contents, StreamCallback callback) {
    GeminiRequest request;
    request.contents = contents;

    nlohmann::json json_body = request;
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"x-goog-api-key", api_key_}
    };

    std::string url = config_.base_url + "/models/" + model + ":streamGenerateContent?alt=sse";

    // Note: Streaming requires SSE support in HttpClient
    std::string response = http_client_->post(url, json_body.dump(), headers);
    callback(response);
}

GeminiEmbedding GoogleClient::embedContent(const std::string& model, const std::string& text) {
    nlohmann::json json_body = {
        {"model", "models/" + model},
        {"content", {{"parts", {{{"text", text}}}}}}
    };

    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"x-goog-api-key", api_key_}
    };

    std::string url = config_.base_url + "/models/" + model + ":embedContent";
    std::string response = http_client_->post(url, json_body.dump(), headers);

    nlohmann::json json_response = nlohmann::json::parse(response);
    return json_response["embedding"];
}

std::vector<GeminiEmbedding> GoogleClient::batchEmbedContents(const std::string& model, const std::vector<std::string>& texts) {
    nlohmann::json requests = nlohmann::json::array();
    for (const auto& text : texts) {
        requests.push_back({
            {"model", "models/" + model},
            {"content", {{"parts", {{{"text", text}}}}}}
        });
    }

    nlohmann::json json_body = {{"requests", requests}};
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"x-goog-api-key", api_key_}
    };

    std::string url = config_.base_url + "/models/" + model + ":batchEmbedContents";
    std::string response = http_client_->post(url, json_body.dump(), headers);

    nlohmann::json json_response = nlohmann::json::parse(response);
    std::vector<GeminiEmbedding> embeddings;
    for (const auto& emb : json_response["embeddings"]) {
        embeddings.push_back(emb);
    }
    return embeddings;
}

int GoogleClient::countTokens(const std::string& model, const std::string& text) {
    nlohmann::json json_body;
    json_body["contents"] = nlohmann::json::array();
    json_body["contents"][0]["parts"] = nlohmann::json::array();
    json_body["contents"][0]["parts"][0]["text"] = text;

    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"x-goog-api-key", api_key_}
    };

    std::string url = config_.base_url + "/models/" + model + ":countTokens";
    std::string response = http_client_->post(url, json_body.dump(), headers);

    nlohmann::json json_response = nlohmann::json::parse(response);
    return json_response["totalTokens"].get<int>();
}

int GoogleClient::countTokens(const std::string& model, const std::vector<GeminiContent>& contents) {
    nlohmann::json json_body = {{"contents", contents}};

    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"x-goog-api-key", api_key_}
    };

    std::string url = config_.base_url + "/models/" + model + ":countTokens";
    std::string response = http_client_->post(url, json_body.dump(), headers);

    nlohmann::json json_response = nlohmann::json::parse(response);
    return json_response["totalTokens"].get<int>();
}

std::string GoogleClient::batchGenerateContent(const std::string& model, const std::vector<std::vector<GeminiContent>>& batch_contents) {
    nlohmann::json requests = nlohmann::json::array();
    for (const auto& contents : batch_contents) {
        requests.push_back({{"contents", contents}});
    }

    nlohmann::json json_body = {{"requests", requests}};
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"x-goog-api-key", api_key_}
    };

    std::string url = config_.base_url + "/models/" + model + ":batchGenerateContent";
    return http_client_->post(url, json_body.dump(), headers);
}

GeminiFile GoogleClient::uploadFile(const std::string& file_path, const std::string& mime_type) {
    std::map<std::string, std::string> fields = {{"mimeType", mime_type}};
    std::map<std::string, std::string> files = {{"file", file_path}};
    std::map<std::string, std::string> headers = {{"x-goog-api-key", api_key_}};

    std::string url = config_.base_url + "/files";
    std::string response = http_client_->postMultipart(url, fields, files, headers);

    nlohmann::json json_response = nlohmann::json::parse(response);
    return json_response["file"];
}

GeminiFile GoogleClient::getFile(const std::string& file_name) {
    std::map<std::string, std::string> headers = {{"x-goog-api-key", api_key_}};
    std::string url = config_.base_url + "/" + file_name;
    std::string response = http_client_->get(url, headers);

    nlohmann::json json_response = nlohmann::json::parse(response);
    return json_response;
}

void GoogleClient::deleteFile(const std::string& file_name) {
    std::map<std::string, std::string> headers = {{"x-goog-api-key", api_key_}};
    std::string url = config_.base_url + "/" + file_name;
    http_client_->deleteRequest(url, headers);
}

std::vector<GeminiFile> GoogleClient::listFiles() {
    std::map<std::string, std::string> headers = {{"x-goog-api-key", api_key_}};
    std::string url = config_.base_url + "/files";
    std::string response = http_client_->get(url, headers);

    nlohmann::json json_response = nlohmann::json::parse(response);
    std::vector<GeminiFile> files;
    if (json_response.contains("files")) {
        for (const auto& file : json_response["files"]) {
            files.push_back(file);
        }
    }
    return files;
}

void GoogleClient::connectBidiStream(const std::string& model) {
    if (!ws_client_) {
        ws_client_ = std::make_unique<WebSocketClient>();
    }

    std::string url = "wss://generativelanguage.googleapis.com/ws/google.ai.generativelanguage.v1beta.GenerativeService.BidiGenerateContent?key=" + api_key_;
    std::map<std::string, std::string> headers;

    ws_client_->setMessageCallback([this](const std::string& message) {
        if (bidi_callback_) {
            bidi_callback_(message);
        }
    });

    ws_client_->connect(url, headers);
}

void GoogleClient::disconnectBidiStream() {
    if (ws_client_) {
        ws_client_->close();
    }
}

void GoogleClient::sendBidiMessage(const std::vector<GeminiContent>& contents) {
    nlohmann::json message = {{"contents", contents}};
    if (ws_client_) {
        ws_client_->send(message.dump());
    }
}

void GoogleClient::setBidiCallback(StreamCallback callback) {
    bidi_callback_ = callback;
}

} // namespace ai_sdk
