#pragma once

#include <string>
#include <map>
#include <future>
#include "types.hpp"

namespace ai_sdk {

class HttpClient {
public:
    HttpClient();
    ~HttpClient();

    std::string post(const std::string& url,
                     const std::string& body,
                     const std::map<std::string, std::string>& headers);

    std::future<std::string> postAsync(const std::string& url,
                                       const std::string& body,
                                       const std::map<std::string, std::string>& headers);

    void postStream(const std::string& url,
                    const std::string& body,
                    const std::map<std::string, std::string>& headers,
                    StreamCallback callback);

    std::string postMultipart(const std::string& url,
                              const std::map<std::string, std::string>& fields,
                              const std::map<std::string, std::string>& files,
                              const std::map<std::string, std::string>& headers);

    std::vector<uint8_t> postBinary(const std::string& url,
                                     const std::string& body,
                                     const std::map<std::string, std::string>& headers);

private:
    class Impl;
    Impl* impl_;
};

} // namespace ai_sdk
