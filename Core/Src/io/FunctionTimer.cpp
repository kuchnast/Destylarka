#include <io/FunctionTimer.hpp>

namespace io {

    FunctionInfo::FunctionInfo(const std::string& name, std::function<void()> func, uint32_t target_time_ms,
                               uint32_t time_ms, bool is_repeatable):
    name(name), func(func), target_time_ms(target_time_ms), time_ms(time_ms), is_repeatable(is_repeatable)
    {
        Logger("FunctionInfo").info() << "Created new " << (is_repeatable ? "repeatable every " + std::to_string(time_ms) : "")
            << " function " << name << " witch target time" << std::to_string(target_time_ms);
    }

    uint32_t FunctionTimer::next_target_time_ = 0;
    uint32_t FunctionTimer::last_function_id_ = 0;
    std::map<uint32_t, FunctionInfo> FunctionTimer::functions_{};
    Logger logger("FunctionTimer");

    uint32_t FunctionTimer::addFunction(std::function<void()> func, const std::vector<uint32_t>& times_ms,
                                        const std::string& name, bool is_repeatable)
    {
        uint32_t current_time = HAL_GetTick();
        for(const auto& time_ms: times_ms)
        {
            uint32_t target_time = current_time + time_ms;
            logger.info() << "Added function id: " << std::to_string(last_function_id_);
            functions_.try_emplace(last_function_id_++, name, func, target_time, time_ms, is_repeatable);

            // Sprawdź, czy nowo dodana funkcja ma najwcześniejszy target_time
            if (target_time < next_target_time_ || next_target_time_ == 0)
            {
                next_target_time_ = target_time;
                logger.info() << "Next target time updated to " << std::to_string(next_target_time_);
            }
        }

        return last_function_id_ - 1;
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
            if (func_info.second.target_time_ms > current_time &&
                (func_info.second.target_time_ms < next_target_time_ || next_target_time_ == 0))
            {
                next_target_time_ = func_info.second.target_time_ms;
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
                if (current_time >= it->second.target_time_ms)
                {
                    logger.info() << "Function " << it->second.name << " with id " << std::to_string(it->first)
                        << " timed out. Executing...";
                    it->second.func();

                    if(it->second.is_repeatable)
                    {
                        logger.info() << "Function is repeatable, starting again timer.";
                        it->second.target_time_ms = HAL_GetTick() + it->second.time_ms;
                    }
                    else
                    {
                        it = functions_.erase(it);
                    }
                }
                else {
                    ++it;
                }
            }

            // Aktualizuj najbliższy target_time
            next_target_time_ = 0;
            for (const auto &func_info: functions_)
            {
                if (func_info.second.target_time_ms > current_time &&
                    (func_info.second.target_time_ms < next_target_time_ || next_target_time_ == 0))
                {
                    next_target_time_ = func_info.second.target_time_ms;
                }
            }
            logger.info() << "Next target time updated to " << std::to_string(next_target_time_);
        }
    }
}
