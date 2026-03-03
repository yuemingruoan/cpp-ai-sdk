#include "google_client.hpp"
#include <iostream>
#include <cstdlib>

int main() {
    const char* client_id = std::getenv("GOOGLE_CLIENT_ID");
    const char* client_secret = std::getenv("GOOGLE_CLIENT_SECRET");
    const char* refresh_token = std::getenv("GOOGLE_REFRESH_TOKEN");

    if (!client_id || !client_secret || !refresh_token) {
        std::cerr << "Please set GOOGLE_CLIENT_ID, GOOGLE_CLIENT_SECRET, and GOOGLE_REFRESH_TOKEN" << std::endl;
        return 1;
    }

    try {
        ai_sdk::OAuthConfig oauth_config;
        oauth_config.client_id = client_id;
        oauth_config.client_secret = client_secret;
        oauth_config.refresh_token = refresh_token;

        ai_sdk::GoogleClient client(oauth_config);

        std::cout << "Google Gemini OAuth Example" << std::endl;
        std::cout << "===========================" << std::endl;

        std::string response = client.chat("Hello! Tell me a short joke.");
        std::cout << "Response: " << response << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
