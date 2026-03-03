#pragma once

#include <string>
#include <memory>
#include <future>
#include <optional>
#include <vector>
#include "models.hpp"
#include "extended_models.hpp"
#include "types.hpp"

namespace ai_sdk {

class HttpClient;
class ContextManager;
class WebSocketClient;

struct GoogleClientConfig {
    bool auto_context = true;
    int max_context_tokens = 1000000;
    std::string default_model = "gemini-2.5-flash";
    float default_temperature = 0.7f;
    std::string base_url = "https://generativelanguage.googleapis.com/v1beta";
};

class GoogleClient {
public:
    explicit GoogleClient(const std::string& api_key);
    GoogleClient(const std::string& api_key, bool auto_context);
    GoogleClient(const std::string& api_key, const GoogleClientConfig& config);
    ~GoogleClient();

    // Chat API (简化接口)
    std::string chat(const std::string& message);
    std::string chat(const std::string& model, const std::string& message);
    std::string chat(const std::string& model, const std::string& message, float temperature);

    std::future<std::string> chatAsync(const std::string& message);
    std::future<std::string> chatAsync(const std::string& model, const std::string& message);

    void chatStream(const std::string& message, StreamCallback callback);
    void chatStream(const std::string& model, const std::string& message, StreamCallback callback);

    // Generate Content API
    GeminiResponse generateContent(const std::string& model, const std::vector<GeminiContent>& contents);
    GeminiResponse generateContent(const std::string& model, const std::vector<GeminiContent>& contents, const GenerationConfig& config);
    void streamGenerateContent(const std::string& model, const std::vector<GeminiContent>& contents, StreamCallback callback);

    // Embedding API
    GeminiEmbedding embedContent(const std::string& model, const std::string& text);
    std::vector<GeminiEmbedding> batchEmbedContents(const std::string& model, const std::vector<std::string>& texts);

    // Token Counting
    int countTokens(const std::string& model, const std::string& text);
    int countTokens(const std::string& model, const std::vector<GeminiContent>& contents);

    // Batch API
    std::string batchGenerateContent(const std::string& model, const std::vector<std::vector<GeminiContent>>& batch_contents);

    // File API
    GeminiFile uploadFile(const std::string& file_path, const std::string& mime_type);
    GeminiFile getFile(const std::string& file_name);
    void deleteFile(const std::string& file_name);
    std::vector<GeminiFile> listFiles();

    // WebSocket Bidirectional Streaming
    void connectBidiStream(const std::string& model);
    void disconnectBidiStream();
    void sendBidiMessage(const std::vector<GeminiContent>& contents);
    void setBidiCallback(StreamCallback callback);

    // Context Management
    void clearContext();
    std::vector<Message> getContext() const;

private:
    std::string callAPI(const std::string& model, const std::vector<Message>& messages, std::optional<float> temperature = std::nullopt);
    std::vector<GeminiContent> messagesToContents(const std::vector<Message>& messages);
    std::vector<Message> contentsToMessages(const std::vector<GeminiContent>& contents);

    std::unique_ptr<HttpClient> http_client_;
    std::unique_ptr<ContextManager> context_;
    std::unique_ptr<WebSocketClient> ws_client_;
    StreamCallback bidi_callback_;
    GoogleClientConfig config_;
    std::string api_key_;
};

} // namespace ai_sdk
