#include <io/Logger.hpp>

#include <string>

namespace io
{
	std::string num_to_hex(uint8_t num)
	{
		std::string hex = "0x";
		char hex_chars[] = "0123456789ABCDEF";

		char high = hex_chars[(num >> 4) & 0x0F];
		char low = hex_chars[num & 0x0F];

		hex.push_back(high);
		hex.push_back(low);

		return hex;
	}

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
