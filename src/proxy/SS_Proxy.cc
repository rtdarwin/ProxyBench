#include "proxy/SS_Proxy.hh"

#include <boost/any.hpp>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>

#include "Conf.hh"

namespace {

const char* profile_text_templ = R"raw(
{
  "server": "%SERVER%",
  "server_port": %SERVER_PORT%,
  "local_address": "127.0.0.1",
  "local_port": %LOCAL_PORT%,
  "password": "%PASSWORD%",
  "method": "%METHOD%"
  ,%SIMPLE_OBFS%
}
)raw";

} // namespace {

SS_Proxy::SS_Proxy(const ProxyProfile& profile)
    : Proxy()
    , _profile(profile)
{
}

SS_Proxy::~SS_Proxy()
{
    unlink(_profile_tmpfile);
    close(_profile_tmpfile_fd);
}

void
SS_Proxy::generate_conf_tmpfile()
{
    _profile_tmpfile_fd = mkstemp(_profile_tmpfile);
    populate_conf_file(_profile, _profile_tmpfile_fd);
}

void
SS_Proxy::wait_start()
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

        std::string prog = "ss-local";
        if (!Conf::get()->exec.empty())
            prog = Conf::get()->exec;

        // exec never return
        execlp(prog.c_str(), prog.c_str(), "-c", _profile_tmpfile, nullptr);
    }

    std::cout << "** SS_Proxy started" << std::endl; // DEBUG
}

void
SS_Proxy::dump_config_file(const std::string& dir)
{
    ::system(("mkdir -p " + dir).c_str());

// emmm, std::function may be a better choice
#ifdef DUMP_CONFIG_FILE_HELPER
#undef DUMP_CONFIG_FILE_HELPER
#else
#define DUMP_CONFIG_FILE_HELPER(port, file_postfix)                            \
    do {                                                                       \
        ProxyProfile profile_ = _profile;                                      \
        profile_["local_port"] = std::to_string(port);                         \
                                                                               \
        std::string basename =                                                 \
            boost::any_cast<std::string>(profile_.at("name"));                 \
        std::string confile_name = dir + "/" + basename + #file_postfix;       \
                                                                               \
        int open_flags = O_CREAT | O_WRONLY | O_TRUNC | O_CLOEXEC;             \
        int file_perms =                                                       \
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;         \
        int fd = ::open(confile_name.c_str(), open_flags, file_perms);         \
                                                                               \
        if (fd == -1) {                                                        \
            throw std::runtime_error(std::string("open: ") + strerror(errno)); \
        }                                                                      \
        populate_conf_file(profile_, fd);                                      \
        close(fd);                                                             \
    } while (0)
#endif

    DUMP_CONFIG_FILE_HELPER(1080, _local.json);
    DUMP_CONFIG_FILE_HELPER(1081, _redir.json);
}

void
SS_Proxy::populate_conf_file(const ProxyProfile& profile, int fd)
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

    pos = profile_str.find(",%SIMPLE_OBFS%");
    if (profile.count("plugin_option")) {
        std::string obfs_options;

        // clang-format off
        obfs_options = R"raw("plugin": "obfs-local", "plugin_opts": )raw";
        obfs_options += "\"";
        obfs_options += boost::any_cast<std::string>(profile.at("plugin_option"));
        obfs_options += "\"";
        // clang-format on

        profile_str.replace(pos, strlen("%SIMPLE_OBFS%"), obfs_options);
    } else {
        profile_str.erase(pos, strlen(",%SIMPLE_OBFS%"));
    }

    write(fd, profile_str.c_str(), profile_str.length());
}
