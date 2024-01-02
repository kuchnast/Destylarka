#include <io/LogStream.hpp>

namespace io
{

    LogStream::LogStream(Level level, const std::string &prefix) : level_(level), prefix_(prefix) {}

    LogStream::~LogStream()
    {
        std::string levelStr = LevelToString(level_);
        printf("%s", (levelStr + " [" + prefix_ + "]: " + ss_.str() + "\n").c_str());
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
