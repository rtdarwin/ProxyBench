#include "HttpClient.hh"

#include <boost/any.hpp>
#include <curl/curl.h>
#include <exception>
#include <sstream>
#include <stdexcept>

namespace {

class CurlGlobalHandle
{
public:
    CurlGlobalHandle() { curl_global_init(CURL_GLOBAL_DEFAULT); }
    ~CurlGlobalHandle() { curl_global_cleanup(); }
};

// dynamic init at startup
// - might be broken in some embed platforms
CurlGlobalHandle gcurl;

size_t
write_to_stringstream(void* contents, size_t size, size_t nmemb, void* handle)
{
    std::stringstream* stream = (std::stringstream*)handle;
    size_t real_size = size * nmemb;

    stream->write((char*)contents, real_size);

    return real_size;
}

size_t
write_to_empty(void* contents, size_t size, size_t nmemb, void* handle)
{
    return size * nmemb;
}
}
// namespace {

struct HttpClient::Impl
{
    CURL* curlhandle = nullptr;
    bool just_ping = false;
};

HttpClient::HttpClient()
{
    _pImpl.reset(new HttpClient::Impl);

    _pImpl->curlhandle = curl_easy_init();
}

HttpClient::~HttpClient()
{
    curl_easy_cleanup(_pImpl->curlhandle);
}

HttpClient&
HttpClient::socks5_proxy(const std::string& proxy_addr)
{
    CURL* curlhandle = _pImpl->curlhandle;

    curl_easy_setopt(curlhandle, CURLOPT_PROXY, proxy_addr.c_str());

    return *this;
}

HttpClient&
HttpClient::dns_resolver(const std::string& resolver_addr)
{
    CURL* curlhandle = _pImpl->curlhandle;

    curl_easy_setopt(curlhandle, CURLOPT_DNS_SERVERS, resolver_addr.c_str());

    return *this;
}

HttpClient&
HttpClient::timeout(int seconds)
{
    CURL* curlhandle = _pImpl->curlhandle;

    curl_easy_setopt(curlhandle, CURLOPT_TIMEOUT, seconds);

    return *this;
}

HttpClient&
HttpClient::just_ping(bool ping)
{
    _pImpl->just_ping = ping;
    return *this;
}

void
HttpClient::wait_get(const std::string& url, std::string* resultstr)
{
    CURLcode result;
    std::stringstream strstream;

    CURL* curlhandle = _pImpl->curlhandle;

    curl_easy_setopt(curlhandle, CURLOPT_URL, url.c_str());
    // curl_easy_setopt(curlhandle, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    // curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1L); // DEBUG

    if (_pImpl->just_ping) {
        curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, &write_to_empty);

        result = curl_easy_perform(curlhandle);
        if (result != CURLE_OK)
            throw std::runtime_error("Timeout");

    } else {
        curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION,
                         &write_to_stringstream);
        curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, &strstream);

        result = curl_easy_perform(curlhandle);

        if (result != CURLE_OK)
            throw std::runtime_error("Timeout when retrieving " + url);

        resultstr->assign(strstream.str());
    }
}
