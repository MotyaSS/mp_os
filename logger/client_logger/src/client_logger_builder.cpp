#include <not_implemented.h>
#include <exception>
#include <filesystem>
#include <sstream>

#include "client_logger.h"


client_logger_builder::client_logger_builder() : _log_format(LOG_FORMAT_DEFAULT) {}

client_logger_builder::client_logger_builder(const std::string &log_format)
{
    set_log_format(log_format);
}

logger_builder* client_logger_builder::clear()
{
    _data.clear();
    _log_format = LOG_FORMAT_DEFAULT;
    return this;
}

logger_builder* client_logger_builder::set_log_format(const std::string &log_format)
{
    if (!log_format.empty())
    {
        _log_format = log_format;
    }
    return this;
}

logger_builder* client_logger_builder::add_file_stream(
        std::string const &stream_file_path,
        logger::severity severity)
{
    if (stream_file_path.empty())
    {
        throw std::invalid_argument("client_logger_builder::add_file_stream: Filename can't be empty");
    }

    std::string abs_filepath = std::filesystem::absolute(stream_file_path).string();
    _data[abs_filepath].insert(severity);

    return this;
}

logger_builder* client_logger_builder::add_console_stream(
        logger::severity severity)
{
    _data[LOG_CONSOLE_NAME].insert(severity);
    return this;
}

logger_builder* client_logger_builder::add_file_stream(
        std::string const &stream_file_path,
        std::string const &severities)
{
    if (stream_file_path.empty())
    {
        throw std::invalid_argument("Invalid arg");
    }
    std::string abs_path = std::filesystem::absolute(stream_file_path).string();
    for (char c: severities)
    {
        _data[abs_path].insert(char_to_severity(c));
    }
    return this;
}

logger_builder* client_logger_builder::add_console_stream(std::string const &severities)
{
    for (char c: severities)
    {
        _data[LOG_CONSOLE_NAME].insert(char_to_severity(c));
    }
    return this;
}

std::string unquote_string(std::string const &string)
{
    if (string[0] != '"')
    {
        throw std::invalid_argument("string can't be unquoted");
    }

    std::string result;
    for (int i = 1; i < string.size(); i++)
    {
        if (string[i] == '"')
        {
            return result;
        }
        result += string[i];
    }

    throw std::invalid_argument("string can't be unquoted");
}

std::queue<std::string> split_string(const std::string &str, char delim)
{
    std::queue<std::string> result;
    std::string current;
    bool inside_quotes = false;
    char prev;
    for (char c: str)
    {
        if (c == '\"')
        {
            current += '"';
            inside_quotes = !inside_quotes;
        }
        else if (c == delim && !inside_quotes)
        {
            result.push(current);
            current.clear();
        }
        else if (!inside_quotes && prev == '"')
        {
            throw std::invalid_argument("split_string: quotes issue");
        }
        else
        {
            current += c;
        }
        prev = c;
    }

    result.push(current);
    return result;
}

std::queue<std::string> configuration_path_to_path_q(std::string const &conf_path)
{
    try
    {
        return split_string(conf_path, '/');
    }
    catch (std::invalid_argument &)
    {
        throw std::invalid_argument("configuration_path_to_path_q: quotes issue");
    }
}

int skip_to_same_bracket_level(std::ifstream &stream, std::string &str)
{
    int level = 1;
    while (!stream.eof() && level)
    {
        std::getline(stream >> std::ws, str);
        if (str == "}")
        {
            level--;
        }
    }
    if (level)
    {
        throw std::invalid_argument("skip_to_same_bracket_level: brackets issue");
    }
    return 0;
}

std::pair<std::string, std::vector<std::string>> get_config_info(
        std::string const &configuration_file_path,
        std::string const &config_path)
{

    std::queue<std::string> path_queue;
    try
    {
        path_queue = configuration_path_to_path_q(config_path);
    }
    catch (std::invalid_argument &err)
    {
        throw err;
    }
    std::ifstream stream(configuration_file_path);
    if (!stream.is_open())
    {
        throw (std::runtime_error("get_config_info: failed to open file"));
    }


    std::string prev_str, str;
    while (!stream.eof() && !path_queue.empty())
    {
        std::getline(stream >> std::ws, str);
        if (str == "{")
        {
            if (prev_str == "}")
            {
                throw std::invalid_argument("get_config_info: looks like \'} {\' situation");
            }
            std::string temp;
            try { temp = unquote_string(path_queue.front()); }
            catch (std::invalid_argument &) { temp = path_queue.front(); }

            if (prev_str == temp)
            {
                path_queue.pop();
            }
            else
            {
                try
                {
                    skip_to_same_bracket_level(stream, str);
                }
                catch (std::invalid_argument &)
                {
                    throw std::invalid_argument("get_config_info: brackets issue");
                }
            }
        }
        prev_str = str;
    }

    if (stream.eof())
    {
        throw std::invalid_argument("get_config_info: can't find config path");
    }
    std::string log_format;
    std::getline(stream >> std::ws, log_format, '\n');
    if (log_format == "default")
    {
        log_format = LOG_FORMAT_DEFAULT;
    }
    else
    {
        try
        {
            log_format = unquote_string(log_format);
        }
        catch (std::invalid_argument &)
        {
            stream.close();
            throw std::invalid_argument("get_config_info: invalid log format");
        }
    }


    std::vector<std::string> file_sev_vector;
    str = "";

    while (!stream.eof() && str != "}")
    {
        getline(stream >> std::ws, str, '\n');
        if (str != "}")
        {
            file_sev_vector.push_back(str);
        }
    }
    if (stream.eof())
    {
        stream.close();
        throw std::invalid_argument("get_config_info: brackets issue");
    }
    stream.close();
    return std::make_pair(log_format, file_sev_vector);
}

logger_builder* client_logger_builder::transform_with_configuration(
        std::string const &configuration_file_path,
        std::string const &configuration_path)
{
    std::string prev_str, str;

    std::pair<std::string, std::vector<std::string>> logger_settings = get_config_info(configuration_file_path,
                                                                                       configuration_path);
    set_log_format(logger_settings.first);
    for (auto &curr_stream: logger_settings.second)
    {
        std::queue<std::string> file_severities = split_string(curr_stream, ':');
        std::string filename = file_severities.front();
        file_severities.pop();
        std::string severities = file_severities.front();
        file_severities.pop();

        if (filename == "console")
        {
            add_console_stream(severities);
        }

        else
        {
            try
            {
                filename = unquote_string(filename);
            }
            catch (std::invalid_argument &)
            {
                throw std::invalid_argument(
                        "client_logger_builder::transform_with_configuration: failed to unquote filename");
            }
            add_file_stream(filename, severities);
        }
    }
    return this;

}

logger* client_logger_builder::build() const
{
    return new client_logger(_data, _log_format);
}