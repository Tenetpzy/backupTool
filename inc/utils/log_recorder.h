#pragma once

#include <string>

class log_recorder;

class log_helper
{
public:
    static void print_log_to_cerr(const std::string &log);

private:
    static log_recorder* get_instance_cerr();
    static log_recorder* instance_cerr;
};
