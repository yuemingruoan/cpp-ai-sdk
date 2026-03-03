#pragma once

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

namespace ai_sdk {

struct Message {
    std::string role;
    std::string content;
};

struct ChatRequest {
    std::string model;
    std::vector<Message> messages;
    std::optional<float> temperature;
    std::optional<int> max_tokens;
    std::optional<bool> stream;
};

struct Choice {
    int index;
    Message message;
    std::string finish_reason;
};

struct Usage {
    int prompt_tokens;
    int completion_tokens;
    int total_tokens;
};

struct ChatResponse {
    std::string id;
    std::string object;
    int64_t created;
    std::string model;
    std::vector<Choice> choices;
    Usage usage;
};

void to_json(nlohmann::json& j, const Message& m);
void from_json(const nlohmann::json& j, Message& m);
void to_json(nlohmann::json& j, const ChatRequest& r);
void from_json(const nlohmann::json& j, ChatResponse& r);

} // namespace ai_sdk
