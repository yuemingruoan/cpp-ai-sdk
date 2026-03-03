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

ImageResponse OpenAIClient::editImage(const ImageEditRequest& request) {
    std::map<std::string, std::string> fields = {{"prompt", request.prompt}};
    if (request.model) fields["model"] = *request.model;
    if (request.n) fields["n"] = std::to_string(*request.n);
    if (request.size) fields["size"] = *request.size;

    std::map<std::string, std::string> files = {{"image", request.image_path}};
    if (request.mask_path) files["mask"] = *request.mask_path;

    std::map<std::string, std::string> headers = {{"Authorization", "Bearer " + api_key_}};

    std::string response = http_client_->postMultipart(config_.base_url + "/images/edits", fields, files, headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

ImageResponse OpenAIClient::createImageVariation(const ImageVariationRequest& request) {
    std::map<std::string, std::string> fields;
    if (request.model) fields["model"] = *request.model;
    if (request.n) fields["n"] = std::to_string(*request.n);
    if (request.size) fields["size"] = *request.size;

    std::map<std::string, std::string> files = {{"image", request.image_path}};
    std::map<std::string, std::string> headers = {{"Authorization", "Bearer " + api_key_}};

    std::string response = http_client_->postMultipart(config_.base_url + "/images/variations", fields, files, headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

CompletionResponse OpenAIClient::createCompletion(const CompletionRequest& request) {
    nlohmann::json json_body = request;
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + api_key_}
    };

    std::string response = http_client_->post(config_.base_url + "/completions", json_body.dump(), headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

CompletionResponse OpenAIClient::createCompletion(const std::string& model, const std::string& prompt) {
    CompletionRequest request;
    request.model = model;
    request.prompt = prompt;
    return createCompletion(request);
}

FileObject OpenAIClient::uploadFile(const std::string& file_path, const std::string& purpose) {
    std::map<std::string, std::string> fields = {{"purpose", purpose}};
    std::map<std::string, std::string> files = {{"file", file_path}};
    std::map<std::string, std::string> headers = {{"Authorization", "Bearer " + api_key_}};

    std::string response = http_client_->postMultipart(config_.base_url + "/files", fields, files, headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

FileListResponse OpenAIClient::listFiles() {
    std::map<std::string, std::string> headers = {{"Authorization", "Bearer " + api_key_}};
    std::string response = http_client_->get(config_.base_url + "/files", headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

FileObject OpenAIClient::retrieveFile(const std::string& file_id) {
    std::map<std::string, std::string> headers = {{"Authorization", "Bearer " + api_key_}};
    std::string response = http_client_->get(config_.base_url + "/files/" + file_id, headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

void OpenAIClient::deleteFile(const std::string& file_id) {
    std::map<std::string, std::string> headers = {{"Authorization", "Bearer " + api_key_}};
    http_client_->deleteRequest(config_.base_url + "/files/" + file_id, headers);
}

std::string OpenAIClient::retrieveFileContent(const std::string& file_id) {
    std::map<std::string, std::string> headers = {{"Authorization", "Bearer " + api_key_}};
    return http_client_->get(config_.base_url + "/files/" + file_id + "/content", headers);
}

BatchObject OpenAIClient::createBatch(const BatchRequest& request) {
    nlohmann::json json_body = request;
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + api_key_}
    };

    std::string response = http_client_->post(config_.base_url + "/batches", json_body.dump(), headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

BatchObject OpenAIClient::retrieveBatch(const std::string& batch_id) {
    std::map<std::string, std::string> headers = {{"Authorization", "Bearer " + api_key_}};
    std::string response = http_client_->get(config_.base_url + "/batches/" + batch_id, headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

BatchObject OpenAIClient::cancelBatch(const std::string& batch_id) {
    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + api_key_}
    };

    std::string response = http_client_->post(config_.base_url + "/batches/" + batch_id + "/cancel", "", headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

BatchListResponse OpenAIClient::listBatches() {
    std::map<std::string, std::string> headers = {{"Authorization", "Bearer " + api_key_}};
    std::string response = http_client_->get(config_.base_url + "/batches", headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        return json_response;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

AudioTranscriptionResponse OpenAIClient::createTranscription(const AudioTranscriptionRequest& request) {
    std::map<std::string, std::string> fields = {{"model", request.model}};
    if (request.language) fields["language"] = *request.language;
    if (request.prompt) fields["prompt"] = *request.prompt;
    if (request.response_format) fields["response_format"] = *request.response_format;
    if (request.temperature) fields["temperature"] = std::to_string(*request.temperature);

    std::map<std::string, std::string> files = {{"file", request.file_path}};
    std::map<std::string, std::string> headers = {{"Authorization", "Bearer " + api_key_}};

    std::string response = http_client_->postMultipart(config_.base_url + "/audio/transcriptions", fields, files, headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        AudioTranscriptionResponse result;
        result.text = json_response["text"].get<std::string>();
        return result;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

AudioTranscriptionResponse OpenAIClient::createTranscription(const std::string& file_path, const std::string& model) {
    AudioTranscriptionRequest request;
    request.file_path = file_path;
    request.model = model;
    return createTranscription(request);
}

AudioTranslationResponse OpenAIClient::createTranslation(const AudioTranslationRequest& request) {
    std::map<std::string, std::string> fields = {{"model", request.model}};
    if (request.prompt) fields["prompt"] = *request.prompt;
    if (request.response_format) fields["response_format"] = *request.response_format;
    if (request.temperature) fields["temperature"] = std::to_string(*request.temperature);

    std::map<std::string, std::string> files = {{"file", request.file_path}};
    std::map<std::string, std::string> headers = {{"Authorization", "Bearer " + api_key_}};

    std::string response = http_client_->postMultipart(config_.base_url + "/audio/translations", fields, files, headers);

    try {
        nlohmann::json json_response = nlohmann::json::parse(response);
        AudioTranslationResponse result;
        result.text = json_response["text"].get<std::string>();
        return result;
    } catch (const std::exception& e) {
        throw ParseException(std::string("Failed to parse response: ") + e.what());
    }
}

AudioTranslationResponse OpenAIClient::createTranslation(const std::string& file_path, const std::string& model) {
    AudioTranslationRequest request;
    request.file_path = file_path;
    request.model = model;
    return createTranslation(request);
}

std::vector<uint8_t> OpenAIClient::createSpeech(const AudioSpeechRequest& request) {
    nlohmann::json json_body;
    json_body["model"] = request.model;
    json_body["input"] = request.input;
    json_body["voice"] = request.voice;
    if (request.response_format) json_body["response_format"] = *request.response_format;
    if (request.speed) json_body["speed"] = *request.speed;

    std::map<std::string, std::string> headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + api_key_}
    };

    return http_client_->postBinary(config_.base_url + "/audio/speech", json_body.dump(), headers);
}

std::vector<uint8_t> OpenAIClient::createSpeech(const std::string& text, const std::string& voice) {
    AudioSpeechRequest request;
    request.model = "tts-1";
    request.input = text;
    request.voice = voice;
    return createSpeech(request);
}

} // namespace ai_sdk
