#pragma once

#include <string>
#include <memory>
#include <future>
#include "models.hpp"
#include "types.hpp"

namespace ai_sdk {

struct ClientConfig {
    bool auto_context = true;
    int max_context_tokens = 4000;
    std::string default_model = "gpt-4";
    float default_temperature = 0.7f;
    std::string base_url = "https://api.openai.com/v1";
};

class HttpClient;
class ContextManager;

class AnthropicClient {
public:
    explicit AnthropicClient(const std::string& api_key);
    AnthropicClient(const std::string& api_key, bool auto_context);
    AnthropicClient(const std::string& api_key, const ClientConfig& config);
    ~AnthropicClient();

    std::string chat(const std::string& message);
    std::string chat(const std::string& model, const std::string& message);
    std::string chat(const std::string& model, const std::string& message, const std::string& system_prompt);
    std::string chat(const std::string& model, const std::vector<Message>& messages);

    std::future<std::string> chatAsync(const std::string& message);
    std::future<std::string> chatAsync(const std::string& model, const std::string& message);

    void chatStream(const std::string& message, StreamCallback callback);
    void chatStream(const std::string& model, const std::string& message, StreamCallback callback);

    void clearContext();
    std::vector<Message> getContext() const;

    // Message Batches API
    std::string createMessageBatch(const std::string& requests_file_path);
    std::string retrieveMessageBatch(const std::string& batch_id);
    std::string cancelMessageBatch(const std::string& batch_id);

private:
    std::string callAPI(const std::string& model, const std::vector<Message>& messages,
                        const std::string& system_prompt = "");

    std::unique_ptr<HttpClient> http_client_;
    std::unique_ptr<ContextManager> context_;
    ClientConfig config_;
    std::string api_key_;
};

} // namespace ai_sdk
