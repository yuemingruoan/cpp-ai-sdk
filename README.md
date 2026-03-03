# C++ AI SDK

一个统一的C++ SDK，支持OpenAI、Anthropic和Google Gemini的API调用。

## 特性

### OpenAI API支持（27个端点）
- Chat Completions - 对话生成
- Embeddings - 文本向量化
- Models - 模型管理
- Moderations - 内容审核
- Images (Generations/Edits/Variations) - 图像处理
- Completions (legacy) - 传统文本补全
- Audio (Transcriptions/Translations/Speech) - 音频处理
- Files - 文件管理
- Batch - 批处理任务
- Fine-tuning - 模型微调
- Assistants - 助手系统
- Videos (Sora) - 视频生成
- Realtime API - 实时语音对话（WebSocket）
- Response API - WebSocket模式

### Anthropic API支持（2个端点）
- Messages - Claude对话
- Message Batches - 批量消息

### Google Gemini API支持（9类API）
- Generate Content - 标准文本生成
- Stream Generate Content - SSE流式生成
- Embed Content - 文本嵌入（单个/批量）
- Count Tokens - Token计数
- Batch Generate Content - 批量生成
- File API - 文件上传/获取/删除/列表
- Bidirectional Streaming - WebSocket双向流
- Chat API - 简化接口
- Context Management - 自动上下文管理

### 核心功能
- 自动上下文管理（多轮对话）
- 异步调用支持
- 流式响应支持
- 文件上传/下载
- 二进制数据处理
- OAuth 2.0认证（Google Gemini）
- 简洁的API设计

## 依赖

- C++17或更高版本
- libcurl
- nlohmann/json（已包含在项目中）
- IXWebSocket（已包含在项目中）

## 安装依赖

### macOS
```bash
brew install curl
```

### Ubuntu/Debian
```bash
sudo apt-get install libcurl4-openssl-dev
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

### Google Gemini示例

#### API Key认证
```cpp
#include "ai_sdk/google_client.hpp"

int main() {
    ai_sdk::GoogleClient client("your-api-key");
    std::string response = client.chat("Hello!");
    std::cout << response << std::endl;
    return 0;
}
```

#### OAuth 2.0认证
```cpp
#include "ai_sdk/google_client.hpp"

int main() {
    // 方式1：使用refresh_token（推荐）
    ai_sdk::OAuthConfig oauth_config;
    oauth_config.client_id = "your-client-id";
    oauth_config.client_secret = "your-client-secret";
    oauth_config.refresh_token = "your-refresh-token";

    ai_sdk::GoogleClient client(oauth_config);

    // 方式2：直接使用access_token
    ai_sdk::OAuthConfig oauth_config2;
    oauth_config2.access_token = "your-access-token";

    ai_sdk::GoogleClient client2(oauth_config2);

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

### API Key认证
```bash
export OPENAI_API_KEY="your-key"
export ANTHROPIC_API_KEY="your-key"
export GOOGLE_API_KEY="your-key"

./build/openai_example
./build/anthropic_example
./build/google_example
```

### OAuth认证（仅Google）
```bash
# 使用refresh_token
export GOOGLE_CLIENT_ID="your-client-id"
export GOOGLE_CLIENT_SECRET="your-client-secret"
export GOOGLE_REFRESH_TOKEN="your-refresh-token"
./build/google_oauth_example

# 直接使用access_token
export GOOGLE_ACCESS_TOKEN="your-access-token"
./build/google_access_token_example
```

## 项目结构

```
cpp-ai-sdk/
├── code/
│   ├── header/ai_sdk/        # 头文件
│   ├── cpp/                  # 源文件
│   └── third_party/          # 第三方库
├── examples/                 # 示例代码
├── build/                    # 编译输出
└── CMakeLists.txt
```

## 许可证

MIT
