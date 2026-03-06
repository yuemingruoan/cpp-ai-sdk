#include "google_client.hpp"
#include <iostream>
#include <cstdlib>

int main() {
    const char* access_token = std::getenv("GOOGLE_ACCESS_TOKEN");

    if (!access_token) {
        std::cerr << "Please set GOOGLE_ACCESS_TOKEN environment variable" << std::endl;
        return 1;
    }

    ai_sdk::OAuthConfig oauth_config;
    oauth_config.access_token = access_token;

    ai_sdk::GoogleClient client(oauth_config);

    std::cout << "Google Gemini Direct Access Token Example" << std::endl;
    std::cout << "==========================================" << std::endl;

    if (auto response = client.chat("Hello! Tell me a short joke.")) {
        std::cout << "Response: " << response.value() << std::endl;
    } else {
        std::cerr << "Error: " << response.error().message << std::endl;
        return 1;
    }

    return 0;
}
