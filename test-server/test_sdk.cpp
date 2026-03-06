#include "openai_client.hpp"
#include <iostream>

int main() {
    std::cout << "=== Testing OpenAI Client ===" << std::endl;

    ai_sdk::ClientConfig config;
    config.base_url = "http://localhost:8080/v1";
    ai_sdk::OpenAIClient openai("test-key", config);

    std::cout << "Testing chat: ";
    if (auto chat_resp = openai.chat("Hello")) {
        std::cout << "✓ " << chat_resp.value() << std::endl;
    } else {
        std::cout << "✗ " << chat_resp.error().message << std::endl;
        return 1;
    }

    std::cout << "Testing embeddings: ";
    ai_sdk::EmbeddingRequest emb_req;
    emb_req.model = "text-embedding-3-small";
    emb_req.input = {"test"};
    if (auto emb_resp = openai.createEmbedding(emb_req)) {
        std::cout << "✓ Size: " << emb_resp->data[0].embedding.size() << std::endl;
    } else {
        std::cout << "✗ " << emb_resp.error().message << std::endl;
        return 1;
    }

    std::cout << "Testing models: ";
    if (auto models = openai.listModels()) {
        std::cout << "✓ Count: " << models->data.size() << std::endl;
    } else {
        std::cout << "✗ " << models.error().message << std::endl;
        return 1;
    }

    std::cout << "\n=== All Tests Completed ===" << std::endl;
    return 0;
}
