#ifndef IO_TIMER_HPP
#define IO_TIMER_HPP

#include <tim.h>

#include <functional>
#include <map>

namespace io {

struct FunctionInfo
{
    std::function<void()> func;
    uint32_t target_time;

    FunctionInfo( std::function<void()> func, uint32_t target_time);
};

class FunctionTimer
{
public:
    static uint32_t addFunction(std::function<void()> func, uint32_t time_ms);

    static bool removeFunction(uint32_t function_id);

    static bool hasFunction(uint32_t function_id);

    static void handleFunctionsWithTimeout();
private:
    static std::map<uint32_t, FunctionInfo> functions_;
    static uint32_t next_target_time_;
    static uint32_t last_function_id_;
};
}

#endif  // IO_TIMER_HPP