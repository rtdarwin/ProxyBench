#include "proxy/SSR_Proxy.hh"

#include <assert.h>
#include <boost/any.hpp>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "Conf.hh"

namespace {

const char* profile_text_templ = R"raw(
{
  "server": "%SERVER%",
  "server_port": %SERVER_PORT%,
  "local_address": "127.0.0.1",
  "local_port": %LOCAL_PORT%,
  "password": "%PASSWORD%",
  "method": "%METHOD%",
  "protocol": "%PROTOCOL%",
  "protocol_param": "%PROTOCOL_PARAM%",
  "obfs": "%OBFS%",
  "obfs_param": "%OBFS_PARAM%"
}
)raw";

} // namespace {

SSR_Proxy::SSR_Proxy(const ProxyProfile& profile)
    : SS_Proxy(profile)
{
}

void
SSR_Proxy::wait_start()
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

        std::string prog = "ssr-local";
        if (!Conf::get()->exec.empty())
            prog = Conf::get()->exec;

        // exec never return
        execlp(prog.c_str(), prog.c_str(), "-c", _profile_tmpfile, nullptr);
    }

    if (Conf::get()->verbose) {
        std::cout << "** SSR_Proxy  "
                  << boost::any_cast<std::string>(_profile.at("name"))
                  << " started" << std::endl;
    }
}

void
SSR_Proxy::populate_conf_file(const ProxyProfile& profile, int fd)
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

    REPLACEY("%SERVER%", "server");
    REPLACEY("%SERVER_PORT%", "server_port");
    REPLACEY("%LOCAL_PORT%", "local_port");
    REPLACEY("%PASSWORD%", "password");
    REPLACEY("%METHOD%", "method");
    REPLACEY("%PROTOCOL%", "protocol");
    REPLACEY("%PROTOCOL_PARAM%", "protocol_param");
    REPLACEY("%OBFS%", "obfs");
    REPLACEY("%OBFS_PARAM%", "obfs_param");

    write(fd, profile_str.c_str(), profile_str.length());
}
