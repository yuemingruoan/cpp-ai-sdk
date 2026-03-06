cmake_minimum_required(VERSION 3.14)
project(cpp-ai-sdk VERSION 1.0.0 LANGUAGES CXX)

# std::expected is C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

# IXWebSocket配置
set(USE_TLS ON CACHE BOOL "Enable TLS support")
set(USE_ZLIB ON CACHE BOOL "Enable zlib compression")
add_subdirectory(code/third_party/ixwebsocket EXCLUDE_FROM_ALL)

add_library(ai_sdk
    code/cpp/http_client.cpp
    code/cpp/models.cpp
    code/cpp/extended_models.cpp
    code/cpp/context_manager.cpp
    code/cpp/openai_client.cpp
    code/cpp/anthropic_client.cpp
    code/cpp/websocket_client.cpp
    code/cpp/realtime_client.cpp
    code/cpp/google_client.cpp
    code/cpp/oauth_manager.cpp
)

target_include_directories(ai_sdk PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/code/header
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

add_executable(google_example examples/google_example.cpp)
target_link_libraries(google_example PRIVATE ai_sdk)

add_executable(google_oauth_example examples/google_oauth_example.cpp)
target_link_libraries(google_oauth_example PRIVATE ai_sdk)

add_executable(google_access_token_example examples/google_access_token_example.cpp)
target_link_libraries(google_access_token_example PRIVATE ai_sdk)

add_executable(test_sdk test-server/test_sdk.cpp)
target_link_libraries(test_sdk PRIVATE ai_sdk)

add_executable(test_comprehensive test-server/test_comprehensive.cpp)
target_link_libraries(test_comprehensive PRIVATE ai_sdk)
