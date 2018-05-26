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

    std::cout << "** SSR_Proxy started" << std::endl; // DEBUG
}

void
SSR_Proxy::populate_conf_file(const ProxyProfile& profile, int fd)
{
    std::string profile_str(profile_text_templ);
    std::string::size_type pos;

    // Hope the flowing `replace' operations will not that slow

    pos = profile_str.find("%SERVER%");
    profile_str.replace(pos, strlen("%SERVER%"),
                        boost::any_cast<std::string>(profile.at("server")));

    pos = profile_str.find("%SERVER_PORT%");
    profile_str.replace(
        pos, strlen("%SERVER_PORT%"),
        boost::any_cast<std::string>(profile.at("server_port")));

    pos = profile_str.find("%LOCAL_PORT%");
    profile_str.replace(pos, strlen("%LOCAL_PORT%"),
                        boost::any_cast<std::string>(profile.at("local_port")));

    pos = profile_str.find("%PASSWORD%");
    profile_str.replace(pos, strlen("%PASSWORD%"),
                        boost::any_cast<std::string>(profile.at("password")));

    pos = profile_str.find("%METHOD%");
    profile_str.replace(pos, strlen("%METHOD%"),
                        boost::any_cast<std::string>(profile.at("method")));

    pos = profile_str.find("%PROTOCOL%");
    profile_str.replace(pos, strlen("%PROTOCOL%"),
                        boost::any_cast<std::string>(profile.at("protocol")));

    pos = profile_str.find("%PROTOCOL_PARAM%");
    profile_str.replace(
        pos, strlen("%PROTOCOL_PARAM%"),
        boost::any_cast<std::string>(profile.at("protocol_param")));

    pos = profile_str.find("%OBFS%");
    profile_str.replace(pos, strlen("%OBFS%"),
                        boost::any_cast<std::string>(profile.at("obfs")));

    pos = profile_str.find("%OBFS_PARAM%");
    profile_str.replace(pos, strlen("%OBFS_PARAM%"),
                        boost::any_cast<std::string>(profile.at("obfs_param")));

    write(fd, profile_str.c_str(), profile_str.length());
}
