#include <io/FunctionTimer.hpp>

namespace io {

    FunctionInfo::FunctionInfo(const std::string& name, std::function<void()> func, uint32_t target_time) : name(name),
        func(func), target_time(target_time)
    {
        Logger("FunctionInfo").info() << "Created new function " << name << " witch target time" << std::to_string(target_time);
    }

    uint32_t FunctionTimer::next_target_time_ = 0;
    uint32_t FunctionTimer::last_function_id_ = 0;
    std::map<uint32_t, FunctionInfo> FunctionTimer::functions_{};
    Logger logger("FunctionTimer");

    uint32_t FunctionTimer::addFunction(std::function<void()> func, uint32_t time_ms, const std::string& name)
    {

        uint32_t current_time = HAL_GetTick();
        uint32_t target_time = current_time + time_ms;
        logger.info() << "Added function id: " << std::to_string(last_function_id_);
        functions_.try_emplace(last_function_id_, name, func, target_time);

        // Sprawdź, czy nowo dodana funkcja ma najwcześniejszy target_time
        if (target_time < next_target_time_ || next_target_time_ == 0)
        {
            next_target_time_ = target_time;
            logger.info() << "Next target time updated to " << std::to_string(next_target_time_);
        }

        return last_function_id_++;
    }

    bool FunctionTimer::removeFunction(uint32_t function_id)
    {
        bool is_removed = functions_.erase(function_id);
        logger.info() << "Removed function with id " << std::to_string(function_id)
            << (is_removed ? " - successfuly" : " - faild (no id present in map)");

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
        logger.info() << "Next target time updated to " << std::to_string(next_target_time_);

        return is_removed;
    }

    bool FunctionTimer::hasFunction(uint32_t function_id)
    {
        return functions_.count(function_id);
    }

    void FunctionTimer::handleFunctionsWithTimeout()
    {
        if (!next_target_time_)
            return;

        uint32_t current_time = HAL_GetTick();

        if (current_time >= next_target_time_)
        {

            for (auto it = functions_.begin(); it != functions_.end();) {
                if (current_time >= it->second.target_time)
                {
                    logger.info() << "Function with id" << it->second.name << " timed out. Executing...";
                    it->second.func(); // Wywołaj funkcję
                    it = functions_.erase(it);// Usuń funkcję z listy
                }
                else {
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
            logger.info() << "Next target time updated to " << std::to_string(next_target_time_);
        }
    }
}
