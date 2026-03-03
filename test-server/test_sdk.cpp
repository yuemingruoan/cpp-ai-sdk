#include "openai_client.hpp"
#include <iostream>

int main() {
    std::cout << "=== Testing OpenAI Client ===" << std::endl;

    try {
        ai_sdk::ClientConfig config;
        config.base_url = "http://localhost:8080/v1";
        ai_sdk::OpenAIClient openai("test-key", config);

        std::cout << "Testing chat: ";
        auto chat_resp = openai.chat("Hello");
        std::cout << "✓ " << chat_resp << std::endl;

        std::cout << "Testing embeddings: ";
        ai_sdk::EmbeddingRequest emb_req;
        emb_req.model = "text-embedding-3-small";
        emb_req.input = {"test"};
        auto emb_resp = openai.createEmbedding(emb_req);
        std::cout << "✓ Size: " << emb_resp.data[0].embedding.size() << std::endl;

        std::cout << "Testing models: ";
        auto models = openai.listModels();
        std::cout << "✓ Count: " << models.data.size() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "✗ Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n=== All Tests Completed ===" << std::endl;
    return 0;
}
