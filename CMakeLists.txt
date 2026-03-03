cmake_minimum_required(VERSION 3.14)
project(cpp-ai-sdk VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

# IXWebSocket配置
set(USE_TLS ON CACHE BOOL "Enable TLS support")
set(USE_ZLIB ON CACHE BOOL "Enable zlib compression")
add_subdirectory(code/third_party/ixwebsocket EXCLUDE_FROM_ALL)

add_library(ai_sdk
    src/http_client.cpp
    src/models.cpp
    src/extended_models.cpp
    src/context_manager.cpp
    src/openai_client.cpp
    src/anthropic_client.cpp
    src/websocket_client.cpp
)

target_include_directories(ai_sdk PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/code/third_party
)

target_link_libraries(ai_sdk PUBLIC
    CURL::libcurl
    ixwebsocket
)

add_executable(openai_example examples/openai_chat_example.cpp)
target_link_libraries(openai_example PRIVATE ai_sdk)

add_executable(anthropic_example examples/anthropic_example.cpp)
target_link_libraries(anthropic_example PRIVATE ai_sdk)
