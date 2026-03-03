#include "models.hpp"

namespace ai_sdk {

void to_json(nlohmann::json& j, const Message& m) {
    j = nlohmann::json{{"role", m.role}, {"content", m.content}};
}

void from_json(const nlohmann::json& j, Message& m) {
    j.at("role").get_to(m.role);
    j.at("content").get_to(m.content);
}

void to_json(nlohmann::json& j, const ChatRequest& r) {
    j = nlohmann::json{{"model", r.model}, {"messages", r.messages}};
    if (r.temperature) j["temperature"] = *r.temperature;
    if (r.max_tokens) j["max_tokens"] = *r.max_tokens;
    if (r.stream) j["stream"] = *r.stream;
}

void from_json(const nlohmann::json& j, ChatResponse& r) {
    j.at("id").get_to(r.id);
    j.at("object").get_to(r.object);
    j.at("created").get_to(r.created);
    j.at("model").get_to(r.model);

    for (const auto& choice_json : j.at("choices")) {
        Choice choice;
        choice_json.at("index").get_to(choice.index);
        choice_json.at("message").get_to(choice.message);
        choice_json.at("finish_reason").get_to(choice.finish_reason);
        r.choices.push_back(choice);
    }

    const auto& usage_json = j.at("usage");
    usage_json.at("prompt_tokens").get_to(r.usage.prompt_tokens);
    usage_json.at("completion_tokens").get_to(r.usage.completion_tokens);
    usage_json.at("total_tokens").get_to(r.usage.total_tokens);
}

} // namespace ai_sdk
