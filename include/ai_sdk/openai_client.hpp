#pragma once

#include <string>
#include <memory>
#include <future>
#include <optional>
#include "models.hpp"
#include "extended_models.hpp"
#include "types.hpp"

namespace ai_sdk {

class HttpClient;
class ContextManager;

struct ClientConfig {
    bool auto_context = true;
    int max_context_tokens = 4000;
    std::string default_model = "gpt-4";
    float default_temperature = 0.7f;
    std::string base_url = "https://api.openai.com/v1";
};

class OpenAIClient {
public:
    explicit OpenAIClient(const std::string& api_key);
    OpenAIClient(const std::string& api_key, bool auto_context);
    OpenAIClient(const std::string& api_key, const ClientConfig& config);
    ~OpenAIClient();

    std::string chat(const std::string& message);
    std::string chat(const std::string& model, const std::string& message);
    std::string chat(const std::string& model, const std::string& message, float temperature);
    std::string chat(const std::string& model, const std::vector<Message>& messages);

    std::future<std::string> chatAsync(const std::string& message);
    std::future<std::string> chatAsync(const std::string& model, const std::string& message);

    void chatStream(const std::string& message, StreamCallback callback);
    void chatStream(const std::string& model, const std::string& message, StreamCallback callback);

    ChatResponse chatCompletion(const ChatRequest& request);
    std::future<ChatResponse> chatCompletionAsync(const ChatRequest& request);

    void clearContext();
    std::vector<Message> getContext() const;

    // Embeddings API
    EmbeddingResponse createEmbedding(const EmbeddingRequest& request);
    EmbeddingResponse createEmbedding(const std::string& model, const std::vector<std::string>& input);

    // Models API
    ModelsResponse listModels();
    Model retrieveModel(const std::string& model_id);

    // Moderations API
    ModerationResponse createModeration(const ModerationRequest& request);
    ModerationResponse createModeration(const std::string& input);

    // Images API
    ImageResponse createImage(const ImageGenerationRequest& request);
    ImageResponse createImage(const std::string& prompt);

private:
    std::string callAPI(const std::string& model, const std::vector<Message>& messages,
                        std::optional<float> temperature = std::nullopt);

    std::unique_ptr<HttpClient> http_client_;
    std::unique_ptr<ContextManager> context_;
    ClientConfig config_;
    std::string api_key_;
};

} // namespace ai_sdk
