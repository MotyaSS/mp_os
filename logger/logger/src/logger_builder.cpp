#include "../include/logger_builder.h"

logger::severity logger_builder::string_to_severity(
    std::string const &severity_string)
{
    if (severity_string == "trace")
    {
        return logger::severity::trace;
    }
    if (severity_string == "debug")
    {
        return logger::severity::debug;
    }
    if (severity_string == "information")
    {
        return logger::severity::information;
    }
    if (severity_string == "warning")
    {
        return logger::severity::warning;
    }
    if (severity_string == "error")
    {
        return logger::severity::error;
    }
    if (severity_string == "critical")
    {
        return logger::severity::critical;
    }

    throw std::out_of_range("invalid severity string value");
}

logger::severity logger_builder::char_to_severity(char c)
{
    switch (c)
    {
        case 'd':
            return logger::severity::debug;
        case 't':
            return logger::severity::trace;
        case 'i':
            return logger::severity::information;
        case 'w':
            return logger::severity::warning;
        case 'c':
            return logger::severity::critical;
        case 'e':
            return logger::severity::error;
        default:
            return logger::severity::information;
    }
}