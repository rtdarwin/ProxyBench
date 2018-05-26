#include "subscr-parser/V2RayVmessFile_SubscrParser.hh"

#include <fstream>
#include <iostream>
#include <list>
#include <regex>
#include <sstream>
#include <string>

#include "HttpClient.hh"
#include "Proxy.hh"
#include "SyncDnsResolver.hh"
#include "base/Strings.hh"
#include "third-party/cppcodec/base64_rfc4648.hpp"
#include "third-party/nlohmann/json.hpp"

using std::string;
using nlohmann::json;
using namespace cppcodec;
using namespace proxybench;

namespace {

/* v2rayN(vmess://)
{
"v": "2",
"ps": "备注别名",
"add": "111.111.111.111",
"port": "32000",
"id": "1386f85e-657b-4d6e-9d56-78badb75e1fd",
"aid": "100",
"net": "tcp",      // tcp/kcp/ws/h2
"type": "none",    // none/http/srtp/utp/wechat-video
"host": "www.bbb.com",
"path": "/",
"tls": "tls"
}
*/

} // namespace {

void
V2RayVmessFile_SubscrParser::parse(const std::string& subscr_file,
                                   std::list<ProxyProfile>* profiles)
{
    string line_b64;
    std::ifstream nodes_stream(subscr_file);

    while (std::getline(nodes_stream, line_b64)) {
        if (line_b64.empty())
            continue;

        const json& node = json::parse(base64_rfc4648::decode<string>(
            line_b64.c_str() + 8, line_b64.length() - 8));

        ProxyProfile profile;

        profile["name"] = node.at("ps").get<string>();

        // vmess settings
        profile["outbound.address"] = node.at("add").get<string>();
        profile["outbound.port"] = node.at("port").get<string>();
        profile["outbound.user.id"] = node.at("id").get<string>();
        profile["outbound.user.alterId"] = node.at("aid").get<string>();

        // stream setttings
        profile["outbound.stream.network"] = node.at("net").get<string>();
        profile["outbound.stream.security"] = node.at("tls").get<string>();
        profile["outbound.stream.type"] =
            node.at("type").get<string>();
        profile["outbound.stream.host"] = node.at("host").get<string>();

        // version "2" only
        if (node.count("v") && node.at("v").get<string>() == "2") {
            profile["version"] = string("2");
            profile["outbound.stream.path"] = node.at("path").get<string>();
        }

        profiles->push_back(std::move(profile));
    }
}
