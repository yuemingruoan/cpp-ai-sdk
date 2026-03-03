# IXWebSocket集成说明

## 获取IXWebSocket源码

1. 从GitHub下载IXWebSocket源码：
```bash
cd code/third_party/ixwebsocket
git clone https://github.com/machinezone/IXWebSocket.git .
```

2. 或者下载特定版本：
```bash
wget https://github.com/machinezone/IXWebSocket/archive/refs/tags/v11.4.5.tar.gz
tar -xzf v11.4.5.tar.gz
mv IXWebSocket-11.4.5/* .
```

## 项目结构

```
code/third_party/ixwebsocket/
├── ixwebsocket/
│   ├── IXWebSocket.h
│   ├── IXWebSocket.cpp
│   └── ... (其他源文件)
```

## 编译配置

CMakeLists.txt已配置为自动包含IXWebSocket源码。

## 依赖说明

IXWebSocket需要：
- OpenSSL (用于WSS支持)
- zlib (用于压缩)

macOS安装：
```bash
brew install openssl zlib
```

## 注意事项

如果不需要Realtime API，可以跳过此步骤。SDK的其他功能不依赖WebSocket。
