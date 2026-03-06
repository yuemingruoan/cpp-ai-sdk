#include "google_client.hpp"
#include "http_client.hpp"
#include "context_manager.hpp"
#include "websocket_client.hpp"
#include "oauth_manager.hpp"
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

GoogleClient::GoogleClient(const OAuthConfig& oauth_config)
    : GoogleClient(oauth_config, GoogleClientConfig{}) {}

GoogleClient::GoogleClient(const OAuthConfig& oauth_config, const GoogleClientConfig& config)
    : config_(config), http_client_(std::make_unique<HttpClient>()),
      oauth_manager_(std::make_unique<OAuthManager>(oauth_config)), use_oauth_(true) {
    if (config_.auto_context) {
        context_ = std::make_unique<ContextManager>(config_.max_context_tokens);
    }
}

GoogleClient::~GoogleClient() = default;

std::map<std::string, std::string> GoogleClient::getAuthHeaders() {
    std::map<std::string, std::string> headers = {{"Content-Type", "application/json"}};
    if (use_oauth_) {
        auto token_result = oauth_manager_->getAccessToken();
        if (token_result) {
            headers["Authorization"] = "Bearer " + token_result.value();
        }
        // If token retrieval failed, we leave headers without Authorization;
        // the subsequent HTTP call will likely fail and propagate that error.
    } else {
        headers["x-goog-api-key"] = api_key_;
    }
    return headers;
}

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

Result<std::string> GoogleClient::callAPI(const std::string& model,
                                          const std::vector<Message>& messages,
                                          std::optional<float> temperature) {
    auto contents = messagesToContents(messages);

    GeminiRequest request;
    request.contents = contents;
    if (temperature) {
        GenerationConfig gen_config;
        gen_config.temperature = temperature;
        request.generation_config = gen_config;
    }

    nlohmann::json json_body = request;
    auto headers = getAuthHeaders();

    std::string url = config_.base_url + "/models/" + model + ":generateContent";
    auto response_result = http_client_->post(url, json_body.dump(), headers);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    try {
        nlohmann::json json_response = nlohmann::json::parse(response_result.value());
        GeminiResponse gemini_response = json_response;

        if (!gemini_response.candidates.empty() &&
            !gemini_response.candidates[0].content.parts.empty() &&
            gemini_response.candidates[0].content.parts[0].text) {
            return *gemini_response.candidates[0].content.parts[0].text;
        }
        return std::unexpected(
            makeParseError("Failed to parse response: missing candidate text"));
    } catch (const std::exception& e) {
        return std::unexpected(
            makeParseError(std::string("Failed to parse response: ") + e.what()));
    }
}

Result<std::string> GoogleClient::chat(const std::string& message) {
    return chat(config_.default_model, message, config_.default_temperature);
}

Result<std::string> GoogleClient::chat(const std::string& model, const std::string& message) {
    return chat(model, message, config_.default_temperature);
}

Result<std::string> GoogleClient::chat(const std::string& model,
                                       const std::string& message,
                                       float temperature) {
    std::vector<Message> messages;
    if (config_.auto_context && context_) {
        messages = context_->getMessages();
    }
    messages.push_back({"user", message});

    auto response_result = callAPI(model, messages, temperature);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    if (config_.auto_context && context_) {
        context_->addMessage({"user", message});
        context_->addMessage({"assistant", response_result.value()});
    }

    return response_result;
}

std::future<Result<std::string>> GoogleClient::chatAsync(const std::string& message) {
    return std::async(std::launch::async, [this, message]() { return chat(message); });
}

std::future<Result<std::string>> GoogleClient::chatAsync(const std::string& model,
                                                         const std::string& message) {
    return std::async(std::launch::async, [this, model, message]() { return chat(model, message); });
}

Result<GeminiResponse> GoogleClient::generateContent(
    const std::string& model,
    const std::vector<GeminiContent>& contents) {
    GeminiRequest request;
    request.contents = contents;

    nlohmann::json json_body = request;
    auto headers = getAuthHeaders();

    std::string url = config_.base_url + "/models/" + model + ":generateContent";
    auto response_result = http_client_->post(url, json_body.dump(), headers);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    try {
        nlohmann::json json_response = nlohmann::json::parse(response_result.value());
        GeminiResponse result = json_response;
        return result;
    } catch (const std::exception& e) {
        return std::unexpected(
            makeParseError(std::string("Failed to parse response: ") + e.what()));
    }
}

Result<GeminiResponse> GoogleClient::generateContent(
    const std::string& model,
    const std::vector<GeminiContent>& contents,
    const GenerationConfig& config) {
    GeminiRequest request;
    request.contents = contents;
    request.generation_config = config;

    nlohmann::json json_body = request;
    auto headers = getAuthHeaders();

    std::string url = config_.base_url + "/models/" + model + ":generateContent";
    auto response_result = http_client_->post(url, json_body.dump(), headers);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    try {
        nlohmann::json json_response = nlohmann::json::parse(response_result.value());
        GeminiResponse result = json_response;
        return result;
    } catch (const std::exception& e) {
        return std::unexpected(
            makeParseError(std::string("Failed to parse response: ") + e.what()));
    }
}

Result<void> GoogleClient::chatStream(const std::string& message,
                                      StreamCallback callback) {
    return chatStream(config_.default_model, message, callback);
}

Result<void> GoogleClient::chatStream(const std::string& model,
                                      const std::string& message,
                                      StreamCallback callback) {
    std::vector<Message> messages;
    if (config_.auto_context && context_) {
        messages = context_->getMessages();
    }
    messages.push_back({"user", message});

    auto contents = messagesToContents(messages);
    return streamGenerateContent(model, contents, callback);
}

Result<void> GoogleClient::streamGenerateContent(const std::string& model,
                                                 const std::vector<GeminiContent>& contents,
                                                 StreamCallback callback) {
    GeminiRequest request;
    request.contents = contents;

    nlohmann::json json_body = request;
    auto headers = getAuthHeaders();

    std::string url = config_.base_url + "/models/" + model + ":streamGenerateContent?alt=sse";

    // Note: Streaming ideally uses SSE; here we just forward the response body.
    auto response_result = http_client_->post(url, json_body.dump(), headers);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    callback(response_result.value());
    return {};
}

Result<GeminiEmbedding> GoogleClient::embedContent(const std::string& model,
                                                   const std::string& text) {
    nlohmann::json json_body = {
        {"model", "models/" + model},
        {"content", {{"parts", {{{"text", text}}}}}}
    };

    auto headers = getAuthHeaders();

    std::string url = config_.base_url + "/models/" + model + ":embedContent";
    auto response_result = http_client_->post(url, json_body.dump(), headers);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    try {
        nlohmann::json json_response = nlohmann::json::parse(response_result.value());
        GeminiEmbedding embedding = json_response["embedding"];
        return embedding;
    } catch (const std::exception& e) {
        return std::unexpected(
            makeParseError(std::string("Failed to parse response: ") + e.what()));
    }
}

Result<std::vector<GeminiEmbedding>> GoogleClient::batchEmbedContents(
    const std::string& model,
    const std::vector<std::string>& texts) {
    nlohmann::json requests = nlohmann::json::array();
    for (const auto& text : texts) {
        requests.push_back({
            {"model", "models/" + model},
            {"content", {{"parts", {{{"text", text}}}}}}
        });
    }

    nlohmann::json json_body = {{"requests", requests}};
    auto headers = getAuthHeaders();

    std::string url = config_.base_url + "/models/" + model + ":batchEmbedContents";
    auto response_result = http_client_->post(url, json_body.dump(), headers);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    try {
        nlohmann::json json_response = nlohmann::json::parse(response_result.value());
        std::vector<GeminiEmbedding> embeddings;
        for (const auto& emb : json_response["embeddings"]) {
            embeddings.push_back(emb);
        }
        return embeddings;
    } catch (const std::exception& e) {
        return std::unexpected(
            makeParseError(std::string("Failed to parse response: ") + e.what()));
    }
}

Result<int> GoogleClient::countTokens(const std::string& model, const std::string& text) {
    nlohmann::json json_body;
    json_body["contents"] = nlohmann::json::array();
    json_body["contents"][0]["parts"] = nlohmann::json::array();
    json_body["contents"][0]["parts"][0]["text"] = text;

    auto headers = getAuthHeaders();

    std::string url = config_.base_url + "/models/" + model + ":countTokens";
    auto response_result = http_client_->post(url, json_body.dump(), headers);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    try {
        nlohmann::json json_response = nlohmann::json::parse(response_result.value());
        return json_response["totalTokens"].get<int>();
    } catch (const std::exception& e) {
        return std::unexpected(
            makeParseError(std::string("Failed to parse response: ") + e.what()));
    }
}

Result<int> GoogleClient::countTokens(const std::string& model,
                                      const std::vector<GeminiContent>& contents) {
    nlohmann::json json_body = {{"contents", contents}};

    auto headers = getAuthHeaders();

    std::string url = config_.base_url + "/models/" + model + ":countTokens";
    auto response_result = http_client_->post(url, json_body.dump(), headers);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    try {
        nlohmann::json json_response = nlohmann::json::parse(response_result.value());
        return json_response["totalTokens"].get<int>();
    } catch (const std::exception& e) {
        return std::unexpected(
            makeParseError(std::string("Failed to parse response: ") + e.what()));
    }
}

Result<std::string> GoogleClient::batchGenerateContent(
    const std::string& model,
    const std::vector<std::vector<GeminiContent>>& batch_contents) {
    nlohmann::json requests = nlohmann::json::array();
    for (const auto& contents : batch_contents) {
        requests.push_back({{"contents", contents}});
    }

    nlohmann::json json_body = {{"requests", requests}};
    auto headers = getAuthHeaders();

    std::string url = config_.base_url + "/models/" + model + ":batchGenerateContent";
    return http_client_->post(url, json_body.dump(), headers);
}

Result<GeminiFile> GoogleClient::uploadFile(const std::string& file_path,
                                            const std::string& mime_type) {
    std::map<std::string, std::string> fields = {{"mimeType", mime_type}};
    std::map<std::string, std::string> files = {{"file", file_path}};
    std::map<std::string, std::string> headers;
    if (use_oauth_) {
        auto token_result = oauth_manager_->getAccessToken();
        if (token_result) {
            headers["Authorization"] = "Bearer " + token_result.value();
        }
    } else {
        headers["x-goog-api-key"] = api_key_;
    }

    std::string url = config_.base_url + "/files";
    auto response_result = http_client_->postMultipart(url, fields, files, headers);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    try {
        nlohmann::json json_response = nlohmann::json::parse(response_result.value());
        GeminiFile file = json_response["file"];
        return file;
    } catch (const std::exception& e) {
        return std::unexpected(
            makeParseError(std::string("Failed to parse response: ") + e.what()));
    }
}

Result<GeminiFile> GoogleClient::getFile(const std::string& file_name) {
    std::map<std::string, std::string> headers;
    if (use_oauth_) {
        auto token_result = oauth_manager_->getAccessToken();
        if (token_result) {
            headers["Authorization"] = "Bearer " + token_result.value();
        }
    } else {
        headers["x-goog-api-key"] = api_key_;
    }
    std::string url = config_.base_url + "/" + file_name;
    auto response_result = http_client_->get(url, headers);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    try {
        nlohmann::json json_response = nlohmann::json::parse(response_result.value());
        GeminiFile file = json_response;
        return file;
    } catch (const std::exception& e) {
        return std::unexpected(
            makeParseError(std::string("Failed to parse response: ") + e.what()));
    }
}

Result<void> GoogleClient::deleteFile(const std::string& file_name) {
    std::map<std::string, std::string> headers;
    if (use_oauth_) {
        auto token_result = oauth_manager_->getAccessToken();
        if (token_result) {
            headers["Authorization"] = "Bearer " + token_result.value();
        }
    } else {
        headers["x-goog-api-key"] = api_key_;
    }
    std::string url = config_.base_url + "/" + file_name;
    auto result = http_client_->deleteRequest(url, headers);
    if (!result) {
        return std::unexpected(result.error());
    }
    return {};
}

Result<std::vector<GeminiFile>> GoogleClient::listFiles() {
    std::map<std::string, std::string> headers;
    if (use_oauth_) {
        auto token_result = oauth_manager_->getAccessToken();
        if (token_result) {
            headers["Authorization"] = "Bearer " + token_result.value();
        }
    } else {
        headers["x-goog-api-key"] = api_key_;
    }
    std::string url = config_.base_url + "/files";
    auto response_result = http_client_->get(url, headers);
    if (!response_result) {
        return std::unexpected(response_result.error());
    }

    try {
        nlohmann::json json_response = nlohmann::json::parse(response_result.value());
        std::vector<GeminiFile> files;
        if (json_response.contains("files")) {
            for (const auto& file : json_response["files"]) {
                files.push_back(file);
            }
        }
        return files;
    } catch (const std::exception& e) {
        return std::unexpected(
            makeParseError(std::string("Failed to parse response: ") + e.what()));
    }
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
