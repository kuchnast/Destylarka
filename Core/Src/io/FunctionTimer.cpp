#include <io/FunctionTimer.hpp>

namespace io {

    FunctionInfo::FunctionInfo(std::function<void()> func, uint32_t target_time) : func(func), target_time(target_time)
    {
    }

    uint32_t FunctionTimer::next_target_time_ = 0;
    uint32_t FunctionTimer::last_function_id_ = 0;
    std::map<uint32_t, FunctionInfo> FunctionTimer::functions_{};

    uint32_t FunctionTimer::addFunction(std::function<void()> func, uint32_t time_ms)
    {
        uint32_t current_time = HAL_GetTick();
        uint32_t target_time = current_time + time_ms;
        functions_.try_emplace(last_function_id_, func, target_time);

        // Sprawdź, czy nowo dodana funkcja ma najwcześniejszy target_time
        if (target_time < next_target_time_ || next_target_time_ == 0)
        {
            next_target_time_ = target_time;
        }

        return last_function_id_++;
    }

    bool FunctionTimer::removeFunction(uint32_t function_id)
    {
        bool is_removed = functions_.erase(function_id);

        // Aktualizuj najbliższy target_time
        uint32_t current_time = HAL_GetTick();
        next_target_time_ = 0;
        for (const auto &func_info: functions_)
        {
            if (func_info.second.target_time > current_time &&
                (func_info.second.target_time < next_target_time_ || next_target_time_ == 0))
            {
                next_target_time_ = func_info.second.target_time;
            }
        }

        return is_removed;
    }

    bool FunctionTimer::hasFunction(uint32_t function_id)
    {
        return functions_.count(function_id);
    }

    void FunctionTimer::handleFunctionsWithTimeout()
    {
        uint32_t current_time = HAL_GetTick();

        if (current_time >= next_target_time_)
        {
            for (auto it = functions_.begin(); it != functions_.end();) {
                if (current_time >= it->second.target_time) {
                    it->second.func(); // Wywołaj funkcję
                    it = functions_.erase(it);// Usuń funkcję z listy
                } else {
                    ++it;
                }
            }

            // Aktualizuj najbliższy target_time
            next_target_time_ = 0;
            for (const auto &func_info: functions_)
            {
                if (func_info.second.target_time > current_time &&
                    (func_info.second.target_time < next_target_time_ || next_target_time_ == 0))
                {
                    next_target_time_ = func_info.second.target_time;
                }
            }
        }
    }
}