#include "ai_sdk/extended_models.hpp"

namespace ai_sdk {

void to_json(nlohmann::json& j, const EmbeddingRequest& r) {
    j = nlohmann::json{{"model", r.model}, {"input", r.input}};
    if (r.encoding_format) j["encoding_format"] = *r.encoding_format;
    if (r.dimensions) j["dimensions"] = *r.dimensions;
}

void from_json(const nlohmann::json& j, EmbeddingResponse& r) {
    j.at("object").get_to(r.object);
    j.at("model").get_to(r.model);

    for (const auto& item : j.at("data")) {
        EmbeddingData data;
        item.at("index").get_to(data.index);
        item.at("embedding").get_to(data.embedding);
        r.data.push_back(data);
    }

    j.at("usage").at("prompt_tokens").get_to(r.usage.prompt_tokens);
    j.at("usage").at("total_tokens").get_to(r.usage.total_tokens);
}

void from_json(const nlohmann::json& j, ModelsResponse& r) {
    j.at("object").get_to(r.object);
    for (const auto& item : j.at("data")) {
        Model model;
        item.at("id").get_to(model.id);
        item.at("object").get_to(model.object);
        item.at("created").get_to(model.created);
        item.at("owned_by").get_to(model.owned_by);
        r.data.push_back(model);
    }
}

void to_json(nlohmann::json& j, const ImageGenerationRequest& r) {
    j = nlohmann::json{{"prompt", r.prompt}};
    if (r.model) j["model"] = *r.model;
    if (r.n) j["n"] = *r.n;
    if (r.quality) j["quality"] = *r.quality;
    if (r.response_format) j["response_format"] = *r.response_format;
    if (r.size) j["size"] = *r.size;
    if (r.style) j["style"] = *r.style;
}

void from_json(const nlohmann::json& j, ImageResponse& r) {
    j.at("created").get_to(r.created);
    for (const auto& item : j.at("data")) {
        ImageData data;
        if (item.contains("url")) data.url = item["url"].get<std::string>();
        if (item.contains("b64_json")) data.b64_json = item["b64_json"].get<std::string>();
        if (item.contains("revised_prompt")) data.revised_prompt = item["revised_prompt"].get<std::string>();
        r.data.push_back(data);
    }
}

void to_json(nlohmann::json& j, const ModerationRequest& r) {
    j = nlohmann::json{{"input", r.input}};
    if (r.model) j["model"] = *r.model;
}

void from_json(const nlohmann::json& j, ModerationResponse& r) {
    j.at("id").get_to(r.id);
    j.at("model").get_to(r.model);

    for (const auto& item : j.at("results")) {
        ModerationResult result;
        item.at("flagged").get_to(result.flagged);

        const auto& cats = item.at("categories");
        result.categories.hate = cats["hate"];
        result.categories.hate_threatening = cats["hate/threatening"];
        result.categories.harassment = cats["harassment"];
        result.categories.harassment_threatening = cats["harassment/threatening"];
        result.categories.self_harm = cats["self-harm"];
        result.categories.self_harm_intent = cats["self-harm/intent"];
        result.categories.self_harm_instructions = cats["self-harm/instructions"];
        result.categories.sexual = cats["sexual"];
        result.categories.sexual_minors = cats["sexual/minors"];
        result.categories.violence = cats["violence"];
        result.categories.violence_graphic = cats["violence/graphic"];

        const auto& scores = item.at("category_scores");
        result.category_scores.hate = scores["hate"];
        result.category_scores.hate_threatening = scores["hate/threatening"];
        result.category_scores.harassment = scores["harassment"];
        result.category_scores.harassment_threatening = scores["harassment/threatening"];
        result.category_scores.self_harm = scores["self-harm"];
        result.category_scores.self_harm_intent = scores["self-harm/intent"];
        result.category_scores.self_harm_instructions = scores["self-harm/instructions"];
        result.category_scores.sexual = scores["sexual"];
        result.category_scores.sexual_minors = scores["sexual/minors"];
        result.category_scores.violence = scores["violence"];
        result.category_scores.violence_graphic = scores["violence/graphic"];

        r.results.push_back(result);
    }
}

void to_json(nlohmann::json& j, const CompletionRequest& r) {
    j = nlohmann::json{{"model", r.model}, {"prompt", r.prompt}};
    if (r.max_tokens) j["max_tokens"] = *r.max_tokens;
    if (r.temperature) j["temperature"] = *r.temperature;
}

void from_json(const nlohmann::json& j, CompletionResponse& r) {
    j.at("id").get_to(r.id);
    j.at("object").get_to(r.object);
    j.at("created").get_to(r.created);
    j.at("model").get_to(r.model);

    for (const auto& item : j.at("choices")) {
        CompletionChoice choice;
        item.at("text").get_to(choice.text);
        item.at("index").get_to(choice.index);
        item.at("finish_reason").get_to(choice.finish_reason);
        r.choices.push_back(choice);
    }

    j.at("usage").at("prompt_tokens").get_to(r.usage.prompt_tokens);
    j.at("usage").at("completion_tokens").get_to(r.usage.completion_tokens);
    j.at("usage").at("total_tokens").get_to(r.usage.total_tokens);
}

void from_json(const nlohmann::json& j, FileObject& r) {
    j.at("id").get_to(r.id);
    j.at("object").get_to(r.object);
    j.at("bytes").get_to(r.bytes);
    j.at("created_at").get_to(r.created_at);
    j.at("filename").get_to(r.filename);
    j.at("purpose").get_to(r.purpose);
}

void from_json(const nlohmann::json& j, FileListResponse& r) {
    j.at("object").get_to(r.object);
    for (const auto& item : j.at("data")) {
        FileObject file;
        from_json(item, file);
        r.data.push_back(file);
    }
}

void to_json(nlohmann::json& j, const BatchRequest& r) {
    j = nlohmann::json{
        {"input_file_id", r.input_file_id},
        {"endpoint", r.endpoint},
        {"completion_window", r.completion_window}
    };
    if (r.metadata) j["metadata"] = *r.metadata;
}

void from_json(const nlohmann::json& j, BatchObject& r) {
    j.at("id").get_to(r.id);
    j.at("object").get_to(r.object);
    j.at("endpoint").get_to(r.endpoint);
    j.at("input_file_id").get_to(r.input_file_id);
    j.at("completion_window").get_to(r.completion_window);
    j.at("status").get_to(r.status);
    j.at("created_at").get_to(r.created_at);
}

void from_json(const nlohmann::json& j, BatchListResponse& r) {
    j.at("object").get_to(r.object);
    for (const auto& item : j.at("data")) {
        BatchObject batch;
        from_json(item, batch);
        r.data.push_back(batch);
    }
}

void to_json(nlohmann::json& j, const FineTuningRequest& r) {
    j = nlohmann::json{{"model", r.model}, {"training_file", r.training_file}};
    if (r.validation_file) j["validation_file"] = *r.validation_file;
    if (r.n_epochs) j["hyperparameters"]["n_epochs"] = *r.n_epochs;
}

void from_json(const nlohmann::json& j, FineTuningJob& r) {
    j.at("id").get_to(r.id);
    j.at("object").get_to(r.object);
    j.at("model").get_to(r.model);
    j.at("created_at").get_to(r.created_at);
    j.at("status").get_to(r.status);
    j.at("training_file").get_to(r.training_file);
}

void from_json(const nlohmann::json& j, FineTuningListResponse& r) {
    j.at("object").get_to(r.object);
    for (const auto& item : j.at("data")) {
        FineTuningJob job;
        from_json(item, job);
        r.data.push_back(job);
    }
}

void to_json(nlohmann::json& j, const AssistantRequest& r) {
    j = nlohmann::json{{"model", r.model}};
    if (r.name) j["name"] = *r.name;
    if (r.instructions) j["instructions"] = *r.instructions;
}

void from_json(const nlohmann::json& j, Assistant& r) {
    j.at("id").get_to(r.id);
    j.at("object").get_to(r.object);
    j.at("created_at").get_to(r.created_at);
    j.at("model").get_to(r.model);
    if (j.contains("name") && !j["name"].is_null()) r.name = j["name"].get<std::string>();
    if (j.contains("instructions") && !j["instructions"].is_null()) r.instructions = j["instructions"].get<std::string>();
}

void from_json(const nlohmann::json& j, Thread& r) {
    j.at("id").get_to(r.id);
    j.at("object").get_to(r.object);
    j.at("created_at").get_to(r.created_at);
}

void from_json(const nlohmann::json& j, ThreadMessage& r) {
    j.at("id").get_to(r.id);
    j.at("object").get_to(r.object);
    j.at("created_at").get_to(r.created_at);
    j.at("thread_id").get_to(r.thread_id);
    j.at("role").get_to(r.role);
    if (j.contains("content") && j["content"].is_array() && !j["content"].empty()) {
        r.content = j["content"][0]["text"]["value"].get<std::string>();
    }
}

void from_json(const nlohmann::json& j, Run& r) {
    j.at("id").get_to(r.id);
    j.at("object").get_to(r.object);
    j.at("created_at").get_to(r.created_at);
    j.at("thread_id").get_to(r.thread_id);
    j.at("assistant_id").get_to(r.assistant_id);
    j.at("status").get_to(r.status);
}

void to_json(nlohmann::json& j, const VideoGenerationRequest& r) {
    j["prompt"] = r.prompt;
    if (r.resolution) j["resolution"] = *r.resolution;
    if (r.duration) j["duration"] = *r.duration;
}

void from_json(const nlohmann::json& j, VideoObject& r) {
    j.at("id").get_to(r.id);
    j.at("status").get_to(r.status);
    if (j.contains("url") && !j["url"].is_null()) r.url = j["url"].get<std::string>();
    if (j.contains("created_at")) r.created_at = j["created_at"].get<int64_t>();
}

void from_json(const nlohmann::json& j, VideoListResponse& r) {
    j.at("data").get_to(r.data);
}

} // namespace ai_sdk
