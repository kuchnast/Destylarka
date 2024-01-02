#ifndef IO_LOGSTREAM_HPP
#define IO_LOGSTREAM_HPP

#include <iostream>
#include <sstream>
#include <string>

namespace io
{
    enum class Level
    {
        INF,
        DBG,
        ERR,
        WRN
    };

    class LogStream
    {
    public:
        LogStream(Level level, const std::string &prefix);
        ~LogStream();

        template<typename T>
        LogStream &operator<<(const T &value)
        {
            ss_ << value;
            return *this;
        }

    private:
        std::string LevelToString(Level level);

        std::stringstream ss_;
        Level level_;
        std::string prefix_;
    };
}

#endif // IO_LOGSTREAM_HPP
