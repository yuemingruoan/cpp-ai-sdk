#pragma once

#include <vector>
#include "models.hpp"

namespace ai_sdk {

class ContextManager {
public:
    explicit ContextManager(int max_tokens = 4000);

    void addMessage(const Message& message);
    std::vector<Message> getMessages() const;
    void clearMessages();
    int estimateTokens() const;
    void trimToLimit();
    void setMaxTokens(int max_tokens);

private:
    std::vector<Message> messages_;
    int max_tokens_;
};

} // namespace ai_sdk
