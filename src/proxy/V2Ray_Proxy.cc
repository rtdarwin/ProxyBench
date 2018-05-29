#include "proxy/V2Ray_Proxy.hh"

#include <boost/any.hpp>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>

#include "Conf.hh"

namespace {

// kcp h2 not implemented
const char* profile_text_templ = R"raw(
{
  "inbound": {
    "port": %LOCAL_PORT%,
    "protocol": "socks",
    // "domainOverride": ["tls","http"],
    "settings": {
      "auth": "noauth"
    }
  },
  "outbound": {
    "tag": "%OUTBOUND.TAG%",
    "protocol": "vmess",
    "settings": {
      "vnext": [
        {
          "address": "%OUTBOUND.ADDRESS%",
          "port": %OUTBOUND.PORT%,
          "users": [
            {
              "id": "%OUTBOUND.USER.ID%",
              "alterId": %OUTBOUND.USER.ALTERID%
            }
          ]
        }
      ]
    },
    "streamSettings":{
      "network": "%OUTBOUND.STREAM.NETWORK%",
      "security": "%OUTBOUND.STREAM.SECURITY%",
      "tcpSettings": {
        "header": {
          "type": "%OUTBOUND.STREAM.TCP.HEADER.TYPE%"
        }
      },
      "wsSettings": {
        "path": "%OUTBOUND.STREAM.WS.PATH%",
        "headers": {
          "Host": "%OUTBOUND.STREAM.WS.HOST%"
        }
      }
    }
  }
}
)raw";

} // namespace {

V2Ray_Proxy::V2Ray_Proxy(const ProxyProfile& profile)
    : _profile(profile)
{
}

V2Ray_Proxy::~V2Ray_Proxy()
{
    unlink(_profile_tmpfile);
    close(_profile_tmpfile_fd);
}

void
V2Ray_Proxy::wait_start()
{
    // config tmpfile
    generate_conf_tmpfile();

    _proxy_pid = fork();

    if (_proxy_pid > 0) { // parent

        // Full buffering of stdio.stdout results in long time waitting...
        // Just wait for 100ms, bless me...
        ::usleep(100 * 1000);     // 100 ms
    } else if (_proxy_pid == 0) { // child
        int nullfd = open("/dev/null", O_RDWR);
        dup2(nullfd, STDOUT_FILENO);
        dup2(nullfd, STDERR_FILENO);
        dup2(nullfd, STDIN_FILENO);

        std::string prog = "v2ray";
        if (!Conf::get()->exec.empty())
            prog = Conf::get()->exec;

        // exec never return
        execlp(prog.c_str(), prog.c_str(), "-config", _profile_tmpfile,
               nullptr);
    }

    if (Conf::get()->verbose) {
        std::cout << "** V2Ray_Proxy  "
                  << boost::any_cast<std::string>(_profile.at("name"))
                  << " started" << std::endl;
    }
}

void
V2Ray_Proxy::dump_config_file(const std::string& dir)
{
    // TODO
}

void
V2Ray_Proxy::generate_conf_tmpfile()
{
    _profile_tmpfile_fd = mkstemp(_profile_tmpfile);
    populate_conf_file(_profile, _profile_tmpfile_fd);
}

void
V2Ray_Proxy::populate_conf_file(const ProxyProfile& profile, int fd)
{
    std::string profile_str(profile_text_templ);
    std::string::size_type pos;

// Hope the flowing `replace' operations will not that slow

#ifdef REPLACE
#undef REPLACE
#endif
#define REPLACEY(a, b)                                                         \
    do {                                                                       \
        pos = profile_str.find(a);                                             \
        profile_str.replace(pos, strlen(a),                                    \
                            boost::any_cast<std::string>(profile.at(b)));      \
    } while (0)

    REPLACEY("%LOCAL_PORT%", "local_port");
    REPLACEY("%OUTBOUND.TAG%", "name");
    REPLACEY("%OUTBOUND.ADDRESS%", "outbound.address");
    REPLACEY("%OUTBOUND.PORT%", "outbound.port");
    REPLACEY("%OUTBOUND.USER.ID%", "outbound.user.id");
    REPLACEY("%OUTBOUND.USER.ALTERID%", "outbound.user.alterId");
    REPLACEY("%OUTBOUND.STREAM.NETWORK%", "outbound.stream.network");
    REPLACEY("%OUTBOUND.STREAM.SECURITY%", "outbound.stream.security");

    REPLACEY("%OUTBOUND.STREAM.TCP.HEADER.TYPE%", "outbound.stream.type");
    REPLACEY("%OUTBOUND.STREAM.WS.HOST%", "outbound.stream.host");

    if (profile.count("version") &&
        boost::any_cast<std::string>(profile.at("version")) == "2") {
        REPLACEY("%OUTBOUND.STREAM.WS.PATH%", "outbound.stream.path");
    } else {
        profile_str.erase(profile_str.find("%OUTBOUND.STREAM.WS.PATH%"),
                          strlen("%OUTBOUND.STREAM.WS.PATH%"));
    }

    write(fd, profile_str.c_str(), profile_str.length());
}
