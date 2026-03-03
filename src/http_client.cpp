#include "ai_sdk/http_client.hpp"
#include "ai_sdk/types.hpp"
#include <curl/curl.h>
#include <sstream>

namespace ai_sdk {

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

class HttpClient::Impl {
public:
    Impl() { curl_global_init(CURL_GLOBAL_DEFAULT); }
    ~Impl() { curl_global_cleanup(); }
};

HttpClient::HttpClient() : impl_(new Impl()) {}
HttpClient::~HttpClient() { delete impl_; }

std::string HttpClient::post(const std::string& url,
                              const std::string& body,
                              const std::map<std::string, std::string>& headers) {
    CURL* curl = curl_easy_init();
    if (!curl) throw NetworkException("Failed to initialize CURL");

    std::string response;
    struct curl_slist* header_list = nullptr;

    for (const auto& [key, value] : headers) {
        header_list = curl_slist_append(header_list, (key + ": " + value).c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw NetworkException(std::string("CURL error: ") + curl_easy_strerror(res));
    }

    if (http_code >= 400) {
        throw APIException("HTTP error: " + response, http_code);
    }

    return response;
}

std::future<std::string> HttpClient::postAsync(const std::string& url,
                                                const std::string& body,
                                                const std::map<std::string, std::string>& headers) {
    return std::async(std::launch::async, [this, url, body, headers]() {
        return post(url, body, headers);
    });
}

void HttpClient::postStream(const std::string& url,
                             const std::string& body,
                             const std::map<std::string, std::string>& headers,
                             StreamCallback callback) {
    CURL* curl = curl_easy_init();
    if (!curl) throw NetworkException("Failed to initialize CURL");

    struct curl_slist* header_list = nullptr;
    for (const auto& [key, value] : headers) {
        header_list = curl_slist_append(header_list, (key + ": " + value).c_str());
    }

    auto stream_callback = [](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
        auto* cb = static_cast<StreamCallback*>(userp);
        std::string chunk((char*)contents, size * nmemb);
        (*cb)(chunk);
        return size * nmemb;
    };

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, stream_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &callback);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw NetworkException(std::string("CURL error: ") + curl_easy_strerror(res));
    }
}

std::string HttpClient::postMultipart(const std::string& url,
                                       const std::map<std::string, std::string>& fields,
                                       const std::map<std::string, std::string>& files,
                                       const std::map<std::string, std::string>& headers) {
    CURL* curl = curl_easy_init();
    if (!curl) throw NetworkException("Failed to initialize CURL");

    std::string response;
    curl_mime* mime = curl_mime_init(curl);

    for (const auto& [key, value] : fields) {
        curl_mimepart* part = curl_mime_addpart(mime);
        curl_mime_name(part, key.c_str());
        curl_mime_data(part, value.c_str(), CURL_ZERO_TERMINATED);
    }

    for (const auto& [key, filepath] : files) {
        curl_mimepart* part = curl_mime_addpart(mime);
        curl_mime_name(part, key.c_str());
        curl_mime_filedata(part, filepath.c_str());
    }

    struct curl_slist* header_list = nullptr;
    for (const auto& [key, value] : headers) {
        header_list = curl_slist_append(header_list, (key + ": " + value).c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(header_list);
    curl_mime_free(mime);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw NetworkException(std::string("CURL error: ") + curl_easy_strerror(res));
    }

    if (http_code >= 400) {
        throw APIException("HTTP error: " + response, http_code);
    }

    return response;
}

static size_t WriteBinaryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    auto* buffer = static_cast<std::vector<uint8_t>*>(userp);
    size_t total_size = size * nmemb;
    buffer->insert(buffer->end(), (uint8_t*)contents, (uint8_t*)contents + total_size);
    return total_size;
}

std::vector<uint8_t> HttpClient::postBinary(const std::string& url,
                                             const std::string& body,
                                             const std::map<std::string, std::string>& headers) {
    CURL* curl = curl_easy_init();
    if (!curl) throw NetworkException("Failed to initialize CURL");

    std::vector<uint8_t> response;
    struct curl_slist* header_list = nullptr;

    for (const auto& [key, value] : headers) {
        header_list = curl_slist_append(header_list, (key + ": " + value).c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteBinaryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw NetworkException(std::string("CURL error: ") + curl_easy_strerror(res));
    }

    if (http_code >= 400) {
        throw APIException("HTTP error", http_code);
    }

    return response;
}

} // namespace ai_sdk
