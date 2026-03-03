#include "ai_sdk/anthropic_client.hpp"
#include <iostream>

int main() {
    try {
        const char* api_key = std::getenv("ANTHROPIC_API_KEY");
        if (!api_key) {
            std::cerr << "请设置 ANTHROPIC_API_KEY 环境变量" << std::endl;
            return 1;
        }

        std::cout << "=== 示例1: 简单调用 ===" << std::endl;
        ai_sdk::AnthropicClient client(api_key);
        std::string response1 = client.chat("What is AI?");
        std::cout << response1 << std::endl << std::endl;

        std::cout << "=== 示例2: 多轮对话 ===" << std::endl;
        std::string response2 = client.chat("My name is Bob");
        std::cout << response2 << std::endl;

        std::string response3 = client.chat("What's my name?");
        std::cout << response3 << std::endl << std::endl;

        std::cout << "=== 示例3: 使用system prompt ===" << std::endl;
        std::string response4 = client.chat("claude-3-opus-20240229", "Write a haiku", "You are a poet");
        std::cout << response4 << std::endl << std::endl;

        client.clearContext();
        std::cout << "上下文已清空" << std::endl;

    } catch (const ai_sdk::APIException& e) {
        std::cerr << "API错误 [" << e.getStatusCode() << "]: " << e.what() << std::endl;
        return 1;
    } catch (const ai_sdk::SDKException& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
