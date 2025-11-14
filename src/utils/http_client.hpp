#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include <string>
#include <map>
#include <functional>

namespace vzcode {
namespace utils {

/**
 * Simple HTTP client for making API requests
 * Uses Emscripten's fetch API when compiled to WASM
 */
class HttpClient {
public:
    struct Response {
        int status_code;
        std::string body;
        std::map<std::string, std::string> headers;
        bool success;
        std::string error_message;
    };

    using ResponseCallback = std::function<void(const Response&)>;

    HttpClient();
    ~HttpClient();

    /**
     * Make a synchronous GET request
     */
    Response get(const std::string& url,
                 const std::map<std::string, std::string>& headers = {});

    /**
     * Make a synchronous POST request
     */
    Response post(const std::string& url,
                  const std::string& body,
                  const std::map<std::string, std::string>& headers = {});

    /**
     * Make an asynchronous GET request
     */
    void get_async(const std::string& url,
                   ResponseCallback callback,
                   const std::map<std::string, std::string>& headers = {});

    /**
     * Make an asynchronous POST request
     */
    void post_async(const std::string& url,
                    const std::string& body,
                    ResponseCallback callback,
                    const std::map<std::string, std::string>& headers = {});

private:
    static std::string build_headers_string(const std::map<std::string, std::string>& headers);
};

} // namespace utils
} // namespace vzcode

#endif // HTTP_CLIENT_HPP
