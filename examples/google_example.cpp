#include "google_client.hpp"
#include <iostream>
#include <cstdlib>

int main() {
    const char* api_key = std::getenv("GOOGLE_API_KEY");
    if (!api_key) {
        std::cerr << "Please set GOOGLE_API_KEY environment variable" << std::endl;
        return 1;
    }

    try {
        ai_sdk::GoogleClient client(api_key);

        std::cout << "Google Gemini Chat Example" << std::endl;
        std::cout << "=========================" << std::endl;

        std::string response = client.chat("Hello! Tell me a short joke.");
        std::cout << "Response: " << response << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
