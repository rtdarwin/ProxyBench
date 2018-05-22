#pragma once

class ConfReader
{
public:
    ConfReader() = default;
    // Rule of zero

    void readConf(int argc, const char* const argv[]);
};
