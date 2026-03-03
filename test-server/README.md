# AI SDK 测试服务器

## 概述
本测试服务器用于验证C++ AI SDK的所有接口是否正常工作。服务器返回硬编码的mock数据，确保SDK能正确发起请求和处理响应。

## 启动服务器

```bash
cd test-server
npm install
npm start
```

服务器将在 http://localhost:8080 启动

## 已实现的端点

### OpenAI API (http://localhost:8080/v1)
- ✅ POST /chat/completions - Chat对话
- ✅ POST /embeddings - 文本向量化
- ✅ GET /models - 列出模型
- ✅ GET /models/:model - 获取模型信息
- ✅ POST /moderations - 内容审核
- ✅ POST /images/generations - 图像生成
- ✅ POST /audio/transcriptions - 音频转录
- ✅ POST /audio/translations - 音频翻译
- ✅ POST /audio/speech - 语音合成
- ✅ POST /files - 上传文件
- ✅ GET /files - 列出文件
- ✅ GET /files/:file_id - 获取文件信息
- ✅ GET /files/:file_id/content - 下载文件内容
- ✅ DELETE /files/:file_id - 删除文件
- ✅ POST /batches - 创建批处理
- ✅ GET /batches/:batch_id - 获取批处理状态
- ✅ POST /batches/:batch_id/cancel - 取消批处理
- ✅ GET /batches - 列出批处理
- ✅ POST /fine_tuning/jobs - 创建微调任务
- ✅ GET /fine_tuning/jobs - 列出微调任务
- ✅ GET /fine_tuning/jobs/:job_id - 获取微调任务
- ✅ POST /fine_tuning/jobs/:job_id/cancel - 取消微调任务
- ✅ POST /assistants - 创建助手
- ✅ GET /assistants/:assistant_id - 获取助手
- ✅ DELETE /assistants/:assistant_id - 删除助手
- ✅ POST /threads - 创建线程
- ✅ POST /threads/:thread_id/messages - 创建消息
- ✅ POST /threads/:thread_id/runs - 创建运行
- ✅ GET /threads/:thread_id/runs/:run_id - 获取运行状态
- ✅ POST /videos/generations - 创建视频
- ✅ GET /videos/:video_id - 获取视频状态
- ✅ GET /videos - 列出视频
- ✅ POST /completions - 文本补全

### Anthropic API (http://localhost:8080/v1)
- ✅ POST /messages - Claude对话
- ✅ POST /messages/batches - 创建批量消息
- ✅ GET /messages/batches/:batch_id - 获取批量任务状态
- ✅ POST /messages/batches/:batch_id/cancel - 取消批量任务
- ✅ GET /messages/batches - 列出批量任务
- ✅ GET /messages/batches/:batch_id/results - 获取批量结果

### Google Gemini API (http://localhost:8080/v1beta)
- ✅ POST /models/:model/generateContent - 生成内容
- ✅ POST /models/:model/streamGenerateContent - 流式生成
- ✅ POST /models/:model/embedContent - 文本嵌入
- ✅ POST /models/:model/batchEmbedContents - 批量嵌入
- ✅ POST /models/:model/countTokens - 计算Token
- ✅ POST /models/:model/batchGenerateContent - 批量生成
- ✅ POST /files - 上传文件
- ✅ GET /files/:file_name - 获取文件
- ✅ DELETE /files/:file_name - 删除文件
- ✅ GET /files - 列出文件

## 测试结果

### 已验证的功能
- ✅ OpenAI Chat Completions - 正常工作
- ✅ OpenAI Embeddings - 正常工作，返回1536维向量

### 使用SDK测试

修改SDK配置指向本地测试服务器：

```cpp
ai_sdk::ClientConfig config;
config.base_url = "http://localhost:8080/v1";
ai_sdk::OpenAIClient client("test-key", config);

auto response = client.chat("Hello");
```

## 注意事项

1. 所有返回值都是硬编码的mock数据
2. 服务器不验证API key，任何key都可以使用
3. 服务器仅用于测试SDK的请求/响应处理能力
4. 不支持WebSocket端点（Realtime API、Bidirectional Streaming）
