
#ifndef IO_LOGGER_HPP
#define IO_LOGGER_HPP

#include <io/LogStream.hpp>

#include <iostream>
#include <sstream>
#include <string>

namespace io
{
    class Logger
    {
    public:
        explicit Logger(const std::string &prefix);

        LogStream info();

        LogStream debug();

        LogStream error();

        LogStream warning();
    private:
        std::string m_prefix;
    };
}

#endif // IO_LOGGER_HPP
