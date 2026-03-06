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

    Result<std::string> post(const std::string& url,
                             const std::string& body,
                             const std::map<std::string, std::string>& headers);

    std::future<Result<std::string>> postAsync(const std::string& url,
                                               const std::string& body,
                                               const std::map<std::string, std::string>& headers);

    Result<void> postStream(const std::string& url,
                            const std::string& body,
                            const std::map<std::string, std::string>& headers,
                            StreamCallback callback);

    Result<std::string> postMultipart(const std::string& url,
                                      const std::map<std::string, std::string>& fields,
                                      const std::map<std::string, std::string>& files,
                                      const std::map<std::string, std::string>& headers);

    Result<std::vector<uint8_t>> postBinary(const std::string& url,
                                            const std::string& body,
                                            const std::map<std::string, std::string>& headers);

    Result<std::string> get(const std::string& url,
                            const std::map<std::string, std::string>& headers);

    Result<void> deleteRequest(const std::string& url,
                               const std::map<std::string, std::string>& headers);

private:
    class Impl;
    Impl* impl_;
};

} // namespace ai_sdk
