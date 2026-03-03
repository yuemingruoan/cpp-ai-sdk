#include "openai_client.hpp"
#include <iostream>
#include <fstream>

void testOpenAI() {
    std::cout << "\n=== Testing OpenAI Client ===" << std::endl;

    ai_sdk::ClientConfig config;
    config.base_url = "http://localhost:8080/v1";
    ai_sdk::OpenAIClient client("test-key", config);

    // Test 1: Chat
    try {
        std::cout << "1. Chat: ";
        auto resp = client.chat("Hello");
        std::cout << "✓" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ " << e.what() << std::endl;
    }

    // Test 2: Embeddings
    try {
        std::cout << "2. Embeddings: ";
        ai_sdk::EmbeddingRequest req;
        req.model = "text-embedding-3-small";
        req.input = {"test"};
        auto resp = client.createEmbedding(req);
        std::cout << "✓ (size: " << resp.data[0].embedding.size() << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ " << e.what() << std::endl;
    }

    // Test 3: List Models
    try {
        std::cout << "3. List Models: ";
        auto resp = client.listModels();
        std::cout << "✓ (count: " << resp.data.size() << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ " << e.what() << std::endl;
    }

    // Test 4: Retrieve Model
    try {
        std::cout << "4. Retrieve Model: ";
        auto resp = client.retrieveModel("gpt-4");
        std::cout << "✓ (id: " << resp.id << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ " << e.what() << std::endl;
    }

    // Test 5: Moderation
    try {
        std::cout << "5. Moderation: ";
        auto resp = client.createModeration("test text");
        std::cout << "✓ (flagged: " << resp.results[0].flagged << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ " << e.what() << std::endl;
    }

    // Test 6: Image Generation
    try {
        std::cout << "6. Image Generation: ";
        auto resp = client.createImage("A cat");
        std::cout << "✓" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ " << e.what() << std::endl;
    }
}

int main() {
    testOpenAI();
    std::cout << "\n=== Tests Completed ===" << std::endl;
    return 0;
}
