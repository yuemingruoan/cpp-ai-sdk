#include "context_manager.hpp"

namespace ai_sdk {

ContextManager::ContextManager(int max_tokens) : max_tokens_(max_tokens) {}

void ContextManager::addMessage(const Message& message) {
    messages_.push_back(message);
    trimToLimit();
}

std::vector<Message> ContextManager::getMessages() const {
    return messages_;
}

void ContextManager::clearMessages() {
    messages_.clear();
}

int ContextManager::estimateTokens() const {
    int total = 0;
    for (const auto& msg : messages_) {
        total += msg.content.length() / 4;
    }
    return total;
}

void ContextManager::trimToLimit() {
    while (estimateTokens() > max_tokens_ && messages_.size() > 1) {
        messages_.erase(messages_.begin());
    }
}

void ContextManager::setMaxTokens(int max_tokens) {
    max_tokens_ = max_tokens;
    trimToLimit();
}

} // namespace ai_sdk
