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
    ImageResponse editImage(const ImageEditRequest& request);
    ImageResponse createImageVariation(const ImageVariationRequest& request);

    // Audio API
    AudioTranscriptionResponse createTranscription(const AudioTranscriptionRequest& request);
    AudioTranscriptionResponse createTranscription(const std::string& file_path, const std::string& model);
    AudioTranslationResponse createTranslation(const AudioTranslationRequest& request);
    AudioTranslationResponse createTranslation(const std::string& file_path, const std::string& model);
    std::vector<uint8_t> createSpeech(const AudioSpeechRequest& request);
    std::vector<uint8_t> createSpeech(const std::string& text, const std::string& voice);

    // Completions API (legacy)
    CompletionResponse createCompletion(const CompletionRequest& request);
    CompletionResponse createCompletion(const std::string& model, const std::string& prompt);

    // Files API
    FileObject uploadFile(const std::string& file_path, const std::string& purpose);
    FileListResponse listFiles();
    FileObject retrieveFile(const std::string& file_id);
    void deleteFile(const std::string& file_id);
    std::string retrieveFileContent(const std::string& file_id);

    // Batch API
    BatchObject createBatch(const BatchRequest& request);
    BatchObject retrieveBatch(const std::string& batch_id);
    BatchObject cancelBatch(const std::string& batch_id);
    BatchListResponse listBatches();

    // Fine-tuning API
    FineTuningJob createFineTuningJob(const FineTuningRequest& request);
    FineTuningListResponse listFineTuningJobs();
    FineTuningJob retrieveFineTuningJob(const std::string& job_id);
    FineTuningJob cancelFineTuningJob(const std::string& job_id);

    // Assistants API
    Assistant createAssistant(const AssistantRequest& request);
    Assistant retrieveAssistant(const std::string& assistant_id);
    void deleteAssistant(const std::string& assistant_id);
    Thread createThread();
    ThreadMessage createMessage(const std::string& thread_id, const std::string& role, const std::string& content);
    Run createRun(const std::string& thread_id, const std::string& assistant_id);
    Run retrieveRun(const std::string& thread_id, const std::string& run_id);

    // Videos API
    VideoObject createVideo(const VideoGenerationRequest& request);
    VideoObject getVideoStatus(const std::string& video_id);
    VideoListResponse listVideos();

private:
    std::string callAPI(const std::string& model, const std::vector<Message>& messages,
                        std::optional<float> temperature = std::nullopt);

    std::unique_ptr<HttpClient> http_client_;
    std::unique_ptr<ContextManager> context_;
    ClientConfig config_;
    std::string api_key_;
};

} // namespace ai_sdk
