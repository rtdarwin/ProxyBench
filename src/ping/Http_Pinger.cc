#include "ping/Http_Pinger.hh"

#include <curl/curl.h>
#include <exception>
#include <iostream>

#include "Conf.hh"
#include "HttpClient.hh"
#include "base/Times.hh"

using namespace proxybench;

#define PING_URL "https://www.google.com/generate_204"

bool
Http_Pinger::require_proxy()
{
    return true;
}

void
Http_Pinger::wait_ping(PingResult* result)
{
    HttpClient httpclient;
    httpclient.socks5_proxy(_socks5_proxy).timeout(2).just_ping(true);

    int delay_accum = 0;
    int nping = Conf::get()->nping;
    int ntimeout = 0;

    for (int i = 0; i < nping; i++) {
        try {
            int64_t begin = Times::current_millis();
            httpclient.wait_get(PING_URL, NULL);
            int64_t end = Times::current_millis();

            delay_accum += end - begin;
        } catch (const std::exception& e) {
            ntimeout += 1;
        }
    }

    int delay = (nping - ntimeout) == 0 ? -1 : delay_accum / (nping - ntimeout);

    (*result)["http_delay"] = delay;
    (*result)["http_nping"] = nping;
    (*result)["http_ntimeout"] = ntimeout;

    std::cout << "** HTTP Ping finished" << std::endl; // DEBUG
}
