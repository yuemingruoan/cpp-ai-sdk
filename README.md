# C++ AI SDK

一个统一的C++ SDK，支持OpenAI和Anthropic的API调用。

## 特性

- 支持OpenAI Chat Completions API
- 支持Anthropic Messages API
- 自动上下文管理（多轮对话）
- 异步调用支持
- 流式响应支持
- 简洁的API设计

## 依赖

- C++17或更高版本
- libcurl
- nlohmann/json

## 安装依赖

### macOS
```bash
brew install curl nlohmann-json
```

### Ubuntu/Debian
```bash
sudo apt-get install libcurl4-openssl-dev nlohmann-json3-dev
```

## 构建

```bash
mkdir build && cd build
cmake ..
make
```

## 快速开始

### OpenAI示例

```cpp
#include "ai_sdk/openai_client.hpp"

int main() {
    ai_sdk::OpenAIClient client("your-api-key");
    std::string response = client.chat("Hello!");
    std::cout << response << std::endl;
    return 0;
}
```

### Anthropic示例

```cpp
#include "ai_sdk/anthropic_client.hpp"

int main() {
    ai_sdk::AnthropicClient client("your-api-key");
    std::string response = client.chat("Hello!");
    std::cout << response << std::endl;
    return 0;
}
```

## API文档

### 构造函数

```cpp
// 使用默认配置
OpenAIClient client("api-key");

// 禁用自动上下文
OpenAIClient client("api-key", false);

// 自定义配置
ClientConfig config;
config.auto_context = true;
config.max_context_tokens = 8000;
OpenAIClient client("api-key", config);
```

### chat() 方法

```cpp
// 使用默认model
client.chat("message");

// 指定model
client.chat("gpt-4", "message");

// 指定model和temperature
client.chat("gpt-4", "message", 0.7);
```

### 异步调用

```cpp
auto future = client.chatAsync("Tell me a story");
std::string response = future.get();
```

### 流式响应

```cpp
client.chatStream("Tell me a story", [](const std::string& chunk) {
    std::cout << chunk << std::flush;
});
```

## 运行示例

```bash
export OPENAI_API_KEY="your-key"
export ANTHROPIC_API_KEY="your-key"

./build/openai_example
./build/anthropic_example
```

## 许可证

MIT
