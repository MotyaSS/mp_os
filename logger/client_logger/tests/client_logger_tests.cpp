
#include "client_logger_builder.h"
#include "client_logger.h"

int main()
{
    auto* new_logger = new client_logger_builder("%f %t %m");
    logger* log = new_logger
            ->add_console_stream(logger::severity::information)
            ->transform_with_configuration("config", R"("config example 1"/"valid log_cfg")")
            ->build();
    log->log("HAHAHHA", logger::severity::information);
    log->log("SHOOTING))))))", logger::severity::warning);

    delete new_logger;
    delete log;
}