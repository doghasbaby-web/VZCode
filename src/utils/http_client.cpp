#include "http_client.hpp"
#include <sstream>

#ifdef __EMSCRIPTEN__
#include <emscripten/fetch.h>
#include <emscripten/emscripten.h>
#endif

namespace vzcode {
namespace utils {

HttpClient::HttpClient() {}

HttpClient::~HttpClient() {}

std::string HttpClient::build_headers_string(const std::map<std::string, std::string>& headers) {
    std::stringstream ss;
    for (const auto& pair : headers) {
        ss << pair.first << ": " << pair.second << "\r\n";
    }
    return ss.str();
}

#ifdef __EMSCRIPTEN__

// WASM implementation using Emscripten fetch API
HttpClient::Response HttpClient::post(const std::string& url,
                                      const std::string& body,
                                      const std::map<std::string, std::string>& headers) {
    Response response;

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;

    // Set request body
    attr.requestData = body.c_str();
    attr.requestDataSize = body.length();

    // Build headers array
    std::vector<const char*> header_array;
    std::vector<std::string> header_strings;

    for (const auto& pair : headers) {
        header_strings.push_back(pair.first);
        header_strings.push_back(pair.second);
    }

    for (const auto& str : header_strings) {
        header_array.push_back(str.c_str());
    }
    header_array.push_back(nullptr);

    attr.requestHeaders = header_array.data();

    // Perform fetch
    emscripten_fetch_t* fetch = emscripten_fetch(&attr, url.c_str());

    if (fetch->status >= 200 && fetch->status < 300) {
        response.success = true;
        response.status_code = fetch->status;
        response.body = std::string(fetch->data, fetch->numBytes);
    } else {
        response.success = false;
        response.status_code = fetch->status;
        response.error_message = "HTTP request failed with status: " + std::to_string(fetch->status);
    }

    emscripten_fetch_close(fetch);
    return response;
}

HttpClient::Response HttpClient::get(const std::string& url,
                                     const std::map<std::string, std::string>& headers) {
    Response response;

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;

    // Build headers
    std::vector<const char*> header_array;
    std::vector<std::string> header_strings;

    for (const auto& pair : headers) {
        header_strings.push_back(pair.first);
        header_strings.push_back(pair.second);
    }

    for (const auto& str : header_strings) {
        header_array.push_back(str.c_str());
    }
    header_array.push_back(nullptr);

    attr.requestHeaders = header_array.data();

    emscripten_fetch_t* fetch = emscripten_fetch(&attr, url.c_str());

    if (fetch->status >= 200 && fetch->status < 300) {
        response.success = true;
        response.status_code = fetch->status;
        response.body = std::string(fetch->data, fetch->numBytes);
    } else {
        response.success = false;
        response.status_code = fetch->status;
        response.error_message = "HTTP request failed";
    }

    emscripten_fetch_close(fetch);
    return response;
}

#else

// Native implementation (placeholder for native builds)
HttpClient::Response HttpClient::post(const std::string& url,
                                      const std::string& body,
                                      const std::map<std::string, std::string>& headers) {
    Response response;
    response.success = false;
    response.error_message = "HTTP client not implemented for native builds";
    return response;
}

HttpClient::Response HttpClient::get(const std::string& url,
                                     const std::map<std::string, std::string>& headers) {
    Response response;
    response.success = false;
    response.error_message = "HTTP client not implemented for native builds";
    return response;
}

#endif

void HttpClient::get_async(const std::string& url,
                          ResponseCallback callback,
                          const std::map<std::string, std::string>& headers) {
    // For now, just call synchronous version
    // In production, this would use async fetch
    Response response = get(url, headers);
    callback(response);
}

void HttpClient::post_async(const std::string& url,
                           const std::string& body,
                           ResponseCallback callback,
                           const std::map<std::string, std::string>& headers) {
    Response response = post(url, body, headers);
    callback(response);
}

} // namespace utils
} // namespace vzcode
