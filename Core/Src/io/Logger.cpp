#include <io/Logger.hpp>

#include <string>

namespace io
{
    Logger::Logger(const std::string &prefix) : m_prefix(prefix) {}

    LogStream Logger::info()
    {
        return LogStream(Level::INF, m_prefix);
    }

    LogStream Logger::debug()
    {
        return LogStream(Level::DBG, m_prefix);
    }

    LogStream Logger::error()
    {
        return LogStream(Level::ERR, m_prefix);
    }

    LogStream Logger::warning()
    {
        return LogStream(Level::WRN, m_prefix);
    }
}
