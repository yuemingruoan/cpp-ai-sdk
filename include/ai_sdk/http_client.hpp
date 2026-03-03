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

private:
    class Impl;
    Impl* impl_;
};

} // namespace ai_sdk
