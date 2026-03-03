#include "openai_client.hpp"
#include <iostream>

int main() {
    try {
        // 从环境变量获取API key
        const char* api_key = std::getenv("OPENAI_API_KEY");
        if (!api_key) {
            std::cerr << "请设置 OPENAI_API_KEY 环境变量" << std::endl;
            return 1;
        }

        // 示例1: 最简单的调用
        std::cout << "=== 示例1: 简单调用 ===" << std::endl;
        ai_sdk::OpenAIClient client(api_key);
        std::string response1 = client.chat("What is C++?");
        std::cout << response1 << std::endl << std::endl;

        // 示例2: 多轮对话（自动上下文）
        std::cout << "=== 示例2: 多轮对话 ===" << std::endl;
        std::string response2 = client.chat("My name is Alice");
        std::cout << response2 << std::endl;

        std::string response3 = client.chat("What's my name?");
        std::cout << response3 << std::endl << std::endl;

        // 示例3: 指定模型和温度
        std::cout << "=== 示例3: 指定参数 ===" << std::endl;
        std::string response4 = client.chat("gpt-3.5-turbo", "Tell me a joke", 0.9f);
        std::cout << response4 << std::endl << std::endl;

        // 示例4: 清空上下文
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
