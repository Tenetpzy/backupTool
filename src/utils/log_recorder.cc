#include "utils/log_recorder.h"

#include <iostream>

class log_recorder
{
public:
    virtual void print_log(const std::string &msg) = 0;
    virtual ~log_recorder() = default;
};

class log_recorder_cerr : public log_recorder
{
public:
    void print_log(const std::string &msg) override
    {
        std::cerr << msg << std::endl;
    }

private:
    log_recorder_cerr() = default;

    friend class log_helper;
};

log_recorder* log_helper::instance_cerr = nullptr;

log_recorder* log_helper::get_instance_cerr()
{
    if (instance_cerr == nullptr)
        instance_cerr = new log_recorder_cerr;
    return instance_cerr;
}

void log_helper::print_log_to_cerr(const std::string &log)
{
    log_recorder *instance = get_instance_cerr();
    instance->print_log(log);
}