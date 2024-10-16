
#include "client_logger_builder.h"
#include "client_logger.h"

int main()
{
    auto* builder = new client_logger_builder("%f %t %m");
    logger* log = builder
            ->add_console_stream(logger::severity::information)
            ->transform_with_configuration("config", R"("config example 1"/"valid log_cfg")")
            ->build();

    log->information("boobies")->warning("some warning");
    delete builder;
    delete log;
}