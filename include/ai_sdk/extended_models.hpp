#pragma once

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

namespace ai_sdk {

// Embeddings
struct EmbeddingRequest {
    std::string model;
    std::vector<std::string> input;
    std::optional<std::string> encoding_format;
    std::optional<int> dimensions;
};

struct EmbeddingData {
    std::vector<float> embedding;
    int index;
};

struct EmbeddingResponse {
    std::string object;
    std::vector<EmbeddingData> data;
    std::string model;
    struct {
        int prompt_tokens;
        int total_tokens;
    } usage;
};

// Models
struct Model {
    std::string id;
    std::string object;
    int64_t created;
    std::string owned_by;
};

struct ModelsResponse {
    std::string object;
    std::vector<Model> data;
};

// Images
struct ImageGenerationRequest {
    std::string prompt;
    std::optional<std::string> model;
    std::optional<int> n;
    std::optional<std::string> quality;
    std::optional<std::string> response_format;
    std::optional<std::string> size;
    std::optional<std::string> style;
};

struct ImageData {
    std::optional<std::string> url;
    std::optional<std::string> b64_json;
    std::optional<std::string> revised_prompt;
};

struct ImageResponse {
    int64_t created;
    std::vector<ImageData> data;
};

struct ImageEditRequest {
    std::string image_path;
    std::string prompt;
    std::optional<std::string> mask_path;
    std::optional<std::string> model;
    std::optional<int> n;
    std::optional<std::string> size;
};

struct ImageVariationRequest {
    std::string image_path;
    std::optional<std::string> model;
    std::optional<int> n;
    std::optional<std::string> size;
};

// Audio
struct AudioTranscriptionRequest {
    std::string file_path;
    std::string model;
    std::optional<std::string> language;
    std::optional<std::string> prompt;
    std::optional<std::string> response_format;
    std::optional<float> temperature;
};

struct AudioTranscriptionResponse {
    std::string text;
};

struct AudioTranslationRequest {
    std::string file_path;
    std::string model;
    std::optional<std::string> prompt;
    std::optional<std::string> response_format;
    std::optional<float> temperature;
};

struct AudioTranslationResponse {
    std::string text;
};

struct AudioSpeechRequest {
    std::string model;
    std::string input;
    std::string voice;
    std::optional<std::string> response_format;
    std::optional<float> speed;
};

// Moderations
struct ModerationRequest {
    std::string input;
    std::optional<std::string> model;
};

struct ModerationCategories {
    bool hate;
    bool hate_threatening;
    bool harassment;
    bool harassment_threatening;
    bool self_harm;
    bool self_harm_intent;
    bool self_harm_instructions;
    bool sexual;
    bool sexual_minors;
    bool violence;
    bool violence_graphic;
};

struct ModerationCategoryScores {
    float hate;
    float hate_threatening;
    float harassment;
    float harassment_threatening;
    float self_harm;
    float self_harm_intent;
    float self_harm_instructions;
    float sexual;
    float sexual_minors;
    float violence;
    float violence_graphic;
};

struct ModerationResult {
    bool flagged;
    ModerationCategories categories;
    ModerationCategoryScores category_scores;
};

struct ModerationResponse {
    std::string id;
    std::string model;
    std::vector<ModerationResult> results;
};

// Completions (legacy)
struct CompletionRequest {
    std::string model;
    std::string prompt;
    std::optional<int> max_tokens;
    std::optional<float> temperature;
};

struct CompletionChoice {
    std::string text;
    int index;
    std::string finish_reason;
};

struct CompletionResponse {
    std::string id;
    std::string object;
    int64_t created;
    std::string model;
    std::vector<CompletionChoice> choices;
    struct {
        int prompt_tokens;
        int completion_tokens;
        int total_tokens;
    } usage;
};

// Files API
struct FileObject {
    std::string id;
    std::string object;
    int bytes;
    int64_t created_at;
    std::string filename;
    std::string purpose;
};

struct FileListResponse {
    std::string object;
    std::vector<FileObject> data;
};

void to_json(nlohmann::json& j, const EmbeddingRequest& r);
void from_json(const nlohmann::json& j, EmbeddingResponse& r);
void from_json(const nlohmann::json& j, ModelsResponse& r);
void to_json(nlohmann::json& j, const ImageGenerationRequest& r);
void from_json(const nlohmann::json& j, ImageResponse& r);
void to_json(nlohmann::json& j, const ModerationRequest& r);
void from_json(const nlohmann::json& j, ModerationResponse& r);
void to_json(nlohmann::json& j, const CompletionRequest& r);
void from_json(const nlohmann::json& j, CompletionResponse& r);
void from_json(const nlohmann::json& j, FileObject& r);
void from_json(const nlohmann::json& j, FileListResponse& r);

} // namespace ai_sdk
