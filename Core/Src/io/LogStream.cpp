#include <io/LogStream.hpp>
#include "usart.h"

namespace io
{

    LogStream::LogStream(Level level, const std::string &prefix) : level_(level), prefix_(prefix) {}

    LogStream::~LogStream()
    {
    	std::stringstream ss;
    	ss << LevelToString(level_) << " [" << prefix_ << "]: " << ss_.str() << "\n";
    	std::string buf = ss.str();
    	HAL_UART_Transmit(&huart2, (uint8_t *)buf.data(), buf.size(), 100);
    }


    std::string LogStream::LevelToString(Level level)
    {
        switch (level)
        {
            case Level::INF:
                return "INF";
            case Level::DBG:
                return "DBG";
            case Level::ERR:
                return "ERR";
            case Level::WRN:
                return "WRN";
            default:
                return "UNKNOWN";
        }
    }
}
