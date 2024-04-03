#include <not_implemented.h>
#include <map>
#include <memory>
#include "../include/client_logger.h"


std::unordered_map<std::string, std::pair<std::ostream*, size_t>> client_logger::_all_streams;

client_logger::client_logger(std::map<std::string, std::set<logger::severity>> const &data,
                             std::string const &log_format)
{
    _log_format = log_format;
    for (const auto &log_element: data)
    {
        std::ostream* stream;
        if (!_all_streams.count(log_element.first))
        {
            if (log_element.first == LOG_CONSOLE_NAME)
            {
                stream = &std::cout;
            }
            else
            {
                stream = static_cast<std::ostream*>(new std::ofstream(log_element.first));
            }
            _all_streams[log_element.first] = std::make_pair(stream, 0);
        }
        _all_streams[log_element.first].second++;
        _streams[log_element.first] = std::make_pair(stream, log_element.second);
    }
}

client_logger::client_logger(
        client_logger const &other)
{
    for (auto const &data: other._streams)
    {
        std::string const &filename = data.first;
        auto const &p_str_sev = data.second;
        _all_streams[filename].second++;
        _streams[filename] = p_str_sev;
    }
}

client_logger &client_logger::operator=(
        client_logger const &other)
{
    for (auto &_streams_elem: _streams)
    {
        std::string const &filename = _streams_elem.first;
        std::ostream* stream = _streams_elem.second.first;
        if (--_all_streams[filename].second == 0)
        {
            stream->flush();
            if (filename != LOG_CONSOLE_NAME)
            {
                dynamic_cast<std::ofstream*>(stream)->close();
            }
            _all_streams.erase(filename);
            _streams.erase(_streams_elem.first);
        }
    }

    for (auto const &data: other._streams)
    {
        std::string const &filename = data.first;
        auto const &p_str_sev = data.second;
        _all_streams[filename].second++;
        _streams[filename] = p_str_sev;
    }

    return *this;
}

client_logger::client_logger(
        client_logger &&other) noexcept: _streams(std::move(other._streams)),
                                         _log_format(std::move(other._log_format)) {}

client_logger &client_logger::operator=(
        client_logger &&other) noexcept
{
    for (auto &_streams_elem: _streams)
    {
        std::string const &filename = _streams_elem.first;
        std::ostream* stream = _streams_elem.second.first;
        if (--_all_streams[filename].second == 0)
        {
            stream->flush();
            if (filename != LOG_CONSOLE_NAME)
            {
                dynamic_cast<std::ofstream*>(stream)->close();
            }
            _all_streams.erase(filename);
        }
        _streams.erase(_streams_elem.first);
    }

    _streams = std::move(other._streams);
    _log_format = std::move(other._log_format);
}

client_logger::~client_logger() noexcept
{
    for (auto &_streams_elem: _streams)
    {
        std::string const &filename = _streams_elem.first;
        std::ostream* stream = _streams_elem.second.first;
        if (--_all_streams[filename].second == 0)
        {
            stream->flush();
            if (filename != LOG_CONSOLE_NAME)
            {
                dynamic_cast<std::ofstream*>(stream)->close();
            }
            _all_streams.erase(filename);
        }
    }
}

std::string client_logger::parse_string(const std::string &logger_msg, logger::severity severity) const
{
    std::string string_to_return;
    size_t log_structure_size = _log_format.size();
    for (int i = 0; i < log_structure_size; ++i)
    {
        if (_log_format[i] == '%' && (i != (log_structure_size - 1)))
        {
            switch (_log_format[i + 1])
            {
                case 'd':
                    string_to_return += current_date_to_string();
                    break;
                case 't':
                    string_to_return += current_time_to_string();
                    break;
                case 's':
                    string_to_return += severity_to_string(severity);
                    break;
                case 'm':
                    string_to_return += logger_msg;
                    break;
                default:
                    string_to_return += '%';
                    string_to_return += _log_format[i + 1];
                    break;
            }
            i++;
        }
        else
        {
            string_to_return += _log_format[i];
        }
    }
    return string_to_return;
}

logger const* client_logger::log(
        const std::string &text,
        logger::severity severity) const noexcept
{

    std::string out_string = parse_string(text, severity);
    for (auto &curr_stream: _streams)
    {
        if (curr_stream.second.second.count(severity) != 0)
        {
            (*curr_stream.second.first << out_string << std::endl).flush();
        }
    }
    return this;
}