# C++ AI SDK API 文档

## 目录

- [通用说明](#通用说明)
- [OpenAI API](#openai-api)
- [Anthropic API](#anthropic-api)
- [Google Gemini API](#google-gemini-api)

## 通用说明

### 命名空间

所有类和函数都在 `ai_sdk` 命名空间下。

### 客户端配置

所有客户端都支持三种构造方式：

```cpp
// 1. 使用默认配置
Client client("api-key");

// 2. 指定是否启用自动上下文
Client client("api-key", true);  // 启用（默认）
Client client("api-key", false); // 禁用

// 3. 完整配置
ClientConfig config;
config.auto_context = true;
config.max_context_tokens = 4000;
config.default_model = "model-name";
config.default_temperature = 0.7;
Client client("api-key", config);
```

### 自动上下文管理

启用自动上下文后，客户端会自动维护对话历史：

```cpp
OpenAIClient client("api-key");  // 默认启用

auto resp1 = client.chat("My name is Alice");
auto resp2 = client.chat("What's my name?");  // 自动包含历史

client.clearContext();  // 清空历史
```

### 异常处理

所有API调用可能抛出以下异常：

- `NetworkException` - 网络连接失败
- `APIException` - API返回错误（包含HTTP状态码）
- `ParseException` - JSON解析失败

```cpp
try {
    auto response = client.chat("Hello");
} catch (const APIException& e) {
    std::cerr << "API Error [" << e.getStatusCode() << "]: " << e.what() << std::endl;
} catch (const NetworkException& e) {
    std::cerr << "Network Error: " << e.what() << std::endl;
}
```

---

## OpenAI API

### 构造函数

```cpp
#include "openai_client.hpp"

ai_sdk::OpenAIClient client("your-api-key");
```

### Chat Completions

#### chat() - 简化接口

```cpp
// 使用默认model
std::string response = client.chat("Hello!");

// 指定model
std::string response = client.chat("gpt-4", "Hello!");

// 指定model和temperature
std::string response = client.chat("gpt-4", "Hello!", 0.7);
```

#### chatAsync() - 异步调用

```cpp
auto future = client.chatAsync("Tell me a story");
// 做其他工作...
std::string response = future.get();
```

#### chatStream() - 流式响应

```cpp
client.chatStream("Tell me a story", [](const std::string& chunk) {
    std::cout << chunk << std::flush;
});
```

#### chatCompletion() - 完整配置

```cpp
ChatCompletionConfig config;
config.model = "gpt-4";
config.messages = {{"user", "Hello"}};
config.temperature = 0.7;
config.max_tokens = 1000;

ChatResponse response = client.chatCompletion(config);
std::cout << response.choices[0].message.content << std::endl;
```

### Embeddings

```cpp
// 简化接口
std::vector<float> embedding = client.createEmbedding("text-embedding-3-small", "Hello world");

// 完整配置
EmbeddingRequest request;
request.model = "text-embedding-3-small";
request.input = {"text1", "text2"};
request.dimensions = 1536;

EmbeddingResponse response = client.createEmbedding(request);
for (const auto& data : response.data) {
    // data.embedding 是 vector<float>
}
```

### Models

```cpp
// 列出所有模型
ModelsResponse models = client.listModels();
for (const auto& model : models.data) {
    std::cout << model.id << std::endl;
}

// 获取特定模型信息
Model model = client.retrieveModel("gpt-4");
```

### Moderations

```cpp
// 内容审核
ModerationResponse response = client.createModeration("text to moderate");
if (response.results[0].flagged) {
    std::cout << "Content flagged" << std::endl;
}
```

### Images

#### 图像生成

```cpp
// 简化接口
ImageResponse response = client.createImage("A cat on a beach");

// 完整配置
ImageGenerationRequest request;
request.prompt = "A cat on a beach";
request.model = "dall-e-3";
request.n = 1;
request.size = "1024x1024";

ImageResponse response = client.createImage(request);
std::cout << response.data[0].url << std::endl;
```

#### 图像编辑

```cpp
ImageResponse response = client.editImage(
    "path/to/image.png",
    "path/to/mask.png",
    "Add a hat to the cat"
);
```

#### 图像变体

```cpp
ImageResponse response = client.createImageVariation("path/to/image.png");
```

### Audio

#### 音频转录

```cpp
// 简化接口
AudioTranscriptionResponse response = client.createTranscription("audio.mp3", "whisper-1");
std::cout << response.text << std::endl;

// 完整配置
AudioTranscriptionRequest request;
request.file_path = "audio.mp3";
request.model = "whisper-1";
request.language = "en";
request.response_format = "json";

AudioTranscriptionResponse response = client.createTranscription(request);
```

#### 音频翻译

```cpp
AudioTranslationResponse response = client.createTranslation("audio.mp3", "whisper-1");
```

#### 语音合成

```cpp
// 返回二进制音频数据
std::vector<uint8_t> audio = client.createSpeech("Hello world", "alloy");

// 保存到文件
std::ofstream file("output.mp3", std::ios::binary);
file.write(reinterpret_cast<const char*>(audio.data()), audio.size());
```

### Files

```cpp
// 上传文件
FileObject file = client.uploadFile("data.jsonl", "fine-tune");

// 列出文件
FileListResponse files = client.listFiles();

// 获取文件信息
FileObject file = client.retrieveFile("file-id");

// 下载文件内容
std::string content = client.retrieveFileContent("file-id");

// 删除文件
client.deleteFile("file-id");
```

### Batch

```cpp
// 创建批处理任务
BatchRequest request;
request.input_file_id = "file-id";
request.endpoint = "/v1/chat/completions";
request.completion_window = "24h";

BatchObject batch = client.createBatch(request);

// 获取批处理状态
BatchObject batch = client.retrieveBatch("batch-id");

// 取消批处理
BatchObject batch = client.cancelBatch("batch-id");

// 列出批处理任务
BatchListResponse batches = client.listBatches();
```

### Fine-tuning

```cpp
// 创建微调任务
FineTuningRequest request;
request.training_file = "file-id";
request.model = "gpt-3.5-turbo";

FineTuningJob job = client.createFineTuningJob(request);

// 列出微调任务
FineTuningListResponse jobs = client.listFineTuningJobs();

// 获取任务状态
FineTuningJob job = client.retrieveFineTuningJob("job-id");

// 取消任务
FineTuningJob job = client.cancelFineTuningJob("job-id");
```

### Assistants

```cpp
// 创建助手
AssistantRequest request;
request.model = "gpt-4";
request.name = "My Assistant";
request.instructions = "You are a helpful assistant";

Assistant assistant = client.createAssistant(request);

// 获取助手
Assistant assistant = client.retrieveAssistant("assistant-id");

// 删除助手
client.deleteAssistant("assistant-id");

// 创建线程
Thread thread = client.createThread();

// 创建消息
ThreadMessage message = client.createMessage(thread.id, "user", "Hello");

// 运行助手
Run run = client.createRun(thread.id, assistant.id);

// 获取运行状态
Run run = client.retrieveRun(thread.id, run.id);
```

### Videos (Sora)

```cpp
// 创建视频生成任务
VideoGenerationRequest request;
request.prompt = "A cat playing piano";
request.resolution = "720x1280";
request.duration = 5;

VideoObject video = client.createVideo(request);

// 获取视频状态
VideoObject video = client.getVideoStatus("video-id");

// 列出视频
VideoListResponse videos = client.listVideos();
```

### Realtime API (WebSocket)

```cpp
#include "realtime_client.hpp"

// 创建实时客户端
RealtimeClient realtime("api-key", "gpt-4o-realtime-preview-2024-12-17");

// 设置事件回调
realtime.setEventCallback([](const std::string& event_type, const std::string& data) {
    std::cout << "Event: " << event_type << std::endl;
});

// 连接
realtime.connect();

// 发送事件
realtime.sendEvent("session.update", R"({"session": {"modalities": ["text", "audio"]}})");

// 断开连接
realtime.disconnect();
```

### Response API (WebSocket模式)

```cpp
// 连接WebSocket
client.connectResponseWebSocket();

// 设置事件回调
client.setResponseEventCallback([](const std::string& event) {
    std::cout << event << std::endl;
});

// 发送消息
std::vector<Message> messages = {{"user", "Hello"}};
client.sendResponseCreate("gpt-4", messages);

// 断开连接
client.disconnectResponseWebSocket();
```

---

## Anthropic API

### 构造函数

```cpp
#include "anthropic_client.hpp"

ai_sdk::AnthropicClient client("your-api-key");
```

### Messages

#### chat() - 简化接口

```cpp
// 使用默认model
std::string response = client.chat("Hello!");

// 指定model
std::string response = client.chat("claude-3-opus-20240229", "Hello!");

// 指定model和system prompt
std::string response = client.chat("claude-3-opus-20240229", "Hello!", "You are a poet");
```

#### chatAsync() - 异步调用

```cpp
auto future = client.chatAsync("Tell me a story");
std::string response = future.get();
```

#### chatStream() - 流式响应

```cpp
client.chatStream("Tell me a story", [](const std::string& chunk) {
    std::cout << chunk << std::flush;
});
```

#### sendMessage() - 完整配置

```cpp
MessageConfig config;
config.model = "claude-3-opus-20240229";
config.messages = {{"user", "Hello"}};
config.max_tokens = 1024;
config.system = "You are a helpful assistant";

MessageResponse response = client.sendMessage(config);
```

### Message Batches

```cpp
// 创建批量消息任务
std::string batch_id = client.createMessageBatch("requests.jsonl");

// 获取批量任务状态
MessageBatchResponse batch = client.retrieveMessageBatch(batch_id);

// 取消批量任务
MessageBatchResponse batch = client.cancelMessageBatch(batch_id);

// 列出批量任务
MessageBatchListResponse batches = client.listMessageBatches();

// 获取批量结果
std::string results = client.retrieveMessageBatchResults(batch_id);
```

---

## Google Gemini API

### 构造函数

```cpp
#include "google_client.hpp"

ai_sdk::GoogleClient client("your-api-key");
```

### Chat API

#### chat() - 简化接口

```cpp
// 使用默认model
std::string response = client.chat("Hello!");

// 指定model
std::string response = client.chat("gemini-2.5-flash", "Hello!");

// 指定model和temperature
std::string response = client.chat("gemini-2.5-flash", "Hello!", 0.7);
```

#### chatAsync() - 异步调用

```cpp
auto future = client.chatAsync("Tell me a story");
std::string response = future.get();
```

#### chatStream() - 流式响应

```cpp
client.chatStream("Tell me a story", [](const std::string& chunk) {
    std::cout << chunk << std::flush;
});
```

### Generate Content

```cpp
// 构建内容
std::vector<GeminiContent> contents;
GeminiContent content;
content.role = "user";
GeminiPart part;
part.text = "Hello";
content.parts.push_back(part);
contents.push_back(content);

// 生成内容
GeminiResponse response = client.generateContent("gemini-2.5-flash", contents);

// 带配置的生成
GenerationConfig config;
config.temperature = 0.7;
config.max_output_tokens = 1000;

GeminiResponse response = client.generateContent("gemini-2.5-flash", contents, config);
```

### Stream Generate Content

```cpp
client.streamGenerateContent("gemini-2.5-flash", contents, [](const std::string& chunk) {
    std::cout << chunk << std::flush;
});
```

### Embed Content

```cpp
// 单个文本嵌入
GeminiEmbedding embedding = client.embedContent("text-embedding-004", "Hello world");
for (float value : embedding.values) {
    std::cout << value << " ";
}

// 批量嵌入
std::vector<std::string> texts = {"text1", "text2", "text3"};
std::vector<GeminiEmbedding> embeddings = client.batchEmbedContents("text-embedding-004", texts);
```

### Count Tokens

```cpp
// 计算文本的token数
int tokens = client.countTokens("gemini-2.5-flash", "Hello world");

// 计算内容的token数
int tokens = client.countTokens("gemini-2.5-flash", contents);
```

### Batch Generate Content

```cpp
// 批量生成
std::vector<std::vector<GeminiContent>> batch_contents = {contents1, contents2, contents3};
std::string response = client.batchGenerateContent("gemini-2.5-flash", batch_contents);
```

### File API

```cpp
// 上传文件
GeminiFile file = client.uploadFile("path/to/file.pdf", "application/pdf");
std::cout << "File URI: " << file.uri << std::endl;

// 获取文件信息
GeminiFile file = client.getFile("files/file-id");

// 列出文件
std::vector<GeminiFile> files = client.listFiles();

// 删除文件
client.deleteFile("files/file-id");
```

### Bidirectional Streaming (WebSocket)

```cpp
// 连接双向流
client.connectBidiStream("gemini-2.5-flash");

// 设置回调
client.setBidiCallback([](const std::string& message) {
    std::cout << "Received: " << message << std::endl;
});

// 发送消息
client.sendBidiMessage(contents);

// 断开连接
client.disconnectBidiStream();
```

---

## 完整示例

### OpenAI多轮对话

```cpp
#include "openai_client.hpp"
#include <iostream>

int main() {
    ai_sdk::OpenAIClient client("your-api-key");

    std::cout << client.chat("My name is Alice") << std::endl;
    std::cout << client.chat("What's my name?") << std::endl;  // 输出: Alice

    return 0;
}
```

### Anthropic流式响应

```cpp
#include "anthropic_client.hpp"
#include <iostream>

int main() {
    ai_sdk::AnthropicClient client("your-api-key");

    client.chatStream("Tell me a story", [](const std::string& chunk) {
        std::cout << chunk << std::flush;
    });

    return 0;
}
```

### Google嵌入向量

```cpp
#include "google_client.hpp"
#include <iostream>

int main() {
    ai_sdk::GoogleClient client("your-api-key");

    auto embedding = client.embedContent("text-embedding-004", "Hello world");
    std::cout << "Embedding size: " << embedding.values.size() << std::endl;

    return 0;
}
```
