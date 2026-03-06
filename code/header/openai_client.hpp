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
class WebSocketClient;

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

    Result<std::string> chat(const std::string& message);
    Result<std::string> chat(const std::string& model, const std::string& message);
    Result<std::string> chat(const std::string& model, const std::string& message, float temperature);
    Result<std::string> chat(const std::string& model, const std::vector<Message>& messages);

    std::future<Result<std::string>> chatAsync(const std::string& message);
    std::future<Result<std::string>> chatAsync(const std::string& model, const std::string& message);

    Result<void> chatStream(const std::string& message, StreamCallback callback);
    Result<void> chatStream(const std::string& model, const std::string& message, StreamCallback callback);

    Result<ChatResponse> chatCompletion(const ChatRequest& request);
    std::future<Result<ChatResponse>> chatCompletionAsync(const ChatRequest& request);

    void clearContext();
    std::vector<Message> getContext() const;

    // Embeddings API
    Result<EmbeddingResponse> createEmbedding(const EmbeddingRequest& request);
    Result<EmbeddingResponse> createEmbedding(const std::string& model, const std::vector<std::string>& input);

    // Models API
    Result<ModelsResponse> listModels();
    Result<Model> retrieveModel(const std::string& model_id);

    // Moderations API
    Result<ModerationResponse> createModeration(const ModerationRequest& request);
    Result<ModerationResponse> createModeration(const std::string& input);

    // Images API
    Result<ImageResponse> createImage(const ImageGenerationRequest& request);
    Result<ImageResponse> createImage(const std::string& prompt);
    Result<ImageResponse> editImage(const ImageEditRequest& request);
    Result<ImageResponse> createImageVariation(const ImageVariationRequest& request);

    // Audio API
    Result<AudioTranscriptionResponse> createTranscription(const AudioTranscriptionRequest& request);
    Result<AudioTranscriptionResponse> createTranscription(const std::string& file_path, const std::string& model);
    Result<AudioTranslationResponse> createTranslation(const AudioTranslationRequest& request);
    Result<AudioTranslationResponse> createTranslation(const std::string& file_path, const std::string& model);
    Result<std::vector<uint8_t>> createSpeech(const AudioSpeechRequest& request);
    Result<std::vector<uint8_t>> createSpeech(const std::string& text, const std::string& voice);

    // Completions API (legacy)
    Result<CompletionResponse> createCompletion(const CompletionRequest& request);
    Result<CompletionResponse> createCompletion(const std::string& model, const std::string& prompt);

    // Files API
    Result<FileObject> uploadFile(const std::string& file_path, const std::string& purpose);
    Result<FileListResponse> listFiles();
    Result<FileObject> retrieveFile(const std::string& file_id);
    Result<void> deleteFile(const std::string& file_id);
    Result<std::string> retrieveFileContent(const std::string& file_id);

    // Batch API
    Result<BatchObject> createBatch(const BatchRequest& request);
    Result<BatchObject> retrieveBatch(const std::string& batch_id);
    Result<BatchObject> cancelBatch(const std::string& batch_id);
    Result<BatchListResponse> listBatches();

    // Fine-tuning API
    Result<FineTuningJob> createFineTuningJob(const FineTuningRequest& request);
    Result<FineTuningListResponse> listFineTuningJobs();
    Result<FineTuningJob> retrieveFineTuningJob(const std::string& job_id);
    Result<FineTuningJob> cancelFineTuningJob(const std::string& job_id);

    // Assistants API
    Result<Assistant> createAssistant(const AssistantRequest& request);
    Result<Assistant> retrieveAssistant(const std::string& assistant_id);
    Result<void> deleteAssistant(const std::string& assistant_id);
    Result<Thread> createThread();
    Result<ThreadMessage> createMessage(const std::string& thread_id, const std::string& role, const std::string& content);
    Result<Run> createRun(const std::string& thread_id, const std::string& assistant_id);
    Result<Run> retrieveRun(const std::string& thread_id, const std::string& run_id);

    // Videos API
    Result<VideoObject> createVideo(const VideoGenerationRequest& request);
    Result<VideoObject> getVideoStatus(const std::string& video_id);
    Result<VideoListResponse> listVideos();

    // WebSocket Response API
    void connectResponseWebSocket();
    void disconnectResponseWebSocket();
    void sendResponseCreate(const std::string& model, const std::vector<Message>& messages, const std::string& previous_response_id = "");
    void setResponseEventCallback(StreamCallback callback);

private:
    Result<std::string> callAPI(const std::string& model,
                                const std::vector<Message>& messages,
                                std::optional<float> temperature = std::nullopt);

    std::unique_ptr<HttpClient> http_client_;
    std::unique_ptr<ContextManager> context_;
    std::unique_ptr<WebSocketClient> ws_client_;
    StreamCallback response_event_callback_;
    ClientConfig config_;
    std::string api_key_;
};

} // namespace ai_sdk
