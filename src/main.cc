#include "ProxyBench.hh"

int
main(int argc, char* argv[])
{
    auto bench = ProxyBench::get();
    bench->init(argc, argv);
    bench->run();
    return 0;
}
