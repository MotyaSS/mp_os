#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H

#include <map>
#include <set>
#include <queue>
#include "logger_builder.h"


#define LOG_FORMAT_DEFAULT "[%d %t] (%s): %m"
#define LOG_CONSOLE_NAME ""

class client_logger_builder final :
        public logger_builder
{

private:
    std::map<std::string, std::set<logger::severity>> _data;
    std::string _log_format;

public:

    explicit client_logger_builder(std::string const &log_format);

    client_logger_builder();

    client_logger_builder(
            client_logger_builder const &other) = default;

    client_logger_builder &operator=(
            client_logger_builder const &other) = default;

    client_logger_builder(
            client_logger_builder &&other) noexcept = default;

    client_logger_builder &operator=(
            client_logger_builder &&other) noexcept = default;

    ~client_logger_builder() noexcept override = default;

public:

    logger_builder* add_file_stream(
            std::string const &stream_file_path,
            logger::severity severity) override;

    logger_builder* add_console_stream(
            logger::severity severity) override;

    logger_builder* add_file_stream(
            std::string const &stream_file_path,
            std::string const &severity);

    logger_builder* add_console_stream(
            std::string const &severity);

    logger_builder* transform_with_configuration(
            std::string const &configuration_file_path,
            std::string const &configuration_path) override;

    logger_builder* clear() override;

    logger_builder* set_log_format(std::string const &log_format);


    [[nodiscard]] logger* build() const override;

private:

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H